#include "kernel.h"
#include "syscall.h"

uint32_t sbrk( int inc ) {
	syscall_args	args;

	#ifdef kdebug_syscall
	klog( "calling syscall sbrk with inc = %d\n", inc );
	#endif

	args.arg_1 = inc;

	return syscall( SYSCALL_SBRK, 1, &args );
}

uint32_t syscall_sbrk( int inc ) {
	uint32_t num_read = 0;

	return num_read;
}
