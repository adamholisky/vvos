#include <kernel.h>
#include <vga.h>
#include <vui/vui.h>
#include <vui/desktop.h>

vui_desktop *_main_desktop;

vui_desktop *vui_desktop_new( int x, int y, int width, int height, uint32_t color ) {
    vui_desktop *d = vui_add_handle( VUI_TYPE_DESKTOP );

    d->color = color;
    d->height = height;
    d->width = width;
    d->x = x;
    d->y = y;

    return d;
}

vui_desktop *vui_get_main_desktop( void ) {
	return _main_desktop;
}

vui_desktop *vui_set_main_desktop( vui_desktop *desktop ) {
    _main_desktop = desktop;

    return _main_desktop;
}

void vui_desktop_destroy( vui_desktop *desktop ) {

}

bool vui_desktop_draw( vui_desktop *desktop ) {
    rect r = {
		.h = desktop->height,
		.w = desktop->width,
		.x = desktop->x,
		.y = desktop->y
	};

	draw_rect( r, desktop->color );

    return true;
}