#if !defined(VUIDESKTOP_INCLUDED)
#define VUIDESKTOP_INCLUDED

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

    uint32_t    color;
} vui_desktop;

vui_desktop *vui_desktop_new( int x, int y, int width, int height, uint32_t color );
void vui_desktop_destroy( vui_desktop *desktop );
bool vui_desktop_draw( vui_desktop *desktop );
vui_desktop *vui_get_main_desktop( void );
vui_desktop *vui_set_main_desktop( vui_desktop *desktop );

#ifdef __cplusplus
}
#endif

#endif