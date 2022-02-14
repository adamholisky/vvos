#include <stdlib.h>
#include <stdio.h>
#include <debug.h>
#include <syscall.h>
#include <task.h>

extern "C" void module_init( void ) {
	//debugf( "In Module: Init\n" );
}

extern "C" void module_exit( void ) {
	//debugf( "In Module: Exit\n" );
}

int main( int argc, char *argv[] ) {
	printf( "Process Info\n" );

	task *t = get_tasks();

	printf( "ID   S         Type        EIP           Name\n" );

	for( int i = 0; i < TASK_MAX; i++ ) {
		if( t[i].id != TASK_ID_INVALID ) {
			x86_context *context = (x86_context *)t[i].context_at_interrupt;
			uint32_t eip = context->eip;
			debugf( "%d    %s  %s    0x%08X    %s\n", i, task_status_to_string( t[i].status ), task_type_to_string( t[i].type ), eip, t[i].name );
		}
	}

	return 0;
}