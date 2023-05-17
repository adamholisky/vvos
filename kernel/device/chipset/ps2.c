#include <kernel.h>
#include <interrupts.h>
#include <ps2.h>

void ps2_initalize( void ) {
    asm("cli");
	mouse_initalize();
	keyboard_initalize();	
	add_interrupt( 0x21, interrupt_0x21, 0 );
	add_interrupt( 0x2C, interrupt_0x2C, 0 );
	interrupt_unmask_irq( 0x21 );
	interrupt_unmask_irq( 0x2C );
	asm( "sti" );
}