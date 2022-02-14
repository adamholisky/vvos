#include "kernel.h"
#include "syscall.h"

uint32_t read( int fd, void * buff, uint32_t size ) {
	syscall_args	args;

	#ifdef kdebug_syscall
	klog( "calling syscall read with fd = %d, buff = 0x%08X, size = %d\n", fd, buff, size );
	#endif

	args.arg_1 = fd;
	args.arg_2 = (uint32_t)buff;
	args.arg_3 = size;

	return syscall( 0, 3, &args );
}

uint32_t syscall_read( int fd, void * buff, uint32_t size ) {
	uint32_t num_read = 0;

	return num_read;
}
