#if !defined(VUIWINDOW_INCLUDED)
#define VUIWINDOW_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdint.h>
#include <stdbool.h>
#include <vui/vui.h>

#define VUI_WINDOW_COLOR_BORDER 0x00D6D6D6
#define VUI_WINDOW_COLOR_TITLEBAR 0x00363636
#define VUI_WINDOW_COLOR_INNERWINDOW 0x001E1E1E

typedef struct {
	vui_object  common;
	
	int			x;
	int			y;
	int			width;
	int			height;

	char		*title;
	
	vui_callback_with_handle	event_handler;
} vui_window;

vui_window *vui_window_new( int x, int y, int width, int height, char *title );
void vui_window_destroy( vui_window *window );
bool vui_window_draw( vui_window *window );
rect *vui_window_get_inner_rect( vui_window *window, rect *r );
void vui_window_set_event_handler( vui_window *window, vui_callback_with_handle eh );

#ifdef __cplusplus
}
#endif

#endif