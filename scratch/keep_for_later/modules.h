#if !defined(MODULES_INCLUDED)
#define MODULES_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef void (*module_init_func)( void );
typedef void (*module_exit_func)( void );
typedef void (*module_main_func)( int argc, char *argv[] );

class Module {
	private:
		uint32_t	elf_object_start_addr;
		uint32_t	elf_object_size;

		uint32_t *	VIRT_HEAP_BASE;
		void *		code_start_virt;
		void *		code_start_phys;
		uint32_t *	stack;
		void *		data_start_virt;
		void *		data_start_phys;
		
		char		name[25];

		module_init_func init;
		module_exit_func exit;
		module_main_func main;

		bool		successfully_loaded = false;

		void setup_pages( void );
	public:
		uint32_t	task_id;

		bool load( uint32_t *module_start, char *name );
		void call_init( void );
		void call_exit( void );
		int call_main( void );
};

#ifdef __cplusplus
}
#endif

#endif