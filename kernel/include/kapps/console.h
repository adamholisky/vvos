#if !defined(KAPPSCONSOLE_INCLUDED)
#define KAPPSCONSOLE_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdint.h>
#include <stdbool.h>
#include <vui/vui.h>

int vui_console_main( int argc, char *argv[] );
void kapps_console_putc( char c );
void kapps_console_handle_events( vui_handle h, char *id, void *obj );
char kapps_console_getchar( void );

#ifdef __cplusplus
}
#endif

#endif