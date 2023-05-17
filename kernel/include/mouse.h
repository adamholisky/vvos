#if !defined(MOUSE_INCLUDED)
#define MOUSE_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define MOUSE_BUTTON_LEFT 0
#define MOUSE_BUTTON_MIDDLE 1
#define MOUSE_BUTTON_RIGHT 2

void mouse_write( uint8_t byte );
uint8_t mouse_read( void );
void mouse_wait( uint8_t type );
void mouse_initalize( void );
void mouse_handler( void );


#ifdef __cplusplus
}
#endif

#endif