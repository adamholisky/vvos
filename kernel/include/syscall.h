#if !defined(SYSCALL_INCLUDED)
#define SYSCALL_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "interrupts.h"

#define SYSCALL_READ	0
#define SYSCALL_WRITE	1
#define SYSCALL_OPEN	2
#define SYSCALL_CLOSE	3
#define SYSCALL_SCHED_YIELD	4
#define SYSCALL_SBRK	5
#define SYSCALL_END		6
#define SYSCALL_EXIT	7
#define SYSCALL_ACTTASK	8
#define SYSCALL_PARTIAL_CONTEXT_SWITCH 9


#define SYSCALL_RT_SUCCESS 0
#define SYSCALL_RT_ERROR 1

typedef struct {
	uint32_t	arg_1;
	uint32_t	arg_2;
	uint32_t	arg_3;
	uint32_t	arg_4;
	uint32_t	arg_5;
	uint32_t	arg_6;
} syscall_args;

uint32_t syscall_handler( x86_context ** stack );
uint32_t syscall( uint32_t call_num, uint32_t num_args, syscall_args * args );

uint32_t read( int fd, void * buff, uint32_t size );
uint32_t syscall_read( int fd, void * buff, uint32_t size );

uint32_t sched_yield( void );
uint32_t syscall_sched_yield( x86_context ** _stack );

uint32_t partial_context_switch( x86_context * new_context );
uint32_t syscall_partial_context_switch( x86_context ** _stack, x86_context * new_context );

uint32_t sbrk( int inc );
uint32_t syscall_sbrk( int inc );

uint32_t exit_from_wrapper( void );
uint32_t syscall_exit_from_wrapper( void );

uint32_t syscall_exit( int code );

uint32_t syscall_activate_task( int32_t task_id );

#define exit_test( n, ret ) asm volatile ( \
				"movl %1, %%eax \n" \
				"movl %2, %%edi \n" \
				"int %3 \n" \
				"movl %%eax, %0" \
				:"=r"(ret) \
				:"r"(SYSCALL_EXIT), "r"(n), "i"(0x99) \
				:"%eax" \
			)

#define activate_task( n, ret ) asm volatile ( \
				"movl %1, %%eax \n" \
				"movl %2, %%edi \n" \
				"int %3 \n" \
				"movl %%eax, %0" \
				:"=r"(ret) \
				:"r"(SYSCALL_ACTTASK), "r"(n), "i"(0x99) \
				:"%eax" \
			)

#ifdef __cplusplus
}
#endif

#endif