#include <kernel.h>
#include <syscall.h>
#include <keyboard.h>
#include <terminal.h>
#include <vui/vui.h>
#include <vui/console.h>
//-----
#include <vui/window.h>
#include <vui/label.h>
#include <kapps/console.h>

typedef struct {
	vui_window *win;
	vui_console *con;

	rect pane;
} console_app_state;

console_app_state capp_state;


#define CONSOLE_BUFFER_SIZE 255
int console_in_buffer_current = 0;
int console_in_buffer_end = 0;
char console_in_buffer[ CONSOLE_BUFFER_SIZE ];


int vui_console_main( int argc, char *argv[] ) {
	log_entry_enter();

	memset( console_in_buffer, 0, 255 );

	rect *r;

	capp_state.win = vui_window_new( 10, 10, 750, (16 * 25) + 30, "Fully Instanced vUI Console" );
	vui_set_parent( capp_state.win, vui_get_main_desktop() );

	r = vui_window_get_inner_rect( capp_state.win, &capp_state.pane );
	capp_state.con = vui_console_new( r->x + 5, r->y + 5, r->w - 5, r->h - 5 );
	vui_set_parent( capp_state.con, capp_state.win );

/* 	capp_state.con->transparent_text_background = true;
	capp_state.con->foreground = CONSOLE_COLOR_GREEN; */

	vui_console_clear( capp_state.con );
	set_terminal_redirect( kapps_console_putc );
	set_stdin_redirect( kapps_console_getchar );

	vui_window_set_event_handler( capp_state.win, kapps_console_handle_events );
	vui_set_active_window( capp_state.win->common.handle );

	vui_refresh();

 	vui_console_puts( capp_state.con, "The first duty of every \x1b[0;31;49mStarfleet officer\x1b[0;0;0m is to the truth, whether it's scientific truth or historical truth or personal truth." );

	vui_console_puts( capp_state.con, "\nNew line.\nAnother.\n" );

	log_entry_exit();

	return 0;
}

void kapps_console_putc( char c ) {
    vui_console_putc( capp_state.con, c );

	//klog( "!" );
}

void kapps_console_handle_key_down( int scanecode ) {

}

/**
 * @brief 
 * 
 * @param id 
 * @param obj 
 */
void kapps_console_handle_events( vui_handle h, char *id, void *obj ) {
	klog( "got one\n" );
	//vui_refresh();

	if( strcmp( "keypress", id ) == 0 ) {
		keyboard_event *k = (keyboard_event *)obj;

		console_in_buffer[ console_in_buffer_end ] = k->c;
		console_in_buffer_end++;

		if( console_in_buffer_end > CONSOLE_BUFFER_SIZE ) {
			console_in_buffer_end = 0;
		}
	}
}


char kapps_console_getchar( void ) {
	char c = 0;
	bool done = false;

	//return 'a';

	do {
		if( console_in_buffer_current != console_in_buffer_end ) {
			c = console_in_buffer[ console_in_buffer_current ];
			console_in_buffer_current++;

			if( console_in_buffer_current > CONSOLE_BUFFER_SIZE ) {
				console_in_buffer_current = 0;
			}

			done = true;
		} else {
			sched_yield();
		}
	} while( !done );

	return c;
}