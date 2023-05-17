#include <kernel.h>
#include <syscall.h>
#include <afs.h>

#undef KDEBUG_SYSCALL_OPEN
/**
 * @brief 
 * 
 * @param pathname 
 * @param flags 
 * @return int 
 */
int open( const char *pathname, int flags ) {
	syscall_args	args;

	#ifdef KDEBUG_SYSCALL_OPEN
	klog( "calling syscall open with pathname = \"%s\", flags = 0x%X\n", pathname, flags );
	#endif

	args.arg_1 = pathname;
	args.arg_2 = flags;

	return syscall( SYSCALL_OPEN, 2, &args );
}

int syscall_open( const char *pathname, int flags ) {
	vv_file *file = 0;
	int result;

	// If we're opening something in dev, handle separately
	if( strstr( pathname, "/dev" ) ) {
		return 0;
	}

	// Handle FS
	file = afs_open( get_fs_internal(), pathname, flags );

	if( file )
		result = file->fd;
	else
		result = -1;
	
	return result;
}
