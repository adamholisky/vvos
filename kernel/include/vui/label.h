#if !defined(VUILABEL_INCLUDED)
#define VUILABEL_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdint.h>
#include <stdbool.h>
#include <vui/vui.h>

typedef struct {
	vui_object  common;
	
	int			x;
	int			y;
	int			width;
	int			height;

    char        *text;
    int         font;
    int         font_size;
} vui_label;

vui_label *vui_label_new( int x, int y, int font, int size, char *text );
void vui_label_destroy( vui_label *label );
bool vui_label_draw( vui_label *label );

#ifdef __cplusplus
}
#endif

#endif