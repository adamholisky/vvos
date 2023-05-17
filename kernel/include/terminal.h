#if !defined(TERMINAL_INCLUDED)
#define TERMINAL_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "vga.h"
#include "printf.h"

typedef void (*putc_redirect_func)(char c);

/* typedef struct {
    uint32_t	current_row;
    uint32_t	current_column;
    uint32_t	foreground_color;
    uint32_t	background_color;
} term; */

void term_initalize( void );
/* void term_print_color( void );
void term_set_color( uint32_t foreground, uint32_t background ); */
void set_terminal_redirect( putc_redirect_func func );
void term_put_char( char c );
/* void term_put_char_at( char c, unsigned char color, unsigned int x, unsigned int y );
void term_clear_last_char( void ); */
void term_put_string( const char* data, size_t size );
/* void term_write_string( const char* data ); */
void set_debug_output( bool d );
void write_serial(char a);
void write_to_serial_port( char c );

/* void terminal_device_init( void );
int terminal_device_write( uint8_t *buff, uint32_t size );
int terminal_device_read( uint8_t *buff, uint32_t size );

void console_init( char * name, unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int bg_color, unsigned int fg_color );
void console_draw( void );
void console_put_char_at( char c, unsigned int x, unsigned int y );
void console_put_char( char c );
void console_move_row( unsigned int dest, unsigned int src );
void console_scroll_forever_test( void ); */

#ifdef __cplusplus
}
#endif

#endif