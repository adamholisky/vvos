#include "kernel.h"
#include "debug.h"
#include "elf.h"
#include "modules_bootstrap.h"
#include "task.h"

//#define kdebug_process_loader

uint32_t num_modules;
kmodule *modules;

void modules_initalize( void ) {
	char *postfix_start;
	char name_temp[50];
	char end_name_temp[50];
	kdebug_symbol *end_symbol;
	uint32_t module_obj_size;

	// Search the symbol table for the matching strings
	kdebug_symbol * syms = kdebug_get_symbol_array();
	modules = kmalloc( sizeof(kmodule) * MODULES_MAX );

	for( int i = 0; i < kdebug_get_total_symbols(); i++ ) {
		postfix_start = NULL;
		end_symbol = NULL;
		module_obj_size = 0;

		if( syms[i].name[0] == 0 ) continue; 

		//klog( "name: %d %s\n", i, syms[i].name );

		strcpy( name_temp, syms[i].name );

		if( strstr(name_temp, "vvs_app_") != NULL ) {
			postfix_start = strstr(name_temp, "_start");

			if( postfix_start != NULL ) {
				// Extract and copy over the name of the module
				postfix_start[0] = 0;
				char *name_start = name_temp;
				name_start = name_start + 9;
				strcpy( modules[num_modules].name, name_start );

				// Store the start of the ELF object
				modules[num_modules].elf_object_start_addr = syms[i].addr;

				// Calculate the size of the ELF object representing the module
				strcpy( end_name_temp, ".vvs_app_" );
				strcat( end_name_temp, name_start );
				strcat( end_name_temp, "_end" );
				end_symbol = kdebug_get_symbol( end_name_temp );
				module_obj_size = end_symbol->addr - syms[i].addr;
				modules[num_modules].elf_object_size = module_obj_size;

				klog( "Module found: %s @ 0x%08X for 0x%X bytes\n", modules[num_modules].name, modules[num_modules].elf_object_start_addr, modules[num_modules].elf_object_size );
				num_modules++;
			}
		}
	}
	
	// Process each module into its own process
	for(int j = 0; j < num_modules; j++) {
		load_module( (modules + j) );
	}

	// Profit?
}

void load_module( kmodule *mod ) {
	#ifdef kdebug_process_loader
	klog( "Loading %s...\n", mod->name);
	#endif
	uint32_t task_id = load_module_elf_image( (uint32_t *)mod->elf_object_start_addr );
	mod->task_id = task_id;
	task_set_name( task_id, mod->name );
}

void run_module_by_name( char * name ) {
	for( int i = 0; i < num_modules; i++ ) {
		if( strcmp( name, modules[i].name ) == 0 ) {
			//klog( "running %d\n", modules[i].task_id );
			task_initalize_and_run( modules[i].task_id );
		}
	}
}

uint32_t load_module_elf_image( uint32_t *raw_data_start ) {
	// Locate the raw data, confirm it's what we expect (ELF Ident)
	// debugf( "raw_data_start: 0x%08X\n", raw_data_start);
	// kdebug_peek_at( raw_data_start );

	// Setup the process
	task *module_task = kmalloc( sizeof( task ) );

	module_task->code_start_virt = page_allocate( 1 );
	module_task->code_start_phys = module_task->code_start_virt - (void *)KERNEL_VIRT_HEAP_BASE + (void *)get_physical_memory_base();
	module_task->stack = page_allocate( 1 );
	module_task->data_start_virt = page_allocate( 1 );
	module_task->data_start_phys = module_task->data_start_virt - (void *)KERNEL_VIRT_HEAP_BASE + (void *)get_physical_memory_base();

	#ifdef kdebug_process_loader
	klog( "code_start_virt: 0x%08X\n", module_task->code_start_virt );
	klog( "code_start_phys: 0x%08X\n", module_task->code_start_phys );
	#endif

	// Parse raw data to identify secrtions to load, and to copy them into memory
	uint8_t *process_space = module_task->code_start_virt;
	Elf32_Ehdr *elf_header = (Elf32_Ehdr *)raw_data_start;

	#ifdef kdebug_process_loader
	klog( "proccess_space: 0x%08X\n", process_space);
	klog( "elf_header: 0x%08X\n", elf_header);
	#endif

	Elf32_Shdr* rel_plt = elf_find_rel_plt((uint32_t*)raw_data_start, elf_header);
    if (rel_plt != NULL) {
        uint32_t* data = (uint32_t*)((uint8_t*)raw_data_start + rel_plt->sh_offset);

		#ifdef kdebug_process_loader
		debugf( "raw data start: %X\n", raw_data_start );
		debugf( "rel_plt:sh_offset %X\n", rel_plt->sh_offset);
		debugf("data %X %x\n", data, *data );
        debugf(".rel.plt out:\n");
        for (int j = 0; j < (rel_plt->sh_size/4); j++) {
            debugf("%08X\t", (uint32_t) * (data + j));
        }
        debugf("\n\n");
		#endif
    }
    else {
        klog("Could not find .rel.plt section\n");
    }

    elf_load_program_headers(elf_header, (uint8_t *)process_space, (uint8_t *)raw_data_start);

	/* I have no idea why I did this or what 0x1288 means...
	#ifdef kdebug_process_loader
	for (int k = 0x1288; k < 0x12a0; k = k + 4) {
        debugf("0x%03X: %02X %02X %02X %02X\n", k, (uint8_t) * ((uint8_t*)process_space + k), (uint8_t) * ((uint8_t*)process_space + k + 1), (uint8_t) * ((uint8_t*)process_space + k + 2), (uint8_t) * ((uint8_t*)process_space + k + 3));
    }
	#endif 
	*/

	Elf32_Shdr* got_plt = elf_find_got_plt((uint32_t*)raw_data_start, elf_header);
    if (got_plt != NULL) {
        uint32_t* data = (uint32_t*)(process_space + got_plt->sh_addr);

		#ifdef kdebug_process_loader
        debugf(".got.plt out:\n");
        for (int j = 0; j < (got_plt->sh_size/4); j++) {
            debugf("%08X\t", (uint32_t) * (data + j));
        }
        debugf("\n\n");
		#endif
    }
    else {
        klog("Could not find .got.plt section\n");
    }

	module_task->entry = (void *)elf_header->e_entry;
	
	//debugf( "entry: 0x%08X\n", module_task->entry);
	
    if (rel_plt == NULL) {
		klog( "rel_pplt is null\n");
    }

    if (got_plt == NULL) {
       klog( "got_plt is null\n");
    }

	// Loop through the rel_plt and updating the corresponding GOT entry
	for(int rel_num = 0; rel_num < (rel_plt->sh_size/4)/2; rel_num++) {
		Elf32_Rel *elf_rel = (Elf32_Rel*)((uint8_t*)raw_data_start + rel_plt->sh_offset + (rel_num * sizeof(Elf32_Rel)));

		if( elf_get_sym_shndx_from_index((uint32_t*)raw_data_start, elf_header, ELF32_R_SYM(elf_rel->r_info)) == 0 ) {
			uint32_t *got_entry = (uint32_t*)(process_space + elf_rel->r_offset);
			*got_entry = (uint32_t)kdebug_get_symbol_addr( elf_get_sym_name_from_index((uint32_t*)raw_data_start, elf_header, ELF32_R_SYM(elf_rel->r_info)) );

			#ifdef kdebug_process_loader
			klog( "rel sym: 0x%08X, %d, %d, %X, %s\n", elf_rel->r_offset, ELF32_R_TYPE(elf_rel->r_info), ELF32_R_SYM(elf_rel->r_info),  elf_get_sym_shndx_from_index((uint32_t*)raw_data_start, elf_header, ELF32_R_SYM(elf_rel->r_info)), elf_get_sym_name_from_index((uint32_t*)raw_data_start, elf_header, ELF32_R_SYM(elf_rel->r_info)) );
			#endif
		} else {
			// Link main -- I think I'm doing something wrong by having to do this, maybe not handling got right?
			uint32_t *got_entry = (uint32_t*)(process_space + elf_rel->r_offset);

			*got_entry = (uint32_t)elf_get_sym_value_from_index((uint32_t*)raw_data_start, elf_header, ELF32_R_SYM(elf_rel->r_info));
		}
		
		//#ifdef kdebug_process_loader
		
		//#endif
	}

	#ifdef kdebug_process_loader
	uint32_t* data2 = (uint32_t*)((uint32_t)process_space + got_plt->sh_addr);
	debugf(".got.plt out:\n");
	for (int x = 0; x < (got_plt->sh_size/4); x++) {
		debugf("%08X\t", (uint32_t) * (data2 + x));
	}
	debugf("\n\n");
	

	debugf( "    module_task entry:      0x%08X\n", module_task->entry);
	debugf( "    module_task code virt:  0x%08X\n", module_task->code_start_virt );
	debugf( "    module_task stack virt: 0x%08X\n", module_task->stack );
	#endif

	Elf32_Shdr * strtab_shdr = elf_find_sym_strtab( (uint32_t*)raw_data_start, elf_header );
	Elf32_Shdr * elf_sym_shdr = elf_find_symtab( raw_data_start, elf_header); 


	module_task->raw_data = (uint8_t *)raw_data_start;
	module_task->sym_table = (Elf32_Sym*)((uint8_t*)raw_data_start + elf_sym_shdr->sh_offset);
	module_task->str_table = (uint8_t *)raw_data_start + strtab_shdr->sh_offset;
	module_task->num_syms = elf_sym_shdr->sh_size / sizeof( Elf32_Sym );

	module_task->context.eip = (uint32_t)module_task->entry;

	module_task->type = TASK_TYPE_MODULE;

	uint32_t task_id = task_add( module_task );
	task_set_modhack( task_id );

	return task_id;
}