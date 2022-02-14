#include "kernel.h"
#include "syscall.h"
#include "task.h"

#undef kdebug_sched_yield

uint32_t sched_yield( void ) {
	syscall_args args;

	return syscall( 4, 0, &args );
}

uint32_t syscall_sched_yield( x86_context ** _stack ) {
	x86_context * stack = *_stack;
	task *t;

	t = get_current_task();
	t->stack_eip = stack->eip;
	t->context_at_interrupt = (uint32_t *)*_stack;

	#ifdef kdebug_sched_yield
	debugf( "pre switch:  *_stack: %08X  stack: %08X  eip: %08X\n", *_stack, stack, (*_stack)->eip );
	#endif
	
	t = switch_next_task();
	*_stack = (x86_context *)t->context_at_interrupt;

/* 	if( p->reset == true ) {
		p->stack_eip = p->entry;
	} */

	(*_stack)->eip = t->stack_eip;

	#ifdef kdebug_sched_yield
	debugf( "post switch: *_stack: %08X  stack: %08X  eip: %08X\n", *_stack, stack, (*_stack)->eip );
	#endif

	if( t->id != 0 ) {
		set_task_pde( t->code_page_table );
	}

	#ifdef kdebug_sched_yield
	for( int i = 0; i < 10; i++ ) {
		uint32_t *eip = (uint32_t *)(*_stack)->eip + i;
		klog( "0x%08X: 0x%08X\n", eip, *eip);
	}

	debugf( "sched_yield done\n" );
	#endif

	return SYSCALL_RT_SUCCESS;
}