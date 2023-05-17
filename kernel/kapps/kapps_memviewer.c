#include <kernel.h>
#include <vui/vui.h>
#include <vui/testapp.h>
#include <fs.h>
#include <afs.h>
#include <syscall.h>
#include <vui/window.h>
#include <vui/label.h>

typedef struct {
	vui_window *win;
	vui_label *top_header;

	rect pane;

	uint32_t bg;
	uint32_t fg;

	void *mem_start;
} mem_viewer_state;

void mem_viewer_custom_paint( void *obj );

mem_viewer_state state;

void kapps_memviewer_main( int argc, char *argv[] ) {
	log_entry_enter();

	rect *r;

	state.bg = 0xFF1E1E1E;
	state.fg = 0xFFFFFFFF;

	state.win = vui_window_new( 50, 400, 750, 310, "Mem Viewer" );
	vui_set_parent( state.win, vui_get_main_desktop() );

	r = vui_window_get_inner_rect( state.win, &state.pane );
	state.top_header = vui_label_new( r->x + 5, r->y + 16, VUI_FONT_VERA, 12, "Memory at: 0xc011d459" );
	vui_set_parent( state.top_header, state.win );

	state.win->common.custom_paint_func = mem_viewer_custom_paint;

	vui_refresh();

	log_entry_exit();
}

void mem_viewer_custom_paint( void *obj ) {
	log_entry_enter();

	int text_size = 16;



	rect *r;
	r = vui_window_get_inner_rect( state.win, &state.pane );
	vui_draw_string(  r->x + 5, r->y + 45, text_size, state.bg, state.fg, VUI_FONT_FIRACODE, "            00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  Decoded Text" );

	uint8_t *addr = 0xc011d459;
	int str_size = 100;
	char str[str_size];
	uint8_t *ptrn = (uint8_t *)addr;
	char *ptr = (char *)addr;

	for( int x = 0; x < 10; x++ ) {
		snprintf( str, str_size, "0x%08X  %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X  %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c",
		addr + (x * 0xF), 
		*(ptrn + 0), *(ptrn + 1), *(ptrn + 2), *(ptrn + 3), *(ptrn + 4), *(ptrn + 5), *(ptrn + 6), *(ptrn + 7), *(ptrn + 8), *(ptrn + 9), *(ptrn + 10), *(ptrn + 11), *(ptrn + 12), *(ptrn + 13), *(ptrn + 14), *(ptrn + 15),
		peek_char( *(ptr + 0) ), peek_char( *(ptr + 1) ), peek_char( *(ptr + 2) ), peek_char( *(ptr + 3) ), peek_char( *(ptr + 4) ), peek_char( *(ptr + 5) ), peek_char( *(ptr + 6) ), peek_char( *(ptr + 7) ), peek_char( *(ptr + 8) ), peek_char( *(ptr + 9) ), peek_char( *(ptr + 10) ), peek_char( *(ptr + 11) ), peek_char( *(ptr + 12) ), peek_char( *(ptr + 13) ), peek_char( *(ptr + 14) ), peek_char( *(ptr + 15) ) );

		vui_draw_string(  r->x + 5, r->y + 75 + (17 * x), text_size, state.bg, state.fg, VUI_FONT_FIRACODE, str );

		klog( "%s\n", str );
		klog( "x: %d\n", x );
		ptr = ptr + 0x10;
		ptrn = ptrn + 0x10;
	}

	vga_draw_screen();		
	
	klog( "DONE!\n" );
}