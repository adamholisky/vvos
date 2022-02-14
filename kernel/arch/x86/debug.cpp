#include <debug.h>
#include "interrupts.h"
#include "bootstrap.h"
#include "string.h"
#include <kmalloc.h>
#include <kernel_symbols.h>
#include <ftp.h>

uint32_t kernel_symbol_top = 0;
kdebug_symbol kernel_symbols[ KDEBUG_MAX_SYMBOLS ];
KernelSymbols symbols;

char * sys_text_arr[] = { 
							"System",
							"Kernel",
							"Intel8254",
							"Memory",
							"PCI",
							"Interrupts",
							"Paging",
							"ELF"
						};

void kdebug_initalize( void ) {
	memset( &kernel_symbols, 0, sizeof(kdebug_symbol) * KDEBUG_MAX_SYMBOLS );

	symbols.initalize();
}

void ftp_test( void ) {
	FTP ftp;

	printf( "\n" );

	ftp.init();
	
	klog( "Test start\n" );
	ftp.test();
	klog( "Test end\n" );

}

void kdebug_add_symbol( char * name, uint32_t addr, uint32_t size ) {
	symbols.add( name, addr, size );
}

kdebug_symbol * kdebug_get_symbol_array( void ) {
	//return kernel_symbols;

	return (kdebug_symbol *)symbols.get_symbol_array();
}

char * kdebug_get_function_at( uint32_t addr ) {
	return symbols.get_function_at( addr );
}

kdebug_symbol * kdebug_get_symbol( char * name ) {
	return (kdebug_symbol *)symbols.get_symbol( name );
}

uint32_t kdebug_get_total_symbols( void ) {
	return symbols.get_total_symbols();
}

uint32_t kdebug_get_symbol_addr( char * name ) {
	return symbols.get_symbol_addr( name );
}

char * kdebug_peek_at( uint32_t addr ) {
	printf( "\nLooking at 0x%08X\n", addr );
	debugf( "\nLooking at 0x%08X\n", addr );

	uint8_t *ptrn = (uint8_t *)addr;
	char *ptr = (char *)addr;

	for( int x = 0; x < 20; x++ ) {
		debugf( "0x%08X    %02X %02X %02X %02X    %02X %02X %02X %02X    %c%c%c%c%c%c%c%c\n",
		addr + (x * 8), 
		*(ptrn + 0), *(ptrn + 1), *(ptrn + 2), *(ptrn + 3), *(ptrn + 4), *(ptrn + 5), *(ptrn + 6), *(ptrn + 7), 
		peek_char( *(ptr + 0) ), peek_char( *(ptr + 1) ), peek_char( *(ptr + 2) ), peek_char( *(ptr + 3) ), peek_char( *(ptr + 4) ), peek_char( *(ptr + 5) ), peek_char( *(ptr + 6) ), peek_char( *(ptr + 7) ) );

		ptr = ptr + 8;
		ptrn = ptrn + 8;
	}

	printf( "\n" );

	return "KDEBUG_RETURN";
}

// Modified from https://stackoverflow.com/questions/9144800/c-reverse-bits-in-unsigned-integer
uint32_t reverse_uint32( uint32_t x ) {
    x = ((x >> 1) & 0x55555555u) | ((x & 0x55555555u) << 1);
    x = ((x >> 2) & 0x33333333u) | ((x & 0x33333333u) << 2);
    x = ((x >> 4) & 0x0f0f0f0fu) | ((x & 0x0f0f0f0fu) << 4);
    x = ((x >> 8) & 0x00ff00ffu) | ((x & 0x00ff00ffu) << 8);
    x = ((x >> 16) & 0xffffu) | ((x & 0xffffu) << 16);

    return x;
}

void klog_func( char * func, uint32_t line, char * message, ... ) {
	va_list args;

	debug_out_on();
	printf( "[\x1b[0;34;49m%s:%d\x1b[0;00;00m] ", func, line ); 
	va_start( args, message );
	vprintf_( message, args ); 
	va_end( args );
	printf( "\n" );
	debug_out_off();
}

/* static const char * var_map[1][1][1] = 
	{ 
		{0,"paging_level_active","uint32_t"}

	};

void klog_variable_func( uint32_t type, char * name, void * var, char * func, uint32_t line ) {
	char var_display[25];
	uint32_t var_num = 0;
	char display[100];

	switch( type ) {
		case TYPE_NUMBER:
			var_num = *(uint32_t *)var;
			sprintf( var_display, ": 0x%08X (%d)", var_num, var_num );
			break;
		case TYPE_STRING:
			strcpy( &var_display, (char *)var );
			break;
		case TYPE_ADDR:
		default:
			strcpy( &var_display, "" );
	}

	sprintf( display, "%s @ 0x%08X %s", name, var, var_display );

	klog_func( func, line, display );
} */

void k_log( uint32_t system_id, uint32_t level, char * message, ... ) {
	va_list args;

	debug_out_on();

	printf( "[%s] ", sys_text[ system_id ] );
	va_start( args, message );
	vprintf_( message, args ); 
	va_end( args );
	printf( "\n" );

	debug_out_off();

	printf( "[%s] ", sys_text[ system_id ] );
	va_start( args, message );
	vprintf_( message, args ); 
	va_end( args );
	printf( "\n" );
}

void kpanic( char * message, ... ) {
	va_list args;

	debug_out_on();

	printf( "\x1b[1;33mKernel Panic: " );
	va_start( args, message );
	vprintf_( message, args );
	va_end( args );
	printf( "\n" );

	debug_out_off();

	printf( "\x1b[0;31mKernel Panic: " );
	va_start( args, message );
	vprintf_( message, args );
	va_end( args );
	printf( "\n" );

	outportb( 0xF4, 0x00 );
}


char peek_char( char c ) {
	if( (c > 31) && (c < 127) ) {
		return c;
	} else {
		return '.';
	}
}

profile_information profile_from_latest_run;

void profile_start( void ) {
	profile_from_latest_run.start = get_timer_counter();
	profile_from_latest_run.stop = 0;
	profile_from_latest_run.length = 0;
}

void profile_stop( void ) {
	profile_from_latest_run.stop = get_timer_counter();
	profile_from_latest_run.length = profile_from_latest_run.stop - profile_from_latest_run.start;
}

profile_information * get_profile_information( void ) {
	return &profile_from_latest_run;
}

void profile_test( void ) {
	profile_test_run( 0xFFFFFFFF );
	profile_test_run( 0xFFFFFFFF / 2 );
	profile_test_run( 0xFFFFFFFF / 0xF );
}

void profile_test_run( uint32_t num_of_nops ) {
	uint32_t count;
	profile_information * test_info = get_profile_information();

	profile_start();
	for( count = 0; count < num_of_nops; count++ ) {
		asm( "nop" );
	}
	profile_stop();

	debugf( "\nProfile Test : 0x%08X\n", num_of_nops );
	debugf( "-------------------------------------------------------------\n" );
	debugf( "profile.start = 0x%08X\n", test_info->start );
	debugf( "profile.stop = 0x%08X\n", test_info->stop );
	debugf( "profile.length = 0x%08X / %d ticks / %d milliseconds\n", test_info->length, test_info->length, (test_info->length * 10) );
}

inline void debugf_stack_trace( void ) {
	stackframe *frame;

	asm ("movl %%ebp,%0" : "=r"(frame));

	klog( "Stack Trace\n----------------------\n" );
	
	for( int i = 0; (frame != NULL) && (i < STACKFRAME_MAX); i++ ) {
		klog( "% 2d:    0x%08X %s\n", i+1, frame->eip, kdebug_get_function_at(frame->eip) );
		frame = (stackframe *)frame->ebp;
	}
}

#pragma GCC push_options
#pragma GCC optimize ("O0")

void stack_trace_test_func_a( void ) {
	stack_trace_test_func_b();
}

void stack_trace_test_func_b( void ) {
	stack_trace_test_func_c();
}

void stack_trace_test_func_c( void ) {
	stack_trace_test_func_d();
}

void stack_trace_test_func_d( void ) {
	debugf_stack_trace();
}

#pragma GCC pop_options