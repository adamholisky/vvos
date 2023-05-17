#include <kernel.h>
#include <vui/vui.h>
#include <vui/testapp.h>
#include <fs.h>
#include <afs.h>
#include <syscall.h>
#include <vui/window.h>
#include <vui/label.h>
#include <vui/button.h>
#include <vui/alert.h>

void test_app_main( void ) {
	log_entry_enter();

	vui_alert_new( "alert_hi", "Hello, world of alerts!\n" );

	log_entry_exit();
}
