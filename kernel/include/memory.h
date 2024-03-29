#if !defined(MEMORY_INCLUDED)
#define MEMORY_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif


#ifdef PAGING_4K
	#define page_size_in_bytes 0x00001000
	#define PAGE_SIZE_IN_BYTES 0x00001000
	#define PAGE_DIR_SIZE_IN_BYTES 0x00400000
	#define PAGE_NUM_TABLES 1024
	#define PAGE_NUM_DIRS 1024
	#define SBRK_NUM_PAGES 16
	#define PAGING_LEVEL 1
#elif PAGING_4M
	#define page_size_in_bytes 0x00400000
	#define PAGE_SIZE_IN_BYTES 0x00400000
	#define PAGE_DIR_SIZE_IN_BYTES 0x00400000
	#define SBRK_NUM_PAGES 1
	#define PAGING_LEVEL 3
#elif PAGING_PAE
	#define page_size_in_bytes 0x00200000
	#define PAGE_SIZE_IN_BYTES 0x00200000
	#define PAGE_DIR_SIZE_IN_BYTES 0x40000000
	#define PAGE_NUM_TABLES 512
	#define PAGE_NUM_DIRS 4
	#define SBRK_NUM_PAGES 2
	#define PAGING_LEVEL 2
#else
	#define page_size_in_bytes 0x00001000
	#define PAGE_SIZE_IN_BYTES 0x00001000
	#define PAGE_DIR_SIZE_IN_BYTES 0x00400000
	#define PAGE_NUM_TABLES 1024
	#define PAGE_NUM_DIRS 1024
	#define SBRK_NUM_PAGES 16
	#define PAGING_LEVEL 1
#endif

#define PAGING_LEVEL_PAE 2
#define PAGING_LEVEL_4K 1
#define PAGING_LEVEL_4M 3

#define KERNEL_VIRT_LOAD_BASE 0xC0000000
#define KERNEL_VIRT_HEAP_BASE 0xD0000000

#define PTE_BIT_PRESENT 0
#define PTE_BIT_READ_WRITE 1
#define PTR_BIT_USER_SUPERVISOR 2
#define PTE_BIT_WRITE_THROUGH 3
#define PTE_BIT_CACHE_DISABLED 4
#define PTE_BIT_ACCESSED 5
#define PTE_BIT_DIRTY 6
#define PTE_BIT_PAGE_SIZE 7
#define PTE_BIT_GLOBAL 8

#ifndef __ASSEMBLER__

typedef struct {
	uint8_t		present : 1; // 0
	uint8_t		rw : 1; // 1
	uint8_t		privilege : 1; // 2
	uint8_t		write_through : 1; // 3

	uint8_t		cache_disabled : 1; // 4
	uint8_t		accessed : 1; // 5
	uint8_t	    dirty : 1; // 6
	uint8_t		page_size : 1; // 7
	
	uint8_t		global : 1; // 8
	uint8_t		available : 3; // 9, 10, 11
	
	uint8_t     pat : 1; // 12
	uint8_t     reserved : 8; // 13, 14, 15, 16, 17, 18, 19, 20
	uint32_t	address : 29; // 21 - 50
	uint8_t		reserved_2 : 1; // 51
	uint8_t     available_2 : 7; // 52, 53, 54, 55, 56, 57, 58
	uint8_t     protection_key : 4; // 59, 60, 61, 62
	uint8_t	    execute_disable : 1;
} __attribute__ ((packed)) page_directory_entry;

void memory_initalize( void );
uint32_t * page_map( uint32_t *virt_addr, uint32_t *phys_addr );
uint32_t * page_allocate( uint32_t num );
uint32_t * page_allocate_and_map( uint32_t * phys_addr );
uint32_t * page_identity_map( uint32_t * phys_addr );
void set_task_pde( page_directory_entry * pte );
uint32_t * get_physical_memory_base( void );
void echo_page( page_directory_entry *page );
void dump_active_pt( void );
uint32_t * get_physical_addr_from_virt( uint32_t * virt );
uint32_t * mem_virt_to_phys( uint32_t * virt );
page_directory_entry * get_page( uint32_t * virt );

#endif // __ASSEMBLER__

#ifdef __cpluscplus
inline void *operator new(size_t, void *p)     throw() { return p; }
inline void *operator new[](size_t, void *p)   throw() { return p; }
inline void  operator delete  (void *, void *) throw() { };
inline void  operator delete[](void *, void *) throw() { };
#endif

#ifdef __cplusplus
}
#endif

#endif // MEMORY_INCLUDED