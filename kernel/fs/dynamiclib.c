#include <kernel.h>
#include <dlfcn.h>
#include <fs.h>
#include <afs.h>

dl_info dl;

#define KDEBUG_DLOPEN
/**
 * @brief Opens a dynamlic library, readying it for use
 * 
 * @param filename Path + filename of the library
 * @param flags DL flags
 * @return void* Pointer to the DL handle for use
 */
void *dlopen( const char *filename, int flags ) {
    int FD = open( filename, 0 );

    if( FD == -1 ) {
        klog( "Open returned -1.\n" );

        return NULL;
    }

    int file_size = get_file_size(FD);
    dl.base = malloc( file_size );
    
    int bytes_read = read( FD, dl.base, file_size );
    Elf32_Ehdr *elf_header = (Elf32_Ehdr *)dl.base;

    #ifdef KDEBUG_DLOPEN
    klog( "file size:  %d\n", file_size );
    klog( "bytes read: %d\n", bytes_read );
    klog( "base:       0x%X\n", dl.base );
    klog( "elf_header: 0x%X\n", elf_header );
    klog( "elf.e_ident: \"%s\"\n", elf_header->e_ident );
    #endif
    
    Elf32_Shdr* rel_plt = elf_find_rel_plt((uint32_t*)dl.base, elf_header);
    if (rel_plt != NULL) {
        uint32_t* data = (uint32_t*)((uint8_t*)dl.base + rel_plt->sh_offset);

		#ifdef KDEBUG_DLOPEN
		debugf( "raw data start: %X\n", dl.base );
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

    
	Elf32_Shdr* got_plt = elf_find_got_plt((uint32_t*)dl.base, elf_header);
    if (got_plt != NULL) {
        uint32_t* data = (uint32_t*)(dl.base + got_plt->sh_addr);

		#ifdef KDEBUG_DLOPEN
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


    // Loop through the rel_plt and updating the corresponding GOT entry
	for(int rel_num = 0; rel_num < (rel_plt->sh_size/4)/2; rel_num++) {
		Elf32_Rel *elf_rel = (Elf32_Rel*)((uint8_t*)dl.base + rel_plt->sh_offset + (rel_num * sizeof(Elf32_Rel)));

		if( elf_get_sym_shndx_from_index((uint32_t*)dl.base, elf_header, ELF32_R_SYM(elf_rel->r_info)) == 0 ) {
			uint32_t *got_entry = (uint32_t*)((uint32_t)dl.base + elf_rel->r_offset);
			*got_entry = (uint32_t)kdebug_get_symbol_addr( elf_get_sym_name_from_index((uint32_t*)dl.base, elf_header, ELF32_R_SYM(elf_rel->r_info)) );

			#ifdef KDEBUG_DLOPEN
            klog( "GOT entry: 0x%X\n", *got_entry );
			klog( "rel sym: 0x%08X, %d, %d, %X, %s\n", elf_rel->r_offset, ELF32_R_TYPE(elf_rel->r_info), ELF32_R_SYM(elf_rel->r_info),  elf_get_sym_shndx_from_index((uint32_t*)dl.base, elf_header, ELF32_R_SYM(elf_rel->r_info)), elf_get_sym_name_from_index((uint32_t*)dl.base, elf_header, ELF32_R_SYM(elf_rel->r_info)) );
			#endif
		} else {
            klog( "Should not go here.\n" );
			// Link main -- I think I'm doing something wrong by having to do this, maybe not handling got right?
			uint32_t *got_entry = (uint32_t*)(dl.base + elf_rel->r_offset);

			*got_entry = (uint32_t)elf_get_sym_value_from_index((uint32_t*)dl.base, elf_header, ELF32_R_SYM(elf_rel->r_info));
		}
	}

    dl.string_section = elf_find_sym_strtab( (uint32_t*)dl.base, elf_header );
	dl.symbol_section = elf_find_symtab( dl.base, elf_header); 


    // dump final got
    uint32_t* got_final = (uint32_t*)((uint32_t)dl.base + got_plt->sh_addr);

    #ifdef KDEBUG_DLOPEN
    debugf(".got.plt out:\n");
    for (int j = 0; j < (got_plt->sh_size/4); j++) {
        debugf("%08X\t", (uint32_t) * (got_final + j));
    }
    debugf("\n\n");
    #endif
}

int dlclose( void *handle ) {
    free(  dl.base );
}

// 4105f0
#define KDEBUG_DLSYM
void *dlsym( void *handle, const char *symbol ) {
    uint32_t *result = NULL;
    int num_syms = dl.symbol_section->sh_size / sizeof( Elf32_Sym );
    Elf32_Sym * sym_table = (Elf32_Sym*)((uint8_t*)dl.base + dl.symbol_section->sh_offset);
	uint8_t * str_table = (uint8_t *)dl.base + dl.string_section->sh_offset;

    #ifdef KDEBUG_DLSYM
    klog( "num syms: %d\n", num_syms );
    #endif

    for( int sym_i = 0; sym_i < num_syms; sym_i++ ) {
        #ifdef KDEBUG_DLSYM
        klog( "sym name: \"%s\"\n", (char *)(str_table + sym_table[sym_i].st_name) );
        #endif

		if( strcmp( symbol, (char *)(str_table + sym_table[sym_i].st_name) ) == 0 ) {
			klog( "Found %s at 0x%08X\n", symbol, sym_table[sym_i].st_value );
			result = (uint32_t)dl.base + (sym_table[sym_i].st_value);
            sym_i = num_syms + 1;
		}
	}

    #ifdef KDEBUG_DLSYM
        klog( "result: 0x%08X\n", result );
    #endif

    return result;
}
