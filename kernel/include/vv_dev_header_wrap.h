#ifndef VV_DEV_HEADER_WRAP
#define VV_DEV_HEADER_WRAP

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel.h"

#define vv_dev_optional_main(name) void main_##name ( void  )
#define vv_main_two(x) #x
#define vv_malloc( size ) kmalloc( size )
#define vv_free( ptr ) asm("nop")
#define vv_printf( ... ) debugf( __VA_ARGS__ )

#define hide_from_kernel

#ifdef __cplusplus
}
#endif

#endif