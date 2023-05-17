#ifndef VGA_INCLUDED
#define VGA_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define VVOS_SCREEN_WIDTH 1024
#define VVOS_SCREEN_HEIGHT 768

enum vga_color
{
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};


typedef struct {
   char VbeSignature[4];             // == "VESA"
   uint16_t VbeVersion;                 // == 0x0300 for VBE 3.0
   uint16_t OemStringPtr[2];            // isa vbeFarPtr
   uint8_t Capabilities[4];
   uint16_t VideoModePtr[2];         // isa vbeFarPtr
   uint16_t TotalMemory;             // as # of 64KB blocks
} vesa_info_block;


typedef struct  {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} vesa_mode_info;

#define color_red 			0xFF0000
#define color_green 		0x00FF00
#define color_blue 			0x0000FF
#define color_white 		0xFFFFFF
#define color_black			0x000000
#define color_adams_blue	0x3753D1

#define cursor_up		1
#define cursor_down		2
#define	cursor_left		3
#define cursor_right	4	

typedef struct {
	uint32_t x;
	uint32_t y;
} point;

typedef struct {
	uint32_t x;
	uint32_t y;
	uint32_t w;
	uint32_t h;
} rect;

typedef struct {
	uint8_t * fbuffer;
	uint8_t * buffer;
	uint32_t pitch;
	uint32_t width;
	uint32_t height;
	uint32_t bpp;
	uint32_t pixel_width;

	uint32_t char_width;
	uint32_t char_height;

	uint32_t fg_color;
	uint32_t bg_color;
} vga_information;

void vga_initalize( void );
void fillrect( uint8_t * buffer, uint32_t color, unsigned int x, unsigned int y, unsigned int w, unsigned int h);
void put_pixel( unsigned int x, unsigned int y, int color );
void putpixel( uint8_t * buffer, unsigned int x, unsigned int y, int color);
void draw_string( char * string, unsigned int x, unsigned int y, unsigned int fg_color, unsigned int bg_color );
void draw_rect( rect r, unsigned int color );
void move_rect( rect dest, rect src );
void moverect( uint8_t *buff, rect dest, rect src );
void vga_move_line( unsigned int dest_y, unsigned int src_y );
void vga_put_char( unsigned char c, unsigned int x, unsigned int y );
void draw_char( uint8_t * buffer, unsigned int x, unsigned int y, uint32_t fg, uint32_t bg, unsigned int font_char );
void vga_draw_screen( void );
void vga_draw_screen_box( rect *r );
vga_information * vga_get_info( void );
void framebuffer_copy_to_frontbuffer( int x, int y, int w, int h );
void framebuffer_fill_rect_in_backbuffer( int x, int y, int w, int h, uint32_t color );
void framebuffer_move_rect_in_backbuffer( int from_x, int from_y, int w, int h, int to_x, int to_y );

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

#ifdef __cplusplus
}
#endif

#endif