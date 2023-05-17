#ifndef INTERRUPTS_INCLUDED
#define INTERRUPTS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define INT_0 0x8E00     // 1000111000000000 = present, ring0, int_gate
#define INT_3 0xEE00     // 1110111000000000 = present, ring3, int_gate

#define EXCEPTION_DIVIDE_BY_ZERO 0x0
#define EXCEPTION_DEBUG 0x1
#define EXCEPTION_NMI 0x2
#define EXCEPTION_BREAKPOINT 0x3
#define EXCEPTION_OVERFLOW 0x4
#define EXCEPTION_BOUND_RANGE_EXCEEDED 0x5
#define EXCEPTION_INVALID_OPCODE 0x6
#define EXCEPTION_DEVICE_NOT_AVAILABLE 0x7
#define EXCEPTION_DOUBLE_FAULT 0x8
#define EXCEPTION_INVALID_TSS 0xA
#define EXCEPTION_SEGMENT_NOT_PRESENT 0xB
#define EXCEPTION_STACK_SEGMENT_FAULT 0xC 
#define EXCEPTION_GENERAL_PROTECTION_FAULT 0xD
#define EXCEPTION_PAGE_FAULT 0xE
#define EXCEPTION_FLOATING_POINT 0x10
#define EXCEPTION_ALIGNMENT_CHECK 0x11
#define EXCEPTION_MACHINE_CHECK 0x12
#define EXCEPTION_SIMD_FLOATING_POINT 0x13
#define EXCEPTION_VIRT_EXCEPTION 0x14
#define EXCEPTION_CONTROL_PROTECTION 0x15
#define EXCEPTION_HV_INJECTION 0x1C
#define EXCEPTION_VMM_COMM 0x1D
#define EXCEPTION_SECURITY 0x1E


#define ALL       0xFF
#define TIMER     0
#define KEYBOARD  1
#define CASCADE   2
#define COM2_4    3
#define COM1_3    4
#define LPT       5
#define FLOPPY    6
#define FREE7     7
#define CLOCK     8
#define FREE9     9
#define FREE10    10
#define FREE11    11
#define PS2MOUSE  12
#define COPROC    13
#define IDE_1     14
#define IDE_2     15

#define PIC_PRIMARY			0x20
#define PIC_PRIMARY_DATA	0x21
#define PIC_SECONDARY		0xA0
#define PIC_SECONDARY_DATA	0xA1
#define PIC_EOI				0x20

#define ICW1_INIT    0x10               // required for PIC initialisation
#define ICW1_EDGE    0x08               // edge triggered IRQs
#define ICW1_SINGLE  0x02               // only MASTER (not cascaded)
#define	ICW1_ICW4    0x01               // there IS an ICW4 control word

#define ICW4_SFNM    0x10               // Special Fully Nested Mode
#define ICW4_BUFFER  0x08               // Buffered Mode
#define ICW4_MASTER  0x04               // this is the Master PIC
#define ICW4_AEOI    0x02               // Auto EOI
#define ICW4_8086    0x01               // 80/86 Mode

typedef struct {
	uint16_t            low_offset;                         // low nibble of offset to handler of interrupt 
	uint16_t            selector;                           // GDT selector used 
	uint16_t            settings;                           // settings for int 
	uint16_t            high_offset;                        // high nibble to handler code 
}  __attribute__ ((packed)) x86_interrupt;

typedef struct {
     uint16_t           limit;                             // limit or Size of IDT 
     x86_interrupt  	*base;                    // a pointer to the base of the IDT 
} __attribute__ ((packed)) idtr;

typedef struct {
	uint16_t	gs; // 0
	uint16_t	gs_padding;
	uint16_t	fs; // 4 8
	uint16_t	fs_padding;
	uint16_t	es; // 8 10
	uint16_t	es_padding;
	uint16_t	ds; // c 18
	uint16_t	ds_padding;
	
	uint32_t	edi; // 10 20
	uint32_t	esi; // 14 28
	uint32_t	ebp; // 18 30
	uint32_t	esp; // 1c 38
	uint32_t	ebx; // 20 40
	uint32_t	edx; // 24 48
	uint32_t	ecx; // 28 50
	uint32_t	eax; // 2C 58

	uint32_t	_esp; // 30
	
	uint32_t	err; // 34
	uint32_t	eip; // 38
	uint16_t	cs; // 3C
	uint16_t	cs_padding;
	uint32_t	eflags; // 40
	uint32_t  	useresp; // 44
	uint32_t	ss; //48
} __attribute__ ((packed)) x86_context;

union x86flags {
	uint32_t	all_flags;

	struct {
		uint32_t cf:1; 
		uint32_t must_be_1:1;
		uint32_t pf:1;
		uint32_t must_be_0_1:1;
		uint32_t af:1; 
		uint32_t must_be_0_2:1;
		uint32_t zf:1;
		uint32_t sf:1;
		uint32_t tf:1;  
		uint32_t ifen:1;  
		uint32_t df:1;
		uint32_t of:1;
		uint32_t iopl:2; 
		uint32_t nt:1;
		uint32_t must_be_0_3:1;
		uint32_t rf:1; 
		uint32_t vm:1;
		uint32_t ac:1;
		uint32_t vif:1;
		uint32_t vip:1; 
		uint32_t id:1;   
		uint32_t must_be_0_4:2;  
		uint32_t must_be_0_5:4; 
	} eflags_bits;
};

typedef struct {
	uint32_t present:1;
	uint32_t write:1;
	uint32_t user:1;
	uint32_t reserved_write:1;
	uint32_t instruction_fetch:1;
	uint32_t protection_key:1;
	uint32_t shadow_stack:1;
	uint32_t reserved:8;
	uint32_t sgx:1;
	uint32_t reserved2:15;
} __attribute__ ((packed)) page_fault_err;

extern void interrupt_0( void );
extern void interrupt_1( void );
extern void interrupt_2( void );
extern void interrupt_3( void );
extern void interrupt_4( void );
extern void interrupt_5( void );
extern void interrupt_6( void );
extern void interrupt_7( void );
extern void interrupt_8( void );
extern void interrupt_9( void );
extern void interrupt_10( void );
extern void interrupt_11( void );
extern void interrupt_12( void );
extern void interrupt_13( void );
extern void interrupt_14( void );
extern void interrupt_16( void );
extern void interrupt_17( void );
extern void interrupt_18( void );
extern void interrupt_19( void );
extern void interrupt_0x20( void );
extern void interrupt_0x21( void );
extern void interrupt_0x22( void );
extern void interrupt_0x23( void );
extern void interrupt_0x24( void );
extern void interrupt_0x25( void );
extern void interrupt_0x26( void );
extern void interrupt_0x27( void );
extern void interrupt_0x28( void );
extern void interrupt_0x29( void );
extern void interrupt_0x2A( void );
extern void interrupt_0x2B( void );
extern void interrupt_0x2C( void );
extern void interrupt_0x2D( void );
extern void interrupt_0x2E( void );
extern void interrupt_0x2F( void );
extern void interrupt_0x30( void );
extern void interrupt_0x31( void );
extern void interrupt_0x32( void );
extern void interrupt_0x99( void );

void interrupts_initalize( void );
void interrupt_default_handler( uint32_t *stack, uint32_t interrupt_num, uint32_t route_code, x86_context ** _stack );
void load_idtr( void );
void add_interrupt( int number, void (*handler)(), uint32_t dpl );
void load_exceptions();
void remap_pic( int pic1, int pic2 );
void interrupt_mask_irq( uint8_t irq );
void interrupt_unmask_irq( uint8_t irq );
uint32_t get_timer_counter( void );
void replace_stack_on_int_exit( x86_context * stack );
void page_fault_test( void );
extern uint32_t get_cr3( void );
void serial_interrupt_read_from_com2( void );
void serial_interrupt_read_from_com1( void );

#ifdef __cplusplus
}
#endif

#endif