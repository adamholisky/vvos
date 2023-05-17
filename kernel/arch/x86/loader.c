#include <kernel.h>
#include <elf.h>
#include <loader.h>
#include <task.h>
#include <syscall.h>

#define KDEBUG_LOADER_LOAD_AND_RUN
/**
 * @brief Load and run the given program
 * 
 * @param buff pointer to the file's data
 * @param argc number of arugments
 * @param argv string array of arguments
 * @return int 
 */
int loader_load_and_run( uint8_t *buff, int argc, char *argv[] ) {
    log_entry_enter();

    uint32_t task_id = loader_load( buff, argv[0] );

    //printf( "argv: 0x%08X\n", argv );
    //printf( "argv[0]: \"%s\"\n", argv[0] );

    task *tasks = get_tasks();

    uint32_t *stack = tasks[ task_id ].stack_top;
    *(stack - 0) = argv;
    *(stack - 1) = argc;

    if( task_id != 0 ) {
        sched_yield_to( task_id );
    } else {
        printf( "Loader: failed to load program.\n" );
    }

    log_entry_exit();
}

#define KDEBUG_LOADER_LOAD
/**
 * @brief Loads the program, links accordingly
 * 
 * @param buff pointer to the raw data
 * @return true program has been loaded and is ready to run
 * @return false program has failed to load
 */
uint32_t loader_load( uint8_t *buff, char *name ) {
    log_entry_enter();

    // put it into a task
    uint32_t *page_start = page_allocate(1);
    uint32_t *phys_page = mem_virt_to_phys( page_start );
    uint32_t *page = page_map( (uint32_t *)0x00000000, phys_page );
    //memcpy( page, buff, size );

    // resolve task symbols
    Elf32_Ehdr *elf_header = (Elf32_Ehdr *)buff;
    Elf32_Shdr* rel_plt = elf_find_rel_plt((uint32_t*)buff, elf_header);

    elf_load_program_headers( elf_header, (uint8_t *)page, buff );

    if (rel_plt != NULL) {
        uint32_t* data = (uint32_t*)((uint8_t*)page + rel_plt->sh_offset);

        #ifdef KDEBUG_LOADER_LOAD
        debugf( "raw data start: %X\n", page );
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

    
    Elf32_Shdr* got_plt = elf_find_got_plt((uint32_t*)buff, elf_header);
    if (got_plt != NULL) {
        uint32_t* data = (uint32_t*)(page + got_plt->sh_addr);

        #ifdef KDEBUG_LOADER_LOAD
        debugf(".got.plt out:\n");
        for (int j = 0; j < (got_plt->sh_size/4); j++) {
            debugf("%08X\t", (uint32_t) * (data + j));
        }
        debugf("\n\n");
        #endif
    }
    else {
        klog("Could not find .got.plt section\n");
        do_immediate_shutdown();
    }


    klog( "rel_plt num: %d\n", (rel_plt->sh_size/4)/2 );
    // Loop through the rel_plt and updating the corresponding GOT entry
    for(int rel_num = 0; rel_num < (rel_plt->sh_size/4)/2; rel_num++) {
        Elf32_Rel *elf_rel = (Elf32_Rel*)((uint8_t*)buff + rel_plt->sh_offset + (rel_num * sizeof(Elf32_Rel)));

        if( elf_get_sym_shndx_from_index((uint32_t*)buff, elf_header, ELF32_R_SYM(elf_rel->r_info)) == 0 ) {
            uint32_t *got_entry = (uint32_t*)((uint32_t)page + elf_rel->r_offset);
            *got_entry = (uint32_t)kdebug_get_symbol_addr( elf_get_sym_name_from_index((uint32_t*)buff, elf_header, ELF32_R_SYM(elf_rel->r_info)) );

            #ifdef KDEBUG_LOADER_LOAD
            klog( "GOT entry: 0x%X\n", *got_entry );
            klog( "rel sym: 0x%08X, %d, %d, %X, %s\n", elf_rel->r_offset, ELF32_R_TYPE(elf_rel->r_info), ELF32_R_SYM(elf_rel->r_info),  elf_get_sym_shndx_from_index((uint32_t*)buff, elf_header, ELF32_R_SYM(elf_rel->r_info)), elf_get_sym_name_from_index((uint32_t*)buff, elf_header, ELF32_R_SYM(elf_rel->r_info)) );
            #endif
        } else {
            klog( "Should not go here.\n" );
            // Link main -- I think I'm doing something wrong by having to do this, maybe not handling got right?
            uint32_t *got_entry = (uint32_t*)(buff + elf_rel->r_offset);

            *got_entry = (uint32_t)elf_get_sym_value_from_index((uint32_t*)buff, elf_header, ELF32_R_SYM(elf_rel->r_info));
        }

        
    }

    // dump final got
    #ifdef KDEBUG_LOADER_LOAD
    uint32_t* got_final = (uint32_t*)((uint32_t)page + got_plt->sh_addr);
    debugf(".got.plt out:\n");
    for (int j = 0; j < (got_plt->sh_size/4); j++) {
        debugf("%08X\t", (uint32_t) * (got_final + j));
    }
    debugf("\n\n");
    #endif

    task *t = kmalloc( sizeof(task) );
    uint32_t task_id;

    t->entry = elf_header->e_entry;
    t->stack_base = kmalloc(16 * 1024);
    t->stack_top = t->stack_base + (16 * 1024);
    t->type = TASK_TYPE_PROCESS;
    strcpy( t->name, name );

    task_id = task_add( t );

    return task_id;
}