#ifndef KERNELHEADER_INCLUDED
#define KERNELHEADER_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <globals.h>
#include <printf.h>
#include <debug.h>
#include <kmalloc.h>
#include <string.h>
#include <memory.h>
#include <bootstrap.h>

typedef uint8_t byte;
typedef uint16_t word;

#define do_immediate_shutdown() outportb( 0xF4, 0x00 )

#ifdef __cplusplus
}
#endif

#endif