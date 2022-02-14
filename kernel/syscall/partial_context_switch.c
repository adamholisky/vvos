#include "kernel.h"
#include "syscall.h"
#include "task.h"

#undef kdebug_partial_context_switch

uint32_t partial_context_switch( x86_context * new_context ) {
	syscall_args args;

	args.arg_1 = (uint32_t)new_context;

	return syscall( SYSCALL_PARTIAL_CONTEXT_SWITCH, 1, &args );
}

uint32_t syscall_partial_context_switch( x86_context ** _stack, x86_context * new_context ) {
	x86_context * stack = *_stack;

	(*_stack)->eax = new_context->eax;
	(*_stack)->ebx = new_context->ebx;
	(*_stack)->ecx = new_context->ecx;
	(*_stack)->edx = new_context->edx;
	(*_stack)->_esp = new_context->_esp;
	(*_stack)->ebp = new_context->ebp;
	(*_stack)->esi = new_context->esi;
	(*_stack)->edi = new_context->edi;
	(*_stack)->eflags = new_context->eflags;

	return SYSCALL_RT_SUCCESS;
}