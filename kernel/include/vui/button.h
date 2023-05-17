#if !defined(VUIBUTTON_INCLUDED)
#define VUIBUTTON_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdint.h>
#include <stdbool.h>
#include <vui/vui.h>

#define VUI_BUTTON_STATE_NORMAL 0
#define VUI_BUTTON_STATE_PRESSED 1
#define VUI_BUTTON_STATE_HOVER 2
#define VUI_BUTTON_STATE_DISABLED 3

typedef struct {
    vui_object      common;
    int             x;
    int             y;
    int             width;
    int             height;

    char            id[25];
    char            label[25];

    vui_callback    on_mouseup;
    vui_callback    on_enter;
    vui_callback    on_exit;

    int             state;
} vui_button;

vui_button *vui_button_new( int x, int y, int width, int height, char *id, char *label );
void vui_button_destroy( vui_button *button );
bool vui_button_draw( vui_button *button );


#ifdef __cplusplus
}
#endif

#endif