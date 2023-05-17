#include "kernel.h"
#include "syscall.h"
#include "task.h"

#define kdebug_sched_yield_to

uint32_t sched_yield_to( uint32_t task_id ) {
	syscall_args args;

	args.arg_1 = task_id;

	return syscall( SYSCALL_SCHED_YIELD_TO, 1, &args );
}
