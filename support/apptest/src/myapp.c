#include <stdio.h>
#include <stdbool.h>

extern void task_exit( void );

#undef KDEBUG_CAT
int main( int argc, char *argv[] ) {
	bool show_help = false;

	printf( "argc: %d\n", argc );
	printf( "argv: 0x%08X\n", argv );

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

	task_exit();

	return 0;
}