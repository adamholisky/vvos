#ifndef KERNELMAIN_INCLUDED
#define KERNELMAIN_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <multiboot.h>

void kernel_main( unsigned long mb_magic, multiboot_info_t *mb_info );

#ifdef __cplusplus
}
#endif

#endif