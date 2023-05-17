#if !defined(DLFCN_INCLUDED)
#define DLFCN_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <elf.h>

typedef struct {
    char filename[256];
    int flags;
    uint32_t *base;
    Elf32_Shdr *string_section;
    Elf32_Shdr *symbol_section;
} dl_info;

void *dlopen( const char *filename, int flags );
int dlclose( void *handle );

void *dlsym( void *handle, const char *symbol );


#ifdef __cplusplus
}
#endif

#endif