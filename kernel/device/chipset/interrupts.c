#include "kernel.h"
#include "interrupts.h"
#include "task.h"
#include "syscall.h"
#include "keyboard.h"
#include "mouse.h"

idtr            sIDTR;
x86_interrupt   IDT[256];
uint8_t	        pic1_irq_mask;
uint8_t	        pic2_irq_mask;
bool	        debug_interrupts = false;
bool 	        gpf_fired = false;
bool			can_switch = true;
uint32_t 		timer_var = 0;
uint32_t		timer_counter = 0;
x86_context		new_post_x86_context;
bool			use_new_post_x86_context = false;
bool			handle_page_fault_test = false;
uint32_t 		*page_fault_mem;
bool			prevent_page_fault_loop = false;
uint32_t		*prevent_pfl_page = NULL;
char			*invalid_memory_address = "Invalid memory address.";

extern uint32_t get_cr2( void );

void interrupts_initalize( void ) {
	//log_entry_enter();
    remap_pic( 0x20, 0x28 );

    add_interrupt(0, interrupt_0, 0);
    add_interrupt(1, interrupt_1, 0);
    add_interrupt(2, interrupt_2, 0);
    add_interrupt(3, interrupt_3, 0);
    add_interrupt(4, interrupt_4, 0);
    add_interrupt(5, interrupt_5, 0);
    add_interrupt(6, interrupt_6, 0);
    add_interrupt(7, interrupt_7, 0);
    add_interrupt(8, interrupt_8, 0);
    add_interrupt(9, interrupt_9, 0);
    add_interrupt(10, interrupt_10, 0);
    add_interrupt(11, interrupt_11, 0);
    add_interrupt(12, interrupt_12, 0);
    add_interrupt(13, interrupt_13, 0);
    add_interrupt(14, interrupt_14, 0);
    add_interrupt(16, interrupt_16, 0);
    add_interrupt(17, interrupt_17, 0);
    add_interrupt(18, interrupt_18, 0);
    add_interrupt(19, interrupt_19, 0);

	add_interrupt( 0x20, interrupt_0x20, 0 );
	
	add_interrupt( 0x22, interrupt_0x22, 0 );
	add_interrupt( 0x23, interrupt_0x23, 0 );
	add_interrupt( 0x24, interrupt_0x24, 0 );
	add_interrupt( 0x25, interrupt_0x25, 0 );
	add_interrupt( 0x26, interrupt_0x26, 0 );
	add_interrupt( 0x27, interrupt_0x27, 0 );
	add_interrupt( 0x28, interrupt_0x28, 0 );
	add_interrupt( 0x29, interrupt_0x29, 0 );
	add_interrupt( 0x2A, interrupt_0x2A, 0 );
	add_interrupt( 0x2B, interrupt_0x2B, 0 );
	
	add_interrupt( 0x2D, interrupt_0x2D, 0 );
	add_interrupt( 0x2E, interrupt_0x2E, 0 );
	add_interrupt( 0x2F, interrupt_0x2F, 0 );
	add_interrupt( 0x30, interrupt_0x30, 0 );
	add_interrupt( 0x31, interrupt_0x31, 0 );
	add_interrupt( 0x32, interrupt_0x32, 0 );
	add_interrupt( 0x99, interrupt_0x99, 0 );

	// mask everything

/* 	outportb( 0x21, 0xFF );
	outportb( 0xA1, 0xFF ); */

	// WTF

	outportb( PIC_PRIMARY_DATA, 0x00 );
	outportb( PIC_SECONDARY_DATA, 0x00 );

	uint16_t divisor = 11931;      // Calculate our divisor, default 65535 --> 1193180/hz
    outportb( 0x43, 0x36 );             // Set our command byte 0x36
    outportb( 0x40, divisor & 0xFF );   // Set low byte of divisor
    outportb( 0x40, divisor >> 8 );     // Set high byte of divisor

	// unmask the timer
	interrupt_unmask_irq( 0x20 );
	
	interrupt_unmask_irq( 0x23 );
	interrupt_unmask_irq( 0x24 );
	

    load_idtr();

    asm( "sti" );

	klog( "Enabled\n" );

	page_fault_mem = kmalloc( 4 );

	//asm( "int %0" : : "i"(0x30) );
	log_entry_exit();
}

/* #pragma GCC push_options
#pragma GCC optimize ("O0") */
void interrupt_default_handler( uint32_t *stack, uint32_t interrupt_num, uint32_t route_code, x86_context ** _context ) {
	task *p;
	x86_context * context = *_context;
	uint32_t * uint32_stack_pointer = (uint32_t *)*_context;
	uint32_t fault_addr = 0;
	uint32_t corrected_err = context->err;
	uint32_t corrected_eip = context->eip;
	page_fault_err *pf_err = NULL;
	bool error_code_present = false;
	bool allow_return = false;
	//dbC();
	//klog( "interrupt_default_handler:\n    interrupt_num: 0x%X\n    route_code: 0x%0X\n", interrupt_num, route_code );

	if( route_code == 0x01 ) {
        switch( interrupt_num ) {
			case EXCEPTION_DIVIDE_BY_ZERO:
				klog( "Exception: EXCEPTION_DIVIDE_BY_ZERO\n");
				break;
			case EXCEPTION_DEBUG:
				klog( "Exception: EXCEPTION_DEBUG\n");
				break;
			case EXCEPTION_NMI:
				klog( "Exception: EXCEPTION_NMI\n");
				break;
			case EXCEPTION_BREAKPOINT:
				klog( "Exception: EXCEPTION_BREAKPOINT\n");
				break;
			case EXCEPTION_OVERFLOW:
				klog( "Exception: EXCEPTION_OVERFLOW\n");
				break;
			case EXCEPTION_BOUND_RANGE_EXCEEDED:
				klog( "Exception: EXCEPTION_BOUND_RANGE_EXCEEDED\n");
				break;
			case EXCEPTION_INVALID_OPCODE:
				klog( "Exception: EXCEPTION_INVALID_OPCODE\n");
				break;
			case EXCEPTION_DEVICE_NOT_AVAILABLE:
				klog( "Exception: EXCEPTION_DEVICE_NOT_AVAILABLE\n");
				break;
			case EXCEPTION_DOUBLE_FAULT:
				klog( "Exception: EXCEPTION_DOUBLE_FAULT\n");
				error_code_present = true;
				break;
			case EXCEPTION_INVALID_TSS:
				klog( "Exception: EXCEPTION_INVALID_TSS\n");
				error_code_present = true;
				break;
			case EXCEPTION_SEGMENT_NOT_PRESENT:
				klog( "Exception: EXCEPTION_SEGMENT_NOT_PRESENT\n");
				error_code_present = true;
				break;
			case EXCEPTION_STACK_SEGMENT_FAULT :
				klog( "Exception: EXCEPTION_STACK_SEGMENT_FAULT\n");
				error_code_present = true;
				break;
			case EXCEPTION_GENERAL_PROTECTION_FAULT:
				klog( "Exception: EXCEPTION_GENERAL_PROTECTION_FAULT\n");
				error_code_present = true;
				break;
			case EXCEPTION_PAGE_FAULT:
				debugf( "\n" );
				klog( "Exception: EXCEPTION_PAGE_FAULT\n");
				error_code_present = true;
				klog( "    %s() @ 0x%08X\n", kdebug_get_function_at( context->eip ), context->eip );

				//asm volatile("movL %%cr2,%0" :"=m"(fault_addr));
				fault_addr = get_cr2();
				klog( "    Fault address: 0x%08X\n", fault_addr );

				pf_err = (page_fault_err *)&context->err;
				klog( "    Flags present:" );
				if( pf_err->present ) { debugf( " present "); }
				if( pf_err->write ) { debugf( " write "); }
				if( pf_err->user ) { debugf( " user "); }
				if( pf_err->reserved_write ) { debugf( " reserved_write "); }
				if( pf_err->instruction_fetch ) { debugf( " inst_fetch "); }
				if( pf_err->protection_key ) { debugf( " pk "); }
				if( pf_err->shadow_stack ) { debugf( " shadow_stack "); }
				if( pf_err->sgx ) { debugf( " sgx "); }
				debugf( "\n" );	

				
				if( handle_page_fault_test ) {
					uint32_t *addr = (uint32_t *)fault_addr;
					klog( "    Pagefault test caught 0x%08X\n", addr );

					uint32_t *allocd = page_allocate( 1 );
					uint32_t *phys_addr = get_physical_addr_from_virt( allocd );
					page_map( addr, phys_addr );

					allow_return = true;
				} else {
					if( prevent_page_fault_loop ) {
						uint32_t *pfl_phys = get_physical_addr_from_virt( prevent_pfl_page );
						page_map( (uint32_t *)fault_addr, pfl_phys );

						allow_return = true;
					} else {
						prevent_page_fault_loop = true;
						prevent_pfl_page = page_allocate(1);
						uint32_t *pfl_page_phys = get_physical_addr_from_virt( prevent_pfl_page );
						page_map( (uint32_t *)fault_addr, pfl_page_phys );
						memcpy( prevent_pfl_page, invalid_memory_address, strlen( invalid_memory_address ) );
					}					
				}
				
				break;
			case EXCEPTION_FLOATING_POINT :
				klog( "Exception: EXCEPTION_FLOATING_POINT\n");
				break;
			case EXCEPTION_ALIGNMENT_CHECK :
				klog( "Exception: EXCEPTION_ALIGNMENT_CHECK\n");
				error_code_present = true;
				break;
			case EXCEPTION_MACHINE_CHECK :
				klog( "Exception: EXCEPTION_MACHINE_CHECK\n");
				break;
			case EXCEPTION_SIMD_FLOATING_POINT :
				klog( "Exception: EXCEPTION_SIMD_FLOATING_POINT\n");
				break;
			case EXCEPTION_VIRT_EXCEPTION :
				klog( "Exception: EXCEPTION_VIRT_EXCEPTION\n");
				break;
			case EXCEPTION_CONTROL_PROTECTION :
				klog( "Exception: EXCEPTION_CONTROL_PROTECTION\n");
				error_code_present = true;
				break;
			case EXCEPTION_HV_INJECTION :
				klog( "Exception: EXCEPTION_HV_INJECTION\n");
				break;
			case EXCEPTION_VMM_COMM :
				klog( "Exception: EXCEPTION_VMM_COMM\n");
				error_code_present = true;
				break;
			case EXCEPTION_SECURITY :
				klog( "Exception: EXCEPTION_SECURITY\n");
				error_code_present = true;
				break;
            default:
                klog( "Exception: Unhandled %02X.\n", interrupt_num );
        }

		if( !error_code_present ) {
			corrected_eip = context->err;
			corrected_err = 0;
		}

		klog( "    current_task: 0x%X\n", get_current_task_id() );
		klog( "    eax:  0x%08X  ebx:  0x%08X  ecx:  0x%08X  edx:  0x%08X\n", context->eax, context->ebx, context->ecx, context->edx );
		klog( "    esp:  0x%08X  ebp:  0x%08X  esi:  0x%08X  edi:  0x%08X\n", context->esp, context->ebp, context->esi, context->edi );
		klog( "    ds:   0x%04X  es:   0x%04X  fs:   0x%04X  gs:   0x%04X\n", context->ds, context->es, context->fs, context->gs );
		klog( "    esp:  0x%08X  cs:   0x%04X  ef:   0x%08X  err:  0x%08X\n", context->_esp, context->cs, context->eflags, corrected_err);
		klog( "    eip:  0x%08X\n", corrected_eip );

		stackframe *frame;

		klog( "\n" );
		klog( "    Stack Trace\n" );
		klog( "    ----------------------\n" );
		klog( "    % 2d:    0x%08X %s\n", 1, corrected_eip, kdebug_get_function_at(corrected_eip) );

		asm ("movl %%ebp,%0" : "=r"(frame));
		frame = (stackframe *)frame->ebp;

		
		for( int i = 1; (frame != NULL) && (i < STACKFRAME_MAX); i++ ) {
			klog( "    % 2d:    0x%08X %s\n", i+1, frame->eip, kdebug_get_function_at(frame->eip) );
			//kdebug_peek_at( frame );
			frame = (stackframe *)frame->ebp;
		}

		/* if( uint32_stack_pointer == NULL ) {
			klog( "uint32_stack_pointer is null, output surpressed\n" );
		} else {
			for( int i = -2; i < 30; i++ ) {
				klog( "%d: 0x%08X:    0x%08X\n", i, (uint32_stack_pointer + i), *(uint32_stack_pointer + i) );
			}
		} */

		if( !allow_return ) {
			klog( "Shutdown via END_IMMEDIATELY.\n");
			outportb( 0xF4, 0x00 );
		} else {
			klog( "Allowing return...\n" );
		}
    }

	if( route_code == 0x02 ) {
        switch( interrupt_num ) {
			case 0x20:
				//debugf( "." );
				timer_counter++;
				if( timer_var < 101 ) {  // default 19
					timer_var++;
				} else {
					debugf( "!" );
					timer_var = 0;
				}
				break;
			case 0x21:
				keyboard_interrupt_handler();
				break;
			case 0x23:
				serial_interrupt_read_from_com2();
				break;
			case 0x24:
				serial_interrupt_read_from_com1();
				break;
			case 0x2C:
				mouse_handler();
				break;
			case 0x30:
				debugf( "+\n" );
				break;
			case 0x31:
				debugf( "-\n" );
				break;
			case 0x99:
				syscall_handler( stack, _context );
				break;
		}
	}
	
	// Acknowledge the interrupt with the pic
	if( interrupt_num > 0x28 ) {
		outportb( 0xA0, 0x20 );
	}

	outportb( 0x20, 0x20 );
}
/* #pragma GCC pop_options
 */
void replace_stack_on_int_exit( x86_context * stack ) {
	memcpy( &new_post_x86_context, stack, sizeof( x86_context ) );
	use_new_post_x86_context = true;
}

uint32_t get_timer_counter( void ) {
	return timer_counter;
}

void load_idtr( void ) {
    sIDTR.limit  = 256*(sizeof(x86_interrupt)-1);
    sIDTR.base   = IDT;

	idtr *IDTRptr = &sIDTR;

	//debug_f( "IDTR: 0x%08X\n", IDTRptr );

    asm volatile("lidt (%0) ": : "r"(IDTRptr));
}

void remap_pic(int pic1, int pic2) {
	uint8_t primary_data;
	uint8_t secondary_data;

	primary_data = inportb( PIC_PRIMARY_DATA );                     // save state of MASTER DATA
	secondary_data = inportb( PIC_SECONDARY_DATA );                      // save state of SLAVE DATA

	outportb( PIC_PRIMARY, PIC_EOI );                      // Send EOI | resets the chip

	outportb( PIC_PRIMARY, ICW1_INIT + ICW1_ICW4 );      // ICW1 control word setup | just basic PIC stuff
	outportb( PIC_SECONDARY, ICW1_INIT + ICW1_ICW4 );       // see pic.h for more details about the values

	outportb( PIC_PRIMARY_DATA, pic1 );                 // ICW2 maps IRQs 0-7 to whatever kernel passes
	outportb( PIC_SECONDARY_DATA, pic2 );                  // and same here except with IRQs 8-15 

	outportb( PIC_PRIMARY_DATA, 0x04 );                 // ICW3 
	outportb( PIC_SECONDARY_DATA, 0x02 );

	outportb( PIC_PRIMARY_DATA, ICW4_8086 );            // ICW4 control word setup
	outportb( PIC_SECONDARY_DATA, ICW4_8086 );

	outportb( PIC_PRIMARY_DATA, primary_data );                    // restore both MASTER DATA
	outportb( PIC_SECONDARY_DATA, secondary_data );                     // restore SLAVE DATA
}

void add_interrupt( int number, void (*handler)(), uint32_t dpl ) {
	 uint16_t selector = 0x08;
	 uint16_t settings = 0;
     uint32_t offset = (uint32_t)handler;

	 /* get CS selector */
     asm volatile("movw %%cs,%0" :"=g"(selector));

	 /* set settings options depending on dpl */
	 switch(dpl) {
	 	case 0: 
			settings = INT_0;
			break;
	 	case 1:
	 	case 2:
	 	case 3: 
			settings = INT_3;
			break;
	 }
	 
	 /* set actual values of int */
	 IDT[number].low_offset   = (offset & 0xFFFF);
	 IDT[number].selector     = selector;
	 IDT[number].settings     = settings;
     IDT[number].high_offset  = (offset >> 16);
}

/* makes IRQ unavailable so it can't fire an interrupt */
void interrupt_mask_irq( uint8_t irq ) {
	if( irq == ALL ) {
		outportb( PIC_PRIMARY_DATA, 0xFF );
		outportb( PIC_SECONDARY_DATA, 0xFF );
		pic1_irq_mask = 0xFF;
		pic2_irq_mask = 0xFF;
	} else {
		irq = irq | (1 << irq);
		if( irq < 8 )
			outportb( PIC_PRIMARY_DATA, irq & 0xFF );
		else
			outportb( PIC_SECONDARY_DATA, irq >> 8 );
	}
}

/* opposite of above */
void interrupt_unmask_irq( uint8_t irq ) {
	if( irq == ALL ) {
		outportb( PIC_PRIMARY_DATA, 0x00 );
		outportb( PIC_SECONDARY_DATA, 0x00 );
		pic1_irq_mask = 0x00;
		pic2_irq_mask = 0x00;
	} else {
		irq = irq & (1 << irq);

		if( irq < 8 ) {
			pic1_irq_mask = pic1_irq_mask ^ (1 << irq);
			outportb( PIC_PRIMARY_DATA, pic1_irq_mask );
		}
		else {
			pic2_irq_mask = pic2_irq_mask ^ (1 << irq);
			outportb( PIC_SECONDARY_DATA, pic2_irq_mask );
		}
	}
}

// let's see if we can handle a page fault correctly!
void page_fault_test( void ) {
	uint32_t *page_fault_pointer = (uint32_t *)0xFFFF0000;

	handle_page_fault_test = true;

	klog( "Starting test...\n" );
	
	*page_fault_pointer = 0xBBBBAAAA;

	if( *page_fault_pointer == 0xBBBBAAAA) {
		klog( "Test passed! page_fault_pointer @ 0x%08X == 0x%08X\n", page_fault_pointer, *page_fault_pointer );
	} else {
		klog( "Test failed? Ummm...\n" );
	}
	
	handle_page_fault_test = false;
}


void serial_interrupt_read_from_com2( void ) {
	serial_interrupt_handler( COM2 );

	/* if( serial_buffer_is_ready() ) {
		klog( "COM2: %c\n", serial_buffer_get_char() );
	} */
}

void serial_interrupt_read_from_com1( void ) {
	// serial_read_port( COM1 );
	serial_interrupt_handler( COM1 );
}

