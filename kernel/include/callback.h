#ifndef CALLBACK_INCLUDED
#define CALLBACK_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <task.h>

typedef struct {
	task * t;
	uint32_t addr;
	char func_name[25];
	void * arg1;
} context_switch_callback;

context_switch_callback * register_callback( context_switch_callback * callback, void * func_name );
void do_callback( context_switch_callback * callback );
void do_callback_with_data( context_switch_callback * callback, void * data );

void callback_test_run( void );
void callback_test_register( char * func_name );
void callback_test_docallback( void );

#ifdef __cplusplus
}
#endif

#endif