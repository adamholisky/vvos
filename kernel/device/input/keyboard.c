#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <kernel.h>
#include <observer.h>
#include <keyboard.h>
#include <syscall.h>

unsigned char keyboard_map[128] = {
	0, 27, '1', '2', '3', '4', '5', '6', '7', '8',	  /* 9 */
	'9', '0', '-', '=', '\b',						  /* Backspace */
	'\t',											  /* Tab */
	'q', 'w', 'e', 'r',								  /* 19 */
	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	  /* Enter key */
	0,												  /* 29   - Control */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
	'\'', '`', 0,									  /* Left shift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n',				  /* 49 */
	'm', ',', '.', '/', 0,							  /* Right shift */
	'*',
	0,	 /* Alt */
	' ', /* Space bar */
	0,	 /* Caps lock */
	0,	 /* 59 - F1 key ... > */
	0, 0, 0, 0, 0, 0, 0, 0,
	0, /* < ... F10 */
	0, /* 69 - Num lock*/
	0, /* Scroll Lock */
	0, /* Home key */
	0, /* Up Arrow */
	0, /* Page Up */
	'-',
	0, /* Left Arrow */
	0,
	0, /* Right Arrow */
	'+',
	0, /* 79 - End key*/
	0, /* Down Arrow */
	0, /* Page Down */
	0, /* Insert Key */
	0, /* Delete Key */
	0, 0, 0,
	0, /* F11 Key */
	0, /* F12 Key */
	0, /* All other keys are undefined */
};

unsigned char keyboard_map_shift[128] = {
	0, 27, '!', '@', '#', '$', '%', '^', '&', '*',	  /* 9 */
	'(', ')', '_', '+', '\b',						  /* Backspace */
	'\t',											  /* Tab */
	'Q', 'W', 'E', 'R',								  /* 19 */
	'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	  /* Enter key */
	0,												  /* 29   - Control */
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
	'"', '~', 0,									  /* Left shift */
	'|', 'Z', 'X', 'C', 'V', 'B', 'N',				  /* 49 */
	'M', '<', '>', '?', 0,							  /* Right shift */
	'*',
	0,	 /* Alt */
	' ', /* Space bar */
	0,	 /* Caps lock */
	0,	 /* 59 - F1 key ... > */
	0, 0, 0, 0, 0, 0, 0, 0,
	0, /* < ... F10 */
	0, /* 69 - Num lock*/
	0, /* Scroll Lock */
	0, /* Home key */
	0, /* Up Arrow */
	0, /* Page Up */
	'-',
	0, /* Left Arrow */
	0,
	0, /* Right Arrow */
	'+',
	0, /* 79 - End key*/
	0, /* Down Arrow */
	0, /* Page Down */
	0, /* Insert Key */
	0, /* Delete Key */
	0, 0, 0,
	0, /* F11 Key */
	0, /* F12 Key */
	0, /* All other keys are undefined */
};

char current_scancode;
bool is_shift;
bool is_waiting;
char character_buffer[255];
uint16_t character_buffer_end;
uint16_t character_buffer_current;

void keyboard_initalize(void)
{
	observer_function f = keyboard_event_handler;
	// Clear the buffer
	while (inportb(0x64) & 1)
	{
		inportb(0x60);
	}

	is_waiting = false;
	is_shift = false;
	current_scancode = 0;

	character_buffer_current = 0;
	character_buffer_end = 0;

	observer_register_subject( "keyboard" );
	observer_register( "keyboard_event_handler", f );
	observer_attach_to_subject( "keyboard", "keyboard_event_handler", 1 );
}

void keyboard_interrupt_handler(void) {
	uint8_t status;
	char scancode;
	keyboard_event *k;
	event_message e;

	status = inportb(0x64);

	if (status & 0x01)
	{
		scancode = inportb(0x60);

		//printf( " - %d - ", scancode );

		if (scancode == 42 || scancode == 54)
		{
			is_shift = true;
		}
		else if (scancode == -86 || scancode == -74)
		{
			is_shift = false;
		}
		else if (scancode > 0)
		{
			e.data = kmalloc( sizeof( keyboard_event ) );
			e.subject_name = kmalloc( 25 );

			k = (keyboard_event *)e.data;

			current_scancode = scancode;

			k->scanecode = scancode;

			if (is_shift)
			{
				k->shift_down = true;
				k->c = (char)keyboard_map_shift[(uint32_t)scancode];
			}
			else
			{
				k->shift_down = false;
				k->c = (char)keyboard_map[(uint32_t)scancode];
			}

			observer_notify( "keyboard", &e );

			kfree( e.subject_name );
			kfree( e.data );
		}
	}
}

char get_character(unsigned int scancode) {
	if (is_shift)
	{
		return keyboard_map_shift[scancode];
	}

	return keyboard_map[scancode];
}

char keyboard_get_char_nonblocking( void ) {
	return keyboard_get_char_stage2( false );
}

char keyboard_get_char_blocking( void ) {
	return keyboard_get_char_stage2( true );
}

char keyboard_get_char_stage2( bool blocking ) {
	bool done = false;
	char c;

	do {
		if( character_buffer_current != character_buffer_end ) {
			c = character_buffer[ character_buffer_current ];
			character_buffer_current++;

			if( character_buffer_current > 255 ) {
				character_buffer_current = 0;
			}

			done = true;
		} else {
			if( !blocking ) {
				sched_yield();
			}
		}
	} while( !done );

	return c;
}

void keyboard_event_handler( event_message * message ) {
	keyboard_event * k = (keyboard_event *)message->data;

	character_buffer[ character_buffer_end ] = k->c;
	character_buffer_end++;
	
	if( character_buffer_end > 255 ) {
		character_buffer_end = 0;
	}
}
