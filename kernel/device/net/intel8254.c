#include "kernel.h"
#include "pci.h"
#include "intel8254.h"

#define kdebug_intel8254

uint32_t * intel8254_io_port;
extern uint8_t hack_8254_bus;
extern uint8_t hack_8254_device;

uint8_t mac_address[6];

uint16_t read_eprom( uint32_t *addr, uint32_t offset );

void intel8254_initalize( void ) {
	log_entry_enter();
	
	pci_header * device = pci_get_header_by_device_id( 0x100E );

	if( device == NULL ) {
		klog( "Could not find PCI entry for intel8254.\n" );
		return;
	} else {
		klog( "Found\n" );
	}

	klog( "hack_8254_bus: 0x%X\n", hack_8254_bus );
	klog( "hack_8254_device: 0x%X\n", hack_8254_device );

	pci_bist_register * bist = (pci_bist_register *)&(device->bist);

	klog( "Class: %02X   Subclass: %02X   Prog IF: %02X   Revision: %02X   Vendor: %04X   Device ID: %04X\n",
			device->class_code, device->subclass, device->prog_if, device->revision_id, device->vendor_id, device->device_id );

	#ifdef kdebug_intel8254
	klog( "Command:\n" );
	debugf_bit_array( device->command );

	klog( "Bist:\n" );
	debugf_bit_array( device->bist );

	klog( "Status:\n" );
	debugf_bit_array( device->status );
	klog( "Interrupt line: 0x%02X\n", device->interrupt_line );
	klog( "Interrupt pin: 0x%02X\n", device->interrupt_pin );
	klog( "header type: %X\n", device->header_type );
	klog( "Bar0: 0x%08X\n", device->bar0 );
	klog( "Bar1: 0x%08X\n", device->bar1 );
	klog( "Bar2: 0x%08X\n", device->bar2 );
	klog( "Bar3: 0x%08X\n", device->bar3 );
	klog( "Bar4: 0x%08X\n", device->bar4 );
	klog( "Bar5: 0x%08X\n", device->bar5 );

	intel8254_io_port = (uint32_t *)device->bar1;

	uint32_t reg_eerd = 0x0014;

	uint16_t data = 0;

	for( int x = 0; x < 0x40; x = x+4 ) {
		klog( "XXXX x: %X  : 0x%08X\n", x, pci_read_dword( hack_8254_bus, hack_8254_device, 0, x) );
	}
	
	

	
	data = pci_read_dword( hack_8254_bus, hack_8254_device, 0, 0x4 );
	klog( "data: 0x%04X\n", data );
	pci_write_dword( hack_8254_bus, hack_8254_device, 0, 0x4, data | 0x7);

	data = pci_read_dword( hack_8254_bus, hack_8254_device, 0, 0x4 );
	klog( "data: 0x%04X\n", data );
	debugf_bit_array( data );

	uint32_t *dev = page_allocate_and_map( (uint32_t *)0xFEA00000 );
	
	uint32_t *addr_status = (uint32_t *)((uint32_t)dev + (0xFEB80000 - 0xFEA00000) + 0x8);
	dbA();

	klog( "status: 0x%08X\n", *addr_status );
	

	uint16_t mac_address_temp = 0;
	mac_address_temp = read_eprom( dev, 0 );
	mac_address[0] = mac_address_temp & 0xFF;
	mac_address[1] = (mac_address_temp >> 8) & 0xFF;

	mac_address_temp = read_eprom( dev, 1 );
	mac_address[2] = mac_address_temp & 0xFF;
	mac_address[3] = (mac_address_temp >> 8) & 0xFF;

	mac_address_temp = read_eprom( dev, 2 );
	mac_address[4] = mac_address_temp & 0xFF;
	mac_address[5] = (mac_address_temp >> 8) & 0xFF;

	klog( "Mac Address: %02X::%02X::%02X::%02X::%02X::%02X\n", mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5] );

	//klog( "Command:\n" );
	//debugf_bit_array( device->command );
	klog( "status: 0x%08X\n", pci_read_dword( hack_8254_bus, hack_8254_device, 0, 0x2 ) );
	//debugf( "Capabilities:\n" );
	//debugf_bit_array( device->capabilities_pointer );
	#endif

	log_entry_exit();
}

uint16_t read_eprom( uint32_t *addr, uint32_t offset ) {
	uint32_t *addr_eerd = (uint32_t *)((uint32_t)addr + (0xFEB80000 - 0xFEA00000) + 0x0014);

	*addr_eerd = (1) | ((uint32_t)(offset) << 8);

	uint32_t tmp = 0;
	while( !((tmp = *addr_eerd) & (1 << 4)) ) {
		asm volatile(
			"nop \n\t"
			"nop \n\t"
		);
	}

	//klog( "Data Returned: %04X\n",  (uint16_t)((tmp >> 16) & 0xFFFF) );

	return (uint16_t)((tmp >> 16) & 0xFFFF);
}