#if !defined(TASK_INCLUDED)
#define TASK_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "bootstrap.h"
#include "interrupts.h"
#include "memory.h"
#include "elf.h"

#define TASK_A_CHECK_NUM 37
#define TASK_B_CHECK_NUM 42

#define TASK_MAX 255

#define TASK_ID_INVALID -1
#define TASK_ID_KERNEL 0

#define TASK_TYPE_INVALID -1
#define TASK_TYPE_PROCESS 0
#define TASK_TYPE_THREAD  1
#define TASK_TYPE_SERVICE 2
#define TASK_TYPE_MODULE  3
#define TASK_TYPE_KPROCESS 4

#define TASK_STATUS_INVALID -1
#define TASK_STATUS_INACTIVE 0
#define TASK_STATUS_ACTIVE 1
#define TASK_STATUS_WAIT 2
#define TASK_STATUS_MODHACK 9

#define TASK_VIRT_HEAP_BASE 0x40000000

typedef struct {
	int32_t id; //0
	int32_t type; //4
	int32_t status; //8
	uint32_t saved_esp; //C

	int32_t num_syms;
 	Elf32_Sym * sym_table;
	uint8_t * str_table;
	uint8_t * raw_data;
	void * code_start_phys;
	void * data_start_phys;
	void * code_start_virt;
	void * data_start_virt;
	void * virt_heap_top;
	void * stack_base;
	void * stack_top;
	void * entry;
	page_directory_entry * code_page_table;
	page_directory_entry * data_page_table;

	uint32_t start_time;
	uint32_t total_share_time;
	uint32_t share_time_start;

	x86_context * context_at_interrupt;
	x86_context	context;

	char name[25];
} task;



void task_initalize( void );
int32_t task_add( task *t );
task * switch_next_task( void );
task * switch_task_to( uint32_t task_id );
task * get_current_task( void );
int32_t get_current_task_id( void );
void task_set_name( int32_t task_id, char * n );
void task_set_active( int32_t task_id );
void task_set_inactive( int32_t task_id );
void task_set_modhack( int32_t task_id );
task * get_tasks( void );
void task_initalize_and_run( int32_t task_id );
x86_context * change_to_partial_task_context( int32_t task_id, x86_context *old_context );
void restore_from_partial_task_context( int32_t previous_task_id, int32_t current_task_id, x86_context *previous_task_context );
char * task_type_to_string( int32_t type );
char * task_status_to_string( int32_t status );
void task_exit( void );
void task_end_from_wrapper( void );
void set_next_active_task( int32_t task_id );
void set_current_task_id( int32_t task_id );
int kfork( void );
void setup_test_task( void );
void test_task_call( void );
void task_check( void );
void task_check_a( void );
void task_check_b( void );
uint32_t kexec( char * task_name, uint32_t * addr, const char *argv[] );

#ifdef __cplusplus
}
#endif

#endif