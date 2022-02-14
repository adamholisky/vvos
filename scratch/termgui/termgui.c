#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include "termgui.h"

gt_term_info term;
gt_ui_info double_weight_ui;
gt_ui_info normal_weight_ui;

// UI characters get replacced with F7 - FF
//   on terminal use Extended ASCII
//   on windows/others use UNICODE

void main( void ) {
	printf("A");

	struct winsize w;

	ioctl(0, TIOCGWINSZ, &w);

	

	term.height = w.ws_row;
	term.width = w.ws_col;
	term.buffer = malloc( sizeof(char)*term.width*term.height );
	memset(term.buffer, 0x20, sizeof(char)*term.width*term.height);

	printf( "B" );

	normal_weight_ui.top_left = ui_corner_top_left;
	normal_weight_ui.top_right = ui_corner_top_right;
	normal_weight_ui.bottom_left = ui_corner_bottom_left;
	normal_weight_ui.bottom_right = ui_corner_bottom_right;
	normal_weight_ui.vertical_bar = ui_vertical_bar;
	normal_weight_ui.horizontal_bar = ui_horizontal_bar;
	normal_weight_ui.title_line_left = ui_title_line_left;
	normal_weight_ui.title_line_right = ui_title_line_right;

	//clear_screen();

	gt_grid * grid = malloc( sizeof(gt_grid) );
	grid = new_grid( grid, term.height, term.width );

	gt_window * window = malloc( sizeof(gt_window) );
	window->x = 0;
	window->y = 0;
	window->height = GT_MAX;
	window->width = GT_MAX;
	window->title = malloc( sizeof("Test Window") );
	strcpy( window->title, "Test Window" );

	//grid = add_to_grid( grid, 0, 0, new_window( window ) );
	//grid = add_to_grid( grid, 1, 1, new_window( window ) );
	//grid = add_to_grid( grid, 2, 1, new_window( window ) );
	gt_component * window_component = new_window( window );
	window_component->span_x = GT_GRID_WIDTH;
	window_component->span_y = GT_GRID_HEIGHT;
	grid = add_to_grid( grid, 0, 0, window_component );
	
	//gt_component * window_lower = new_window( window );
	//window_lower->span_x = GT_GRID_WIDTH;
	//grid = add_to_grid( grid, 0, 2, window_lower );

	gt_draw_grid( grid );

	/* gt_rect r;
	r.height = 15;
	r.width = 30;
	r.x_left = 5;
	r.y_top = 3;

	draw_window( &r, normal_weight, "Hello, world!" ); */

	refresh_screen();

	print_at_term( 0, term.height, "" );
	//printf( "x: %d, y: %d\n", term.width, term.height );
	//printf( "x: %d, y: %d\n", grid->width, grid->height );
}

void gt_draw_grid( gt_grid * grid ) {
	for( int x = 0; x < GT_GRID_WIDTH; x++ ) {
		for( int y = 0; y < GT_GRID_HEIGHT; y++ ) {
			if( grid->space[x][y] != NULL ) {
				gt_draw_component_at_grid( grid, x, y );
			}
		}
	}
}

void gt_draw_component_at_grid( gt_grid * grid, int x, int y ) {
	gt_component * component = grid->space[x][y];
	gt_rect r;

	switch( component->type ) {
		case GT_TYPE_WINDOW:
			switch( component->window->height ) {
				case GT_MAX:
					r.height = (component->span_y * grid->height);
					break;
				case GT_HALF:
					r.height = (grid->height * (component->span_y * grid->height))/2;
					break;
				case GT_THIRD:
					r.height = (grid->height * (component->span_y * grid->height))/3;
					break;
				case GT_QUARTER:
					r.height = (grid->height * (component->span_y * grid->height))/4;
					break;
				default:
					r.height = component->window->height;
			}

			switch( component->window->width ) {
				case GT_MAX:
					r.width = component->span_x * (grid->width);
					break;
				case GT_HALF:
					r.width = (grid->width + (component->span_x * grid->width))/2;
					break;
				case GT_THIRD:
					r.width = (grid->width + (component->span_x * grid->width))/3;
					break;
				case GT_QUARTER:
					r.width = (grid->width + (component->span_x * grid->width))/4;
					break;
				default:
					r.width = component->window->width;
			}

			r.x_left = (x) * (grid->width);
			r.y_top = (y) * (grid->height);
			
			//r.x_left++;
			r.y_top++;

			draw_window( &r, normal_weight, component->window->title );
			break;
	}
}

gt_component * new_window( gt_window * window ) {
	gt_component * component = malloc( sizeof( gt_component ) );

	window->type = GT_TYPE_WINDOW;

	component->type = GT_TYPE_WINDOW;
	component->children = NULL;
	component->window = window;
	component->span_x = 1;
	component->span_y = 1;

	return component;
}

gt_grid * add_to_grid( gt_grid * grid, int x, int y, gt_component * component ) {
	// Todo: bound checks

	grid->space[x][y] = component;

	return grid;
}

gt_grid * new_grid( gt_grid * grid, int screen_height, int screen_width ) {
	grid->height = (screen_height) / GT_GRID_HEIGHT;
	grid->width = (screen_width - 1) / GT_GRID_WIDTH;

	return grid;
}

gt_component * new_component( gt_component * component ) {
	component->children = malloc( sizeof(gt_component) * MAX_COMPONENTS );
	memset( component->children, 0, sizeof(gt_component) * MAX_COMPONENTS );

	return component;
}

void clear_screen( void ) {
	printf( "\e[2J" );
}

void refresh_screen( void ) {
	clear_screen();
	char line[ term.width - 1 ];

	for( int i = 0; i < term.height; i++ ) {
		memset( line, 0x20, term.width - 1);
		memcpy( line, term.buffer + (i * (term.width - 1)), term.width - 1);
		line[ term.width - 1 ] = 0;
		print_at_term( 0, i, line );
		//printf( "." );
	}
}

void print_at( int x, int y, char  * fmt, ... ) {
	va_list args_vsp;
	char str[term.width - 1];
	char *buff_loc;
	int length = 0;

	memset( str, 0, term.width - 1 );

	va_start( args_vsp, fmt );
	vsprintf( str, fmt, args_vsp );
	va_end( args_vsp );

	buff_loc = term.buffer + (y * (term.width - 1)) + x;
	length = strlen( str );

	//printf( "string %d: \"%x\"\n", length, (unsigned char)str[0]);
	
	memcpy( term.buffer + (y * (term.width - 1)) + x, str, length );
	//printf( "bl: \"%x\"\n", (unsigned char)buff_loc[0] );
}

void print_at_term( int x, int y, char * fmt, ... ) {
	va_list args;

	printf( "\e[%d;%dH", y, x);
	//printf( "y" );

	// do the string replace here
	// strreplace( fmt, '\xF7', unicode or term );

/* 	gt_str_replace( fmt, '\xF7', '1' );
	gt_str_replace( fmt, '\xF8', '2' );
	gt_str_replace( fmt, '\xF9', '3' );
	gt_str_replace( fmt, '\xFA', '4' );
	gt_str_replace( fmt, '\xFB', '5' );
	gt_str_replace( fmt, '\xFC', '6' );
	gt_str_replace( fmt, '\xFD', '7' );
	gt_str_replace( fmt, '\xFE', '8' );
	gt_str_replace( fmt, '\xFF', '9' ); */

	gt_str_replace( fmt, '\xF7', '1' );
	gt_str_replace( fmt, '\xF8', '2' );
	gt_str_replace( fmt, '\xF9', '3' );
	gt_str_replace( fmt, '\xFA', '4' );
	gt_str_replace( fmt, '\xFB', '5' );
	gt_str_replace( fmt, '\xFC', '6' );
	gt_str_replace( fmt, '\xFD', '7' );
	gt_str_replace( fmt, '\xFE', '8' );
	gt_str_replace( fmt, '\xFF', '9' );
	
	va_start( args, fmt );
	vprintf( fmt, args ); 
	va_end( args );
}

char * gt_str_replace( char * s, char find, char replace ) {
	for( ; s = strchr(s, find); ++s ) { 
        *s = replace; 
    }
}

gt_ui_info * get_weight( int weight ) {
	switch( weight ) {
		case double_weight:
			return (gt_ui_info *)&double_weight_ui;
			break;
		case normal_weight:
		default:
			return (gt_ui_info *)&normal_weight_ui;
	}
}

void draw_window( gt_rect * r, int weight, char * title ) {
	draw_box( r->x_left, r->y_top, r->width, r->height, weight );
	draw_title_line( r->x_left, r->y_top + 2, r->width, weight );
	print_at( r->x_left + 2, r->y_top + 1, title );
}

void draw_box( int x_left, int y_top, int width, int height, int weight ) {
	gt_ui_info * ui = get_weight( weight );

	for( int i = 0; i < width; i++ ) {
		print_at( x_left + i, y_top, ui->horizontal_bar );
		print_at( x_left + i, y_top + height, ui->horizontal_bar );
	}

	for( int i = 0; i < height; i++ ) {
		print_at( x_left, y_top + i, ui->vertical_bar );
		print_at( x_left + width, y_top + i, ui->vertical_bar );
	}

	print_at( x_left, y_top, ui->top_left );
	print_at( x_left + width, y_top, ui->top_right );
	print_at( x_left, y_top + height, ui->bottom_left );
	print_at( x_left + width, y_top + height, ui->bottom_right );
}

void draw_title_line( int x_left, int y_top, int width, int weight ) {
	gt_ui_info * ui = get_weight( weight );
	
	for( int i = 0; i < width; i++ ) {
		print_at( x_left + i, y_top, ui->horizontal_bar );
	}

	print_at( x_left, y_top, ui->title_line_left );
	print_at( x_left + width, y_top, ui->title_line_right );
}