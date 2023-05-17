#include <kernel.h>
#include <vui/vui.h>
#include <vui/alert.h>
#include <vui/window.h>
#include <vui/label.h>
#include <vui/button.h>

vui_alert *current_alert = NULL;

vui_alert *vui_alert_new( char *id, char *text ) {
	vui_alert *a = vui_alert_new_with_callbacks( id, text, "Ok", (vui_callback)vui_alert_ok_default_callback, NULL, NULL, NULL, NULL );

	return a;
}

#undef KDEBUG_VUI_ALERT_NEW_WITH_CALLBACKS
vui_alert *vui_alert_new_with_callbacks( char *id, char *text, char *button1_text, vui_callback button1_callback, char *button2_text, vui_callback button2_callback, char *button3_text, vui_callback button3_callback ) {
	vui_alert *ret = vui_add_handle( VUI_TYPE_ALERT );
	ret->window = vui_window_new( 600, 400, 400, 150, "Alert" );
	vui_set_parent( ret->window, vui_get_main_desktop() );

	rect *r = vui_window_get_inner_rect( ret->window, &ret->pane );
	ret->alert_text = vui_label_new( r->x + 5, r->y + 16, VUI_FONT_VERA, 14, text );
	vui_set_parent( ret->alert_text, ret->window );

	ret->button_two = NULL;
	ret->button_three = NULL;

	ret->button_one = vui_button_new( r->x + r->w - 60, r->y + r->h - 35, 50, 25, "btn_one", button1_text );
	vui_set_parent( ret->button_one, ret->window );
	ret->button_one->on_mouseup = button1_callback;

	if( button2_text != NULL && button2_callback != 0 ) {
		ret->button_two = vui_button_new( r->x + r->w - 120, r->y + r->h - 35, 50, 25, "btn_two", button2_text );
		vui_set_parent( ret->button_two, ret->window );

		if( button3_text != NULL && button3_callback != 0 ) {
			ret->button_three = vui_button_new( r->x + r->w - 180, r->y + r->h - 35, 50, 25, "btn_three", button3_text );
			vui_set_parent( ret->button_three, ret->window );
		}
	}

	//ret->common.custom_paint_func = alert_custom_paint;

	#ifdef KDEBUG_VUI_ALERT_NEW_WITH_CALLBACKS
	klog( "Alert window handle: %d\n", ret->window->common.handle );
	vui_set_active_window( ret->window->common.handle );
	#endif

	current_alert = ret;

	vui_refresh();

	return ret;
}

void vui_alert_destroy( vui_alert *a ) {
	vui_free_handle( a->button_one->common.handle );
	vui_free_handle( a->alert_text->common.handle );

	if( a->button_two != NULL ) {
		vui_free_handle( a->button_two->common.handle );
	}

	if( a->button_three != NULL ) {
		vui_free_handle( a->button_three->common.handle );
	}

	vui_free_handle( a->window->common.handle );
}

bool vui_alert_draw( vui_alert *a ) {

}

void vui_alert_ok_default_callback( char *id, void *obj ) {
	vui_free_handle( current_alert->common.handle );
	vui_refresh();
}

void alert_custom_paint( void *obj ) {
	log_entry_enter();

	log_entry_exit();
}