#include <kernel.h>
#include <multiboot.h>
#include <stdint.h>
#include <vga.h>
#include <tamzen.h>

extern void asm_refresh_cr3( void );

uint32_t char_width;
uint32_t char_height;
uint32_t screen_border;
uint32_t fg_color;
uint32_t bg_color;
bool 	 render_background;
uint32_t *bg_mask;
bool	 font_smoothing_active;
uint8_t  *fb_buffer;

vga_information vga_info;

vesa_info_block *vesa_info;

vga_information * vga_get_info( void ) {
	return &vga_info;
}

void vga_initalize( void ) {
	multiboot_info_t *mb = get_multiboot_header();
	uint32_t fb_addr = (uint32_t)mb->framebuffer_addr;

	klog( "fb_addr: 0x%08X\n", fb_addr );


	vga_info.pitch = mb->framebuffer_pitch;
	vga_info.width = mb->framebuffer_width;
	vga_info.height = mb->framebuffer_height;
	vga_info.bpp = mb->framebuffer_bpp;
	vga_info.pixel_width = vga_info.pitch / vga_info.width;
	vga_info.fbuffer = (uint8_t *)0xE0000000;
	font_smoothing_active = true;

	klog( "pitch:   0x%X (%d)\n", vga_info.pitch, vga_info.pitch );
	klog( "width:   %d\n", vga_info.width );
	klog( "height:  %d\n", vga_info.height );

	int i;
	uint32_t virt = 0;
	uint32_t phys = 0;
	
	uint32_t pages_needed = ((vga_info.pitch * vga_info.height)/PAGE_SIZE_IN_BYTES);
	(vga_info.pitch * vga_info.height)-(pages_needed * PAGE_SIZE_IN_BYTES) != 0 ? pages_needed++ : 0;

	for( i = 0; i < pages_needed; i++ ) {
		virt = 0xE0000000 + (i * PAGE_SIZE_IN_BYTES);
		phys = fb_addr + (i * PAGE_SIZE_IN_BYTES);
		page_map( (uint32_t *)virt, (uint32_t *)phys );

		//klog( "VGA Mapped: 0x%08X -> 0x%08X\n", virt, phys );
	}

	asm_refresh_cr3();

	klog( "VGA Mapped %d pages.\n", i );

	vga_info.buffer = kmalloc( sizeof(uint8_t) * VVOS_SCREEN_WIDTH * VVOS_SCREEN_HEIGHT * 4 );

	klog( "fb_buffer: 	    0x%08X\n", vga_info.buffer );
	klog( "fb_buffer end:   0x%08X\n", ((uint32_t)vga_info.buffer + (VVOS_SCREEN_WIDTH*VVOS_SCREEN_HEIGHT*4)) );

	*vga_info.buffer = 0xBA;
	klog( "*fb_buffer:      0x%X\n", *vga_info.buffer );

	*(vga_info.buffer + (VVOS_SCREEN_WIDTH*VVOS_SCREEN_HEIGHT*4) ) = 0xDA;
	klog( "*fb_buffer end:  0x%X\n", *(vga_info.buffer + (VVOS_SCREEN_WIDTH*VVOS_SCREEN_HEIGHT*4) ));

	//If we didn't pagefault, yay! paging is really working then...
	
	memset( vga_info.buffer, 0, VVOS_SCREEN_WIDTH*VVOS_SCREEN_HEIGHT*4 );


	//dump_active_pt();

	vesa_info = (vesa_info_block *)(mb->vbe_control_info + KERNEL_VIRT_LOAD_BASE);
	klog( "VESA control: %c%c%c%c\n", vesa_info->VbeSignature[0], vesa_info->VbeSignature[1], vesa_info->VbeSignature[2], vesa_info->VbeSignature[3]);

	vesa_mode_info *vesa_mode = (vesa_mode_info *)(mb->vbe_mode_info + KERNEL_VIRT_LOAD_BASE );
	klog( "VESA fb: 0x%08X\n", vesa_mode->framebuffer );

	klog( "fbuffer: 0x%08X\n", vga_info.fbuffer );

	vga_info.char_height = 14;
	vga_info.char_width = 7;
	vga_info.fg_color = 0x00000000;
	//vga_info.bg_color = 0x00262A30;
	vga_info.bg_color = 0x00333399;

	char_height = 14;
	char_width = 7;
	screen_border = 10;

	bg_color = vga_info.bg_color;
	fg_color = 0x00000000;
	render_background = true;

	bg_mask = kmalloc( sizeof(int) * 14 * 7 );

	fillrect( vga_info.buffer, vga_info.bg_color, 0, 0, VVOS_SCREEN_WIDTH, VVOS_SCREEN_HEIGHT );
	klog( "Drawing first screen.\n" );
	vga_draw_screen();

	//draw_string( "Hello, world!", 5, 5, fg_color, bg_color );

	//putpixel( vga_info.fbuffer, 10, 10, 0x00FF0000 );

	klog( "exit\n" );
}

inline void vga_draw_screen( void ) {
	memcpy( vga_info.fbuffer, vga_info.buffer, VVOS_SCREEN_WIDTH*VVOS_SCREEN_HEIGHT*4 );
}

void framebuffer_copy_to_frontbuffer( int x, int y, int w, int h ) {
	int size = w * 4;

	for( int y_offset = y; y_offset < (h + y); y_offset++ ) {
		int offset = (y_offset * VVOS_SCREEN_WIDTH * 4) + (x * 4);
		uint8_t *front_buffer = vga_info.fbuffer + offset;
		uint8_t *back_buffer = vga_info.buffer + offset;

		//klog( "front: %08X, back: %08X\n", front_buffer, back_buffer );
		memcpy( front_buffer, back_buffer, size );
	}
}

void framebuffer_move_rect_in_backbuffer( int from_x, int from_y, int w, int h, int to_x, int to_y ) {
	int size = w * 4;

	for( int i = 0; i < h; i++ ) {
		int offset_to = ((to_y + i) * VVOS_SCREEN_WIDTH * 4) + (to_x * 4);
		int offset_from = ((from_y + i) * VVOS_SCREEN_WIDTH * 4) + (from_x * 4);
		uint8_t *back_buffer_to = vga_info.buffer + offset_to;
		uint8_t *back_buffer_from = vga_info.buffer + offset_from;

		//klog( "front: %08X, back: %08X\n", front_buffer, back_buffer );
		memcpy( back_buffer_to, back_buffer_from, size );
	}
}

void framebuffer_fill_rect_in_backbuffer( int x, int y, int w, int h, uint32_t color ) {
	fillrect( vga_info.buffer, color, x, y, w, h );
}

void vga_draw_screen_box( rect *r ) {
	//klog( "Redrawing area: (%d, %d) for w: %d, h: %d\n", r->x, r->y, r->w, r->h );

	int size = r->w * 4;

	for( int y_offset = r->y; y_offset < (r->h + r->y); y_offset++ ) {
		int offset = (y_offset * VVOS_SCREEN_WIDTH * 4) + (r->x * 4);
		uint8_t *front_buffer = vga_info.fbuffer + offset;
		uint8_t *back_buffer = vga_info.buffer + offset;

		//klog( "front: %08X, back: %08X\n", front_buffer, back_buffer );
		memcpy( front_buffer, back_buffer, size );
	}
}

inline void fillrect( uint8_t * buffer, uint32_t color, unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
	uint8_t * where = (buffer + (x * vga_info.pixel_width) + (y * vga_info.pitch ));
	unsigned int i, j;
	
	//write_to_serial_port( ',' );

	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			*(uint32_t *)(where + (j * vga_info.pixel_width) ) = color;
			//debug_f( "%d ", j );
		}
		where += vga_info.pitch;
		//debug_f( "%d\n", i );

		//klog("%d\n", i);
	}
} 

void put_pixel( unsigned int x, unsigned int y, int color ) {
	putpixel( vga_info.buffer, x, y, color );
}

inline void putpixel( uint8_t * buffer, unsigned int x, unsigned int y, int color) {
	uint32_t where = (x * vga_info.pixel_width) + (y * vga_info.width * vga_info.pixel_width);
	buffer[where] = color & 255;              // BLUE
	buffer[where + 1] = (color >> 8) & 255;   // GREEN
	buffer[where + 2] = (color >> 16) & 255;  // RED
}

void draw_string( char * string, unsigned int x, unsigned int y, unsigned int fg_color, unsigned int bg_color ) {
	while( *string ) {
		draw_char( vga_info.fbuffer, x , y, fg_color, bg_color, *string );

		x = x + 7;

		string++;
	}
}

void draw_rect( rect r, unsigned int color ) {
	fillrect( vga_info.buffer, color, r.x, r.y, r.w, r.h );
}

inline void move_rect( rect dest, rect src ) {
	moverect( vga_info.buffer, dest, src );
}

inline void moverect( uint8_t *buff, rect dest, rect src ) {
	unsigned int i = 0;
	uint8_t * mem_dest;
	uint8_t * mem_src;
	unsigned int mem_size;

	for( i = 0; i < src.h; i++ ) {
		mem_dest = buff + (dest.x * vga_info.pixel_width) + ((dest.y + i) * vga_info.pitch );
		mem_src = buff + (src.x * vga_info.pixel_width) + ((src.y + i) * vga_info.pitch );
		mem_size = (vga_info.pixel_width * src.w);


		for(; mem_size != 0; mem_size--) *mem_dest++ = *mem_src++;
	}
}

void vga_move_line( unsigned int dest_y, unsigned int src_y ) {
	rect src;
	rect dest;

	src.x = 0;
	src.y = screen_border + (src_y * char_height);
	src.w = vga_info.width;
	src.h = char_height;

	dest.x = 0;
	dest.y = screen_border + (dest_y * char_height );
	dest.w = vga_info.width;
	dest.h = char_height;

	move_rect( dest, src );
}

void vga_put_char( unsigned char c, unsigned int x, unsigned int y ) {
	draw_char( vga_info.buffer, screen_border + (x * char_width), screen_border + (y * char_height), vga_info.fg_color, vga_info.bg_color, c );
}


inline void draw_char( uint8_t * buffer, unsigned int x, unsigned int y, uint32_t fg, uint32_t bg, unsigned int font_char ) {
	int row, col;
	uint8_t * where = (buffer + (x * vga_info.pixel_width) + (y * vga_info.width * vga_info.pixel_width));
	uint32_t new_r, new_g, new_b, new_fg;
	double alpha = 0.1;

	new_r = (((fg & 0xFF0000) >> 16) * alpha) + ((1 - alpha) * ((bg & 0xFF0000) >> 16));
	new_g = (((fg & 0x00FF00) >> 8) * alpha) + ((1 - alpha) * ((bg & 0x00FF00) >> 8));
	new_b = (((fg & 0x0000FF)) * alpha) + ((1 - alpha) * ((bg & 0x0000FF)));

	new_fg = ((new_r << 16) & 0xFF0000) | ((new_g << 8) & 0x00FF00) | (new_b & 0x0000FF);

	for (row = 0; row < 14; row++) {
		for (col = 0; col < 7; col++) {
			*(bg_mask + ((row) * 7) + (col) ) = 0;
		}
	}

	for (row = 0; row < 14; row++) {
		for (col = 0; col < 7; col++) {
			if( (_tamzen_bits[ _tamzen_offset[ font_char ] + row ] >> 8 & (1 << 7 - col)) ) {
				*(uint32_t *)(where + (col) * vga_info.pixel_width) = fg;

				if( font_smoothing_active ) {
					//  X.
					//  .E
					//
					//IF pixel right 1 and down 1 exists, fill 1 right with alpha, and fill 1 down with alpha
					if( (col + 1 <= 7) && (row + 1 <= 14) ) { // if right 1 can happen and 1 down can happen
						if( (_tamzen_bits[ _tamzen_offset[ font_char ] + (row + 1) ] >> 8 & (1 << (7 - col - 1)) ) ) { // does right 1 and down 1 exist?
							if( !(_tamzen_bits[ _tamzen_offset[ font_char ] + (row) ] >> 8 & (1 << (7 - col - 1)) ) ) {	// does right 1 NOT exist? If so... fill it with alpha
								*(uint32_t *)(where + ((col + 1) * vga_info.pixel_width)) = new_fg;
								*(bg_mask + ((row) * 7) + (col + 1) ) = 1;
							}

							if( !(_tamzen_bits[ _tamzen_offset[ font_char ] + (row + 1 ) ] >> 8 & (1 << (7 - col)) ) ) {// does down 1 NOT exist? If so... fill it with alpha
								*(uint32_t *)(where + ((col) * vga_info.pixel_width ) + vga_info.pitch ) = new_fg;
								*(bg_mask + ((row + 1) * 7 ) + (col) ) = 1;
							}
						}
					} 	   

					//  .X
					//  E.
					//IF pixel left 1 and down 1 exists, fill 1 left with alpha, and fill 1 down with alpga
					if( (col - 1 >= 0) && (row + 1 <= 14) ) { // if right 1 can happen
						if( (_tamzen_bits[ _tamzen_offset[ font_char ] + (row + 1) ] >> 8 & (1 << (7 - col + 1)) ) ) { // does right 1 and down 1 exist?
							if( !(_tamzen_bits[ _tamzen_offset[ font_char ] + (row) ] >> 8 & (1 << (7 - col + 1)) ) ) {	// does right 1 NOT exist? If so... fill it with alpha
								*(uint32_t *)(where + ((col - 1) * vga_info.pixel_width)) = new_fg;
								*(bg_mask + ((row) * 7) + (col - 1) ) = 1;
							}

							if( !(_tamzen_bits[ _tamzen_offset[ font_char ] + (row + 1 ) ] >> 8 & (1 << (7 - col)) ) ) {// does down 1 NOT exist? If so... fill it with alpha
								*(uint32_t *)(where + ((col) * vga_info.pixel_width ) + vga_info.pitch ) = new_fg;
								*(bg_mask + ((row + 1) * 7 ) + (col) ) = 1;
							}
						}
					} 
				}
			} else {
				if( render_background ) {
					if( *(bg_mask + (row * 7) + (col)) == 0 ) { // Only render the bg if there isn't a filled pixel there
						*(uint32_t *)(where + (col) * vga_info.pixel_width) = bg;
					}
				}
			}
		}
	   
		where += vga_info.pitch;
	}
}