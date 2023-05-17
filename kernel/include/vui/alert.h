#if !defined(VUIALERT_INCLUDED)
#define VUIALERT_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdint.h>
#include <stdbool.h>
#include <vui/vui.h>
#include <vui/window.h>
#include <vui/button.h>
#include <vui/label.h>

typedef struct {
    vui_object      common;
    int             x;
    int             y;
    int             width;
    int             height;

    char            id[25];
    char            text[140];

    vui_window      *window;
    vui_label       *alert_text;
    vui_button      *button_one;
    vui_button      *button_two;
    vui_button      *button_three;

    rect pane;

    vui_callback    on_button_one;
    vui_callback    on_button_two;
    vui_callback    on_button_three;
} vui_alert;

vui_alert *vui_alert_new( char *id, char *text );
vui_alert *vui_alert_new_with_callbacks( char *id, char *text, char *button1_text, vui_callback button1_callback, char *button2_text, vui_callback button2_callback, char *button3_text, vui_callback button3_callback );
void vui_alert_destroy( vui_alert *button );
bool vui_alert_draw( vui_alert *button );
void vui_alert_ok_default_callback( char *id, void *obj );
void alert_custom_paint( void *obj );


#ifdef __cplusplus
}
#endif

#endif