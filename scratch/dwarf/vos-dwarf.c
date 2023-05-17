#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define Elf32_Half uint16_t
#define Elf32_Word uint32_t
#define Elf32_Sword int32_t
#define Elf32_Addr uint32_t
#define Elf32_Off uint32_t

#define EI_NIDENT 16

#define ET_NONE 0 // No file type
#define ET_REL 1 // Relocatable file
#define ET_EXEC 2 // Executable file
#define ET_DYN 3 // Shared object file
#define ET_CORE 4 //Core file
#define ET_LOPROC 0xFF00 // Processor-specific
#define ET_HIPROC 0xFFFF // Processor-specific

#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11

#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4
#define SHF_MASKPROC 0xF0000000

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2

#define ELF32_R_SYM(i) ((i)>>8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))
#define ELF32_R_INFO(s,t) (((s)<<8)+(unsigned char)(t))

#define STT_FUNC 2
#define ELF32_ST_BIND(INFO)	((INFO) >> 4)
#define ELF32_ST_TYPE(INFO)	((INFO) & 0x0F)

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct {
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;

typedef struct {
  uint32_t	sh_name;
  uint32_t	sh_type;
  uint32_t	sh_flags;
  uint32_t	sh_addr;
  uint32_t	sh_offset;
  uint32_t	sh_size;
  uint32_t	sh_link;
  uint32_t	sh_info;
  uint32_t	sh_addralign;
  uint32_t	sh_entsize;
} Elf32_Shdr;

typedef struct {
  uint32_t	st_name;
  uint32_t	st_value;
  uint32_t	st_size;
  unsigned char	st_info;
  unsigned char	st_other;
  uint16_t	st_shndx;
} Elf32_Sym;

typedef struct {
    Elf32_Addr r_offset;
    Elf32_Word r_info;
} Elf32_Rel;

typedef struct {
    Elf32_Addr r_offset;
    Elf32_Word r_info;
    Elf32_Sword r_addend;
} Elf32_Rela;

typedef struct {
    uint32_t length;
    uint16_t version;
    uint8_t unit_type;
    uint8_t size;
    uint32_t offset;
} __attribute__((packed)) dwarf_cu_header;

typedef struct {
    uint32_t num;
    uint8_t bytes[64];
    void *next;
} abbrev_entry;

typedef struct {
    uint8_t name;
    uint8_t value[64];
} die_nv_pair;

typedef struct {
    uint32_t offset;
    uint8_t abbrev_num;
    abbrev_entry *abbrev;

    char name[64];

    die_nv_pair nv_pairs[20];
    uint8_t num_nv_pairs;

    void *next;
} die_entry;

void print_bytes( uint8_t *bytes, int lines );

Elf32_Shdr * elf_get_section_header_by_name( Elf32_Ehdr *elf_header, char * name );

int main( void ) {
    printf( "vOS Dwarf Scratch Work\n" );

    char *buffer;
    long file_length;
    FILE *fp = fopen( "multiboot.o", "rb" );

    fseek( fp, 0, SEEK_END );
	file_length = ftell( fp );
	fseek( fp, 0, SEEK_SET );

    buffer = malloc( file_length + 1 );
    
    fread( buffer, file_length, 1, fp );

    fclose( fp );
    
    printf( "Buffer size: %ld\n", file_length );

    Elf32_Ehdr *elf_header = (Elf32_Ehdr *)buffer;

    printf( "Ident: %s\n", elf_header->e_ident );
    printf( "SH Str Index: %d\n", elf_header->e_shstrndx );

    Elf32_Shdr *elf_s_headers = (Elf32_Shdr *)(buffer + elf_header->e_shoff );

    printf( "SH Str Addr: %d\n", elf_s_headers[ elf_header->e_shstrndx].sh_offset );

    for( int i = 0; i < elf_header->e_shnum; i++ ) {
        printf( "section[%d] %s\n", i, (char *)(buffer + elf_s_headers[elf_header->e_shstrndx].sh_offset + elf_s_headers[i].sh_name) );
    }

    Elf32_Shdr * debug_info = elf_get_section_header_by_name( elf_header, ".debug_info" ) ;

    if( debug_info != NULL ) {
        printf( "Debug info header found\n" );
        printf( "    Size: 0x%X\n", debug_info->sh_size );
    }

    uint32_t *data = (uint32_t *)(buffer + debug_info->sh_offset);

    dwarf_cu_header *dwarf_header = (dwarf_cu_header *)data;

    printf( "length: 0x%X\n", dwarf_header->length );
    printf( "version: 0x%X\n", dwarf_header->version );
    printf( "unit type: 0x%X\n", dwarf_header->unit_type );
    printf( "size: 0x%X (%d)\n", dwarf_header->size, dwarf_header->size );
    printf( "offset: 0x%X (%d)\n", dwarf_header->offset, dwarf_header->offset );

    printf( "debug info: \n" );
    print_bytes( (uint8_t *)(buffer + debug_info->sh_offset), 8 );

    printf( "debug abbrev: \n" );
    Elf32_Shdr * debug_abbrev = elf_get_section_header_by_name( elf_header, ".debug_abbrev" );
    //print_bytes( (uint8_t *)(buffer + debug_abbrev->sh_offset), 8 );

    Elf32_Shdr * debug_str = elf_get_section_header_by_name( elf_header, ".debug_str" );
    uint8_t *debug_string = malloc( debug_str->sh_size );
    memcpy( debug_string, (uint8_t *)(buffer + debug_str->sh_offset), debug_str->sh_size );
    printf( "size of debug_str data: %d\n", debug_str->sh_size );

    printf( "\n" );

    // find name of variable
    printf( "%s\n\n", debug_string );

    int var_name_offset = 0;
    char *var_name = "multiboot_header";
    int var_name_len = strlen( var_name );
    for( int offset = 0; offset < debug_str->sh_size; offset++ ) {
        if( strcmp( (debug_string + offset), var_name ) == 0 ) {
            var_name_offset = offset;
            offset = debug_str->sh_size + 1;
        } else {
            offset = offset + strlen( debug_string + offset );
        }
    }
    printf( "offset of var name: %X\n", var_name_offset );

    // Create the abbrev array
    abbrev_entry *abbrevs = malloc( sizeof(abbrev_entry) );
    abbrev_entry *abbrevs_head = abbrevs;

    uint8_t temp_bytes[64];
    uint32_t num_bytes = 0;
    uint32_t total_bytes = 0;
    bool found_double_zero = false;
    bool end_found = false;
    uint8_t *debug_abbrev_data = (uint8_t *)(buffer + debug_abbrev->sh_offset);
    uint32_t abbrev_count = 0;
    
    while( total_bytes != debug_abbrev->sh_size ) {
        num_bytes = 0;
        memset( temp_bytes, 0, 64 );
        end_found = false;

        do {
                temp_bytes[num_bytes] = *debug_abbrev_data;

                if( *debug_abbrev_data == 0x00 ) {
                    if( *(debug_abbrev_data + 1) == 0x00 ) {
                        end_found = true;
                        temp_bytes[num_bytes + 1] = 0x00;
                        num_bytes++;
                        total_bytes++;
                        debug_abbrev_data++;

                        // seems to be a third 00 possible? if so, skip
                        if( *(debug_abbrev_data + 1) == 0x00 ) {
                            debug_abbrev_data++;
                            num_bytes++;
                            total_bytes++;
                        }
                    }
                }

                debug_abbrev_data++;
                num_bytes++;
                total_bytes++;

                if( total_bytes == debug_abbrev->sh_size ) {
                    end_found = true;
                }
        } while( end_found == false );

        abbrevs->num = temp_bytes[0];
        memcpy( abbrevs->bytes, temp_bytes, 64 );

        if( total_bytes != debug_abbrev->sh_size ) {
            abbrevs->next = malloc( sizeof(abbrev_entry) );
            abbrevs = abbrevs->next;
        }
    }

    abbrevs = abbrevs_head;

    while( abbrevs ) {
        abbrev_count++;
        abbrevs = abbrevs->next;
    };

    printf( "Processed Num Abbrevs: %d\n", abbrev_count );

    // Process the DIEs...

    uint8_t *die_data = (uint8_t *)(buffer + debug_info->sh_offset + 12);
    uint32_t die_size_total = debug_info->sh_size - 12;
    uint32_t die_total_bytes = 0;
    uint32_t die_bytes = 0;
    die_entry *die = malloc( sizeof(die_entry) );
    die_entry *die_head = die;

    while( die_total_bytes != die_size_total ) {
        // set the offset that the die entry started
        die->offset = die_total_bytes + 12;

        // finds the assigned abbrev to grab the meta data for the die
        die->abbrev_num = die_data[die_total_bytes];
        
        abbrevs = abbrevs_head;

        while( abbrevs ) {
            if( abbrevs->num == die->abbrev_num ) {
                die->abbrev = abbrevs;
                break;
            }

            abbrevs = abbrevs->next;
        }

        // process each nv pair as listed in the abbrev

        // name exists? copy it over


        if( die_total_bytes != die_size_total ) {
            die->next = malloc( sizeof(die_entry) );
            die = die->next;
        }
    }    
}

Elf32_Shdr * elf_get_section_header_by_name( Elf32_Ehdr *elf_header, char * name ) {
    char *buffer = (char *)elf_header;
    Elf32_Shdr *elf_s_headers = (Elf32_Shdr *)(buffer + elf_header->e_shoff );
    Elf32_Shdr *ret_val = NULL;

    for( int i = 0; i < elf_header->e_shnum; i++ ) {
        if( strcmp( (char *)(buffer + elf_s_headers[elf_header->e_shstrndx].sh_offset + elf_s_headers[i].sh_name), name ) == 0 ) {
            ret_val = (Elf32_Shdr *)( ((char *)elf_header + elf_header->e_shoff) + (i * elf_header->e_shentsize) );

            i = elf_header->e_shnum + 1;
        }
    }

    return ret_val;
}

void print_bytes( uint8_t *bytes, int lines ) {
    int line_no = 0;

    for( int j = 0; j < (lines * 16); j++ ) {
        if( (j) % 16 == 0 ) {
            printf( "%02X: ", line_no );
            line_no = line_no + 16;
        }

        printf( "%02X ", *(bytes + j) );

        if( (j + 1) % 4 == 0 ) {
            printf( "    " );
        }

        if( (j + 1) % 16 == 0 ) {
            printf( "\n" );
        }
    }
}

