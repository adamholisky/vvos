#if !defined(ELF_INCLUDED)
#define ELF_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

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

void elf_initalize( uint32_t kmain );
Elf32_Shdr* elf_find_rel_plt(uint32_t* mem, Elf32_Ehdr* elf_header);
Elf32_Shdr* elf_find_got_plt(uint32_t* mem, Elf32_Ehdr* elf_header);
char* elf_get_section_name(uint32_t* mem, Elf32_Ehdr* elf_header, uint32_t sec_num);
void elf_load_program_headers(Elf32_Ehdr* elf_header, uint8_t* process_space, uint8_t* data);
char* elf_get_sym_name_from_index(uint32_t* mem, Elf32_Ehdr* elf_header, uint32_t index);
uint16_t elf_get_sym_shndx_from_index(uint32_t* mem, Elf32_Ehdr* elf_header, uint32_t index);
Elf32_Shdr * elf_find_sym_strtab(uint32_t* mem, Elf32_Ehdr* elf_header);
Elf32_Shdr * elf_find_dynstr_tab(uint32_t* mem, Elf32_Ehdr* elf_header);
uint16_t elf_get_sym_value_from_index(uint32_t* mem, Elf32_Ehdr* elf_header, uint32_t index);
Elf32_Shdr * elf_find_symtab(uint32_t* mem, Elf32_Ehdr* elf_header);

#ifdef __cplusplus
}
#endif

#endif