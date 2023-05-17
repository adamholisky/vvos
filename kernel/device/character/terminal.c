#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <vga.h>
#include "terminal.h"
#include "bootstrap.h"
#include "serial.h"
#include "string.h"
#include "memory.h"
#include <debug.h>
#include <kmalloc.h>
#include <globals.h>
#include <device.h>

/* unsigned int term_current_row;
unsigned int term_current_column;
unsigned char term_current_color;
unsigned short * term_buffer; */

putc_redirect_func redirect_putc;

/* char ansi_capture[25];
bool term_capture_ansi_escape_code = false;
uint32_t capture_i = 0;

unsigned int VGA_WIDTH = 80;
unsigned int VGA_HEIGHT = 25;
unsigned int y_start; */

bool is_debug_output = false;
bool is_gui_active = false;

/* typedef struct {
		rect			box;

		unsigned int 	width;
		unsigned int 	height;

		unsigned int 	fg_color;
		unsigned int 	bg_color;

		char 			*buffer;

		unsigned int 	current_x;
		unsigned int 	current_y;

		unsigned int 	prev_x;
		unsigned int 	prev_y;

		bool 			waiting_on_input;
		bool 			can_update_cursor;
} console;
 */

//device *term_device;
//console *main_console;

void term_initalize( void ) {
	// DO NOT PUT KLOG FUNCTIONS HERE
	redirect_putc = NULL;

	/* unsigned int x = 0;
	unsigned int y = 0;

	
	term_current_row = 0;
	term_current_column = 0;
	y_start = 0;
	term_current_color = vga_entry_color( VGA_COLOR_WHITE, VGA_COLOR_BLACK );
	term_buffer = ( unsigned short * )( 0xC00B8000 );

	for( y = 0; y < VGA_HEIGHT; y++ ) {
		for( x = 0; x < VGA_WIDTH; x++ ) {
			term_buffer[( y * VGA_WIDTH ) + x] = vga_entry( ' ', term_current_color );
		}
	} */

	/* term_device = device_add_new();
	term_device->init = terminal_device_init;
	term_device->write = terminal_device_write;
	term_device->read = terminal_device_read;
	strcpy( term_device->name, "Terminal" );
	strcpy( term_device->file, "/dev/tty0" ); */
}

/* void terminal_device_init( void ) {
	// Intentionally blank
}

int terminal_device_write( uint8_t *buff, uint32_t size ) {
	if( size > 1 ) {
		term_put_string( (char *)buff, size );
	} else {
		term_put_char( (char)*buff );
	}

	return size;
}

int terminal_device_read( uint8_t *buff, uint32_t size ) {
	return 0;
}

void term_set_color( uint32_t foreground, uint32_t background ) {
	term_current_color = vga_entry_color( VGA_COLOR_RED, VGA_COLOR_BLUE );
}

void term_put_char_at( char c, unsigned char color, unsigned int x, unsigned int y ) {
	color = term_current_color;

	if( c != '\n' ) {
		term_buffer[( y * VGA_WIDTH ) + x] = vga_entry( c, color );
	}
} */

void set_terminal_redirect( putc_redirect_func func ) {
	redirect_putc = func;
}

void term_put_char( char c ) {
	unsigned int x;
	unsigned int y;
	unsigned int max_row;
	unsigned short fg, bg;
	bool send_to_com4 = false;		// saved file out
	bool send_to_com2 = false;		// emulator's errout
	bool send_to_screen = true;	// OS text or graphics console	

	if( GRAPHICS_ACTIVE == true ) {
		if( is_debug_output == true ) {
			send_to_com4 = true;
			send_to_com2 = true;
			send_to_screen = false;
		} else {
			if( redirect_putc != NULL ) {
				redirect_putc(c);
				return;
			}
		}
	} else {
		send_to_screen = false;

		if( is_debug_output == true ) {
			send_to_com4 = true;
		} else {
			send_to_com2 = true;
		}
	}

	/* if( c == '\x1b' ) {
		term_capture_ansi_escape_code = true;
		capture_i = 0;
		send_to_com4 = false;
		send_to_screen = false;
	} else if( term_capture_ansi_escape_code ) {
		if( c == 'm' ) {
			ansi_capture[capture_i] = 0;
			term_capture_ansi_escape_code = false;
			capture_i = 0;

			// Process escape seq
			// char 3 & 4 = fg
			// char 6 & 7 = bg
			fg = ((ansi_capture[3] - '0') * 10) + (ansi_capture[4] - '0');
			bg = ((ansi_capture[6] - '0') * 10) + (ansi_capture[7] - '0');

			switch( fg ) {
				case 30:
					fg = VGA_COLOR_BLACK;
					break;
				case 31:
					fg = VGA_COLOR_RED;
					main_console->fg_color = 0x00FF0000;
					break;
				case 32:
					fg = VGA_COLOR_GREEN;
					break;
				case 33:
					fg = VGA_COLOR_LIGHT_BROWN;
					break;
				case 34:
					fg = VGA_COLOR_BLUE;
					break;
				case 37:
				case 39:
				case 0:
				default:
					main_console->fg_color = 0x00444444;
					fg = VGA_COLOR_WHITE;
			}

			switch( bg ) {
				case 40:
					bg = VGA_COLOR_BLACK;
					break;
				case 41:
					bg = VGA_COLOR_RED;
					break;
				case 42:
					bg = VGA_COLOR_GREEN;
					break;
				case 44:
					bg = VGA_COLOR_BLUE;
					break;
				case 47:
				case 49:
				case 0:
				default:
					bg = VGA_COLOR_BLACK;
			}

			term_current_color = vga_entry_color( fg, bg );
			send_to_com4 = false;
			send_to_screen = false;
		} else {
			ansi_capture[capture_i] = c;
			capture_i++;
			send_to_com4 = false;
			send_to_screen = false;
		}
	} */

	if( send_to_com4 ) {
		serial_write_port( c, COM4 );
	}

	if( send_to_com2 ) {
		serial_write_port( c, COM2 );
	}

/* 	if( send_to_screen ) {
		if( is_gui_active ) {
			console_put_char( c );
			return;
		}

		if( c != '\n' ) {
			term_put_char_at( c, term_current_color, term_current_column, term_current_row );

			if( is_gui_active ) {
				console_put_char_at( c, term_current_column, term_current_row );
			}
		} else {
			term_current_column = VGA_WIDTH - 1;
		}

		term_current_column++;

		if( term_current_column == VGA_WIDTH ) {
			term_current_column = 0;

			term_current_row++;

			max_row = VGA_HEIGHT;

			if( term_current_row == max_row ) {
				term_current_row = max_row - 1;

				for( y = y_start; y < max_row - 1; y++ ) {
					for( x = 0; x < VGA_WIDTH; x++ ) {
						term_buffer[( y * VGA_WIDTH ) + x] = term_buffer[( ( y + 1 ) * VGA_WIDTH ) + x];
					}

					if( is_gui_active ) {
						//console_move_row( y, y + 1 );
					}
				}

				for( x = 0; x < VGA_WIDTH; x++ ) {
					const size_t index = ( VGA_HEIGHT - 1 ) * VGA_WIDTH + x;
					term_buffer[index] = vga_entry( ' ', term_current_color );
					 if( is_gui_active ) {
						console_put_char_at( ' ', x, VGA_HEIGHT - 1 );
					} 
				}
			}
		}
	} */
}

/* void term_clear_last_char( void ) {
	int term_new_column = term_current_column - 1;

	//printf( "cc: %d --> nc: %d", term_current_column, term_new_column );
	if( term_new_column > 0 ) {
		term_put_char_at( ' ', term_current_color, term_new_column, term_current_row );

		term_current_column = term_new_column;
	}

	//update_cursor( term_current_row, term_current_column );
} */

/* Outputs a string data of size the terminal
 */
void term_put_string( const char * data, size_t size ) {
	for( size_t i = 0; i < size; i++ ) {
		term_put_char( data[i] );
	}
}

/* Calculates the size of *data and outputs the string *data
 */
/* void term_write_string( const char * data ) {
	term_put_string( data, strlen( data ) );
} */

void set_debug_output( bool d ) {
	is_debug_output = d;
}

void write_to_serial_port( char c ) {
	serial_write( c );
}

/* /////


void console_init( char * name, unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int bg_color, unsigned int fg_color ) {
	unsigned int _x, _y;

	main_console = kmalloc( sizeof(console) );

	main_console->box.x = x;
	main_console->box.y = y;
	main_console->box.w = width;
	main_console->box.h = height;

	main_console->width = main_console->box.w / 7;
	main_console->height = main_console->box.h / 14;

	main_console->bg_color = bg_color;
	main_console->fg_color = fg_color;

	main_console->current_x = 0;
	main_console->current_y = 0;
	main_console->prev_x = 0;
	main_console->prev_y = 0;

	main_console->waiting_on_input = false;

	//printf( "w h: %d %d\n", con->width, con->height );

	main_console->buffer = (char *)kmalloc( sizeof(char) * main_console->width * main_console->height );

	for( _y = 0; _y < main_console->height; _y++ ) {
		for( _x = 0; _x < main_console->width; _x++ ) {
			main_console->buffer[ (_y * main_console->width) + _x ] = ' ';
		}
	}

	is_gui_active = true;
}

void console_draw( void ) {
	unsigned int x;
	unsigned int y;
	vga_information * vga = vga_get_info();

	draw_rect( main_console->box, main_console->bg_color );

	for( y = 0; y < main_console->height; y++ ) {
		for( x = 0; x < main_console->width; x++ ) {
			draw_char( vga->buffer, main_console->box.x + (x * vga->char_width), main_console->box.y + (y * vga->char_height), main_console->fg_color, main_console->bg_color, main_console->buffer[ (y * main_console->width) + x ] );
		}
	}

	vga_draw_screen();
}

void console_put_char_at( char c, unsigned int x, unsigned int y ) {
	vga_information * vga = vga_get_info();
	//klog( "%c %d %d\n", c, x, y );

	main_console->buffer[ x + (main_console->width * y) ] = c;
	draw_char( vga->buffer, main_console->box.x + (x * vga->char_width), main_console->box.y + (y * vga->char_height), main_console->fg_color, main_console->bg_color, c );
	draw_char( vga->fbuffer, main_console->box.x + (x * vga->char_width), main_console->box.y + (y * vga->char_height), main_console->fg_color, main_console->bg_color, c );
}

void console_put_char( char c ) {
	unsigned int x;
	unsigned int y;
	bool continue_tabbing = true;

	if( c == '\n' ) {
		console_put_char_at( ' ', main_console->current_x, main_console->current_y );
		main_console->current_x = main_console->width - 1;
	} else if( c == '\t' ) {
		main_console->can_update_cursor = false;

		while( main_console->current_x % 8 ) {
			if( continue_tabbing ) {
				if( main_console->current_x != main_console->width ) {
					console_put_char( ' ' );
				} else {
					continue_tabbing = false;
				}
			}
		}
		
		main_console->can_update_cursor = true;
	} else {
		console_put_char_at( c, main_console->current_x, main_console->current_y );
	}

	main_console->current_x++;

	if( main_console->current_x == main_console->width ) {
		main_console->current_x = 0;

		main_console->current_y++;

		if( main_console->current_y == main_console->height ) {
			main_console->current_y--;

			for( y = 0; y < main_console->height - 1; y++ ) {
				for( x = 0; x < main_console->width; x++ ) {
					main_console->buffer[ (y * main_console->width) + x ] = main_console->buffer[ ((y + 1) * main_console->width) + x ];
				}

				//console_move_row( y, y + 1 );
			}

			for( x = 0; x < main_console->width; x++ ) {
				main_console->buffer[ ((main_console->height - 1) * main_console->width) + x ] = ' ';
			}

			console_draw();
		}
	}

	if( main_console->can_update_cursor ) {
		//gui_update_carrot();
	}
}

void console_move_row( unsigned int _dest, unsigned int _src ) {
	rect src;
	rect dest;

	src.x = main_console->box.x;
	src.y = main_console->box.y + (_src * 17);
	src.w = main_console->box.w;
	src.h = 17;

	dest.x = main_console->box.x;
	dest.y = main_console->box.y + (_dest * 17);
	dest.w = main_console->box.w;
	dest.h = 17;

	move_rect( dest, src );

	console_draw();
} */

void console_scroll_forever_test( void ) {
	for( int i = 0; i < INT_MAX; i++ ) {
		if( i % 2 ) {
			printf( "odd\n" );
		} else {
			printf( "even %d %d %d %d %d %d\n", i, i, i, i, i ,i );
		}
	}
}