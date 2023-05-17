#if !defined(DEVICE_INCLUDED)
#define DEVICE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <kernel.h>

#define DEVICE_DEFAULT_BUFFER_SIZE 4096
#define MAX_DEVICES 10

typedef struct {
    char name[25];
    char file[25];

    void (*init)( void );
    int (*read)( uint8_t *buff, uint32_t size );
    int (*write)( uint8_t *buff, uint32_t size );

} device;

void devices_initalize( void );
device *get_device_by_file( char *file_name );



#ifdef __cplusplus
}
#endif

#endif