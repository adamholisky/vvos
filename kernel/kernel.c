#include "kernel.h"
#include <kernel_main.h>
#include "multiboot.h"
#include "elf.h"
#include "task.h"
#include "interrupts.h"
#include "pci.h"
#include "task.h"
#include "debug.h"
#include "callback.h"
#include "keyboard.h"
#include "observer.h"
#include <string.h>
#include <device.h>
#include <kshell.h>
#include "intel8254.h"
#include <ahci.h>
#include <fs.h>
#include <mouse.h>
#include <vui/vui.h>
#include <vui/window.h>
#include <vui/desktop.h>
#include <kapps/console.h>

#define END_IMMEDIATELY
#define TRIGGER_DIVIDE_BY_ZERO false

void kernel_main( unsigned long mb_magic, multiboot_info_t *mb_info ) {
	int x = 0;
	char c = ' ';
	bool process_keypress;

	GRAPHICS_ACTIVE = true;
	DF_COM4_ONLY = true;

	multiboot_initalize( mb_magic, mb_info );
	devices_initalize();
	term_initalize();
	initalize_serial();

	printf( "\x1b[0;31;49mVersionV\x1b[0;0;0m\n" );
	debugf( "\x1b[0;31;49mVersionV\x1b[0;0;0m Serial Out\n" );

	multiboot_echo_to_serial();
	memory_initalize();
	kdebug_initalize();
	elf_initalize( (uint32_t)kernel_main );
	interrupts_initalize();
	
	observer_initalize();
	ps2_initalize();

	serial_enable_interrupts();
	task_initalize();
	
	pci_initalize();
	ahci_initalize();
	fs_initalize();
	//intel8254_initalize();

	if( GRAPHICS_ACTIVE ) {
		vga_initalize();
		vui_initalize();
		
		vui_console_main( 0, NULL );
	}
	
	serial_clear_buffer( COM2 );
	READY_FOR_INPUT = true;

	task_check();

	asm( "sti" );
	kshell_run();

	klog( "\n\nEnd of line." );

	while( true ) { x = x - x + 1; }
}
