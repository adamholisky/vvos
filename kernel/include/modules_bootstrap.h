#if !defined(MODULES_BOOTSTRAP_INCLUDED)
#define MODULES_BOOTSTRAP_INCLUDED

#define MODULES_MAX 25

typedef struct {
	uint32_t	task_id;
	uint32_t	elf_object_start_addr;
	uint32_t	elf_object_size;
	
	char		name[25];

	void		*entry;
} kmodule;

void modules_initalize( void );
void load_module( kmodule *mod );
uint32_t load_module_elf_image( uint32_t *raw_data_start );
void run_module_by_name( char * name );

#endif