#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <serial.h>
#include <keyboard.h>
#include <debug.h>
#include <kmalloc.h>
#include <kapps.h>
#include <task.h>
#include <kshell.h>
#include <syscall.h>
#include <afs.h>
#include <device.h>
#include <terminal.h>
#include <ahci.h>
#include <fs.h>
#include <dlfcn.h>
#include <loader.h>
#include <unistd.h>
#include <vui/vui.h>
#include <vui/testapp.h>
#include <vui/console.h>
#include <kapps/console.h>
#include <kapps/memviewer.h>

char line[256];
char jail_env[256];
char wd[256];
char* dir_bin;
char* dir_usr_bin;
int line_pos;

/**
 * @brief Run the kshell
 * 
 */
void kshell_run( void ) {
	strcpy( wd, "/" );
	
	kshell_fake_cli( "ps_to_log" );
	kexec( "ps", (uint32_t *)kshell_ps, NULL );

	//kshell_automate( "testlibcall" );
	//kshell_automate( "testapp" );
	//kshell_automate( "cat /etc/magic_key" );

	kshell_automate( "memviewer" );

	//test_app_main();

	//kshell_echo_to_serial();

	//do_immediate_shutdown();

	while( true ) {
		printf( "\x1b[0;31;49mVersionV\x1b[0;0;0m:\x1b[0;32;49m%s\x1b[0;0;0m> ", wd );

		kshell_get_line();

		kshell_process_line();

		sched_yield();
	}
}


void kshell_get_line( void ) {
	char c = ' ';
	bool process_keypress = true;

	memset( line, 0, 256 );
	line_pos = 0;

	while( process_keypress ) {
		if( read( STDIN_FILENO, &c, 1 ) ) {
			if( c != 13 ) {
				printf( "%c", c );
			}
		}

		if( c == '\n' || c == 13 ) {
			//printf( "\n" );
			process_keypress = false;
		} else {
			line[line_pos] = c;
			line_pos++;

			if( line_pos == 256 ) {
				process_keypress = false;
			}
		}
	}
	
}

#undef KDEBUG_KSHELL_PROCESS_LINE
/**
 * @brief Process the shell line
 * 
 */
void kshell_process_line( void ) {
	bool check_file_cmd = true;
	char path[256];
	char args[4][100];
	char *argv_builder[4];
	char *c = line;
	bool keep_processing = true;
	int num_args = 0;

	memset( path, 0, 256 );
	memset( args, 0, 4 * 100 );

	int i = 0;
	int j = 0;

	do {
		if( *c != ' ' && *c != 0 ) {
			args[i][j] = *c; 
			j++;
		} else {
			if( j != 0 ) {
				num_args++;
			}

			args[i][j] = 0;
			i++;
			j = 0;

			if( i > 3 ) {
				keep_processing = false;
			}
		}

		c++;
	} while( keep_processing );

	#ifdef KDEBUG_KSHELL_PROCESS_LINE
	klog( "num_args = %d\n", num_args );
	#endif
	
	for( int z = 0; z < 4; z++ ) {
		#ifdef KDEBUG_KSHELL_PROCESS_LINE
		debugf( "args[%d] = \"%s\"\n", z, args[z] );
		#endif

		argv_builder[z] = args[z];
	} 

	if( strcmp( args[0], "1" ) == 0 ) {
		kshell_test_fork();
	}

	if( strcmp( args[0], "q" ) == 0 ) {
		kshell_shutdown();
	}

	if( strcmp( args[0], "ps" ) == 0 ) {
		kexec( "ps",(uint32_t *)kshell_ps, NULL );
	}

	if( strcmp( args[0], "cat" ) == 0 ) {
		kshell_cat( num_args, argv_builder );
	}

	if( strcmp( args[0], "ls" ) == 0 ) {
		primative_ls( args[1] );
	}

	if( strcmp( args[0], "testlibcall" ) == 0 ) {
		kshell_test_lib_call();
	}

	if( strcmp( args[0], "testapp" ) == 0  ) {
		kshell_test_app();
	}

	if( strcmp( args[0], "memviewer" ) == 0 ) {
		kapps_memviewer_main( 0, NULL );
	}
}

#undef KDEBUG_CAT
void kshell_cat( int argc, char *argv[] ) {
	bool show_help = false;

	if( argc < 2 ) {
		show_help = true;	
	} else if( strcmp( argv[1], "--help" ) == 0 ) {
		show_help = true;
	}

	if( show_help ) {
		printf( "cat: cat <pathname>\n" );
	} else {
		#ifdef KDEBUG_CAT
		klog( "cating \"%s\"\n", argv[1] );
		#endif

		int FD = open( argv[1], 0 );

		if( FD == -1 ) {
			printf( "cat: %s: No such file or directory\n", argv[1] );
		} else {
			char *buff = malloc( 2048 );
			memset( buff, 0, 2048 );

			int size = get_file_size(FD);
			int bytes_read = read( FD, buff, size );

			#ifdef KDEBUG_CAT
			klog( "bytes read: %d\n", bytes_read );
			#endif

			printf( "%s\n", buff );

			free( buff );
		}
	}
}

void kshell_fake_cli( char *cmd ) {
	printf( "\x1b[0;31;49mVersionV\x1b[0;0;0m:\x1b[0;32;49m%s\x1b[0;0;0m> %s\n" , wd, cmd );
}

/**
 * @brief Execute cmd_line as if it was typed into kshell
 * 
 * @param cmd_line line to execute
 */
void kshell_automate( char *cmd_line ) {
	memset( line, 0, 256 );
	strcpy( line, cmd_line );

	kshell_fake_cli( cmd_line );
	kshell_process_line();
}



/**
 * @brief Shutdown the emulator
 * 
 */
void kshell_shutdown( void ) {
	debugf( "\nGoodbye, Dave.\n" );
	outportb( 0xF4, 0x00 );
}

/**
 * @brief Send process list to debug out
 * 
 */
void kshell_ps( void ) {
	debugf( "Process Info\n" );

	task *t = get_tasks();

	debugf( "ID   S         Type        EIP           Name\n" );

	for( int i = 0; i < TASK_MAX; i++ ) {
		if( t[i].id != TASK_ID_INVALID ) {
			x86_context *context = (x86_context *)t[i].context_at_interrupt;
			uint32_t eip = context->eip;
			debugf( "%d    %s  %s    0x%08X    %s\n", i, task_status_to_string( t[i].status ), task_type_to_string( t[i].type ), eip, t[i].name );
			//kdebug_peek_at( context );
		}
	}

	task_exit();
}

/**
 * @brief Test forking
 * 
 */
void kshell_test_fork( void  ) {
	int pid = kfork();

	if( pid == 0 ) {
		printf( "Parent task.\n" );
	} else {
		printf( "Child task: %d\n", pid );
	}
}

/**
 * @brief Test systemcalls
 * 
 */
void test_syscall( void ) {
	char *mem = malloc( 6 );
	memset( mem, 0, 6 );

	uint32_t count = read( -2, mem, 5 );

	printf( "Syscall result:\nmem: \"%s\"\ncount: %d\n", mem, count );
}

/**
 * @brief Run device tests
 * 
 */
void kshell_test_devices( void ) {
	device *term = get_device_by_file( "/dev/tty0" );

	//term->write( "A", 1 );

	//printf( "\n" );

	term->write( "Hello, terminal!\n", strlen( "Hello, terminal!\n" ) );

	task_exit();
}

/**
 * @brief Throw divide by zero exception
 * 
 */
void kshell_divide_by_zero( void ) {
	asm volatile( 
		"movl 0, %eax \n\t"
		"movl 0, %ecx \n\t"
		"div %ecx \n\t"
	);
}

/**
 * @brief Test that AFS drive and root directory are sane
 * 
 */
void kshell_afs_test_alpha( void ) {
	afs_drive *drive = malloc( sizeof(afs_drive) );
	afs_string_table *st = malloc( sizeof(afs_string_table) );
	afs_block_directory *root_dir = malloc( sizeof(afs_block_directory) );
	afs_file *f = malloc( sizeof(afs_file) );

	if( ahci_read_at_byte_offset( 0, sizeof(afs_drive), drive) ) {
		printf( "drive.version: %d\n", drive->version );
		printf( "drive.size: %08X\n", drive->size );
		printf( "drive.name_index: %d\n", drive->name_index );
		printf( "drive.root_directory: %04X\n", drive->root_directory );
		printf( "drive.next_free: %04X\n", drive->next_free );
		printf( "\n\n" );

		if( ahci_read_at_byte_offset( sizeof(afs_drive), sizeof( afs_string_table), st ) ) {
			for( int i = 0; i < st->next_free; i++ ) {
				printf( "string_table[%d] = \"%s\" @ 0x%08X\n", i, st->string[i], st->string[i]);
			}
			printf( "\n" );

			if( ahci_read_at_byte_offset( drive->root_directory, sizeof( afs_block_directory), root_dir ) ) {
				printf( "dir.next_index = %d\n", root_dir->next_index );
				printf( "dir.name_index = %d\n", root_dir->name_index );
				
				for( int i = 0; i < root_dir->next_index; i++ ) {
					printf( "dir.index[%d].start = 0x%X\n", i, root_dir->index[i].start );
					printf( "dir.index[%d].name_index = %d\n", i, root_dir->index[i].name_index );
					char * s = st->string[root_dir->index[i].name_index];
					printf( "dir.index[%d].name  = \"%s\"\n", i, s );
				}
				printf( "\n" );

				for( int i = 0; i < root_dir->next_index; i++ ) {
					memset( f, 0, sizeof( afs_file ) );
					klog( "file table %d\n", i );
					if( ahci_read_at_byte_offset( root_dir->index[i].start, sizeof( afs_file ), f ) ) {

						printf( "f.block_size: %d\n", f->block_size );
						printf( "f.file_size: %d\n", f->file_size );
						printf( "f.name_index = \"%s\"\n", st->string[f->name_index]);
						printf( "\n" );

						//printf( "%s\n\n", (char *)((char *)f + sizeof( afs_file )) );
					} else {
						printf( "file info read failed" );
					}
				}
			} else {
				printf( "dir read failed" );
			}

		} else {
			printf( "string table read failed\n" );
		}
	} else {
		printf( "drive read failed\n" );
	}

	printf( "file system tests done.\n" );
	do_immediate_shutdown();

	task_exit();
}

#define KDEBUG_KSHELL_TEST_LIB_CALL
/**
 * @brief Test calling a shared object library function
 * 
 */
void kshell_test_lib_call( void ) {
	log_entry_enter();

	dl_info *lib = NULL;
	void (*func)() = NULL;

	lib = dlopen( "/lib/libmyshare.so", 0 );
	func = dlsym( lib, "my_lib_call" );

	klog( "func addr: 0x%08X\n", func );

	if( func != NULL ) {
		func();
	}

	dlclose( lib );

	log_entry_exit();
}

#undef KDEBUG_KSHELL_TEST_APP
/**
 * @brief Test calling a shared object library function
 * 
 */
void kshell_test_app( void ) {
	log_entry_enter();

	int FD = open( "/bin/cat", 0 );

	if( FD == -1 ) {
		printf( "cannot open /bin/cat\n" );
	} else {

		int size = get_file_size(FD);
		uint8_t *buff = malloc( size );
		memset( buff, 0, 2048 );

		int bytes_read = read( FD, buff, size );

		#ifdef KDEBUG_KSHELL_TEST_APP
		klog( "size of app: %d\n", size );
		klog( "bytes read: %d\n", bytes_read );

		kdebug_peek_at_n( buff, size / 0xF );
		#endif

		char args[4][100];
		char *argv_builder[4];

		strcpy( args[0], "cat" );
		strcpy( args[1], "/etc/magic_key" );

		for( int z = 0; z < 4; z++ ) {
			argv_builder[z] = args[z];
		} 

		//printf( "argv: 0x%08X\n", argv_builder);
		loader_load_and_run( buff, 2, argv_builder );

		//typedef void (*module_main_func)( int argc, char *argv[] );

/* 		void (*enter)( int argc, char *argv[] ) = elf_header->e_entry;

		klog( "entry at 0x%08X\n", enter );

		enter( 2, argv_builder ); */

		free( buff );
	}

	log_entry_exit();
}

void kshell_echo_to_serial( void ) {
	log_entry_enter();

	int FD = open( "/lib/picard_history.txt", 0 );

	if( FD == -1 ) {
		printf( "cannot open /lib/picard_history.txt\n" );
	} else {

		int size = get_file_size(FD);
		uint8_t *buff = malloc( size );
		memset( buff, 0, 2048 );

		int bytes_read = read( FD, buff, size );

		klog( "ph size: %d\n", size );
		klog( "ph bytes read: %d\n", bytes_read );

		klog( "begin out\n---------\n" );
		klog( "%s", buff );
		klog( "\n----------\nend out\n" );
	}

	log_entry_exit();
}