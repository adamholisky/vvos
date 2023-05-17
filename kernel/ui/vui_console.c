#include <kernel.h>
#include <vui/vui.h>
#include <vui/console.h>
//-----
#include <vui/window.h>
#include <vui/label.h>

vui_console *vui_console_new( int x, int y, int width, int height ) {
	vui_console *console = vui_add_handle( VUI_TYPE_CONSOLE );

/* 	console->font_height = 14;
	console->font_width = 7; */

	console->font_height = 16;
	console->font_width = 8;

	console->cols = width / console->font_width;
	console->rows = height / console->font_height;

	console->x = x;
	console->y = y;
	console->width = width;
	console->height = height;

	console->current_x = 0;
	console->current_y = 0;

	console->foreground = CONSOLE_COLOR_WHITE;
	console->background = CONSOLE_COLOR_BLACK;
	console->transparent_text_background = false;

	console->data = malloc( sizeof(uint16_t) * console->cols * console->rows );
	memset( console->data, 0, sizeof(uint16_t) * console->cols * console->rows );

	klog( "Rows: %d    Cols: %d\n", console->rows, console->cols );

	return console;
}

void vui_console_destroy( vui_console *console ) {

}

bool vui_console_draw( vui_console *console ) {
	for( int i = 0; i < console->rows; i++ ) {
		int y_loc = console->y + (i * console->font_height);

		for( int j = 0; j < console->cols; j++ ) {
			int x_loc = console->x + (j * console->font_width);
			char to_draw[2];

			to_draw[0] = *(console->data + (i * console->cols) + j) & 0x00FF;
			to_draw[1] = 0;

			int fg = 0;
			int bg = 0;

			switch( (*(console->data + (i * console->cols) + j) & 0xF000)>>12 ) {
				case CONSOLE_COLOR_BLACK:
					bg = CONSOLE_COLOR_RGB_BLACK;
					break;
				case CONSOLE_COLOR_BLUE:
					bg = CONSOLE_COLOR_RGB_BLUE;
					break;
				case CONSOLE_COLOR_GREEN:
					bg = CONSOLE_COLOR_RGB_GREEN;
					break;
				case CONSOLE_COLOR_CYAN:
					bg = CONSOLE_COLOR_RGB_CYAN;
					break;
				case CONSOLE_COLOR_RED:
					bg = CONSOLE_COLOR_RGB_RED;
					break;
				case CONSOLE_COLOR_MAGENTA:
					bg = CONSOLE_COLOR_RGB_MAGENTA;
					break;
				case CONSOLE_COLOR_BROWN:
					bg = CONSOLE_COLOR_RGB_BROWN;
					break;
				case CONSOLE_COLOR_LIGHT_GREY:
					bg = CONSOLE_COLOR_RGB_LIGHT_GREY;
					break;
				case CONSOLE_COLOR_DARK_GREY:
					bg = CONSOLE_COLOR_RGB_DARK_GREY;
					break;
				case CONSOLE_COLOR_LIGHT_BLUE:
					bg = CONSOLE_COLOR_RGB_LIGHT_BLUE;
					break;
				case CONSOLE_COLOR_LIGHT_GREEN:
					bg = CONSOLE_COLOR_RGB_LIGHT_GREEN;
					break;
				case CONSOLE_COLOR_LIGHT_CYAN:
					bg = CONSOLE_COLOR_RGB_LIGHT_CYAN;
					break;
				case CONSOLE_COLOR_LIGHT_RED:
					bg = CONSOLE_COLOR_RGB_LIGHT_RED;
					break;
				case CONSOLE_COLOR_LIGHT_MAGENTA:
					bg = CONSOLE_COLOR_RGB_LIGHT_MAGENTA;
					break;
				case CONSOLE_COLOR_YELLOW:
					bg = CONSOLE_COLOR_RGB_YELLOW;
					break;
				case CONSOLE_COLOR_WHITE:
					bg = CONSOLE_COLOR_RGB_WHITE;
					break;
				default:
					bg = CONSOLE_COLOR_RGB_BLACK;
			}

			switch( (*(console->data + (i * console->cols) + j) & 0x0F00)>>8 ) {
				case CONSOLE_COLOR_BLACK:
					fg = CONSOLE_COLOR_RGB_BLACK;
					break;
				case CONSOLE_COLOR_BLUE:
					fg = CONSOLE_COLOR_RGB_BLUE;
					break;
				case CONSOLE_COLOR_GREEN:
					fg = CONSOLE_COLOR_RGB_GREEN;
					break;
				case CONSOLE_COLOR_CYAN:
					fg = CONSOLE_COLOR_RGB_CYAN;
					break;
				case CONSOLE_COLOR_RED:
					fg = CONSOLE_COLOR_RGB_RED;
					break;
				case CONSOLE_COLOR_MAGENTA:
					fg = CONSOLE_COLOR_RGB_MAGENTA;
					break;
				case CONSOLE_COLOR_BROWN:
					fg = CONSOLE_COLOR_RGB_BROWN;
					break;
				case CONSOLE_COLOR_LIGHT_GREY:
					fg = CONSOLE_COLOR_RGB_LIGHT_GREY;
					break;
				case CONSOLE_COLOR_DARK_GREY:
					fg = CONSOLE_COLOR_RGB_DARK_GREY;
					break;
				case CONSOLE_COLOR_LIGHT_BLUE:
					fg = CONSOLE_COLOR_RGB_LIGHT_BLUE;
					break;
				case CONSOLE_COLOR_LIGHT_GREEN:
					fg = CONSOLE_COLOR_RGB_LIGHT_GREEN;
					break;
				case CONSOLE_COLOR_LIGHT_CYAN:
					fg = CONSOLE_COLOR_RGB_LIGHT_CYAN;
					break;
				case CONSOLE_COLOR_LIGHT_RED:
					fg = CONSOLE_COLOR_RGB_LIGHT_RED;
					break;
				case CONSOLE_COLOR_LIGHT_MAGENTA:
					fg = CONSOLE_COLOR_RGB_LIGHT_MAGENTA;
					break;
				case CONSOLE_COLOR_YELLOW:
					fg = CONSOLE_COLOR_RGB_YELLOW;
					break;
				case CONSOLE_COLOR_WHITE:
					fg = CONSOLE_COLOR_RGB_WHITE;
					break;
				default:
					fg = CONSOLE_COLOR_RGB_WHITE;
			}
			
			if( console->transparent_text_background ) {
				vui_draw_string_immediate( x_loc , y_loc, console->font_height, 0xFF000000 | bg, 0xFF000000 | fg, VUI_FONT_FIRACODE, to_draw );
			} else {
				vui_draw_string_immediate( x_loc , y_loc, console->font_height, 0xFF000000 | bg, 0xFF000000 | fg, VUI_FONT_FIRACODE, to_draw );
			}
			
		}
	}

	//framebuffer_copy_to_frontbuffer( console->x, console->y, console->width, console->height );
	
}

bool vui_console_draw_x_y( vui_console *console, int x, int y ) {
	int y_loc = console->y + (y * console->font_height);

	int x_loc = console->x + (x * console->font_width);
	char to_draw[2];

	to_draw[0] = *(console->data + (y * console->cols) + x) & 0x00FF;
	to_draw[1] = 0;

	int fg = 0;
	int bg = 0;

	switch( (*(console->data + (y * console->cols) + x) & 0xF000)>>12 ) {
		case CONSOLE_COLOR_BLACK:
			bg = CONSOLE_COLOR_RGB_BLACK;
			break;
		case CONSOLE_COLOR_BLUE:
			bg = CONSOLE_COLOR_RGB_BLUE;
			break;
		case CONSOLE_COLOR_GREEN:
			bg = CONSOLE_COLOR_RGB_GREEN;
			break;
		case CONSOLE_COLOR_CYAN:
			bg = CONSOLE_COLOR_RGB_CYAN;
			break;
		case CONSOLE_COLOR_RED:
			bg = CONSOLE_COLOR_RGB_RED;
			break;
		case CONSOLE_COLOR_MAGENTA:
			bg = CONSOLE_COLOR_RGB_MAGENTA;
			break;
		case CONSOLE_COLOR_BROWN:
			bg = CONSOLE_COLOR_RGB_BROWN;
			break;
		case CONSOLE_COLOR_LIGHT_GREY:
			bg = CONSOLE_COLOR_RGB_LIGHT_GREY;
			break;
		case CONSOLE_COLOR_DARK_GREY:
			bg = CONSOLE_COLOR_RGB_DARK_GREY;
			break;
		case CONSOLE_COLOR_LIGHT_BLUE:
			bg = CONSOLE_COLOR_RGB_LIGHT_BLUE;
			break;
		case CONSOLE_COLOR_LIGHT_GREEN:
			bg = CONSOLE_COLOR_RGB_LIGHT_GREEN;
			break;
		case CONSOLE_COLOR_LIGHT_CYAN:
			bg = CONSOLE_COLOR_RGB_LIGHT_CYAN;
			break;
		case CONSOLE_COLOR_LIGHT_RED:
			bg = CONSOLE_COLOR_RGB_LIGHT_RED;
			break;
		case CONSOLE_COLOR_LIGHT_MAGENTA:
			bg = CONSOLE_COLOR_RGB_LIGHT_MAGENTA;
			break;
		case CONSOLE_COLOR_YELLOW:
			bg = CONSOLE_COLOR_RGB_YELLOW;
			break;
		case CONSOLE_COLOR_WHITE:
			bg = CONSOLE_COLOR_RGB_WHITE;
			break;
		default:
			bg = CONSOLE_COLOR_RGB_BLACK;
	}

	switch( (*(console->data + (y * console->cols) + x) & 0x0F00)>>8 ) {
		case CONSOLE_COLOR_BLACK:
			fg = CONSOLE_COLOR_RGB_BLACK;
			break;
		case CONSOLE_COLOR_BLUE:
			fg = CONSOLE_COLOR_RGB_BLUE;
			break;
		case CONSOLE_COLOR_GREEN:
			fg = CONSOLE_COLOR_RGB_GREEN;
			break;
		case CONSOLE_COLOR_CYAN:
			fg = CONSOLE_COLOR_RGB_CYAN;
			break;
		case CONSOLE_COLOR_RED:
			fg = CONSOLE_COLOR_RGB_RED;
			break;
		case CONSOLE_COLOR_MAGENTA:
			fg = CONSOLE_COLOR_RGB_MAGENTA;
			break;
		case CONSOLE_COLOR_BROWN:
			fg = CONSOLE_COLOR_RGB_BROWN;
			break;
		case CONSOLE_COLOR_LIGHT_GREY:
			fg = CONSOLE_COLOR_RGB_LIGHT_GREY;
			break;
		case CONSOLE_COLOR_DARK_GREY:
			fg = CONSOLE_COLOR_RGB_DARK_GREY;
			break;
		case CONSOLE_COLOR_LIGHT_BLUE:
			fg = CONSOLE_COLOR_RGB_LIGHT_BLUE;
			break;
		case CONSOLE_COLOR_LIGHT_GREEN:
			fg = CONSOLE_COLOR_RGB_LIGHT_GREEN;
			break;
		case CONSOLE_COLOR_LIGHT_CYAN:
			fg = CONSOLE_COLOR_RGB_LIGHT_CYAN;
			break;
		case CONSOLE_COLOR_LIGHT_RED:
			fg = CONSOLE_COLOR_RGB_LIGHT_RED;
			break;
		case CONSOLE_COLOR_LIGHT_MAGENTA:
			fg = CONSOLE_COLOR_RGB_LIGHT_MAGENTA;
			break;
		case CONSOLE_COLOR_YELLOW:
			fg = CONSOLE_COLOR_RGB_YELLOW;
			break;
		case CONSOLE_COLOR_WHITE:
			fg = CONSOLE_COLOR_RGB_WHITE;
			break;
		default:
			fg = CONSOLE_COLOR_RGB_BLACK;
	}

	if( console->transparent_text_background ) {
		vui_draw_string_immediate( x_loc , y_loc, console->font_height, 0xFF000000 | bg, 0xFF000000 | fg, VUI_FONT_FIRACODE, to_draw );
	} else {
		vui_draw_string_immediate( x_loc , y_loc, console->font_height, 0xFF000000 | bg, 0xFF000000 | fg, VUI_FONT_FIRACODE, to_draw );
	}

	
}


void vui_console_clear( vui_console *console ) {
	for( int i = 0; i < console->rows * console->cols; i++ ) {
		*(console->data + i) = ( console->background<<12 | console->foreground<<8 | ' ');
	}
}

int vui_console_putc_at( vui_console *console, int x, int y, char c ) {

}

#undef KDEBUG_VUI_CONSOLE_PUTC
/** 
 * Puts a character into the console's current r,c position
 * 
 * returns number of characters put (should be 1)
*/
int vui_console_putc( vui_console *console, char c ) {
	vui_console_clear_cursor( console );

	if( c == '\x1b' ) {
		console->capturing_escape_code = true;
		console->capture_num = 0;
	} else if( console->capturing_escape_code ) {
		if( c == 'm' ) {
			console->captured_escape[ console->capture_num ] = 0;
			console->capturing_escape_code = false;
			console->capture_num = 0;

			// char 3 & 4 = fg
			// char 6 & 7 = bg
			int captured_fg = ((console->captured_escape[3] - '0') * 10) + (console->captured_escape[4] - '0');
			int captured_bg = ((console->captured_escape[6] - '0') * 10) + (console->captured_escape[7] - '0');

			int new_fg = 0;
			int new_bg = 0;

			switch( captured_fg ) {
				case 30:
					new_fg = CONSOLE_COLOR_BLACK;
					break;
				case 31:
					new_fg = CONSOLE_COLOR_RED;
					break;
				case 32:
					new_fg = CONSOLE_COLOR_GREEN;
					break;
				case 33:
					new_fg = CONSOLE_COLOR_BROWN;
					break;
				case 34:
					new_fg = CONSOLE_COLOR_BLUE;
					break;
				case 37:
				case 39:
				case 0:
				default:
					new_fg = CONSOLE_COLOR_WHITE;
			}

			switch( captured_bg ) {
				case 40:
					new_bg = CONSOLE_COLOR_BLACK;
					break;
				case 41:
					new_bg = CONSOLE_COLOR_RED;
					break;
				case 42:
					new_bg = CONSOLE_COLOR_GREEN;
					break;
				case 44:
					new_bg = CONSOLE_COLOR_BLUE;
					break;
				case 47:
				case 49:
				case 0:
				default:
					new_bg = CONSOLE_COLOR_BLACK;
			}

			console->foreground = new_fg;
			console->background = new_bg;
		} else {
			console->captured_escape[ console->capture_num ] = c;
			console->capture_num++;
		}
	} else {
		switch( c ) {
			case '\t':
				int new_x = console->current_x + (console->current_x % console->tab_size);

				if( new_x > console->cols ) {
					vui_console_putc( console, '\n' );
					console->current_x = console->cols - new_x;
				} else {
					console->current_x = new_x;
				}

				break;
			case '\n':
				console->current_x = 0;

				if( console->current_y == console->rows - 1 ) {
					for( int i = 1; i < console->rows; i++ ) {
						memcpy( 
							(uint8_t *)(console->data + ((i-1) * console->cols)),
							(uint8_t *)(console->data + (i * console->cols)),
							console->cols * 2   
						);
					}

					for( int i = 0; i < console->cols; i++ ) {
						*(console->data + ((console->cols) * (console->rows - 1) + i)) = 0x0020;
					}

					//0. fill in empty space becaue wtf
					framebuffer_fill_rect_in_backbuffer( console->x, console->y - 5, console->width, 5, VUI_WINDOW_COLOR_INNERWINDOW | 0xFF000000 );

					//1. move back buffer rect
					framebuffer_move_rect_in_backbuffer( console->x, console->y + console->font_height - 1, console->width, console->height - console->font_height + 1, console->x, console->y );

					//2. fill in last line 
					framebuffer_fill_rect_in_backbuffer( console->x, console->y + (console->font_height * (console->rows)) - 6, console->width, console->font_height + 6, VUI_WINDOW_COLOR_INNERWINDOW | 0xFF000000 );
					
					//3. copy back buffer area to front buffer
					rect r = {
						.x = console->x,
						.y = console->y - 5,
						.w = console->width,
						.h = console->height + 5
					};

					vga_draw_screen_box( &r );

					console->current_y--;

					//vui_console_draw( console );
				} else {
					console->current_y++;
				}

				break;
			default:
				vui_console_putc_color( console, c, console->background, console->foreground );

				vui_console_draw_x_y( console, console->current_x, console->current_y );

				console->current_x++;
				
				if( console->current_x == console->cols ) {
					vui_console_putc( console, '\n' );
				}
		}
	}

	vui_console_update_cursor( console );

	#ifdef KDEBUG_VUI_CONSOLE_PUTC
	klog( "cur_x: %d, cur_y: %d, rows: %d, cols: %d\n", console->current_x, console->current_y, console->rows, console->cols );
	#endif

	// THIS WORKS, BUT IS SLOW
	// vui_console_draw( console );
}

int vui_console_putc_color( vui_console * console, char c, int bg, int fg ) {
	uint16_t *chars = console->data;

	*(chars + (console->current_x) + (console->current_y * console->cols) ) = ( console->background<<12 | console->foreground<<8 | c);
	
	return 1;
}

/**
 * @brief Puts a string into the console's current r,c position
 * 
 * @param s null terminated string
 * @return int number of characters put
 */
int vui_console_puts( vui_console *console, char *s ) {
	int i = 0;

	while( *s ) {
		vui_console_putc( console, *s );
		s++;
		i++;
	}

	return i;
}

/**
 * @brief 
 * 
 * @param console 
 * @return char 
 */
char vui_console_getchar( vui_console *console ) {
	
}

void vui_console_flip_cursor( vui_console *console ) {
	if( console->cursor_shown == true ) {
		vui_console_clear_cursor( console );
	} else {
		vui_console_update_cursor( console );
	}
}

void vui_console_clear_cursor( vui_console *console ) {
	/* uint16_t *chars = console->data;

	*(chars + (console->current_x) + (console->current_y * console->cols) ) = ( console->background<<12 | console->foreground<<8 | ' ');

	vui_console_draw_x_y( console, console->current_x, console->current_y );

	console->cursor_shown = false; */
}

void vui_console_update_cursor( vui_console *console ) {
	/* uint16_t *chars = console->data;

	*(chars + (console->current_x) + (console->current_y * console->cols) ) = ( console->background<<12 | console->foreground<<8 | '|');

	vui_console_draw_x_y( console, console->current_x, console->current_y );

	console->cursor_shown = true; */
}