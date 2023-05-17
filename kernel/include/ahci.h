#if !defined(AHCI_INCLUDED)
#define AHCI_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

void ahci_initalize( void );
bool ahci_read_sector( uint32_t sector, uint32_t *buffer );
bool ahci_read_at_byte_offset( uint32_t offset, uint32_t size, uint8_t *buffer );
bool ahci_read_at_byte_offset_512_chunks( uint32_t offset, uint32_t size, uint8_t *buffer );

#ifdef __cplusplus
}
#endif

#endif