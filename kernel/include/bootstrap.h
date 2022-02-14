#if !defined(BOOTSTRAP_INCLUDED)
#define BOOTSTRAP_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint64_t *rem_p);

unsigned long hex2int(char *a, unsigned int len);

#define	isalnum(c)			(isalpha(c) || isdigit(c))
#define	isalpha(c)			(islower(c) || isupper(c))
#define	isblank(c)			((c) == ' ' || (c) == '\t')
#define	iscntrl(c)			((c) >= 0x0 && (c) <= 0x8)
#define	isdigit(c)			((c) >= '0' && (c) <= '9')
#define	isgraph(c)			(ispunct(c) || isalnum(c))
#define	islower(c)			((c) >= 'a' && (c) <= 'z')
#define	isprint(c)			(isgraph(c) || isspace(c))
#define	ispunct(c)			(((c) >= 0x21 && (c) <= 0x2F) || ((c) >= 0x3A && (c) <= 0x40) || ((c) >= 0x5B && (c) <= 0x60) || ((c) >= 0x7B && (c) <= 0x7E))
#define	isspace(c)			((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n' || (c) == '\f' || (c) == '\v')
#define	isupper(c)			((c) >= 'A' && (c) <= 'Z')
#define	isxdigit(c)			(isdigit(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#define	tolower(c)			(isupper(c) ? ((c) + 'a' - 'A') : (c))
#define	toupper(c)			(islower(c) ? ((c) + 'A' - 'a') : (c))

/* Outputs a byte to the specified hardware port */
static inline void outportb( uint32_t port, uint8_t value ) {
    __asm__ __volatile__ ("outb %%al,%%dx"::"d" (port), "a" (value));
}

/* Outputs a word to a port */
static inline void outportw( uint32_t port, uint32_t value ) {
    __asm__ __volatile__ ("outw %%ax,%%dx"::"d" (port), "a" (value));
}

/* gets a byte from a port */
static inline uint8_t inportb( uint32_t port ) {
    uint8_t value;
    __asm__ __volatile__ ("inb %w1,%b0" : "=a"(value) : "d"(port));
    return value;
}

static inline uint8_t inportw( uint32_t port ) {
    uint8_t value;
    __asm__ __volatile__ ("inw %%dx,%%ax" : "=a"(value) : "d"(port));
    return value;
}

static inline void out_port_long( uint16_t port, uint32_t value) {
    __asm__ __volatile__ ( "outl %%eax, %%dx" : : "d" (port), "a" (value) );
}

static inline uint32_t in_port_long( uint16_t port ) {
    uint32_t value;
    __asm__ __volatile__ ("inl %%dx, %%eax" : "=a"(value) : "dN"(port));
    return value;
}

#define debug_region_start(x)   if( x == 1 ) {
#define debug_region_end(x)     }

#define set_bit(x,b) x | 1<<b
#define clear_bit(x,b) x ~ 1<<b 
#define flip_bit(x,b) x ^ 1<<b
#define test_bit(x,b) x & 1<<b

#ifdef __cplusplus
}
#endif

#endif