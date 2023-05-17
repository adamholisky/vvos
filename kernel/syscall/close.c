#include <kernel.h>
#include <syscall.h>
#include <afs.h>

#undef KDEBUG_SYSCALL_CLOSE
/**
 * @brief 
 * 
 * @param fd 
 * @return int 
 */
int close( int fd ) {
	syscall_args	args;

	#ifdef KDEBUG_SYSCALL_CLOSE
	klog( "calling syscall close with gd = 0x%X\n", fd );
	#endif

	args.arg_1 = fd;

	return syscall( SYSCALL_CLOSE, 1, &args );
}

int syscall_close( int _fd ) {
	int result = 0;
	
	return result;
}
