#include <kernel.h>
#include <vui/vui.h>
#include <vui/label.h>

vui_label *vui_label_new( int x, int y, int font, int size, char *text ) {
	vui_label *label = vui_add_handle( VUI_TYPE_LABEL );

	label->x = x;
	label->y = y;
	label->font = font;
	label->font_size = size;
	label->text = malloc( strlen(text) );
	strcpy( label->text, text );

	return label;
}

void vui_label_destroy( vui_label *label ) {
	free( label->text );
}

bool vui_label_draw( vui_label *label ) {
    vui_draw_string( label->x, label->y, label->font_size, 0xFF1E1E1E, 0xFFD9D9D9, label->font, label->text );
}