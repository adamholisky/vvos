#include <kernel.h>
#include <syscall.h>
#include <afs.h>
#include <fs.h>
#include <unistd.h>

#undef KDEBUG_WRITE
uint32_t write( int fd, void * buff, uint32_t count ) {
	syscall_args	args;

	#ifdef KDEBUG_WRITE
	klog( "calling syscall write with fd = %d, buff = 0x%08X, count = %d\n", fd, buff, size );
	#endif

	args.arg_1 = fd;
	args.arg_2 = (uint32_t)buff;
	args.arg_3 = count;

	return syscall( SYSCALL_WRITE, 3, &args );
}

uint32_t syscall_write( int _fd, void * buff, uint32_t count ) {
	uint32_t num_written = 0;
	vv_file_internal *fs = get_fs_internal();
	vv_file *fp;

	fp = &fs->fd[_fd];

	switch( _fd ) {
		case STDIN_FILENO:
			break;
		case STDOUT_FILENO:
			if( count == 1 ) {
				term_put_char( *((char *)buff) );
			} else {
				term_put_string( (char *)buff, count );
			}
			break;
		case STDERR_FILENO:
			if( count == 1 ) {
				debugf( "%c", *((char *)buff) );
			} else {
				debugf( "%s", (char *)buff );
			}
			break;
		default:
			//afs_write( fs, buff, count, fp );
	}

	#ifdef KDEBUG_WRITE
	printf( "syscall_write count: %d\n", size );
	printf( "buff addr: 0x%08X\n", buff );
	klog( "syscall_write bytes: %d\n", num_written );
	#endif
	
	return num_written;
}
