#include <kernel.h>
#include <syscall.h>
#include <afs.h>
#include <fs.h>
#include <unistd.h>
#include <keyboard.h>
#include <task.h>

stdin_func stdin_redirect = NULL;

#undef KDEBUG_READ
uint32_t read( int fd, void * buff, uint32_t size ) {
	syscall_args	args;

	#ifdef KDEBUG_READ
	klog( "calling syscall read with fd = %d, buff = 0x%08X, size = %d\n", fd, buff, size );
	#endif

	args.arg_1 = fd;
	args.arg_2 = (uint32_t)buff;
	args.arg_3 = size;

	// Handle keyboard input for the kernel
	if( get_current_task_id() == 0 && fd == STDIN_FILENO ) {
		//klog( "special!\n" );

		char *cbuff = (char *)buff;

		if( stdin_redirect != NULL ) {
			cbuff[0] = stdin_redirect();
		} else {
			cbuff[0] = keyboard_get_char_nonblocking();
		}
		
		return 1;
	}

	return syscall( SYSCALL_READ, 3, &args );
}

uint32_t syscall_read( int _fd, void * buff, uint32_t size ) {
	uint32_t num_read = 0;
	vv_file_internal *fs = get_fs_internal();
	vv_file *fp;
	char *cbuff = (char *)buff;

	if( _fd == -2 ) {
		strcpy( buff, "VFive" );

		num_read = 651;
	}

	fp = &fs->fd[_fd];

	switch( _fd ) {
		case STDIN_FILENO:
			if( stdin_redirect != NULL ) {
				cbuff[0] = stdin_redirect();
			} else {
				cbuff[0] = keyboard_get_char_nonblocking();
			}
			
			num_read = 1;
			break;
		case STDOUT_FILENO:
			break;
		case STDERR_FILENO:
			break;
		default:
			num_read = afs_read( fs, buff, size, fp );
	}

	#ifdef KDEBUG_READ
	klog( "syscall_read size: %d\n", size );
	klog( "buff addr: 0x%08X\n", buff );
	klog( "syscall_read bytes: %d\n", num_read );
	#endif
	
	return num_read;
}

void set_stdin_redirect( stdin_func f ) {
	stdin_redirect = f;
}

void clear_stdin_redirect( void ) {
	stdin_redirect = NULL;
}