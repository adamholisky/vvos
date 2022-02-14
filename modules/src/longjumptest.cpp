#include <stdlib.h>
#include <stdio.h>
#include <debug.h>
#include <syscall.h>
#include <setjmp.h>

jmp_buf buffer;

extern "C" void module_init( void ) {

}

extern "C" void module_exit( void ) {

}

int main( int argc, char *argv[] ) {
	if( setjmp( buffer ) ) {
		debugf( "Post long jump\n" );
	} else {
		debugf( "Post setjmp\n" );
		
		longjmp( buffer, 1 );
		
		debugf( "This shouldn't be called.\n" );
	}

	return 0;
}