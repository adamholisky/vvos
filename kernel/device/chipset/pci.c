#include "kernel.h"
#include "pci.h"

pci_header	pci_devices[ 15 ];
uint32_t	pci_devices_top;

uint8_t		hack_8254_bus = 0;
uint8_t		hack_8254_device = 0;

#undef kdebug_pci

void pci_initalize( void ) {
	//log_entry_enter();
	uint16_t vendor;
	uint8_t bus;
	uint8_t device;
	pci_header *d;
	int f, i;

	pci_devices_top = 0;

	for( bus = 0; bus < 0xFF; bus++ ) {
		
		for( device = 0; device < 32; device++ ) {
			vendor = 0;

			vendor = pci_config_read( bus, device, 0, 0 );

			if( vendor != 0xFFFF ) {
				pci_read_header( &pci_devices[pci_devices_top], bus, device, 0 );
				pci_devices_top++;

				if( pci_devices[pci_devices_top - 1].device_id == 0x100E ) {
					klog( "0x100E found: bus = %d, device = %d\n", bus, device );
					hack_8254_bus = bus;
					hack_8254_device = device;
				}

				if( pci_devices[ pci_devices_top - 1 ].header_type == 0x80 ) {
					for( f = 1; f < 8; f++ ) {
						vendor = pci_config_read( bus, device, f, 0 );

						if( vendor != 0xFFFF ) {
							pci_read_header( &pci_devices[pci_devices_top], bus, device, f );
							pci_devices_top++;
						}
					}
				}
			}
		}
	}

	#ifdef kdebug_pci
	k_log( sys_pci, level_info, "Num devices: %d", pci_devices_top );
	#endif

	for( i = 0; i < pci_devices_top; i++ ) {
		d = &pci_devices[i];

		if( d->class_code == 0x06 ) continue;

		
		k_log( sys_pci, level_info, "[%d] Class: %02X   Subclass: %02X   Prog IF: %02X   Revision: %02X   Vendor: %04X   Device ID: %04X",
			     i, d->class_code, d->subclass, d->prog_if, d->revision_id, d->vendor_id, d->device_id );
		
	}

	log_entry_exit();
}

uint16_t pci_config_read( uint8_t bus, uint8_t device, uint8_t function, uint8_t offset ) {
	uint32_t address;
	uint32_t bus32  = (uint32_t)bus;
	uint32_t device32 = (uint32_t)device;
	uint32_t function32 = (uint32_t)function;
	uint32_t result = 0;
 
	address = (uint32_t)((bus32 << 16) | (device32 << 11) | (function32 << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
	out_port_long(0xCF8, address);

	result = in_port_long(0xCFC);
	
	/* if( result != 0xFFFFFFFF ) {
		debugf( "result: 0x%08X    move: %d    end: 0x%08X\n", result, (8 * (offset & 0x3)), (uint16_t)((result) >> (8 * (offset & 0x2))));
	} */
	
	result = (result) >> (8 * (offset & 0x3));

	return (uint16_t)result;
}

void pci_read_header( pci_header *header, uint8_t bus, uint8_t device, uint8_t function ) {
	uint32_t address;
	uint32_t bus32  = (uint32_t)bus;
	uint32_t device32 = (uint32_t)device;
	uint32_t function32 = (uint32_t)function;
	uint32_t result;
	uint32_t * head = (uint32_t *)header;
	uint8_t offset;

	for( int i = 0; i < 0xF; i++ ) {
		address = (uint32_t)((bus32 << 16) | (device32 << 11) | (function32 << 8) | ((uint8_t)(i * 4) & 0xfc) | ((uint32_t)0x80000000));
 
		out_port_long(0xCF8, address);

		result = in_port_long(0xCFC);
		*head = result;
		head++;
	}
}

pci_header * pci_get_header_by_device_id( uint32_t _device_id ) {
	pci_header * return_val;
	bool found = false;

	for( int i = 0; i < pci_devices_top; i++ ) {
		return_val = &pci_devices[i];

		if( return_val->device_id == _device_id ) {
			i = pci_devices_top + 1;
			found = true;
		}
	}

	if( !found ) {
		return_val = NULL;
	}

	return return_val;
}

/* TODO: Rewrite */
uint32_t pci_read_dword( const uint16_t bus, const uint16_t dev, const uint16_t func, const uint32_t reg ) {
	out_port_long( 0xCF8, 0x80000000L | ((uint32_t)bus << 16) |((uint32_t)dev << 11) |
	((uint32_t)func << 8) | (reg & ~3) );

	return in_port_long( ( 0xCFC )  >> ((reg & 2) * 8) & 0xFFFF );
	//return in_port_long( 0xCFC + (reg & 3) );
}

/* TODO: Rewrite */
void pci_write_dword( const uint16_t bus, const uint16_t dev, const uint16_t func, const uint32_t reg, unsigned data ) {
	klog( "bus %x, dev %x, func %x, reg %x, data %x\n", bus, dev, func, reg, data );

	out_port_long( 0xCF8, 0x80000000L | ((uint32_t)bus << 16) |((uint32_t)dev << 11) |
	((uint32_t)func << 8) | (reg & ~3) );
	out_port_long( 0xCFC + (reg & 3), data );
}