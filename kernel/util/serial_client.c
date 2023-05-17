#include "kernel.h"
#include "serial_client.h"
#include <task.h>
#include <elf.h>
#include <syscall.h>

char * data;
bool ssvv_read_is_waiting = false;
bool ssvv_read_data_is_ready = false;
uint8_t * ssvv_read_buff;
uint32_t ssvv_read_size;

void serial_client_initalize( void ) {
	data = kmalloc( 1024 * 5 );
	memset( data, 0, 1024 * 5 );
	//klog( "data buffer: 0x%08X\n", data );
	set_data_buffer( data );
	set_data_ready_callback( handle_data_ready );
}

void handle_data_ready( uint32_t size ) {
	set_data_is_being_read( true );

	dbA();
	if( ssvv_read_is_waiting ) {
		dbA();
		memcpy( ssvv_read_buff, data, size );
		ssvv_read_size = size;
		
		memset( data, 0, 1024 * 5 );
		ssvv_read_data_is_ready = true;
		//dbB();
	} else {
		//debugf( "\nsize: %d\n", size );
		//debugf( "%s", data );
	}

	set_data_is_being_read( false );
}

uint32_t ssvv_read( uint8_t * buff ) {
	//
}

void ssvv_send( char * cmd ) {
	log_entry_enter();
	int len = strlen( cmd );

	//debugf( "\nlen: %d\n", len );
	for( int i = 0; i < len; i++ ) {
		serial_write_port( cmd[i], COM1 );
	}

	serial_write_port( 26, COM1 );
	log_entry_exit();
}

uint32_t ssvv_send_ask( char * cmd, uint8_t * buff, uint32_t size ) {
	//log_entry_enter();
	uint32_t _size;
	int i = 0;

	ssvv_read_buff = buff;
	ssvv_read_is_waiting = true;

	klog( "Sending: \"%s\"\n", cmd );
	
	ssvv_send( cmd );

	while( ssvv_read_data_is_ready == false ) {
		//db5();
	}
	
	_size = ssvv_read_size;
	ssvv_read_size = 0;
	ssvv_read_buff = NULL;
	ssvv_read_is_waiting = false;
	ssvv_read_data_is_ready = false;

	//log_entry_exit();
	return _size;
}

uint32_t ssvv_read_file( char * full_path_to_file, uint8_t * buff ) {
	char cmd[100];
	int32_t size;

	strcpy( cmd, "READ " );
	strcat( cmd, full_path_to_file );

	size = ssvv_send_ask( cmd, buff, 2048 );

	return size;
}

void ssvv_read_file_test( void ) {
	char * buff;
	uint32_t size;

	buff = kmalloc( 14 );

	size = ssvv_read_file( "/home/adam/helloworld.txt", buff );

	for( int i = 0; i < size; i++, buff++ ) {
		debugf( "%c", *buff );
	}
}

char * app_page = NULL;

void ssvv_exec_test_app( void ) {
/* 	bool first_run = false;
	uint32_t size;
	uint32_t task_id;

	if( app_page == NULL ) {
		app_page = (char *)page_allocate( 1 );
		first_run = true;
	}

	memset( app_page, 0, 0x200000 );

	size = ssvv_read_file( "/usr/local/osdev/versions/v/modules/build/test_module.o", app_page );

	task_id = load_module_elf_image( (uint32_t *)app_page );

	task_initalize_and_run( task_id ); */

	//sched_yield();
}