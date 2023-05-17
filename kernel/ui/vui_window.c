#include <kernel.h>
#include <vui/vui.h>
#include <vui/window.h>

int vui_window_border = 1;
int vui_window_titlebar_height = 30;
int ui_top_l_width = 22;
int ui_top_r_width = 39;
int ui_bottom_height = 2;

vui_window *vui_window_new( int x, int y, int width, int height, char *title ) {
	vui_window *window = vui_add_handle( VUI_TYPE_WINDOW );

	window->x = x;
	window->y = y;
	window->width = width;
	window->height = height;
	window->title = malloc( strlen(title) );
	strcpy( window->title, title );
	window->common.custom_paint_func = NULL;

	return window;
}

void vui_window_destroy( vui_window *window ) {
	free( window->title );
}

bool vui_window_draw( vui_window *window ) {
	int _x = window->x;
	int _y = window->y;
	int middle_repeat = window->width - ui_top_l_width - ui_top_r_width - 1;

	rect r = {
		.x = window->x,
		.y = window->y,
		.w = window->width,
		.h = window->height
	};
	
	draw_rect( r, VUI_WINDOW_COLOR_BORDER );

	r.h = 30;
	r.w = r.w - 2;
	r.x = r.x + 1;
	r.y = r.y + 1;

	draw_rect( r, VUI_WINDOW_COLOR_TITLEBAR );

	draw_rect( *vui_window_get_inner_rect( window, &r ), VUI_WINDOW_COLOR_INNERWINDOW );

	// title
	int title_width = vui_get_string_width( VUI_FONT_VERAB, 11, window->title );
	int title_x = window->x + 14;
	int title_y = window->y + 10;

	klog( "title width: %d\n", title_width );

	vui_draw_string( title_x, title_y + 10, 12, 0xFF363636, 0xFFFFFFFF, VUI_FONT_VERAB, window->title );

	return true;
}

/**
 * @brief 
 * 
 * @param window 
 * @param r 
 * @return rect* 
 */
rect *vui_window_get_inner_rect( vui_window *window, rect *r ) {
	r->x = window->x + vui_window_border;
	r->y = window->y + vui_window_titlebar_height + vui_window_border;
	r->w = window->width - vui_window_border - vui_window_border;
	r->h = window->height - vui_window_titlebar_height - vui_window_border - vui_window_border;

	return r;
}

/**
 * @brief 
 * 
 * @param window 
 * @param eh 
 */
void vui_window_set_event_handler( vui_window *window, vui_callback_with_handle eh ) {
	window->event_handler = eh;
}


