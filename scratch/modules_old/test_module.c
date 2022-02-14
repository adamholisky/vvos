#include <printf.h>
#include <string.h>
#include <stdbool.h>
#include "debug.h"
#include "syscall.h"
#include <task.h>
#include <callback.h>

void main( void ) {
	uint32_t	ret = 0;
	
	printf( "Hello, dynamic loading!\n" );

	exit_test( 33, ret );
}