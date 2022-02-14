#include <kernel.h>
#include "callback.h"
#include "task.h"
#include <string.h>

context_switch_callback * register_callback( context_switch_callback * callback, void * func_name ) {
	strcpy( callback->func_name, func_name );
	callback->t = get_current_task();

	Elf32_Sym * symbol_table = callback->t->sym_table;
	uint32_t string_addr = (uint32_t)callback->t->str_table;

	for( int i = 0; i < callback->t->num_syms; i++ ) {
		//klog( "0x%08X -> %s\n", symbol_table[i].st_value, (char *)(symbol_table[i].st_name + string_addr)  );
		if( strcmp(callback->func_name, (char *)(symbol_table[i].st_name + string_addr)) == 0 ) {
			callback->addr = symbol_table[i].st_value;
		}
	}

	/* klog( "addr: 0x%08X\n", test_callback_struct.addr );
	klog( "arg1: 0x%08X\n", test_callback_struct.arg1 );
	klog( "t.id: 0x%02X\n", test_callback_struct.t->id );

	klog( "rd:     %X\n", test_callback_struct.t->raw_data );
	klog( "stloc:  %X\n", symbol_table );
	klog( "txtloc: %X\n", string_addr );
	klog( "size:   %d\n", test_callback_struct.t->num_syms ); */

	return callback;
}

void do_callback( context_switch_callback * callback ) {
	x86_context old_context;
	int32_t current_task_id = -1;
	void (*callback_function)(void *);

	// Load task for switch
	memcpy( &old_context, &callback->t->context_at_interrupt, sizeof( x86_context ) );
	current_task_id = get_current_task_id();
	set_current_task_id( callback->t->id );
	set_task_pde( callback->t->code_page_table );

	// switch
	callback_function = (void *)callback->addr;
	callback_function( callback->arg1 );

	// restore task
	set_current_task_id( current_task_id );
	set_task_pde( get_current_task()->code_page_table );
	memcpy( &callback->t->context_at_interrupt, &old_context, sizeof( x86_context ) );
}

void do_callback_with_data( context_switch_callback * callback, void * data ) {
	callback->arg1 = data;
	do_callback( callback );
}

/* Test functions */

context_switch_callback test_callback_struct;

void callback_test_run( void ) {
	do_callback_with_data( &test_callback_struct, "Interprocess callback called!" );
}

void callback_test_register( char * func_name ) {
	register_callback( &test_callback_struct, func_name );
}
