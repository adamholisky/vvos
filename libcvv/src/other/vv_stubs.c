#include <stdint.h>

extern kmalloc( uint32_t size );
extern kfree( void *ptr );

void * malloc( uint32_t size ) {
	return kmalloc( size );
}

void free( void *ptr ) {
	return kfree( ptr );
}