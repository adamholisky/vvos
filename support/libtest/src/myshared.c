#include <stdio.h>

void my_lib_call( void );

void my_lib_call( void ) {
    printf( "This function and call were modified without an OS recompile; only with a library recompile that was then copied into the secondary drive.\n" );
}