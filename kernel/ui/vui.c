#include <kernel.h>
#include <syscall.h>
#include <afs.h>
#include <fs.h>
#include <keyboard.h>
#include <mouse.h>

#include <vui/vui.h>

#include <vui/desktop.h>
#include <vui/label.h>
#include <vui/window.h>
#include <vui/console.h>
#include <vui/button.h>
#include <vui/alert.h>

#define SSFN_IMPLEMENTATION
#include <ssfn.h>

/*
 * I hate fonts
 * 
 * ./sfnconv.exe -T -M 14 -A DejaVuSansMono.ttf mono.sfn
 * ./sfnconv.exe -C mono.sfn
 * 
 */

vui_handle_data handles[ VUI_MAX_HANDLES ];

vui_handle  vui_handle_top;
vui_error   vui_last_error;

vui_desktop	*main_desktop;

int mouse_x;
int mouse_y;
rect prev_rect;
bool has_prev = false;

vui_handle active_window;

ssfn_t ssfn_ctx = { 0 };

uint8_t *vera_font;
uint8_t *uni_font;
uint8_t *firacode_font;

/**
 * @brief 
 * 
 */
void vui_initalize( void ) {
	log_entry_enter();

	vui_handle_top = 0;
	vui_last_error = 0;

	mouse_x = 0;
	mouse_y = 0;
	prev_rect.h = 14;
	prev_rect.w = 16;
	prev_rect.x = 0;
	prev_rect.y = 0;
	has_prev = true;
	
	vga_information *v = vga_get_info();

	main_desktop = vui_desktop_new( 0, 0, v->width, v->height, 0x00374760 );

	vui_set_main_desktop( main_desktop );

	observer_function f = vui_keypress;
	observer_register( "vui_keypress", f );
	observer_attach_to_subject( "keyboard", "vui_keypress", 1 );

	int fd = open( "/font/Vera.sfn", 0 );
	
	if( fd == -1 ) {
		klog( "Cannot open /font/Vera.sfn.\n" );
		printf( "Cannot open /font/Vera.sfn\n" );
	} else {
		int size = get_file_size( fd );

		vera_font = malloc( size );
		memset( vera_font, 0, size );

		int bytes_read = read( fd, vera_font, size );
	}

	int fd_unifont = open( "/font/unifont.sfn.gz", 0 );
	
	if( fd_unifont == -1 ) {
		klog( "Cannot open /font/unifont.srn.gz\n" );
		printf( "Cannot open /font/unifont.sfn.gz\n" );
	} else {
		int size = get_file_size( fd_unifont );

		uni_font = malloc( size );
		memset( uni_font, 0, size );

		int bytes_read = read( fd_unifont, uni_font, size );
	}

	int fd_firacodefont = open( "/font/FiraCode.sfn", 0 );
	
	if( fd_firacodefont == -1 ) {
		klog( "Cannot open /font/FiraCode.sfn\n" );
		printf( "Cannot open /font/FiraCode.sfn\n" );
	} else {
		int size = get_file_size( fd_firacodefont );

		firacode_font = malloc( size );
		memset( firacode_font, 0, size );

		int bytes_read = read( fd_firacodefont, firacode_font, size );
	}

	// load fonts ahead of time?

	memset( &ssfn_ctx, 0, sizeof( ssfn_t ) );
	int err = ssfn_load( &ssfn_ctx, vera_font );

	if( err < 0 ) {
		klog( "ssfn_load err: %d\n", err );
		return;
	}

	err = ssfn_load( &ssfn_ctx, uni_font );

	if( err < 0 ) {
		klog( "ssfn_load err: %d\n", err );
		return;
	}
	
	err = ssfn_load( &ssfn_ctx, firacode_font );

	if( err < 0 ) {
		klog( "ssfn_load err: %d\n", err );
		return;
	}


	vui_refresh();

	log_entry_exit();
}

/**
 * @brief 
 * 
 */
void vui_refresh( void ) {
	log_entry_enter();

	vui_handle_draw( main_desktop->common.handle );
	//console_draw();
	vga_draw_screen();

	log_entry_exit();
}

/**
 * @brief 
 * 
 * @param handle_type 
 * @return void* 
 */
void *vui_add_handle( int handle_type ) {
	void *ret = NULL;

	if( vui_handle_top == VUI_MAX_HANDLES ) {
		vui_last_error = VUI_ERROR_MAX_HANDLES;
		return 0;
	}

	handles[vui_handle_top].handle = vui_handle_top;
	handles[vui_handle_top].handle_type = handle_type;
	handles[vui_handle_top].in_use = true;

	switch( handle_type ) {
		case VUI_TYPE_DESKTOP:
			ret = malloc( sizeof(vui_desktop) );
			break;
		case VUI_TYPE_LABEL:
			ret = malloc( sizeof(vui_label) );
			break;
		case VUI_TYPE_WINDOW:
			ret = malloc( sizeof(vui_window) );
			break;
        case VUI_TYPE_CONSOLE:
            ret = malloc( sizeof(vui_console) );
            break;
		case VUI_TYPE_BUTTON:
			ret = malloc( sizeof(vui_button) );
			break;
		case VUI_TYPE_ALERT:
			ret = malloc( sizeof(vui_alert) );
			break;
		default:
			klog( "Unknown type: 0x%X\n", handle_type );
			dump_stack_trace();
			return NULL;
	}

	if( ret == NULL ) {
		klog( "Ret is NULL\n" );
		return NULL;
	}

	vui_object *obj = (vui_object *)ret;
	obj->handle = vui_handle_top;
	obj->type = handle_type;

	handles[vui_handle_top].resource = ret;

	vui_handle_top++;

	return ret;
}

/**
 * @brief 
 * 
 * @param handle 
 */
void vui_free_handle( vui_handle handle ) {
	log_entry_enter();

	vui_handle_data *hd = &handles[ handle ];

	klog( "htype: %d\n", hd->handle_type );

	switch( hd->handle_type ) {
		case VUI_TYPE_DESKTOP:
			vui_desktop_destroy( (vui_desktop *)hd->resource );
			break;
		case VUI_TYPE_LABEL:
			vui_label_destroy( (vui_label *)hd->resource );
			break;
		case VUI_TYPE_WINDOW:
			vui_window_destroy( (vui_window *)hd->resource );
			break;
		case VUI_TYPE_BUTTON:
			vui_button_destroy( (vui_button *)hd->resource );
			break;
		case VUI_TYPE_ALERT:
			klog( "hi" );
			vui_alert_destroy( (vui_alert *)hd->resource );
			break;
		default:
			klog( "Unknown type: 0x%X\n", hd->handle_type );
			return false;
	}

	handles[handle].in_use = false;

	// TODO: FREE HANDLES OF CHILDREN!!!

	free( handles[handle].resource );

	log_entry_exit();
}

#undef KDEBUG_VUI_HANDLE_DRAW
/**
 * @brief 
 * 
 * @param handle 
 * @return true 
 * @return false 
 */
bool vui_handle_draw( vui_handle handle ) {
	#ifdef KDEBUG_VUI_HANDLE_DRAW
	log_entry_enter();
	#endif

	vui_handle *h;

	// TODO: Check if handle is legit

	vui_handle_data *hd;
	hd = &handles[ handle ];

	switch( hd->handle_type ) {
		case VUI_TYPE_DESKTOP:
			vui_desktop_draw( (vui_desktop *)hd->resource );
			break;
		case VUI_TYPE_LABEL:
			vui_label_draw( (vui_label *)hd->resource );
			break;
		case VUI_TYPE_WINDOW:
			vui_window_draw( (vui_window *)hd->resource );
			break;
        case VUI_TYPE_CONSOLE:
            vui_console_draw( (vui_console *)hd->resource );

			rect r = {
				.x = ((vui_console *)hd->resource)->x,
				.y = ((vui_console *)hd->resource)->y,
				.w = ((vui_console *)hd->resource)->width,
				.h = ((vui_console *)hd->resource)->height
			};

			vga_draw_screen_box( &r );
            break;
		case VUI_TYPE_BUTTON:
			vui_button_draw( (vui_button *)hd->resource );
			break;
		case VUI_TYPE_ALERT:
			vui_alert_draw( (vui_button *)hd->resource );
			break;
		default:
			klog( "Unknown type: 0x%X\n", hd->handle_type );
			return false;
	}

	// Handle children
	for( int i = 1; i < vui_handle_top; i++ ) {
		vui_object *obj = NULL;

		if( handles[i].in_use == true ) {
			obj = (vui_object *)handles[i].resource;

			if( obj->parent == handle ) {
				vui_handle_draw( obj->handle );
			}
		}
	}

	if( hd->handle_type == VUI_TYPE_WINDOW ) {
		if( ((vui_window *)hd->resource)->common.custom_paint_func ) {
			((vui_window *)hd->resource)->common.custom_paint_func( hd->resource );
		}
		
	}

	#ifdef KDEBUG_VUI_HANDLE_DRAW
	log_entry_exit();
	#endif

	return true;
}

/**
 * @brief 
 * 
 * @param child 
 * @param parent 
 * @return true 
 * @return false 
 */
bool vui_set_parent( void *child, void *parent ) {
	vui_object *obj_child = (vui_object *)child;
	vui_object *obj_parent = (vui_object *)parent;

	obj_child->parent = obj_parent->handle;

	return true;
}

/**
 * @brief 
 * 
 * @param x 
 * @param y 
 * @param width 
 * @param height 
 * @param color 
 */
void vui_draw_rectangle( int x, int y, int width, int height, uint32_t color ) {
	rect r = {
		.x = x,
		.y = y,
		.h = height,
		.w = width
	};

	draw_rect( r, color );
}

/**
 * @brief 
 * 
 * @param font 
 * @param size 
 * @param s 
 * @return int 
 */
int vui_get_string_width( int font, int size, char *s ) {
	int width = 0;
	int height = 0;
	int left = 0;
	int top = 0;
	char font_name[25];

	memset( font_name, 0, 25 );

	int style = SSFN_STYLE_REGULAR;

	switch( font ) {
		case VUI_FONT_UNI:
			style = style | SSFN_STYLE_NOAA;
			strcpy( font_name, "Unifont" );
			break;
		case VUI_FONT_FIRACODE:
			strcpy( font_name, "FiraCode Regular" );
			break;
		case VUI_FONT_VERAB:
			style = SSFN_STYLE_BOLD;
		case VUI_FONT_VERA:
		default:
			strcpy( font_name, "Bitstream Vera Sans" );
	}
	
	int err = ssfn_select( &ssfn_ctx, SSFN_FAMILY_BYNAME, font_name, style, size );

	if( err < 0 ) {
		klog( "ssfn_select err: %d\n", err );
		return;
	}

	int bbox_result = ssfn_bbox( &ssfn_ctx, s, &width, &height, &left, &top );

	return width;
}

/**
 * @brief 
 * 
 * @param x 
 * @param y 
 * @param size 
 * @param fg 
 * @param font 
 * @param s 
 */
void vui_draw_string( int x, int y, int size, uint32_t bg, uint32_t fg, int font, char *s ) {
	vga_information *v = vga_get_info();
	char font_name[25];

	memset( font_name, 0, 25 );

	ssfn_buf_t buf_front = {
		.ptr = v->buffer,
		.w = v->width,
		.h = v->height,
		.p = (v->width * 4),
		.x = x,
		.y = y,
		.fg = fg,
		.bg = bg
	};


	int style = SSFN_STYLE_REGULAR;

	switch( font ) {
		case VUI_FONT_UNI:
			style = style ;
			strcpy( font_name, "Unifont" );
			break;
		case VUI_FONT_FIRACODE:
			style = style;
			strcpy( font_name, "FiraCode Regular" );
			break;
		case VUI_FONT_VERAB:
			style = SSFN_STYLE_BOLD;
		case VUI_FONT_VERA:
		default:
			strcpy( font_name, "Bitstream Vera Sans" );
	}
	
	int err = ssfn_select( &ssfn_ctx, SSFN_FAMILY_BYNAME, font_name, style, size );

	if( err < 0 ) {
		klog( "ssfn_select err: %d\n", err );
		return;
	}

	int x_prev = buf_front.x;

	

	for( int i = 0; i < strlen(s); i++ ) {
		//klog( "x_prev = %d\n", x_prev );

		err = ssfn_render( &ssfn_ctx, &buf_front, (s + i) );

		if( err < 0 ) {
			klog( "ssfn_render[%d] err: %d\n", i, err );
			return;
		}
		
		// TODO: Should be based on monospace, not font family name
		if( font == VUI_FONT_FIRACODE ) {
			if( buf_front.x != x_prev + (size/2) ) {
				buf_front.x = x_prev + (size/2);
			}

			rect r = {
				.w = buf_front.x - x_prev,
				.h = size,
				.x = x_prev,
				.y = y
			};

			klog( "r: x = %d    y = %d    w = %d    h = %d\n", r.x, r.y, r.w, r.h );

			//vga_draw_screen_box( &r );
			//framebuffer_copy_to_frontbuffer( x_prev, y, buf_front.x - x_prev, size );

			x_prev = buf_front.x;
		}
		
		x_prev = buf_front.x;
	}
}

void vui_draw_string_immediate( int x, int y, int size, uint32_t bg, uint32_t fg, int font, char *s ) {
	vga_information *v = vga_get_info();
	char font_name[25];

	memset( font_name, 0, 25 );

	ssfn_buf_t buf_back = {
		.ptr = v->buffer,
		.w = v->width,
		.h = v->height,
		.p = (v->width * 4),
		.x = x,
		.y = y,
		.fg = fg,
		.bg = bg
	};

	ssfn_buf_t buf_front = {
		.ptr = v->fbuffer,
		.w = v->width,
		.h = v->height,
		.p = (v->width * 4),
		.x = x,
		.y = y,
		.fg = fg,
		.bg = bg
	};


	int style = SSFN_STYLE_REGULAR;

	switch( font ) {
		case VUI_FONT_UNI:
			style = style ;
			strcpy( font_name, "Unifont" );
			break;
		case VUI_FONT_FIRACODE:
			style = style | SSFN_STYLE_NOKERN;
			buf_back.y = y + (size/2) + 1;
			buf_front.y = y + (size/2) + 1;
			strcpy( font_name, "FiraCode Regular" );
			break;
		case VUI_FONT_VERAB:
			style = SSFN_STYLE_BOLD;
		case VUI_FONT_VERA:
		default:
			strcpy( font_name, "Bitstream Vera Sans" );
	}
	
	int err = ssfn_select( &ssfn_ctx, SSFN_FAMILY_BYNAME, font_name, style, size );

	if( err < 0 ) {
		klog( "ssfn_select err: %d\n", err );
		return;
	}

	int x_prev = buf_front.x;

	

	for( int i = 0; i < strlen(s); i++ ) {
		//klog( "x_prev = %d\n", x_prev );

		err = ssfn_render( &ssfn_ctx, &buf_front, (s + i) );
		err = ssfn_render( &ssfn_ctx, &buf_back, (s + i) );

		if( err < 0 ) {
			klog( "ssfn_render[%d] err: %d\n", i, err );
			return;
		}
		
		// TODO: Should be based on monospace, not font family name
		if( font == VUI_FONT_FIRACODE ) {
			if( buf_front.x != x_prev + (size/2) ) {
				buf_front.x = x_prev + (size/2);
				buf_back.x = x_prev + (size/2);
			}

			/* rect r = {
				.w = buf_front.x - x_prev,
				.h = size,
				.x = x_prev,
				.y = y
			};

			klog( "r: x = %d    y = %d    w = %d    h = %d\n", r.x, r.y, r.w, r.h );
 */
			//vga_draw_screen_box( &r );
			//framebuffer_copy_to_frontbuffer( x_prev, y, buf_front.x - x_prev, size );

			x_prev = buf_front.x;
		}
		
		x_prev = buf_front.x;
	}
}

/**
 * @brief 
 * 
 * @param x 
 * @param y 
 * @param size 
 * @param fg 
 * @param font 
 * @param s 
 */
void vui_draw_string_mono( int x, int y, int size, uint32_t fg, int font, char *s ) {
	vga_information * vga = vga_get_info();

	for( int i = 0; i < strlen(s); i++ ) {
		draw_char( vga->buffer, x + (i * vga->char_width), y, fg, 0x00CCCCCC, s[i] );
		draw_char( vga->fbuffer, x + (i * vga->char_width), y, fg, 0x00CCCCCC, s[i] );
	}
	//draw_string( s, x, y, fg, 0x00FFFFFF );
}

/**
 * @brief 
 * 
 * @param x 
 * @param y 
 * @param size 
 * @param bg 
 * @param fg 
 * @param font 
 * @param s 
 */
void vui_draw_string_mono_with_background( int x, int y, int size, uint32_t bg, uint32_t fg, int font, char *s ) {
	vga_information * vga = vga_get_info();

	for( int i = 0; i < strlen(s); i++ ) {
		draw_char( vga->buffer, x + (i * vga->char_width), y, fg, bg, s[i] );
		draw_char( vga->fbuffer, x + (i * vga->char_width), y, fg, bg, s[i] );
	}
	//draw_string( s, x, y, fg, 0x00FFFFFF );
}

#undef KDEBUG_VUI_MOUSE_CLICK
/**
 * @brief 
 * 
 * @param button 
 */
void vui_mouse_click( uint8_t button ) {
	#ifdef KDEBUG_VUI_MOUSE_CLICK
	switch( button ) {
		case MOUSE_BUTTON_LEFT:
			printf( "mouse click left at %d, %d\n", mouse_x, mouse_y );

			break;
		case MOUSE_BUTTON_MIDDLE:
			printf( "mouse click middle at %d, %d\n", mouse_x, mouse_y );
			break;
		case MOUSE_BUTTON_RIGHT:
			printf( "mouse click right at %d, %d\n", mouse_x, mouse_y );
			break;
		default:
			printf( "unknown button returned!\n" );
	}
	#endif

	if( button == MOUSE_BUTTON_LEFT ) {
		vui_handle obj_handle = vui_get_handle_at_point( mouse_x, mouse_y );

		#ifdef KDEBUG_VUI_MOUSE_CLICK
		printf( "Returned handle: %d\n", obj_handle );
		klog( "Returned handle: %d\n", obj_handle );
		#endif

		if( handles[obj_handle].handle_type == VUI_TYPE_BUTTON ) {
			vui_handle_data *hd;
			hd = &handles[ obj_handle ];
			vui_button *btn = (vui_button *)hd->resource;

			btn->on_mouseup( btn->id, btn );

			#ifdef KDEBUG_VUI_MOUSE_CLICK
			printf( "Click on button id: %s\n", btn->id );
			klog( "Click on button id: %s\n", btn->id );
			#endif
		}
	}
}

/**
 * @brief Keypress event handler
 * 
 * @param message keyboard_event contained in the message's data block
 */
void vui_keypress( event_message * message ) {
	keyboard_event *e = (keyboard_event *)message->data;

	klog( "Scancode: 0x%X\n", e->scanecode );

	vui_handle_data *hd;
	hd = &handles[ active_window ];
	vui_window *w = (vui_window *)hd->resource;

	klog( "eh: 0x%X\n", w->event_handler );

	if( w->event_handler ) {
		w->event_handler( active_window, "keypress", message->data );
	}
}

/**
 * @brief 
 * 
 * @param x 
 * @param y 
 */
void vui_mouse_move( int32_t x, int32_t y ) {
	int old_x = mouse_x;
	int old_y = mouse_y;

	mouse_x = mouse_x + x;
	mouse_y = mouse_y + y;

	if( mouse_x >= VVOS_SCREEN_WIDTH ) {
		mouse_x = VVOS_SCREEN_WIDTH;
	}

	if( mouse_y >= VVOS_SCREEN_HEIGHT ) {
		mouse_y = VVOS_SCREEN_HEIGHT;
	}

	if( mouse_x < 0 ) {
		mouse_x = 0;
	}

	if( mouse_y < 0 ) {
		mouse_y = 0;
	}

	rect r = {
		.x = mouse_x,
		.y = mouse_y,
		.w = 7,
		.h = 14
	};

	
	vga_information *vgainfo = vga_get_info();

	static unsigned long cursor_bitmap_2[] = {
		0x8000,
		0xC000,
		0xE000,
		0xF000,
		0xF800,
		0xFC00,
		0xFE00,
		0xFF00,
		0xFE00,
		0xFC00,
		0xDE00,
		0x8E00,
		0x0E00,
		0x0000,
	};

	vga_draw_screen_box( &prev_rect );

	prev_rect.x = mouse_x;
	prev_rect.y = mouse_y;

	for( int i = 0; i < 14; i++ ) {
		for( int j = 15; j > -1; j-- ) {
			if( test_bit( cursor_bitmap_2[i], j ) ) {
				putpixel( vgainfo->fbuffer, mouse_x + 15 - j, mouse_y + i, 0x00000000 );
			}
		}
	}

	//klog( "Mouse move. Direction: %d, amount %d --- (old: %d, %d.  new: %d, %d.)\n", direction, amount, old_x, old_y, mouse_x, mouse_y );
}

/**
 * @brief 
 * 
 * @param win 
 */
void vui_set_active_window( vui_handle win ) {
	active_window = win;
}

#undef KDEBUG_VUI_GET_HANDLE_RECT
/**
 * @brief 
 * 
 * @param h 
 * @return rect 
 */
rect vui_get_handle_rect( vui_handle h ) {
	// TODO: Check if handle is legit
	// TODO: Make thread safe
	rect rect_get_handle;

	int x, y, w, he = 0;

	vui_handle_data *hd;
	hd = &handles[ h ];

	#ifdef KDEBUG_VUI_GET_HANDLE_RECT
	klog( "XXXXXXXXXXXXXX handle type: %d\n", hd->handle_type );
	#endif

	switch( hd->handle_type ) {
		case VUI_TYPE_WINDOW:
			vui_window *vuiwindow = (vui_window *)hd->resource;
			he = vuiwindow->height;
			w = vuiwindow->width;
			x = vuiwindow->x;
			y = vuiwindow->y;
			break;
		case VUI_TYPE_DESKTOP:
			vui_desktop *vuidesktop = (vui_desktop *)hd->resource;
			he = vuidesktop->height;
			w = vuidesktop->width;
			x = vuidesktop->x;
			y = vuidesktop->y;
			break;
		case VUI_TYPE_LABEL:
			vui_label *vuilabel = (vui_label *)hd->resource;
			he = 1;
			w = 1;
			x = vuilabel->x;
			y = vuilabel->y;
			break;
		case VUI_TYPE_BUTTON:
			vui_button *vuibutton = (vui_button *)hd->resource;
			he = vuibutton->height;
			w = vuibutton->width;
			x = vuibutton->x;
			y = vuibutton->y;
			break;
		default:
			klog( "Unknown type: 0x%X\n", hd->handle_type );
			return;
	}

	#ifdef KDEBUG_VUI_GET_HANDLE_RECT
	klog( "x y w h: %d %d %d %d\n", x, y, w, he );
	#endif

	rect_get_handle.h = he;
	rect_get_handle.w = w;
	rect_get_handle.x = x;
	rect_get_handle.y = y;

	return rect_get_handle;
}

#undef KDEBUG_VUI_GET_HANDLE_AT_POINT
/**
 * @brief 
 * 
 * @param x 
 * @param y 
 * @return vui_handle 
 */
vui_handle vui_get_handle_at_point( int x, int y ) {
	// ONLY do the active window for now

	#ifdef KDEBUG_VUI_GET_HANDLE_AT_POINT
	klog( "finding %d, %d in handle: %d\n", x, y, active_window );
	#endif

	return vui_get_handle_at_point_inside_handle( active_window, x, y );
}

#undef KDEBUG_VUI_GET_HANDLE_AT_POINT_INSIDE_HANDLE
/**
 * @brief 
 * 
 * @param h 
 * @param x 
 * @param y 
 * @return vui_handle 
 */
vui_handle vui_get_handle_at_point_inside_handle( vui_handle h, int x, int y ) {
	#ifdef KDEBUG_VUI_GET_HANDLE_AT_POINT_INSIDE_HANDLE
	log_entry_enter();
	#endif

	vui_handle found_handle = 0;
	vui_handle possible_handle = 0;

	// TODO: Check if handle is legit

	// Is the point in the current handle?
	rect r = vui_get_handle_rect( h );

	#ifdef KDEBUG_VUI_GET_HANDLE_AT_POINT_INSIDE_HANDLE
	klog( "finding %d, %d inside of handle %d (%d, %d, w: %d   h: %d)\n", x, y, h, r.x, r.y, r.w, r.h );
	#endif

	if( x >= r.x ) {
		if( x <= r.x + r.w ) {
			if( y >= r.y ) {
				if( y <= r.y + r.h ) {
					// yay!

					#ifdef KDEBUG_VUI_GET_HANDLE_AT_POINT_INSIDE_HANDLE
					klog( "found in rect\n" );
					#endif

					found_handle = h;

					// now what about children...

					for( int i = 1; i < vui_handle_top; i++ ) {
						vui_object *obj = NULL;

						if( handles[i].in_use == true ) {
							obj = (vui_object *)handles[i].resource;

							if( obj->parent == h ) {
								possible_handle = vui_get_handle_at_point_inside_handle( obj->handle, x, y );

								if( possible_handle != 0 ) {
									found_handle = possible_handle;
								}
							}
						}
					}
				}
			}
		}
	}

	#ifdef KDEBUG_VUI_GET_HANDLE_AT_POINT_INSIDE_HANDLE
	log_entry_exit();
	#endif

	return found_handle;
}

void vui_for_each_child_handle_callback( vui_handle h, vui_callback_with_handle c, char *id, void *obj ) {
	// Handle children
	for( int i = 1; i < vui_handle_top; i++ ) {
		vui_object *obj = NULL;

		if( handles[i].in_use == true ) {
			obj = (vui_object *)handles[i].resource;

			if( obj->parent == h ) {
				c( obj->handle, id, obj );
			}
		}
	}
}

/*
	if( hd->handle_type == VUI_TYPE_WINDOW ) {
		if( ((vui_window *)hd->resource)->common.custom_paint_func ) {
			((vui_window *)hd->resource)->common.custom_paint_func( hd->resource );
		}
		
	}
	
*/