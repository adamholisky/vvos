#include <kernel.h>
#include "multiboot.h"

/*

Memory Map Physical
-----------------------------------
| 0x0010 0000    Kernel Load
| 0x0017 e000    _kernel_end symbol (example)
| 0x0020 0000    Physical memory base (*1) kernel
| 0x4000 0000    Physical memory base user
-----------------------------------

Memory Map Virtual
-----------------------------------
| 0x0000 0000    Free 
| 0x4000 0000    Free 
| 0x8000 0000    Free
| 0xC000 0000    Kernel Load
| 0xD000 0000    Kernel Heap Start
| ...
| 0xFFFF FFFF    End
-----------------------------------

*1: Calculated as nearst 0x20 0000 aligned memory from end of kernel load

*/

extern uint32_t _kernel_end;
extern void asm_refresh_cr3( void );
extern void asm_invalidate_page( void * p );

static const char * vv_magic_word = "VersionV";
uint32_t * physical_memory_base = (uint32_t *)0xFFFFFFFF;
uint32_t * phsyical_memory_top = (uint32_t *)0xFFFFFFFF;
uint32_t * kernel_virtual_memory_base = (uint32_t *)KERNEL_VIRT_HEAP_BASE;
uint32_t * kernel_virtual_memory_top = (uint32_t *)KERNEL_VIRT_HEAP_BASE;

uint64_t kernel_page_data_tables[512]  __attribute__ ((aligned (4096)));
uint64_t global_page_data_directories[3]  __attribute__ ((aligned (4096)));
uint64_t * global_page_data_tables __attribute__ ((aligned (4096)));

//#define kdebug_memory
//#define kdebug_memory_pages

/**
 * @brief Initalize basic memory
 * 
 */

void memory_initalize( void ) {
	//log_entry_enter();
	paging_level_active = PAGING_LEVEL;

	uint32_t mem_start = 0;
	uint32_t mem_size = 0;
	uint32_t alloc_start = (uint32_t)&_kernel_end - KERNEL_VIRT_LOAD_BASE;

	multiboot_info_t *mbh = get_multiboot_header();

	klog( "Paging level: %d\n", paging_level_active );
	klog( "Page Size: 0x%08X\n", page_size_in_bytes );
	

	for( int i = 0; i < mbh->mmap_length; i = i + sizeof(multiboot_memory_map_t)) {
        multiboot_memory_map_t* mm = (multiboot_memory_map_t*) (mbh->mmap_addr + i);
 
        if(mm->type == MULTIBOOT_MEMORY_AVAILABLE) {
			// This corrects for the weird last entry in the mmap saying we have an extra 1G?
			if( mm->addr_high == 0 ) {
				// For now let's just use the largest continual space, which works in QEMU
				if( mem_size < mm->len_low ) {
					mem_start = mm->addr_low; // 32 bit only
					mem_size = mm->len_low; // 32 bit only
				}
				
				klog("Start Addr: %x %x | Length: %x %x | Size: %x | Type: %d\n", mm->addr_high, mm->addr_low, mm->len_high, mm->len_low, mm->size, mm->type);
			}
        }
    }

	alloc_start = alloc_start + 0x200000 - (alloc_start % 0x200000);

	phsyical_memory_top = (uint32_t *)alloc_start;
	physical_memory_base = (uint32_t *)alloc_start;

	klog( "Usable memory detected:    0x%08X\n", mbh->mem_upper * 1024 );
	klog( "Physical memory base:      0x%08X\n", physical_memory_base );
	klog( "Virtual memory base:       0x%08X\n", kernel_virtual_memory_base );
	klog( "Memory allocatable:        0x%08X\n", mem_size );

	// Set up kernel page tables
	// These WILL NOT EVER CHANGE LOCATION
	klog( "K Page Data Table loc:     0x%08X\n", &kernel_page_data_tables );

	// Set the contents of the kernel's initial table entry
	memset( kernel_page_data_tables, 0, 512*8 );
	kernel_page_data_tables[0] = 0x83;
	kernel_page_data_tables[1] = 0x83 + 0x200000;
	//kernel_page_data_tables[2] = 0x83 + 0x400000;
	//kernel_page_data_tables[3] = 0x83 + 0x600000;
	//kernel_page_data_tables[4] = 0x83 + 0x800000;

	asm_refresh_cr3();

	global_page_data_tables = (uint64_t *)(page_directory_entry *)page_allocate( 1 );
	memset( global_page_data_tables, 0, 512 * 3 * 8 );

	#ifdef kdebug_memory_pages
	klog( "0: 0x%08X\n", global_page_data_directories[0] );
	klog( "1: 0x%08X\n", global_page_data_directories[1] );
	klog( "2: 0x%08X\n", global_page_data_directories[2] );
	klog( "3: 0x%08X\n", global_page_data_directories[3] );

	klog( "Page data starts at virt:  0x%08X\n", global_page_data_tables );

	dump_active_pt();

	mem_virt_to_phys( (uint32_t *)mbh );

	echo_page( (page_directory_entry *)&kernel_page_data_tables[128] );

	for( int pt_dump = 0; pt_dump < 512; pt_dump++ ) {
		if( kernel_page_data_tables[pt_dump] != 0 ) {
			klog( "PTD %d: 0x%08X\n", pt_dump, kernel_page_data_tables[pt_dump] );
		}
	}
	#endif
	
	


	void * process_address_space = kmalloc( PAGE_SIZE_IN_BYTES );

	process_address_space = (void *)((uint32_t)process_address_space + PAGE_SIZE_IN_BYTES - 0x0040);

	_Pragma("GCC diagnostic push")
	_Pragma("GCC diagnostic ignored \"-Wpointer-to-int-cast\"")
	uint64_t * pde = (uint64_t *)&global_page_data_directories;
	uint64_t * pte = (uint64_t *)&global_page_data_tables;
	*pte = (((uint64_t)process_address_space + (uint64_t)physical_memory_base) - KERNEL_VIRT_HEAP_BASE + 0x83);
	*pde = ((uint64_t)&global_page_data_tables - KERNEL_VIRT_LOAD_BASE) + 0x1;
	*pde = 0x00000000FFFFFFFF & *pde;
	*pte = 0x00000000FFFFFFFF & *pte;
	_Pragma("GCC diagnostic pop")
	
	asm volatile("invlpg (%0)" ::"r" (pte) : "memory");
	asm_refresh_cr3();

	// Begin sanity check
	uint32_t * pmem = (uint32_t *)process_address_space;
	*(pmem + 1) = 0x0666;
	void * zero_addr_space = 0x00000000;
	uint32_t * zerozero = (uint32_t *)zero_addr_space;
	*(zerozero + 2) = 0xBABABABA;

	if( *(zerozero + 1) != *(pmem + 1) ) {
		kpanic( "0x0 and pmem are not equal!\n" );
	}
	// End sanity check

	// Test identity mapping

	klog( "0: 0x%08X\n", global_page_data_directories[0] );
	klog( "1: 0x%08X\n", global_page_data_directories[1] );
	klog( "2: 0x%08X\n", global_page_data_directories[2] );
	klog( "3: 0x%08X\n", global_page_data_directories[3] );

	dump_active_pt();

	uint64_t *apd = &global_page_data_tables;
	for( int a = 0; a < 512; a++ ) {
		if( apd[a] != 0 ) {
			klog( "pd[0].page[%d] = %08X %08X\n", a, apd[a]>>32, apd[a] );
		}
	}

	uint32_t * ptr_im = page_map( (uint32_t *)0x02800000, (uint32_t *)0x02800000 );
	dump_active_pt();
	//*((uint32_t *)(0x00C00000)) = 0xABBA0000;
	*ptr_im = 0xADA01111;
	klog( "*ptr_im: 0x%08X\n", *ptr_im );

	// End test
 
	log_entry_exit();
}

/**
 * @brief Set up page structures given an area of memory large enough to hold everything
*/
bool create_paging_data_tables( uint32_t *page_data ) {
	memset( page_data, 0, sizeof( uint64_t ) * PAGE_NUM_TABLES * 3 );
}

/**
 * @brief Get the physical memory base
 * 
 * @return uint32_t* Pointer to the physical memory base
 */
uint32_t * get_physical_memory_base( void ) {
	return physical_memory_base;
}

// OLD -- don't use
// TODO: Get rid of
uint32_t * get_physical_addr_from_virt( uint32_t * virt ) {
	return (uint32_t *)((uint32_t)physical_memory_base + (uint32_t)virt - KERNEL_VIRT_HEAP_BASE);
}

/**
 * @brief Coverts virtual to phyiscal address based on mmu
*/
uint32_t * mem_virt_to_phys( uint32_t * virt ) {	
	klog( "Translating 0x%08X to physical address...\n", virt );

	uint32_t dir_index = (uint32_t)virt / PAGE_SIZE_IN_BYTES;
	uint32_t table_index = dir_index % PAGE_NUM_TABLES;
	dir_index = dir_index / PAGE_NUM_TABLES;
	uint32_t mem_offset = (uint32_t)virt - (dir_index * PAGE_DIR_SIZE_IN_BYTES) - (table_index * PAGE_SIZE_IN_BYTES);

	klog( "Dir Index:   0x%X\n", dir_index );
	klog( "Table Index: 0x%X\n", table_index );
	klog( "Mem Offset:  0x%X\n", mem_offset );

	uint64_t * pae_pd = ((uint64_t *)&global_page_data_directories) + dir_index;
	klog ("Dir Present: 0x%X\n", test_bit(*pae_pd, PTE_BIT_PRESENT) );
	klog ("Dir Address: 0x%X\n", (*pae_pd & 0x000FFFFFFFFFF000) );

	uint64_t *pae_pt = (uint64_t *)( (*pae_pd & 0x000FFFFFFFFFF000) + KERNEL_VIRT_LOAD_BASE) + table_index;
	klog( "PT Present:  0x%X\n",  test_bit(*pae_pt, PTE_BIT_PRESENT) );
	klog( "PT RW:       0x%X\n",  *pae_pt );

	klog( "Done:        physical = 0x%08X\n", mem_offset + (uint32_t)(*pae_pt & 0xFFFFFF00));
	klog( "             virtual =  0x%X\n", virt );

	return (uint32_t *)(mem_offset + (uint32_t)(*pae_pt & 0xFFFFFF00));
}

/**
 * @brief Set the process PDE object
 * 
 * @param pte Pointer to the dage_directory_entry to copy into the main process PTE
 */
void set_task_pde( page_directory_entry * pte ) {
	//page_directory_entry * pde = (page_directory_entry *)&boot_page_directory;

/* 	pde->present = 1;
	pde->rw = 1;
	pde->address = ((uint32_t)process_pt - KERNEL_VIRT_LOAD_BASE) >> 11;
 */
	memcpy( &global_page_data_tables, pte, sizeof( uint64_t ) * 512 * 3 );

	// debugf( "pte: 0x%08X\n", (uint32_t)pte );
	// debugf( "pde->addr: 0x%08X\n", pde->address << 11 );
	// debugf( "pte->addr: 0x%08X\n", pte->address << 11 );

	//page_directory_entry * pt = (page_directory_entry *)((pde[0].address << 11) + KERNEL_VIRT_LOAD_BASE);

	asm_refresh_cr3();

	//asm_invalidate_page( (pt[0].address << 11) );
	//asm_invalidate_page( &process_pt );

	/*
	kdebug_peek_at( 0 );

	klog( "Dumping active pages:\n" );
	dump_active_pt();
	klog( "End of active pages.\n" );
	
	klog( "pt[0].address: 0x%08X\n", pt[0].address );
	klog( "pt[0].address << 11: 0x%08X\n", (pt[0].address << 11) );
	klog( "pt[0].address << 11 + KVHB + PMB: 0x%08X\n", (pt[0].address << 11) + KERNEL_VIRT_HEAP_BASE - (uint32_t)get_physical_memory_base() );

	uint32_t *ad = (pt[0].address << 11) + KERNEL_VIRT_HEAP_BASE - (uint32_t)get_physical_memory_base();
	kdebug_peek_at( ad );
	*/
}

void dump_active_pt( void ) {
	#ifdef PAGING_PAE
	uint64_t * pde = (uint64_t *)&global_page_data_directories;
	#else
	uint32_t * pde = (uint32_t *)0x0;
	#endif

	for( int i = 0; i < PAGE_NUM_DIRS; i++, pde++ ) {
		//klog( "pde: 0x%08X\n", pde );

		if( test_bit(*pde, PTE_BIT_PRESENT) == 1 ) {
			//klog( "present!\n" );
			
			#ifdef PAGING_PAE
				uint64_t pt = (*pde & 0x000FFFFFFFFFF000) + KERNEL_VIRT_LOAD_BASE;
			#else
				uint32_t pt = (*pde & 0xFFFFF000) + KERNEL_VIRT_LOAD_BASE;
			#endif

			//klog( "pt: 0x%08X\n", pt );
			_Pragma("GCC diagnostic push")
			_Pragma("GCC diagnostic ignored \"-Wint-to-pointer-cast\"")
			#ifdef PAGING_PAE
				uint64_t *page = (uint64_t *)pt;
			#else
				uint32_t *page = (uint32_t *)pt;
			#endif
			_Pragma("GCC diagnostic pop")

			for( int j = 0; j < PAGE_NUM_TABLES; j++, page++ ) {
				if( page == NULL ) {
					klog( "Error: page is null. i = %d, j - %d\n", i, j );
				}

				#ifdef PAGING_PAE
				uint64_t address = (*page & 0x000FFFFFFFF00000);
				#else
				uint32_t address = (*page & 0xFFFFF000);
				#endif

				if( test_bit(*page, PTE_BIT_PRESENT) == 1 ) {
					klog( "pd[%d].page[%03d] present, virt 0x%08X --> phys mem: 0x%08X\n", i, j, (i * PAGE_DIR_SIZE_IN_BYTES) + (j * PAGE_SIZE_IN_BYTES), address );
				} else {
					//klog( "pd[%d]->pt[%d] is not present.\n", i, j );
				}
			}
		}
	}
}

/**
 * @brief Map the virtual address to the physical address
 * 
 * @param virt_addr 
 * @param phys_addr 
 * @return uint32_t* Returns the virtual address
 */
uint32_t * page_map( uint32_t *virt_addr, uint32_t *phys_addr ) {
	uint32_t pt_addr_physical = 0;

	uint32_t pdpt_index = (uint32_t)virt_addr / 0x40000000; // 0x40000000;
	uint32_t pd_index = ((uint32_t)virt_addr - (pdpt_index * 0x40000000)) / PAGE_SIZE_IN_BYTES; // PAGE_SIZE_IN_BYTES
	uint64_t *pdpt_uint = ((uint64_t *)&global_page_data_directories) + pdpt_index;

	int pt_index_add = pdpt_index * 512;
	uint64_t *pd_uint = (uint64_t *)&global_page_data_tables + pd_index + pt_index_add;
	
	if( virt_addr >= 0xC0000000 ) {
		pd_uint = (uint64_t *)&kernel_page_data_tables + pd_index;
	}
	

	//kdebug_peek_at( pdpt_uint );

	if( test_bit(*pdpt_uint, PTE_BIT_PRESENT) == 0 ) {
		klog( "pdpt is not present.\n" );
		set_bit(*pdpt_uint, PTE_BIT_PRESENT);
	} else {
		*pd_uint = (uint32_t)phys_addr | 0x83;
		*pd_uint = 0x00000000FFFFFFFF & *pd_uint;
	}

	#ifdef kdebug_memory_pages
	debugf( "virt_addr:          0x%08X\n", virt_addr );
	debugf( "phys_addr:          0x%08X\n", phys_addr );
	debugf( "  pdpt_index:         0x%08X\n", pdpt_index );
	debugf( "  pd_index:           0x%08X\n", pd_index );
	debugf( "  PDPT addr:          0x%08X\n", pdpt_uint );
	debugf( "  PD addr:            0x%08X\n", pd_uint );
	debugf( "  PDPT present:       %d\n", test_bit(*pdpt_uint, PTE_BIT_PRESENT) );
	debugf( "  PD contents:        0x%08X\n", *pd_uint ); 
	#endif
	
	asm volatile("invlpg (%0)" ::"r" (pd_uint) : "memory");
	asm_refresh_cr3(); 	

	/* debugf( "  Mem contnets:       0x%08X\n", *virt_addr); */

	#ifdef kdebug_memory_pages_more
	debugf( "bpd_index:          0x%08X\n", pd_index );
	debugf( "pt_index:           0x%08X\n", pt_index );
	debugf( "pt_addr_physical:   0x%08X\n", pt_addr_physical );
	debugf( "spt:                0x%08X\n", second_page_table );
	debugf( "pt_addr >> 11:      0x%08X\n", pt_addr_physical >> 11 );
	debugf( "PD contents:        0x%08X\n", *pd );
	debugf( "PT contents:        0x%08X\n", *pt );
	#endif

	klog( "Mapped: 0x%08X Virtual to 0x%08X Physical for 0x%X bytes.\n", virt_addr, phys_addr, PAGE_SIZE_IN_BYTES );

	// return pointer to first byte
	return virt_addr;
}

uint32_t * page_allocate( uint32_t num ) {
	uint32_t * return_pointer = kernel_virtual_memory_top; 

	for( uint32_t i = 0; i < num; i++ ) {
		page_map( kernel_virtual_memory_top, phsyical_memory_top );
		kernel_virtual_memory_top = (uint32_t *)((uint32_t)kernel_virtual_memory_top + page_size_in_bytes);
		phsyical_memory_top = (uint32_t *)((uint32_t)phsyical_memory_top + page_size_in_bytes);

		#ifdef kdebug_memory_pages
		debugf( "kvm_top: 0x%08X    pmt: 0x%08X\n", kernel_virtual_memory_top, phsyical_memory_top );
		#endif
	}

	return return_pointer;
}

uint32_t * page_allocate_and_map( uint32_t * phys_addr ) {
	uint32_t * return_pointer = kernel_virtual_memory_top;

	return_pointer = page_map( kernel_virtual_memory_top, phys_addr );

	kernel_virtual_memory_top = (uint32_t *)((uint32_t)kernel_virtual_memory_top + page_size_in_bytes);

	return return_pointer;
}

uint32_t * page_identity_map( uint32_t * phys_addr ) {
	uint32_t * return_pointer = phys_addr;

	return_pointer = page_map( phys_addr, phys_addr );

	return return_pointer;
}

void echo_page( page_directory_entry *page ) {
	uint32_t page_as_int = *(uint32_t *)page;

	debugf( "----\n" );
	debugf( "pde size: %d\n", sizeof(page_directory_entry) );
	debugf( "gpdd: 0x%X\n", (uint32_t)global_page_data_directories );
	debugf( "gpdt: 0x%08X\n", (uint32_t)global_page_data_tables );
	debugf( "kpdt: 0x%08X\n", (uint32_t)kernel_page_data_tables);
	debugf( "page: 0x%08X\n", page_as_int );
	debugf( "page->present: %d\n", page->present ); 
	debugf( "page->rw: %d\n", page->rw ); 
	debugf( "page->privilege: %d\n", page->privilege );
	debugf( "page->write_through: %d\n", page->write_through );
	debugf( "page->cache_disabled: %d\n", page->cache_disabled );
	debugf( "page->accessed: %d\n", page->accessed );
	debugf( "page->dirty: %d\n", page->dirty );
	debugf( "page->page_size: %d\n", page->page_size );
	debugf( "page->global: %d\n", page->global );
	debugf( "page->available: %d\n", page->available );
	debugf( "page->pat: %d\n", page->pat );
	debugf( "page->address: 0x%08X\n", (uint32_t)page->address<<21 );
	debugf( "page->available_2: %d\n", page->available_2 );
	debugf( "page->protection_key: %d\n", page->protection_key );
	debugf( "page->execute_disable: %d\n", page->execute_disable );
	debugf( "----\n" );
}

#define memtest_dump( v ) klog( #v " mem dump: 0x%04X %04X\n",( ( uint32_t )( v ) >> 16 ), ( uint32_t )( v )&0xFFFF)

void memory_test( void ) {
	log_entry_enter();

	void * p2;
	void * p3;
	void * p4;
	void * p5;
	
	p2 = kmalloc( 0x256 );
	klog( "kmalloc(0x256) into p2\n" );
	//memtest_dump( p2 );

	klog( "kmalloc(0x512) into p3\n" );
	p3 = kmalloc( 0x512 );
	//memtest_dump( p3 );

	klog( "kmalloc(0x11000) into p4\n" );
	p4 = kmalloc( 0x11000 );
	//memtest_dump( p4 );

	klog( "kmalloc(0x11000) into p5\n" );
	p5 = kmalloc( 0x11000 );
	//memtest_dump( p5 );

	//memtest_dump( kernel_virtual_memory_top );
	
	liballoc_dump();

	log_entry_exit();
}

page_directory_entry * get_page( uint32_t * virt ) {	

	uint32_t dir_index = (uint32_t)virt / PAGE_SIZE_IN_BYTES;
	uint32_t table_index = dir_index % PAGE_NUM_TABLES;
	dir_index = dir_index / PAGE_NUM_TABLES;
	uint32_t mem_offset = (uint32_t)virt - (dir_index * PAGE_DIR_SIZE_IN_BYTES) - (table_index * PAGE_SIZE_IN_BYTES);

	klog( "Dir Index:   0x%X\n", dir_index );
	klog( "Table Index: 0x%X\n", table_index );
	klog( "Mem Offset:  0x%X\n", mem_offset );

	uint64_t * pae_pd = ((uint64_t *)&global_page_data_directories) + dir_index;
	klog ("Dir Present: 0x%X\n", test_bit(*pae_pd, PTE_BIT_PRESENT) );
	klog ("Dir Address: 0x%X\n", (*pae_pd & 0x000FFFFFFFFFF000) );

	uint64_t *pae_pt = (uint64_t *)( (*pae_pd & 0x000FFFFFFFFFF000) + KERNEL_VIRT_LOAD_BASE) + table_index;
	klog( "PT Present:  0x%X\n",  test_bit(*pae_pt, PTE_BIT_PRESENT) );
	klog( "PT RW:       0x%X\n",  *pae_pt );

	return (page_directory_entry *)pae_pt;
}