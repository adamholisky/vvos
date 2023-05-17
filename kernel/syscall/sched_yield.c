#include "kernel.h"
#include "syscall.h"
#include "task.h"

#define kdebug_sched_yield

uint32_t sched_yield( void ) {
	syscall_args args;

	return syscall( 4, 0, &args );
}
