#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "elf.h"
#include "printf.h"
#include "multiboot.h"
#include "debug.h"
#include "terminal.h"
#include "string.h"
#include "memory.h"

void elf_initalize( uint32_t kmain ) {
    //log_entry_enter();

	Elf32_Shdr *section_headers;
	Elf32_Sym *symbol_table;
	uint32_t symtable_num_entries = 0;
	int i = 0;
	uint32_t name_addr = 0;
	int string_index = 0;
	int symtable_index = 0;

	multiboot_info_t *multiboot_header = get_multiboot_header();

	section_headers = (Elf32_Shdr *)(multiboot_header->u.elf_sec.addr);

	for( i = 0; i < multiboot_header->u.elf_sec.num; i++ ) {
		if( section_headers[i].sh_type == SHT_STRTAB ) {
			string_index = i;
 			i = 99999;
		}
	}

	for( i = 0; i < multiboot_header->u.elf_sec.num; i++ ) {
		if( section_headers[i].sh_type == SHT_SYMTAB ) {
			symtable_index = i;
			symtable_num_entries = section_headers[symtable_index].sh_size / sizeof(Elf32_Sym);

			i = 99999;
		}
	}

	name_addr = section_headers[string_index].sh_addr + KERNEL_VIRT_LOAD_BASE;
	symbol_table = (Elf32_Sym *)(section_headers[symtable_index].sh_addr + KERNEL_VIRT_LOAD_BASE );



	for( i = 1; i < symtable_num_entries - 1; i++ ) {
		kdebug_add_symbol( (char *)(name_addr + symbol_table[i].st_name), symbol_table[i].st_value, symbol_table[i].st_size );
	}

    /* 	klog( "0x%08X is in function: %s\n", kmain + 0x10, kdebug_get_function_at( kmain + 0x10 )); */

    log_entry_exit();
}


void elf_load_program_headers(Elf32_Ehdr* elf_header, uint8_t* process_space, uint8_t* data) {
    Elf32_Phdr* elf_pheader = NULL;

    for (int i = 0; i < elf_header->e_phnum; i++) {
        elf_pheader = (Elf32_Phdr*)((uint32_t)elf_header + elf_header->e_phoff + (elf_header->e_phentsize * i));

        if (elf_pheader->p_type == 1) {
            uint8_t* source = (uint8_t*)(data + elf_pheader->p_offset);
            uint8_t* dest = (uint8_t*)(process_space + elf_pheader->p_vaddr);

            #ifdef kdebug_elf
            klog("Loading 0x%X bytes to physical/virtual 0x%08X / 0x%08X.\n", elf_pheader->p_filesz, elf_pheader->p_paddr, elf_pheader->p_vaddr);
            #endif

            memcpy(dest, source, elf_pheader->p_filesz);

           /*  for( int i = 0; i < 3; i++ ) {
                uint32_t *srci = (uint32_t*)source + i;
                klog( "Source 0x%08X: 0x%08X\n", srci, *srci);
            }

            for( int i = 0; i < 3; i++ ) {
                uint32_t *desti = (uint32_t*)dest + i;
                klog( "Dest 0x%08X: 0x%08X\n", desti, *desti);
            } */
        }
    }
}

Elf32_Shdr * elf_find_rel_plt(uint32_t* mem, Elf32_Ehdr* elf_header) {
    Elf32_Shdr* ret = NULL;

    for (int i = 0; i < elf_header->e_shnum; i++) {
        Elf32_Shdr* section = (Elf32_Shdr*)(((uint32_t)mem + elf_header->e_shoff) + (sizeof(Elf32_Shdr) * i));

        if (section->sh_type == SHT_REL) {
            #ifdef kdebug_elf
            klog("rel.plt section: [%d / %x @ 0x%08X] %d    0x%08X    0x%08X\n", i, i, (uint32_t)section, section->sh_type, section->sh_addr, section->sh_offset);
            #endif
            ret = section;
        }
    }

    return ret;
}

Elf32_Shdr * elf_find_symtab(uint32_t* mem, Elf32_Ehdr* elf_header) {
    Elf32_Shdr* ret = NULL;

    for (int i = 0; i < elf_header->e_shnum; i++) {
        Elf32_Shdr* section = (Elf32_Shdr*)(((uint32_t)mem + elf_header->e_shoff) + (sizeof(Elf32_Shdr) * i));

        if (section->sh_type == SHT_SYMTAB) {
            ret = section;
        }
    }

    return ret;
}

Elf32_Shdr * elf_find_sym_strtab(uint32_t* mem, Elf32_Ehdr* elf_header) {
    Elf32_Shdr* ret = NULL;

    for (int i = 0; i < elf_header->e_shnum; i++) {
        Elf32_Shdr* section = (Elf32_Shdr*)(((uint32_t)mem + elf_header->e_shoff) + (sizeof(Elf32_Shdr) * i));

        if (section->sh_type == SHT_STRTAB && strcmp(elf_get_section_name(mem, elf_header, i), ".strtab") == 0) {
            ret = section;
        }
    }

    return ret;
}

Elf32_Shdr * elf_find_dynsym_tab(uint32_t* mem, Elf32_Ehdr* elf_header) {
    Elf32_Shdr* ret = NULL;

    for (int i = 0; i < elf_header->e_shnum; i++) {
        Elf32_Shdr* section = (Elf32_Shdr*)(((uint32_t)mem + elf_header->e_shoff) + (sizeof(Elf32_Shdr) * i));

        if (section->sh_type == SHT_DYNSYM && strcmp(elf_get_section_name(mem, elf_header, i), ".dynsym") == 0) {
            ret = section;
        }
    }

    return ret;
}

Elf32_Shdr * elf_find_dynstr_tab(uint32_t* mem, Elf32_Ehdr* elf_header) {
    Elf32_Shdr* ret = NULL;

    for (int i = 0; i < elf_header->e_shnum; i++) {
        Elf32_Shdr* section = (Elf32_Shdr*)(((uint32_t)mem + elf_header->e_shoff) + (sizeof(Elf32_Shdr) * i));

        if (section->sh_type == SHT_STRTAB && strcmp(elf_get_section_name(mem, elf_header, i), ".dynstr") == 0) {
            ret = section;
        }
    }

    return ret;
}


Elf32_Shdr* elf_find_got_plt(uint32_t* mem, Elf32_Ehdr* elf_header) {
    Elf32_Shdr* ret = NULL;
    char* sec_name = NULL;

    for (int i = 0; i < elf_header->e_shnum; i++) {
        Elf32_Shdr* section = (Elf32_Shdr*)(((uint32_t)mem + elf_header->e_shoff) + (sizeof(Elf32_Shdr) * i));

        if (section->sh_type == SHT_PROGBITS) {
            sec_name = elf_get_section_name(mem, elf_header, i);
            //printf("sn: %s\n", sec_name);
            if (strcmp(sec_name, ".got.plt") == 0) {
                #ifdef kdebug_elf
                klog("got.plt section: [%d / %x @ 0x%08X] %d    0x%08X    0x%08X\n", i, i, (uint32_t)section, section->sh_type, section->sh_addr, section->sh_offset);
                #endif
                ret = section;
            }
        }
    }

    return ret;
}

char* elf_get_section_name(uint32_t* mem, Elf32_Ehdr* elf_header, uint32_t sec_num) {
    Elf32_Shdr* elf_str_shdr = (Elf32_Shdr*)(((uint32_t)elf_header + elf_header->e_shoff) + (sizeof(Elf32_Shdr) * elf_header->e_shstrndx));
    uint32_t elf_str_offset = elf_str_shdr->sh_offset;
    Elf32_Shdr* section = (Elf32_Shdr*)(((uint32_t)mem + elf_header->e_shoff) + (sizeof(Elf32_Shdr) * sec_num));

    char* ret = (char*)((char*)mem + elf_str_offset + section->sh_name);

    return ret;
}



char* elf_get_sym_name_from_index(uint32_t* mem, Elf32_Ehdr* elf_header, uint32_t index) {
	Elf32_Shdr *elf_sym_shdr = elf_find_dynsym_tab(mem, elf_header); 
	Elf32_Shdr *elf_str_shdr = elf_find_sym_strtab(mem, elf_header);
	Elf32_Shdr *elf_dynstr_shdr = elf_find_dynstr_tab(mem, elf_header);
    //Elf32_Shdr *elf_str_shdr = (Elf32_Shdr*)(((uint32_t)elf_header + elf_header->e_shoff) + (sizeof(Elf32_Shdr) * elf_header->e_shstrndx));

	Elf32_Sym* elf_sym = (Elf32_Sym*)(((uint8_t*)mem + elf_sym_shdr->sh_offset) + (sizeof(Elf32_Sym) * index));
	//debugf("st_name: %d\n", elf_sym->st_name);

	//kdebug_peek_at( (uint8_t)mem + elf_dynstr_shdr->sh_offset );

    char* ret = (char*)((char*)mem + elf_dynstr_shdr->sh_offset + elf_sym->st_name);

    return ret;
}

uint16_t elf_get_sym_shndx_from_index(uint32_t* mem, Elf32_Ehdr* elf_header, uint32_t index) {
	Elf32_Shdr *elf_sym_shdr = elf_find_dynsym_tab(mem, elf_header); 
	Elf32_Shdr *elf_str_shdr = elf_find_sym_strtab(mem, elf_header);
	Elf32_Shdr *elf_dynstr_shdr = elf_find_dynstr_tab(mem, elf_header);
    //Elf32_Shdr *elf_str_shdr = (Elf32_Shdr*)(((uint32_t)elf_header + elf_header->e_shoff) + (sizeof(Elf32_Shdr) * elf_header->e_shstrndx));

	Elf32_Sym* elf_sym = (Elf32_Sym*)(((uint8_t*)mem + elf_sym_shdr->sh_offset) + (sizeof(Elf32_Sym) * index));
	//debugf("st_name: %d\n", elf_sym->st_name);

	//kdebug_peek_at( (uint8_t)mem + elf_dynstr_shdr->sh_offset );

    return elf_sym->st_shndx;
}

uint16_t elf_get_sym_value_from_index(uint32_t* mem, Elf32_Ehdr* elf_header, uint32_t index) {
	Elf32_Shdr *elf_sym_shdr = elf_find_dynsym_tab(mem, elf_header); 
	Elf32_Shdr *elf_str_shdr = elf_find_sym_strtab(mem, elf_header);
	Elf32_Shdr *elf_dynstr_shdr = elf_find_dynstr_tab(mem, elf_header);
    //Elf32_Shdr *elf_str_shdr = (Elf32_Shdr*)(((uint32_t)elf_header + elf_header->e_shoff) + (sizeof(Elf32_Shdr) * elf_header->e_shstrndx));

	Elf32_Sym* elf_sym = (Elf32_Sym*)(((uint8_t*)mem + elf_sym_shdr->sh_offset) + (sizeof(Elf32_Sym) * index));
	//debugf("st_name: %d\n", elf_sym->st_name);

	//kdebug_peek_at( (uint8_t)mem + elf_dynstr_shdr->sh_offset );

    return elf_sym->st_value;
}