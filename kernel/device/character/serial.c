#include <stdint.h>
#include <stdbool.h>
#include <globals.h>
#include <observer.h>
#include "bootstrap.h"
#include "serial.h"
#include "debug.h"
#include <keyboard.h>
#include <kmalloc.h>

uint32_t default_port;
uint32_t buffer_add_loc;
uint32_t buffer_read_loc;
void (*data_ready_callback)(int);
char *data_buff;
uint32_t data_buff_loc;
bool data_is_being_read;
int32_t data_buffer_task;
bool data_ready;
bool serial_cr_recvd;

void initalize_serial(void)
{
	data_buff = NULL;
	data_buff_loc = 0;
	data_is_being_read = false;
	data_ready = false;
	serial_cr_recvd = false;

	// DO NOT PUT KLOG FUNCTIONS HERE
	serial_setup_port(COM1);
	serial_setup_port(COM2);
	serial_setup_port(COM3);
	serial_setup_port(COM4);

	default_port = COM1;

	for( int i = 0; i < 1024; i++ ) {
		serial_in_com1_buffer[i] = '\0';
		serial_in_com2_buffer[i] = '\0';
		serial_in_com3_buffer[i] = '\0';
	}

	buffer_add_loc = 0;
	buffer_read_loc = 0;
	data_ready_callback = NULL;
}

void serial_setup_port(uint32_t port)
{
	outportb(port + 1, 0x00); // Disable all interrupts
	outportb(port + 3, 0x80); // Enable DLAB (set baud rate divisor)
	outportb(port + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
	outportb(port + 1, 0x00); //                  (hi byte)
	outportb(port + 3, 0x03); // 8 bits, no parity, one stop bit
	outportb(port + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
	outportb(port + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

void serial_enable_interrupts( void ) {
	outportb(COM1 + 1, 0x01);
	outportb(COM2 + 1, 0x01);
	outportb(COM3 + 1, 0x01);
}

void serial_set_default_port(uint32_t port)
{
	default_port = port;
}

void serial_write_port(char c, uint32_t port)
{
	if (port == serial_use_default_port)
	{
		port = default_port;
	}

	// Make sure the transmit queue is empty
	while((inportb(port + 5) & 0x20) == 0) {
		;
	}

	outportb(port, c);
}

char serial_read_port(uint32_t port)
{
	char c = '\0';

	if (port == serial_use_default_port)
	{
		port = default_port;
	}

	// Wait until we can get something
	while ((inportb(port + 5) & 1) == 0)
	{
		;
	}

	c = inportb( port );

	//debugf( "\n\n. %d -- %c\n", c, c );

	return c;
}

bool serial_buffer_is_ready( void ) {
	return serial_in_com2_buffer[ buffer_read_loc ] != '\0' ? true : false;
}

char serial_buffer_get_char( void ) {
	char ret_c = serial_in_com2_buffer[ buffer_read_loc ];

	serial_in_com2_buffer[ buffer_read_loc ] = '\0';
	buffer_read_loc++;

	if( buffer_read_loc == 256 ) {
		buffer_read_loc = 0;
	}

	return ret_c;
}

void set_data_ready_callback( void *f ) {
	data_ready_callback = f;
}

void set_data_buffer( char *buff ) {
	data_buff = buff;
}

void set_data_is_being_read( bool b ) {
	data_is_being_read = b;
}

void serial_interrupt_handler( uint32_t _port ) {
	uint32_t port = _port;
	
	if( port != COM1 && port != COM2 && port != COM3 ) {
		kpanic( "Unknown serial port 0x%X sent to interrupt handler.\n", port );
	}

	if( (inportb(port + 5) & 1) == 1 ) {
		// Continue on as COM1 or COM2
		serial_do_interrupt_read( port );
	}
	
	if( (inportb(port - 0x10 + 5) & 1) == 1 ) {
		// If COM3 is ready, do it
		serial_do_interrupt_read( port - 0x10 );
	}
}

void serial_clear_buffer( uint32_t port ) {
	switch( port ) {
		case COM1:
			serial_in_com1_buffer[0] = 0;
			serial_com1_buffer_read_loc = 0;
			serial_com1_buffer_add_loc = 0;
			break;
		case COM2:
			serial_in_com2_buffer[0] = 0;
			serial_com2_buffer_read_loc = 0;
			serial_com2_buffer_add_loc = 0;
			break;
		case COM3:
			serial_in_com2_buffer[0] = 0;
			serial_com2_buffer_read_loc = 0;
			serial_com2_buffer_add_loc = 0;
			break;
	}
}

void serial_do_interrupt_read( uint32_t port ) {
	char c = '\0';
	char * buffer;
	uint32_t add_loc;
	uint32_t read_loc;
	uint32_t add_loc_max;

	switch( port ) {
		case COM1:
			buffer = serial_in_com1_buffer;
			read_loc = serial_com1_buffer_read_loc;
			add_loc = serial_com1_buffer_add_loc;
			break;
		case COM2:
			buffer = serial_in_com2_buffer;
			read_loc = serial_com2_buffer_read_loc;
			add_loc = serial_com2_buffer_add_loc;
			break;
		case COM3:
			buffer = serial_in_com3_buffer;
			read_loc = serial_com3_buffer_read_loc;
			add_loc = serial_com3_buffer_add_loc;
			break;
		default:
			kpanic( "Default serial port read reached. Not coded.\n" );
	}

	// Keep reading as long as we have data
	do {
		c = inportb( port );
		//debugf( "%c", c );

		buffer[ add_loc ] = c;
		add_loc++;

		switch( port ) {
			case COM3:
				add_loc_max = 1024 * 20;
				break;
			default:
				add_loc_max = 1024;
		}

		if( add_loc == add_loc_max ) {
			add_loc = 0;
		}
	} while( (inportb(port + 5) & 1) == 1 );

	switch( port ) {
		case COM1:
			serial_com1_buffer_read_loc = read_loc;
			serial_com1_buffer_add_loc = add_loc;
			break;
		case COM2:
			serial_com2_buffer_read_loc = read_loc;
			serial_com2_buffer_add_loc = add_loc;
			break;
		case COM3:
			serial_com3_buffer_read_loc = read_loc;
			serial_com3_buffer_add_loc = add_loc;
			break;
	}

	// If graphics are not on, then read from serial port COM2 and generate a keyboard event
	if( READY_FOR_INPUT == true ) {
		if( GRAPHICS_ACTIVE == false ) {
			if( port == COM2 ) {
				event_message e;
				keyboard_event *k;
				e.data = kmalloc( sizeof( keyboard_event ) );
				e.subject_name = kmalloc( 25 );

				k = (keyboard_event *)e.data;

				k->scanecode = c;
				k->c = c;

				observer_notify( "keyboard", &e );

				kfree( e.subject_name );
				kfree( e.data );

				serial_com2_buffer_add_loc--;
			}
		}
	}
}