#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <kmalloc.h>
#include <elf.h>
#include <task.h>
#include <debug.h>
#include <modules.h>

//#define kdebug_module 

void Module::call_init( void ) {
	x86_context * old_context = (x86_context *)kmalloc( sizeof(x86_context) );
	int32_t old_task_id = get_current_task_id();

	change_to_partial_task_context( this->task_id, old_context );
	this->init();
	restore_from_partial_task_context( old_task_id, this->task_id, old_context );

	kfree( (void *)old_context );
}

void Module::call_exit( void ) {
	x86_context * old_context = (x86_context *)kmalloc( sizeof(x86_context) );
	int32_t old_task_id = get_current_task_id();

	change_to_partial_task_context( this->task_id, old_context );
	this->exit();
	restore_from_partial_task_context( old_task_id, this->task_id, old_context );

	kfree( (void *)old_context );
}

void Module::call_main( void ) {
	x86_context * old_context = (x86_context *)kmalloc( sizeof(x86_context) );
	int32_t old_task_id = get_current_task_id();

	change_to_partial_task_context( this->task_id, old_context );
	this->main( 0, NULL );
	restore_from_partial_task_context( old_task_id, this->task_id, old_context );

	kfree( (void *)old_context );
}

void Module::setup_pages( void ) {
	// Setup Code Page
	this->code_start_virt = page_allocate( 1 );
	uint32_t *code_start_uint32 = reinterpret_cast<uint32_t *>(this->code_start_virt);
	this->code_start_phys = reinterpret_cast<void *>(code_start_uint32 - this->VIRT_HEAP_BASE + get_physical_memory_base());

	// Setup Stack Page
	this->stack = page_allocate( 1 );

	// Setup Data Page
	this->data_start_virt = page_allocate( 1 );
	uint32_t *data_start_uint32 = reinterpret_cast<uint32_t *>(this->data_start_virt);
	this->data_start_phys =reinterpret_cast<void *>(data_start_uint32 - this->VIRT_HEAP_BASE + get_physical_memory_base());

	#ifdef kdebug_module
		klog( "code_start_virt: 0x%08X\n", this->code_start_virt );
		klog( "code_start_phys: 0x%08X\n", this->code_start_phys );
	#endif
}

void Module::load( uint32_t *raw_data_start ) {
	task *module_task = (task *)kmalloc( sizeof( task ) );
	this->VIRT_HEAP_BASE = reinterpret_cast<uint32_t *>(KERNEL_VIRT_HEAP_BASE);
	
	// Setup Pages and copy to task
	this->setup_pages();
	module_task->code_start_virt = this->code_start_virt;
	module_task->code_start_phys = this->code_start_phys;
	module_task->stack = this->stack;
	module_task->data_start_virt = this->data_start_virt;
	module_task->data_start_phys = this->data_start_phys;

	// Parse raw data to identify secrtions to load, and to copy them into memory
	uint8_t *process_space = reinterpret_cast<uint8_t *>( this->code_start_virt );
	Elf32_Ehdr *elf_header = (Elf32_Ehdr *)raw_data_start;

	#ifdef kdebug_module
		klog( "proccess_space: 0x%08X\n", process_space);
		klog( "elf_header: 0x%08X\n", elf_header);
	#endif
	
	// Locate the REL PLT
	Elf32_Shdr* rel_plt = elf_find_rel_plt((uint32_t*)raw_data_start, elf_header);
    if (rel_plt != NULL) {
        uint32_t* data = (uint32_t*)((uint8_t*)raw_data_start + rel_plt->sh_offset);

		#ifdef kdebug_module
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

	// Locate the GOT PLT
	Elf32_Shdr* got_plt = elf_find_got_plt((uint32_t*)raw_data_start, elf_header);
    if (got_plt != NULL) {
        uint32_t* data = (uint32_t*)(process_space + got_plt->sh_addr);

		#ifdef kdebug_module
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

	// Setup Entry Point
	module_task->entry = (void *)elf_header->e_entry;
		
	// Abandon if we can't locate REL or GOT PLT
    if (rel_plt == NULL) {
		klog( "rel_plt is null\n");
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

			#ifdef kdebug_module
				klog( "rel sym: 0x%08X, %d, %d, %X, %s\n", elf_rel->r_offset, ELF32_R_TYPE(elf_rel->r_info), ELF32_R_SYM(elf_rel->r_info),  elf_get_sym_shndx_from_index((uint32_t*)raw_data_start, elf_header, ELF32_R_SYM(elf_rel->r_info)), elf_get_sym_name_from_index((uint32_t*)raw_data_start, elf_header, ELF32_R_SYM(elf_rel->r_info)) );
			#endif
		} else {
			// Link main -- I think I'm doing something wrong by having to do this, maybe not handling got right?
			uint32_t *got_entry = (uint32_t*)(process_space + elf_rel->r_offset);

			*got_entry = (uint32_t)elf_get_sym_value_from_index((uint32_t*)raw_data_start, elf_header, ELF32_R_SYM(elf_rel->r_info));
		}
	}

	#ifdef kdebug_module
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

	this->task_id = task_add( module_task );
	task_set_modhack( this->task_id );

	this->main = reinterpret_cast<module_main_func>(module_task->entry);

	for( int sym_i = 0; sym_i < module_task->num_syms; sym_i++ ) {
		if( strcmp( "module_init", (char *)(module_task->str_table + module_task->sym_table[sym_i].st_name) ) == 0 ) {
			klog( "Found module_init() at 0x%08X\n", module_task->sym_table[sym_i].st_value );
			this->init = (module_init_func)module_task->sym_table[sym_i].st_value;
		}

		if( strcmp( "module_exit", (char *)(module_task->str_table + module_task->sym_table[sym_i].st_name) ) == 0 ) {
			klog( "Found module_exit() at 0x%08X\n", module_task->sym_table[sym_i].st_value );
			this->exit = (module_exit_func)module_task->sym_table[sym_i].st_value;
		}
	}

	//klog( "task_id: %x\n", this->task_id );
}