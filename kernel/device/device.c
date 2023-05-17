#include <kernel.h>
#include <device.h>

uint32_t device_last;

device devices[ MAX_DEVICES ];

void devices_initalize( void ) {
    device_last = 0;
}

device *device_add_new( void ) {
    if( device_last == MAX_DEVICES ) {
        return NULL;
    }

    device_last++;

    return ( devices + device_last - 1 );
}

device *get_device_by_file( char *file_name ) {
    device *result = NULL;

    for( int i = 0; i < device_last; i++ ) {
        if( strcmp( devices[i].file, file_name ) == 0 ) {
            result = &devices[i];
            i = MAX_DEVICES + 1;
        }
    }

    return result;
}