#include <printf.h>
#include <string.h>
#include <stdbool.h>
#include "debug.h"
#include "syscall.h"
#include <task.h>
#include <callback.h>

void callback_test_call( char * str );

void main( void ) {
	callback_test_register( "callback_test_call" );
	klog( "Callback registered.\n" );
	
	while ( true ) {
		sched_yield();
	}
}

void callback_test_call( char * str ) {
	debugf( "\n" );
	klog( "%s\n", str );
}