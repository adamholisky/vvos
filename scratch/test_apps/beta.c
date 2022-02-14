#include <printf.h>
#include <string.h>
#include <stdbool.h>
#include "debug.h"
#include "syscall.h"
#include "setjmp.h"

jmp_buf buffer;

void main( void ) {
		if( setjmp( buffer ) ) {
			debugf( "Post long jump\n" );
		} else {
			debugf( "Post setjmp\n" );
			
			longjmp( buffer, 1 );
			
			debugf( "This shouldn't be called.\n" );
		}

	uint32_t ret;	
	exit_test( 33, ret );
}
