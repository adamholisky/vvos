#include <kernel.h>
#include <vui/vui.h>
#include <vui/button.h>

vui_button *vui_button_new( int x, int y, int width, int height, char *id, char *label ) {
	vui_button *button = vui_add_handle( VUI_TYPE_BUTTON );

	button->x = x;
	button->y = y;
	button->width = width;
	button->height = height;
	strcpy( button->id, id );
    strcpy( button->label, label );

	return button;
}

void vui_button_destroy( vui_button *button ) {

}

bool vui_button_draw( vui_button *button ) {
    vui_draw_rectangle( button->x, button->y, button->width, button->height, 0x00FFFFFF );

    vui_draw_string( button->x + 5, button->y + 16, 14, 0xFFFFFFFF, 0xFF000000, VUI_FONT_VERA, button->label );
}