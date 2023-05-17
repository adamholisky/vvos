#if !defined(LOADER_INCLUDED)
#define LOADER_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

int loader_load_and_run( uint8_t *buff, int argc, char *argv[] );
uint32_t loader_load( uint8_t *buff, char *name );

#ifdef __cplusplus
}
#endif

#endif