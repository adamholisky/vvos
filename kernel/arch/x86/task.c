#include "kernel.h"
#include "interrupts.h"
#include "syscall.h"
#include "elf.h"
#include "task.h"

uint32_t task_current;
uint32_t task_last;
uint32_t task_switch_counter;

task tasks[ TASK_MAX ];

extern void save_context( x86_context *old_context );
extern void restore_context( x86_context *old_context);

void task_initalize( void ) {
	//log_entry_enter();

	int i;

	for( i = 0; i < TASK_MAX; i++ ) {
		tasks[i].id = TASK_ID_INVALID;
		tasks[i].type = TASK_TYPE_INVALID;
		tasks[i].status = TASK_STATUS_INVALID;
	}

	task_current = 0;
	task_last = 0;

	tasks[ TASK_ID_KERNEL ].id = TASK_ID_KERNEL;
	tasks[ TASK_ID_KERNEL ].type = TASK_TYPE_PROCESS;
	tasks[ TASK_ID_KERNEL ].status = TASK_STATUS_ACTIVE;
	tasks[ TASK_ID_KERNEL ].code_start_phys = kernel_main - KERNEL_VIRT_LOAD_BASE;
	tasks[ TASK_ID_KERNEL ].code_start_virt = kernel_main;
	tasks[ TASK_ID_KERNEL ].data_start_phys = (void *)get_physical_memory_base();
	tasks[ TASK_ID_KERNEL ].data_start_virt = (void *)KERNEL_VIRT_HEAP_BASE;
	strcpy( tasks[ TASK_ID_KERNEL ].name, "kernel_main" );

	//log_entry_exit();
}

int32_t task_add( task *t ) {
	int i = 0;

	// Find the first open check. Do a better test for a true free task here.
	for( i = 0; i < TASK_MAX; i++ ) {
		if( tasks[i].id == TASK_ID_INVALID ) {
			break;
		}
	}

	tasks[i].stack_base = t->stack_base;
	tasks[i].stack = t->stack;
	tasks[i].entry = t->entry;
	tasks[i].id = i;
	tasks[i].status = (t->id == TASK_ID_KERNEL ? TASK_STATUS_ACTIVE : TASK_STATUS_WAIT);
	tasks[i].type = t->type;
	tasks[i].code_start_phys = t->code_start_phys;
	tasks[i].code_start_virt = t->code_start_virt;
	tasks[i].data_start_phys = t->data_start_phys;
	tasks[i].data_start_virt = t->data_start_virt;
	tasks[i].virt_heap_top = (void *)TASK_VIRT_HEAP_BASE;	
	tasks[i].sym_table = t->sym_table;
	tasks[i].str_table = t->str_table;
	tasks[i].raw_data = t->raw_data;
	tasks[i].num_syms = t->num_syms;

	tasks[i].context.gs = 0x10;
	tasks[i].context.gs_padding = 0;
	tasks[i].context.fs = 0x10;
	tasks[i].context.fs_padding = 0;
	tasks[i].context.es = 0x10;
	tasks[i].context.es_padding = 0;
	tasks[i].context.ds = 0x10;
	tasks[i].context.ds_padding = 0;
	tasks[i].context.edi = 0;
	tasks[i].context.esi = 0;
	tasks[i].context.ebp = 0;
	tasks[i].context.esp = (uint32_t)tasks[i].stack_base; // ignored in popad, but doing so for good measure
	tasks[i].context.ebx = 0;
	tasks[i].context.edx = 0;
	tasks[i].context.ecx = 0;
	tasks[i].context.eax = 0;
	tasks[i].context._esp = (uint32_t)&tasks[i].context._esp;
	// err element not present? TODO fix logic
	tasks[i].context.eip = (uint32_t)tasks[i].entry;
	tasks[i].context.cs = 0x08;
	tasks[i].context.eflags = 0x202;
	tasks[i].context.ss = 0x10;

	// Load in the stack and set the context up so it handles the first schedule entry correctly
	memcpy( tasks[i].stack, &tasks[i].context, sizeof( x86_context ) );
	tasks[i].context_at_interrupt = tasks[i].stack;
	tasks[i].stack_eip = tasks[i].context.eip;
	
	//debugf( "t[i].stack == 0x%08X\n", tasks[i].stack );
	//debugf( "t[i].context_at_interrupt == 0x%08X\n", tasks[i].context_at_interrupt );

	//debugf( "tasks[i].context._esp: %08X\n", tasks[i].context._esp );

	tasks[i].code_page_table = kmalloc( sizeof( page_directory_entry ) * PAGE_NUM_TABLES );
	memset( tasks[i].code_page_table, 0, sizeof( page_directory_entry ) * PAGE_NUM_TABLES );

	#ifdef PAGING_PAE
	memset( tasks[i].code_page_table, 0, 8 * PAGE_NUM_TABLES );

	_Pragma("GCC diagnostic push")
	_Pragma("GCC diagnostic ignored \"-Wpointer-to-int-cast\"")
	uint64_t *pt = (uint64_t *)tasks[i].code_page_table;
	*pt = ((uint64_t)get_physical_memory_base() + (uint64_t)tasks[i].code_start_virt - KERNEL_VIRT_HEAP_BASE) + 0x83;
	*pt = 0x00000000FFFFFFFF & *pt;
	_Pragma("GCC diagnostic pop")

	#else
	tasks[i].code_page_table[0].rw = 1;
	tasks[i].code_page_table[0].present = 1;
	tasks[i].code_page_table[0].address = ((uint32_t)get_physical_memory_base() + (uint32_t)tasks[i].code_start_virt - KERNEL_VIRT_HEAP_BASE)>>11;

	tasks[i].code_page_table[1].rw = 1;
	tasks[i].code_page_table[1].present = 1;
	tasks[i].code_page_table[1].address = ((uint32_t)get_physical_memory_base() + (uint32_t)tasks[i].code_start_virt + PAGE_SIZE_IN_BYTES - KERNEL_VIRT_HEAP_BASE)>>11;
	#endif

	//klog( "TID Returing: %d\n", i );
	return i;
}

task * switch_next_task( void ) {
	int32_t task_next = -1;

	if( tasks[task_current].status == TASK_STATUS_ACTIVE ) {
		tasks[task_current].status = TASK_STATUS_INACTIVE;
	}
	
	// do a simple round-robin task scheduler, array based for easy debugging
	// starting from current task spot + 1, iterate through end of possible task
	for( int i = task_current + 1; i < TASK_MAX; i++ ) {
		if( tasks[i].status == TASK_STATUS_INACTIVE ) {
			task_next = i;
			break;
		}
	}

	// if we still don't have a new process, then start over at 0
	// iterate up through and including the current process spot, so we can handle a single process system
	if( task_next == -1 ) {
		for( int j = 0; j < task_current + 1; j++ ) {
			if( tasks[j].status == TASK_STATUS_INACTIVE ) {
				task_next = j;
				break;
			}
		}
	}

	// we should never hit this, so die if we do
	if( task_next == -1 ) {
		klog( "END. proc_next is -1.\n");
		outportb( 0xF4, 0x00 );
	}

	task_current = task_next;
	tasks[task_next].status = TASK_STATUS_ACTIVE;

	task_switch_counter++;

/* 	if( process_switch_counter > 100000 ) {
		klog( "END. Processes switched over 100,000 times.\n");
		outportb( 0xF4, 0x00 );
	} */

	return (tasks + task_current);
}

task * get_current_task( void ) {
	return (tasks + task_current);
}

void task_set_name( int32_t task_id, char * n ) {
	strcpy( tasks[task_id].name, n );
}

task * get_tasks( void ) {
	return tasks;
}

void task_set_active( int32_t task_id ) {
	tasks[task_id].status = TASK_STATUS_ACTIVE;
}

void task_set_inactive( int32_t task_id ) {
	tasks[task_id].status = TASK_STATUS_INACTIVE;
}

void task_set_modhack( int32_t task_id ) {
	tasks[task_id].status = TASK_STATUS_MODHACK;
}

void task_end_from_wrapper( void ) {
	tasks[task_current].status = TASK_STATUS_MODHACK;

	// unload memory, etc...

	sched_yield();
}

void task_exit( void ) {
	tasks[task_current].status = TASK_STATUS_MODHACK;

	// unload memory, etc...

	sched_yield();
}

int32_t get_current_task_id( void ) {
	return task_current;
}

void set_current_task_id( int32_t task_id ) {
	task_current = task_id;
}

void task_initalize_and_run( int32_t task_id ) {
	tasks[task_id].status = TASK_STATUS_INACTIVE;

	// setup null return so stack traces function correctly in tasks.
/* 	uint32_t * task_stack = tasks[task_id].stack_base;
	*task_stack = 0x0;
	*(task_stack + 1) = 0x0; */

	tasks[task_id].context.gs = 0x10;
	tasks[task_id].context.gs_padding = 0;
	tasks[task_id].context.fs = 0x10;
	tasks[task_id].context.fs_padding = 0;
	tasks[task_id].context.es = 0x10;
	tasks[task_id].context.es_padding = 0;
	tasks[task_id].context.ds = 0x10;
	tasks[task_id].context.ds_padding = 0;
	tasks[task_id].context.edi = 0;
	tasks[task_id].context.esi = 0;
	tasks[task_id].context.ebp = 0;
	tasks[task_id].context.esp = (uint32_t)tasks[task_id].stack_base + 8; // ignored in popad, but doing so for good measure
	tasks[task_id].context.ebx = 0;
	tasks[task_id].context.edx = 0;
	tasks[task_id].context.ecx = 0;
	tasks[task_id].context.eax = 0;
	tasks[task_id].context._esp = (uint32_t)&tasks[task_id].context._esp;
	// err element not present? TODO fix logic
	tasks[task_id].context.eip = (uint32_t)tasks[task_id].entry;
	tasks[task_id].context.cs = 0x08;
	tasks[task_id].context.eflags = 0x202;
	tasks[task_id].context.ss = 0x10;

	// Load in the stack and set the context up so it handles the first schedule entry correctly
	memcpy( tasks[task_id].stack, &tasks[task_id].context, sizeof( x86_context ) );
	tasks[task_id].context_at_interrupt = tasks[task_id].stack;
	tasks[task_id].stack_eip = tasks[task_id].context.eip;
}

x86_context * change_to_partial_task_context( int32_t task_id, x86_context *old_context ) {
	int32_t current_task_id = -1;
	task *t = (tasks + task_id);

	save_context( old_context );

	//debugf( "esp: 0x%08X\nedi: 0x%08X\n", old_context->esp, old_context->edi );

	set_current_task_id( t->id );
	set_task_pde( t->code_page_table );

	return old_context;	
}

void restore_from_partial_task_context( int32_t previous_task_id, int32_t current_task_id, x86_context *previous_task_context ) {
	uint32_t r_esp, r_edi;

	set_current_task_id( previous_task_id );
	set_task_pde( get_current_task()->code_page_table );
	
	restore_context( previous_task_context );

	asm volatile (
		"movl %%esp, %0 \n"
		"movl %%edi, %1 \n"
		: "=g"(r_esp), "=g"(r_edi)
	);

	//debugf( "esp: 0x%08X\nedi: 0x%08X\n", r_esp, r_edi );
}

char * task_type_to_string( int32_t type ) {
	switch( type ) {
		case TASK_TYPE_INVALID:
			return "INVALID";
		case TASK_TYPE_MODULE:
			return "MODULE ";
		case TASK_TYPE_PROCESS:
			return "PROCESS";
	}

	return "";
}

char * task_status_to_string( int32_t status ) {
	switch( status ) {
		case TASK_STATUS_ACTIVE:
			return "ACTIVE  ";
		case TASK_STATUS_INACTIVE:
			return "INACTIVE";
		case TASK_STATUS_MODHACK:
			return "MODHACK ";
		case TASK_STATUS_WAIT:
			return "WAIT    ";
		case TASK_STATUS_INVALID:
			return "INVALID ";
	}

	return "";
}

void set_next_active_task( int32_t task_id ) {

}


