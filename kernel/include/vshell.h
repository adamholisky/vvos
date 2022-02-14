#ifndef VSHELL_INCLUDED
#define VSHELL_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <ftp.h>
#include <modules.h>

class VShell {
	private:
		char line[256];
		char jail_env[256];
		char wd[256];
		char* dir_bin;
		char* dir_usr_bin;
		int line_pos;
		FTP* ftp;
		Module *module;

		void get_line( void );
		void process_line( void );
		void ls( void );
		void shutdown( void );
	public:
		void init( FTP *f, Module *m );
		void run( void );
		char *get_jailed_path( void );
		bool cd( char *path );
		bool file_exists( char *path, char *file );
		void cat( char *file );
		void load_and_run( char* file_path );
};

#ifdef __cplusplus
}
#endif
#endif