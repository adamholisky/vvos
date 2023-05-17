#include "kernel.h"
#include "syscall.h"
#include "task.h"
#include "interrupts.h"

/**
 * @brief Handle a syscall from the assembly interrupt
 * 
 * @param stack Stack of the syscall
 * @param _context x86 context at the interrupt point
 * @return uint32_t Value expected to return for good measure, actual result goes into eax
 */
uint32_t syscall_handler( uint32_t * stack, x86_context ** _context ) {
	x86_context *context = *_context;
	uint32_t result = 0;

	switch( context->eax ) {
		case SYSCALL_READ:
			//debugf( "[SYSCALL] Read :: fd = %d, buff = 0x%08X, size = %d\n", stack->edi, stack->esi, stack->edx );
			result = syscall_read( context->edi, (void *)context->esi, context->edx );
			context->eax = result;
			break;
		case SYSCALL_WRITE:
			//debugf( "[SYSCALL] Write\n" );
			result = syscall_write( context->edi, (void *)context->esi, context->edx );
			break;
		case SYSCALL_OPEN:
			result = syscall_open( (char *)context->edi, context->esi );
			context->eax = result;
			break;
		case SYSCALL_SCHED_YIELD:
			// This should never be hit, handled prior to being passed to handler
			break;
		case SYSCALL_SCHED_YIELD_TO:
			// This should never be hit, handled prior to being passed to handler
			break;
		case SYSCALL_PARTIAL_CONTEXT_SWITCH:
			syscall_partial_context_switch( _context, (x86_context *)context->edi );
			break;
		case SYSCALL_SBRK:
			syscall_sbrk( context->edi );
			break;
		case SYSCALL_ACTTASK:
			syscall_activate_task( context->edi );
			break;
		case SYSCALL_END:
			syscall_exit_from_wrapper();
			break;
		case SYSCALL_EXIT:
			syscall_exit( context->edi );
			break;
		default:
			klog( "Undefined syscall number: 0x%04X\n", context->eax );
	}

	outportb( 0xA0, 0x20 );
	outportb( 0x20, 0x20 );

	return result;
}

/**
 * @brief Sets up and executes a syscall with the given structures 
 * 
 * @param call_num Syscall number as indicated in defines
 * @param num_args Number of arguments in the syscall
 * @param args Pointer to the argument array
 * @return uint32_t Result of the syscall (from eax)
 */
uint32_t syscall( uint32_t call_num, uint32_t num_args, syscall_args * args ) {
	uint32_t ret = 0;

	//klog( "arg3: 0x%08X\n", args->arg_3 );
	// TODO: WHY DO WE NEED THIS?!?!?
	int arg3 = args->arg_3;

	switch( num_args ) {
		case 6:
		case 5:
		case 4:
		case 3:
			asm	volatile ( 
				"movl %1, %%eax \n"
				"movl %2, %%edi \n"
				"movl %3, %%esi \n"
				"movl %4, %%edx \n"
				"int %5 \n"
				"movl %%eax, %0"
				:"=r"(ret)
				:"r"(call_num), "m"(args->arg_1), "m"(args->arg_2), "m"(arg3), "i"(0x99)
				:"%eax" 
			);
			break;
		case 2:
			asm	volatile ( 
				"movl %1, %%eax \n"
				"movl %2, %%edi \n"
				"movl %3, %%esi \n"
				"int %4 \n"
				"movl %%eax, %0"
				:"=r"(ret)
				:"r"(call_num), "m"(args->arg_1), "m"(args->arg_2), "i"(0x99)
				:"%eax" 
			);
			break;
		case 1:
			asm	volatile ( 
				"movl %1, %%eax \n"
				"movl %2, %%edi \n"
				"int %3 \n"
				"movl %%eax, %0"
				:"=r"(ret)
				:"r"(call_num), "m"(args->arg_1), "i"(0x99)
				:"%eax" 
			);
			break;
		case 0:
			asm	volatile ( 
				"movl %1, %%eax \n"
				"int %2 \n"
				"movl %%eax, %0"
				:"=r"(ret)
				:"r"(call_num), "i"(0x99)
				:"%eax" 
			);
			break;
		default:
			klog( "Syscall called with more than six args.\n" );
	}

	return ret;
}
