#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint32_t swap_uint32(uint32_t val);
void set_debug_output(bool output);
void swap_task(void);
typedef void (*make_call)(bool);

uint32_t swap_uint32(uint32_t val)
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
}

#undef do_ext2_tests
#undef do_elf_tests

#define MAXBUFLEN 1000000

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
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct {
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half st_shndx;
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
    uint32_t	s_inodes_count;		/* Inodes count */
    uint32_t	s_blocks_count;		/* Blocks count */
    uint32_t	s_r_blocks_count;	/* Reserved blocks count */
    uint32_t	s_free_blocks_count;	/* Free blocks count */
    uint32_t	s_free_inodes_count;	/* Free inodes count */
    uint32_t	s_first_data_block;	/* First Data Block */
    uint32_t	s_log_block_size;	/* Block size */
    uint32_t	s_log_frag_size;	/* Fragment size */
    uint32_t	s_blocks_per_group;	/* # Blocks per group */
    uint32_t	s_frags_per_group;	/* # Fragments per group */
    uint32_t	s_inodes_per_group;	/* # Inodes per group */
    uint32_t	s_mtime;		/* Mount time */
    uint32_t	s_wtime;		/* Write time */
    uint16_t	s_mnt_count;		/* Mount count */
    uint16_t	s_max_mnt_count;	/* Maximal mount count */
    uint16_t	s_magic;		/* Magic signature */
    uint16_t	s_state;		/* File system state */
    uint16_t	s_errors;		/* Behaviour when detecting errors */
    uint16_t	s_minor_rev_level; 	/* minor revision level */
    uint32_t	s_lastcheck;		/* time of last check */
    uint32_t	s_checkinterval;	/* max. time between checks */
    uint32_t	s_creator_os;		/* OS */
    uint32_t	s_rev_level;		/* Revision level */
    uint16_t	s_def_resuid;		/* Default uid for reserved blocks */
    uint16_t	s_def_resgid;		/* Default gid for reserved blocks */
} ext2_super_block;

typedef struct {
	uint32_t	inode;			/* Inode number */
	uint16_t	rec_len;		/* Directory entry length */
	uint16_t	name_len;		/* Name length */
	char	    name[];			/* File name, up to EXT2_NAME_LEN */
} ext2_dir_entry;

typedef struct {
    uint32_t	bg_block_bitmap;		/* Blocks bitmap block */
    uint32_t	bg_inode_bitmap;		/* Inodes bitmap block */
    uint32_t	bg_inode_table;		/* Inodes table block */
    uint16_t	bg_free_blocks_count;	/* Free blocks count */
    uint16_t	bg_free_inodes_count;	/* Free inodes count */
    uint16_t	bg_used_dirs_count;	/* Directories count */
    uint16_t	bg_pad;
    uint32_t	bg_reserved[3];
} ext2_block_group_desc;

typedef struct {
    uint16_t	i_mode;		/* File mode */
    uint16_t	i_uid;		/* Low 16 bits of Owner Uid */
    uint32_t	i_size;		/* Size in bytes */
    uint32_t	i_atime;	/* Access time */
    uint32_t	i_ctime;	/* Creation time */
    uint32_t	i_mtime;	/* Modification time */
    uint32_t	i_dtime;	/* Deletion Time */
    uint16_t	i_gid;		/* Low 16 bits of Group Id */
    uint16_t	i_links_count;	/* Links count */
    uint32_t	i_blocks;	/* Blocks count */
    uint32_t	i_flags;	/* File flags */
    uint32_t    os_1_a;			/* OS dependent 1 */
    uint32_t	i_block[15]; /* Pointers to blocks <-- linux/ext2.h @ line 221 - 228 */
    uint32_t	i_generation;	/* File version (for NFS) */
    uint32_t	i_file_acl;	/* File ACL */
    uint32_t	i_dir_acl;	/* Directory ACL */
    uint32_t	i_faddr;	/* Fragment address */
    uint32_t    os_2_a;
    uint32_t    os_2_b;
    uint32_t    os_2_c;     /* OS dependent 2 */
} ext2_inode;


void elf_run(void);
Elf32_Shdr* elf_find_rel_plt(uint32_t* mem, Elf32_Ehdr* elf_header);
Elf32_Shdr* elf_find_got_plt(uint32_t* mem, Elf32_Ehdr* elf_header);
char* elf_get_section_name(uint32_t* mem, Elf32_Ehdr* elf_header, uint32_t sec_num);
void elf_load_program_headers(Elf32_Ehdr* elf_header, uint8_t* process_space, uint8_t* data);

#define set_bit(x,b) x | 1<<b
#define clear_bit(x,b) x ~ 1<<b 
#define flip_bit(x,b) x ^ 1<<b
#define test_bit(x,b) x & 1<<b

#define test_bit_array(a,b) ((1<<(b%32)) & a[b/32])

int find_open_page_1( uint32_t *pages );
int find_open_page_2( uint32_t *pages );

int main() {

    #ifdef do_ext2_tests
    uint8_t *source = nullptr;
    long numbytes;

    Elf32_Ehdr *elf_header = nullptr;
    Elf32_Shdr *elf_sheader = nullptr;

    ext2_super_block *super_block = nullptr;

    uint32_t num_bytes_to_read = 1024 * 1024 * 400;

    FILE *fp = fopen( "Y:\\versions\\v\\vs_scratch\\vvscratch\\Debug\\vv_hd.img", "r" );

    if( fp != NULL ) {
        fseek( fp, 0L, SEEK_END );
        numbytes = ftell( fp );

        fseek( fp, 0L, SEEK_SET );

        //source = (char*)calloc(numbytes, sizeof(char));
        source = (uint8_t *)calloc( num_bytes_to_read, sizeof( uint8_t ) );

        if( source == NULL ) {
            printf( "souce is null\n" );
        }

        //fread(source, sizeof(char), numbytes, fp);
        fread( source, sizeof( uint8_t ), num_bytes_to_read, fp );

        if( ferror( fp ) != 0 ) {
            fputs( "Error reading file\n", stderr );
        }

        printf( "Total bytes read: 0x%08X\n", num_bytes_to_read );

        fclose( fp );
    } else {
        printf( "fp is null\n" );
    }

    if( source != NULL ) {
        printf( "00 and 01: %02X %02X\n", *(source), *(source + 1) );
        printf( "Should be 0x0000: %02X %02X\n", *(source + 0x1BC - 1), *(source + 0x1BD - 1) );
        printf( "Drive attrib: %02X\n", *(source + 0x1BE - 1) );
        printf( "parition type: %02X\n", *(source + 0x1BE + 0x4 - 1) );
        printf( "num sectors in partition: %02X %02X %02X %02X\n", *(source + 0x1BE + 0x0C - 1), *(source + 0x1BE + 0x0D - 1), *(source + 0x1BE + 0x0E - 1), *(source + 0x1BE + 0x0F - 1) );

        uint32_t num_sectors = (*(source + 0x1BE + 0x0C - 1) << 24) + ((*(source + 0x1BE + 0x0D - 1)) << 16) + ((*(source + 0x1BE + 0x0E - 1)) << 8) + (*(source + 0x1BE + 0x0F - 1));

        uint32_t partition_start = *((uint32_t *)(source + 0x1BE + 0x8 - 1));
        uint32_t superblock_offset = (partition_start * 512) + 1024;

        printf( "partition start:            %08X\n", partition_start );
        printf( "num sectors little-endian:  %08X\n", *((uint32_t *)(source + 0x1BE + 0xC - 1)) );
        printf( "expecting superblock at:    %08X\n", superblock_offset );

        super_block = (ext2_super_block *)(source + superblock_offset);
        printf( "lol magic? 0x%04X\n", super_block->s_magic );

        uint32_t block_size = 1024 << (super_block->s_log_block_size);
        printf( "block size: 0x%08X\n", block_size );

        uint32_t num_block_groups = super_block->s_blocks_count / super_block->s_blocks_per_group;
        printf( "num block groups: %d\n", num_block_groups );

        for( int z = 0; z < num_block_groups; z++ ) {
            uint32_t block_adjustment = (z > 0)?(block_size + (sizeof( ext2_block_group_desc ) * z)):block_size;
            uint32_t blockgroup_offset = (partition_start * 512) + block_adjustment;

            ext2_block_group_desc *blockgroup = (ext2_block_group_desc *)(source + blockgroup_offset);

            printf( "block of inode table: %d\n", blockgroup->bg_inode_table );
            printf( "num directories: %d\n", blockgroup->bg_used_dirs_count );

            uint32_t inode_table_offset = (partition_start * 512) + (block_size * blockgroup->bg_inode_table);
            printf( "inode table offset: 0x%08X\n", inode_table_offset );
            printf( "free inodes: 0x%04X\n", blockgroup->bg_free_inodes_count );
            uint16_t num_inodes = super_block->s_inodes_per_group - blockgroup->bg_free_inodes_count;
            printf( "num inodes in bg: %d / 0x%04X\n", num_inodes, num_inodes );

            for( int i = 0; i < num_inodes; i++ ) {
                ext2_inode *inode = (ext2_inode *)(source + inode_table_offset + (sizeof( ext2_inode ) * i));
                ext2_dir_entry *dir = (ext2_dir_entry *)(source + (partition_start * 512) + (block_size * inode->i_block[0]));

                //printf("%02d: %04X", i + 1, (uint16_t)(inode->i_mode)); //i+1 b/c ext2 starts at 1

                if( inode->i_mode & 0x4000 ) {
                    printf( "   dir" );
                    printf( "   start addr: 0x%08X\n", (uint32_t)((uint32_t *)(dir)) );

                    uint16_t current_rec_length = 0xFFFF; // safety feature

                    do {
                        if( dir->inode != 0 ) {
                            dir->name[dir->name_len - 512] = 0;
                            printf( "    Name: \"%s\"    \n    inode: %d    name_len: %d    rec len: 0x%04X    next addr: 0x%08X\n    -----\n", dir->name, dir->inode, dir->name_len - 512, dir->rec_len, (uint32_t)((uint32_t *)(dir)) + dir->rec_len );
                        }

                        current_rec_length = dir->rec_len;

                        dir = (ext2_dir_entry *)(((uint32_t)dir) + ((uint32_t)dir->rec_len));

                    } while( (current_rec_length & 0x0F00) == 0 );

                    printf( "\n" );
                }

                if( inode->i_mode & 0x8000 ) {
                    // printf("   file");
                }
            }
        }
    }
    #endif

    #ifdef do_elf_tests
    elf_run();
    #endif

    uint32_t pages[32];
    int32_t open_page_major = -1;
    int32_t open_page_minor = -1;
    int i, j;

    memchr( pages, 0, 31 );

    for( i = 0; i < 200000; i++ ) {
       printf( "Open page number: %d\n", find_open_page_1( pages ) );

       printf( "Open page number: %d\n", find_open_page_2( pages ) );
    }

    exit( 0 );
}

int find_open_page_1( uint32_t *pages ) {
    int open_page_major = -1;
    int open_page_minor = -1;
    int i, j;

    // find open page
    for( i = 0; i < 32; i++ ) {
        for( j = 0; j < 32; j++ ) {
            if( (1 << j & pages[i]) == 0 ) {
                open_page_major = i;
                open_page_minor = j;

                j = 10000;
                i = 10000;
            }
        }
    }

    return (open_page_major * 32) + open_page_minor;
}

int find_open_page_2( uint32_t *pages ) {
    int i;

    for( i = 0; i < 1024; i++ ) {
        if( test_bit_array( pages, i ) == 0 ) {
            break;
        }
    }

    return i;
}

void elf_run(void) {
    char* source = nullptr;
    uint32_t* process_space = (uint32_t*)malloc(0x50000);
    long numbytes;

    Elf32_Ehdr* elf_header = nullptr;
    Elf32_Shdr* elf_sheader = nullptr;
    Elf32_Phdr* elf_pheader = nullptr;

    FILE* fp = fopen("Y:\\versions\\v\\vs_scratch\\vvscratch\\Debug\\alpha.vvs", "rb");

    if (fp != NULL) {
        fseek(fp, 0L, SEEK_END);
        numbytes = ftell(fp);

        fseek(fp, 0L, SEEK_SET);

        source = (char*)calloc(numbytes * 2, sizeof(char));

        if (source == NULL) {
            printf("souce is null\n");
            exit(1);
        }

        if (source == 0) {
            printf("source is 0\n");
            exit(1);
        }

        fread(source, sizeof(char), numbytes, fp);

        if (ferror(fp) != 0) {
            fputs("Error reading file\n", stderr);
        }

        fclose(fp);
    }
    else {
        printf("fp is null, errno %d\n", errno);
    }

    if (source == NULL) {
        printf("source is null -- number 2\n");
        exit(1);
    }

    elf_header = (Elf32_Ehdr*)source;

    printf("source: 0x%08X\n", (uint32_t)source);
    printf("numbytes: 0x%08X\n", numbytes);

    printf("ELF Header\n");
    printf("Ident: %s\n", elf_header->e_ident);
    printf("Type: 0x%X\n", elf_header->e_type);
    printf("Machine: 0x%X\n", elf_header->e_machine);
    printf("Version: 0x%X\n", elf_header->e_version);
    printf("Entry: 0x%X\n", elf_header->e_entry);
    printf("Program header offset: 0x%X\n", elf_header->e_phoff);
    printf("Section header offset: 0x%X\n", elf_header->e_shoff);
    printf("Flags: 0x%X\n", elf_header->e_flags);
    printf("Program Header Entries: 0x%X\n", elf_header->e_phnum);
    printf("Number of section headers: 0x%X\n", elf_header->e_shnum);
    printf("Section header size: 0x%X (sizeof: 0x%X)\n", elf_header->e_shentsize, sizeof(Elf32_Shdr));
    printf("Section name string tbl index: 0x%X\n", elf_header->e_shstrndx);
    printf("Section header in mem: 0x%08X\n", (uint32_t)elf_header + elf_header->e_shoff);

    elf_sheader = (Elf32_Shdr*)((uint32_t)elf_header + elf_header->e_shoff);
    Elf32_Shdr * elf_str_shdr = (Elf32_Shdr*)(((uint32_t)elf_header + elf_header->e_shoff) + (sizeof(Elf32_Shdr) * elf_header->e_shstrndx));
    uint32_t elf_str_offset = elf_str_shdr->sh_offset;
    printf("str offset: 0x%X\n", elf_str_offset);
    printf("Section Headers\n");
    printf("num d/h  mem offst   typ  addr          file offset\n");
    for (int i = 0; i < elf_header->e_shnum; i++) {
        Elf32_Shdr* section = (Elf32_Shdr*)(((uint32_t)elf_header + elf_header->e_shoff) + (sizeof(Elf32_Shdr) * i));
        printf("[%d / %x @ 0x%08X] %d    0x%08X    0x%08X  -->  %s\n", i, i, (uint32_t)section, section->sh_type, section->sh_addr, section->sh_offset, (char*)(source + elf_str_offset + section->sh_name));
    }

    printf("\nProgram Headers\n");

    for (int i = 0; i < elf_header->e_phnum; i++) {
        elf_pheader = (Elf32_Phdr*)((uint32_t)elf_header + elf_header->e_phoff + (elf_header->e_phentsize * i));

        printf("\n");

        printf("Program header type: %X\n", elf_pheader->p_type);
        printf("PH Offset: 0x%08X\n", elf_pheader->p_offset);
        printf("PH vaddr: 0x%08X\n", elf_pheader->p_vaddr);
        printf("PH paddr: 0x%08X\n", elf_pheader->p_paddr);
        printf("PH filesz: 0x%08X\n", elf_pheader->p_filesz);
        printf("PH memsz: 0x%08X\n", elf_pheader->p_memsz);
        printf("PH flags: 0x%08X\n", elf_pheader->p_flags);
        printf("PH align: 0x%08X\n", elf_pheader->p_align);

        if (elf_pheader->p_type == 1) {
            uint8_t* data = (uint8_t *)(source + elf_pheader->p_offset);
            printf("I would load 0x%X bytes starting at file location 0x%X into physical memory address 0x%08X in 0x%X bytes of allocated space.\n", elf_pheader->p_filesz, elf_pheader->p_offset, elf_pheader->p_paddr, elf_pheader->p_memsz );
            printf("The first 10 bytes of loaded data looks like: ");

            for (int j = 0; j < 9; j++) {
                printf("%02X ", (uint8_t)*(data + j));
            }

            printf("\n");
        }
    }

    printf("\n----------\nBeginning OS simulation\n");

    Elf32_Shdr* rel_plt = elf_find_rel_plt((uint32_t*)source, elf_header);
    if (rel_plt != NULL) {
        uint32_t* data = (uint32_t*)(source + rel_plt->sh_offset);
        printf(".rel.plt out:\n");
        for (int j = 0; j < rel_plt->sh_size; j++) {
            printf("%08X\t", (uint32_t) * (data + j));
        }
        printf("\n\n");
    }
    else {
        printf("Could not find .rel.plt section\n");
    }

    Elf32_Shdr* got_plt = elf_find_got_plt((uint32_t*)source, elf_header);
    if (got_plt != NULL) {
        uint32_t* data = (uint32_t*)(source + got_plt->sh_offset);
        printf(".got.plt out:\n");
        for (int j = 0; j < got_plt->sh_size; j++) {
            printf("%08X\t", (uint32_t) * (data + j));
        }
        printf("\n\n");
    }
    else {
        printf("Could not find .got.plt section\n");
    }

    elf_load_program_headers(elf_header, (uint8_t *)process_space, (uint8_t *)source);

    for (int k = 0; k < 0x200; k = k+4) {
        //printf("0x%03X: %02X %02X %02X %02X\n", k, (uint8_t)*((uint8_t*)process_space +k), (uint8_t) * ((uint8_t*)process_space + k + 1), (uint8_t) * ((uint8_t*)process_space + k + 2), (uint8_t) * ((uint8_t*)process_space + k + 3));
    }

    for (int k = 0x1288; k < 0x12a0; k = k + 4) {
        printf("0x%03X: %02X %02X %02X %02X\n", k, (uint8_t) * ((uint8_t*)process_space + k), (uint8_t) * ((uint8_t*)process_space + k + 1), (uint8_t) * ((uint8_t*)process_space + k + 2), (uint8_t) * ((uint8_t*)process_space + k + 3));
    }

    uint32_t* func_printf = (uint32_t*)_printf_p;
    uint32_t* func_set_debug_output = (uint32_t*)set_debug_output;
    uint32_t* func_sched_yield = (uint32_t*)swap_task;

    if (rel_plt == NULL) {
        exit(1);
    }

    if (got_plt == NULL) {
        exit(1);
    }


    uint32_t* data1 = (uint32_t*)((uint32_t)process_space + got_plt->sh_addr);
    printf(".got.plt out:\n");
    for (int j = 0; j < got_plt->sh_size; j++) {
        printf("%08X\t", (uint32_t) * (data1 + j));
    }
    printf("\n\n");

    Elf32_Rel* elf_rel1 = (Elf32_Rel*)(source + rel_plt->sh_offset);
    Elf32_Rel* elf_rel2 = (Elf32_Rel*)(source + rel_plt->sh_offset + sizeof(Elf32_Rel));
    Elf32_Rel* elf_rel3 = (Elf32_Rel*)(source + rel_plt->sh_offset + 2 * sizeof(Elf32_Rel));

    uint32_t* got_entry = (uint32_t*)((uint8_t*)process_space + elf_rel1->r_offset);
    *got_entry = (uint32_t)func_set_debug_output;

    uint32_t* got_entry2 = (uint32_t*)((uint8_t*)process_space + elf_rel2->r_offset);
    *got_entry2 = (uint32_t)func_printf;

    uint32_t* got_entry3 = (uint32_t*)((uint8_t*)process_space + elf_rel3->r_offset);
    *got_entry3 = (uint32_t)func_sched_yield;
    
    //uint32_t* got_entry = (uint32_t*)((uint8_t*)process_space + elf_rel1->r_offset);
    
    uint32_t* data2 = (uint32_t*)((uint32_t)process_space + got_plt->sh_addr);
    printf(".got.plt out:\n");
    for (int x = 0; x < got_plt->sh_size; x++) {
        printf("%08X\t", (uint32_t) * (data2 + x));
    }
    printf("\n\n");

    make_call the_call = (make_call)*got_entry;
    printf("Calling 0x%08X\n", (uint32_t)the_call);
    the_call(true);

   /* the_call = (make_call)*got_entry2;
    printf("Calling 0x%08X\n", (uint32_t)the_call);
    the_call(true);*/

    the_call = (make_call)*got_entry3;
    printf("Calling 0x%08X\n", (uint32_t)the_call);
    the_call(true);

    printf("end\n");
}

void set_debug_output(bool output) {
    printf("set_debug_output called with %d\n", output);
}

void swap_task(void) {
    printf("sched wait called\n");
}

void elf_load_program_headers(Elf32_Ehdr* elf_header, uint8_t* process_space, uint8_t* data) {
    Elf32_Phdr* elf_pheader = nullptr;

    for (int i = 0; i < elf_header->e_phnum; i++) {
        elf_pheader = (Elf32_Phdr*)((uint32_t)elf_header + elf_header->e_phoff + (elf_header->e_phentsize * i));

        if (elf_pheader->p_type == 1) {
            uint8_t* source = (uint8_t*)(data + elf_pheader->p_offset);
            uint8_t* dest = (uint8_t*)(process_space + elf_pheader->p_vaddr);
    
            printf("Loading 0x%X bytes to physical/virtual 0x%08X / 0x%08X.\n", elf_pheader->p_filesz, elf_pheader->p_paddr, elf_pheader->p_vaddr);

            memcpy(dest, source, elf_pheader->p_filesz);
        }
    }
}

Elf32_Shdr * elf_find_rel_plt(uint32_t* mem, Elf32_Ehdr* elf_header) {
    Elf32_Shdr* ret = nullptr;

    for (int i = 0; i < elf_header->e_shnum; i++) {
        Elf32_Shdr* section = (Elf32_Shdr*)(((uint32_t)mem + elf_header->e_shoff) + (sizeof(Elf32_Shdr) * i));

        if (section->sh_type == SHT_REL) {
            printf("rel.plt section: [%d / %x @ 0x%08X] %d    0x%08X    0x%08X\n", i, i, (uint32_t)section, section->sh_type, section->sh_addr, section->sh_offset);
            ret = section;
        }
    }

    return ret;
}

Elf32_Shdr* elf_find_got_plt(uint32_t* mem, Elf32_Ehdr* elf_header) {
    Elf32_Shdr* ret = nullptr;
    char* sec_name = nullptr;

    for (int i = 0; i < elf_header->e_shnum; i++) {
        Elf32_Shdr* section = (Elf32_Shdr*)(((uint32_t)mem + elf_header->e_shoff) + (sizeof(Elf32_Shdr) * i));

        if (section->sh_type == SHT_PROGBITS) {
            sec_name = elf_get_section_name(mem, elf_header, i);
            //printf("sn: %s\n", sec_name);
            if (strcmp(sec_name, ".got.plt") == 0) {
                printf("got.plt section: [%d / %x @ 0x%08X] %d    0x%08X    0x%08X\n", i, i, (uint32_t)section, section->sh_type, section->sh_addr, section->sh_offset);
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