#if !defined(VUICONSOLE_INCLUDED)
#define VUICONSOLE_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdint.h>
#include <stdbool.h>
#include <vui/vui.h>

#define CONSOLE_COLOR_BLACK 0
#define CONSOLE_COLOR_BLUE 1
#define CONSOLE_COLOR_GREEN 2
#define CONSOLE_COLOR_CYAN 3
#define CONSOLE_COLOR_RED 4
#define CONSOLE_COLOR_MAGENTA 5
#define CONSOLE_COLOR_BROWN 6
#define CONSOLE_COLOR_LIGHT_GREY 7
#define CONSOLE_COLOR_DARK_GREY 8
#define CONSOLE_COLOR_LIGHT_BLUE 9
#define CONSOLE_COLOR_LIGHT_GREEN 10
#define CONSOLE_COLOR_LIGHT_CYAN 11
#define CONSOLE_COLOR_LIGHT_RED 12
#define CONSOLE_COLOR_LIGHT_MAGENTA 13
#define CONSOLE_COLOR_YELLOW 14
#define CONSOLE_COLOR_WHITE 15

/* Original colors...
#define CONSOLE_COLOR_RGB_BLACK 	0x00000000
*/

#define CONSOLE_COLOR_RGB_BLACK 0x001E1E1E
#define CONSOLE_COLOR_RGB_BLUE 0x000000CC
#define CONSOLE_COLOR_RGB_GREEN	0x0000CC00
#define CONSOLE_COLOR_RGB_CYAN 0x0000CCCC
#define CONSOLE_COLOR_RGB_RED 0x00CC0000
#define CONSOLE_COLOR_RGB_MAGENTA 0x00AA00AA
#define CONSOLE_COLOR_RGB_BROWN 0x00AA5500
#define CONSOLE_COLOR_RGB_LIGHT_GREY 0x00AAAAAA
#define CONSOLE_COLOR_RGB_DARK_GREY 0x00555555
#define CONSOLE_COLOR_RGB_LIGHT_BLUE 0x005555FF
#define CONSOLE_COLOR_RGB_LIGHT_GREEN 0x0055FF55
#define CONSOLE_COLOR_RGB_LIGHT_CYAN 0x0055FFFF
#define CONSOLE_COLOR_RGB_LIGHT_RED	0x00FF5555
#define CONSOLE_COLOR_RGB_LIGHT_MAGENTA 0x00FF55FF
#define CONSOLE_COLOR_RGB_YELLOW 0x00FFFF55
#define CONSOLE_COLOR_RGB_WHITE 0x00D9D9D9

typedef struct {
	vui_object  common;
	
	int			x;
	int			y;
	int			width;
	int			height;

	int         font_width;
	int         font_height;

	int         cols;
	int         rows;

	int         current_x;
	int         current_y;

	bool		cursor_shown;

	uint8_t		foreground;
	uint8_t		background;
	bool		transparent_text_background;

	bool		capturing_escape_code;
	int			capture_num;
	char		captured_escape[25];
	int			tab_size;

	char		character_buffer[255];

	uint16_t    *data;
} vui_console;

vui_console *vui_console_new( int x, int y, int width, int height );
void vui_console_destroy( vui_console *console );
bool vui_console_draw( vui_console *console );
bool vui_console_draw_x_y( vui_console *console, int x, int y );
int vui_console_putc_at( vui_console *console, int x, int y, char c );
int vui_console_putc( vui_console *console, char c );
int vui_console_putc_color( vui_console * console, char c, int bg, int fg );
void vui_console_clear( vui_console *console );
int vui_console_puts( vui_console *console, char *s );
void vui_console_update_cursor( vui_console *console );
void vui_console_clear_cursor( vui_console *console );

#ifdef __cplusplus
}
#endif

#endif