#ifndef KSHELL_INCLUDED
#define KSHELL_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif



void kshell_get_line( void );
void kshell_process_line( void );
void kshell_fake_cli( char *cmd ); 
void kshell_automate( char *cmd_line );
void kshell_run( void );
void kshell_shutdown( void );

void kshell_ps( void );
void kshell_cat( int argc, char *argv[] );

void kshell_test_fork( void );
void kshell_divide_by_zero( void );
void kshell_test_devices( void );
void test_syscall( void );
void kshell_test_loaded_file( void );
void kshell_afs_test_alpha( void );
void kshell_test_lib_call( void );
void kshell_test_app( void );

void kshell_echo_to_serial( void );

#ifdef __cplusplus
}
#endif
#endif