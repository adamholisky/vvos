#include <printf.h>
#include <string.h>
#include <stdbool.h>
#include "debug.h"
#include "syscall.h"
#include "modules_bootstrap.h"
#include "task.h"

void main( void ) {
	while (1) {
		debugf( "Process Info\n" );

		task *t = get_tasks();

		debugf( "ID   S         Type        EIP           Name\n" );

		for( int i = 0; i < TASK_MAX; i++ ) {
			if( t[i].id != TASK_ID_INVALID ) {
				x86_context *context = (x86_context *)t[i].context_at_interrupt;
				uint32_t eip = context->eip;
				debugf( "%d    %s  %s    0x%08X    %s\n", i, task_status_to_string( t[i].status ), task_type_to_string( t[i].type ), eip, t[i].name );
				//kdebug_peek_at( context );
			}
		}

		syscall_exit_from_wrapper();
	}
}