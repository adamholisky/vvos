#include "kernel.h"
#include "multiboot.h"
#include "elf.h"
#include "task.h"
#include "interrupts.h"
#include "pci.h"
#include "intel8254.h"
#include "vv_list.h"
#include "ata.h"
#include <cpuid.h>
#include "task.h"
#include "syscall.h"
#include "serial_client.h"
#include "debug.h"
#include "callback.h"
#include "keyboard.h"
#include "observer.h"
#include <ftp.h>
#include <modules.h>
#include <vshell.h>

#define END_IMMEDIATELY
#define TRIGGER_DIVIDE_BY_ZERO false

void kernel_main( unsigned long mb_magic, multiboot_info_t *mb_info ) {
	int x = 0;
	char c = ' ';
	bool process_keypress;

	multiboot_initalize( mb_magic, mb_info );
	term_initalize();
	initalize_serial();

	printf( "\x1b[0;31;49mVersionV\x1b[0;0;0m\n" );
	debugf( "\x1b[0;31;49mVersionV\x1b[0;0;0m Serial Out\n" );

	// Handle CLI from multiboot header
 	if( strstr( (char *)mb_info->cmdline, "graphics_on" ) ) {
		debugf( "CLI: Graphics on\n" );
		GRAPHICS_ACTIVE = false;
	} else {
		debugf( "CLI: Graphics off\n" );
		GRAPHICS_ACTIVE = false;
		DF_COM4_ONLY = true;
	}

	multiboot_echo_to_serial();
	memory_initalize();
	kdebug_initalize();

	elf_initalize( reinterpret_cast<uint32_t>(kernel_main) );
	interrupts_initalize();
	observer_initalize();
	//memory_test();
	//dump_active_pt();

	if( GRAPHICS_ACTIVE ) {
		vga_initalize();
		console_init( "default-console", 3, 3, 7 * 120, 14 * 50, 0x00282C34, 0x00AAAAAA );
		console_draw();
	}

/* 	uint32_t cpuid_return = 0;
	uint32_t eax, unused;
	__get_cpuid( 1, &eax, &unused, &unused, &cpuid_return );
	klog( "CPUID edx feature output:\n" );
	debugf_bit_array( cpuid_return ); */

	serial_enable_interrupts();
	task_initalize();
	//process_initalize();
	//modules_initalize();

	pci_initalize();
	intel8254_initalize();
	//ata_initalize();

	//page_fault_test();
	serial_client_initalize();

	observer_test();

	keyboard_initalize();

	serial_clear_buffer( COM2 );
	READY_FOR_INPUT = true;

	if( TRIGGER_DIVIDE_BY_ZERO ) {
		asm volatile( 
			"movl 0, %eax \n\t"
			"movl 0, %ecx \n\t"
			"div %ecx \n\t"
		);
	}

	//multiboot_echo_to_serial();
	FTP host_ftp;

	host_ftp.init();
	host_ftp.login( "vv", "vv" );
	host_ftp.cwd( "/usr/local/osdev/versions/v/modules/build" );
	host_ftp.get_file( "reference.vvs" );
	
	Module m;
	m.load( (uint32_t *)host_ftp.data_buffer );
	m.call_main();

	VShell v;
	v.init( &host_ftp, &m );
	v.run();

	#ifdef END_IMMEDIATELY

	klog( "Shutdown via END_IMMEDIATELY at end of kernel.c.\n" );
	outportb( 0xF4, 0x00 );

	#endif

	#ifdef kdebug_paging

	char *page_fault_addr = NULL;
	page_fault_addr = (char *)KERNEL_VIRT_HEAP_BASE;
	klog( "String at 0x%08X: \"%s\"\n", page_fault_addr, page_fault_addr );
	page_fault_addr = (char *)page_allocate( 1 );
	klog( "String at 0x%08X: \"%s\"\n", page_fault_addr, page_fault_addr );

	#endif

	#ifdef kdebug_cpuid

	uint32_t cpuid_return = 0;
	uint32_t eax, unused;
	__get_cpuid( 1, &eax, &unused, &unused, &cpuid_return );
	klog( "CPUID edx feature output:\n" );
	debugf_bit_array( cpuid_return );

	#endif

	#ifdef kdebug_profile_test

	profile_test();

	#endif

	klog( "\n\nEnd of line." );

	while( true ) { x = x - x + 1; }
}

int register_io_device( char *name, void (*read_func)(), void (*write_func)() ) {
	// TBD

	return 0;
}

uint32_t write( int file, void *buff, uint32_t count ) {
	// TBD 

	return 0;
}
