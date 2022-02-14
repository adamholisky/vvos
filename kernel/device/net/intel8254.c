#include "kernel.h"
#include "pci.h"
#include "intel8254.h"

#undef kdebug_intel8254

void intel8254_initalize( void ) {
	log_entry_enter();
	
	pci_header * device = pci_get_header_by_device_id( 0x100E );

	if( device == NULL ) {
		klog( "Could not find PCI entry for intel8254.\n" );
		return;
	} else {
		klog( "Found\n" );
	}

	pci_bist_register * bist = (pci_bist_register *)&(device->bist);

	klog( "Class: %02X   Subclass: %02X   Prog IF: %02X   Revision: %02X   Vendor: %04X   Device ID: %04X\n",
			device->class_code, device->subclass, device->prog_if, device->revision_id, device->vendor_id, device->device_id );

	#ifdef kdebug_intel8254
	klog( "Bist:\n" );
	//debugf_bit_array( device->bist );

	klog( "Interrupt line: 0x%02X\n", device->interrupt_line );
	klog( "Interrupt pin: 0x%02X\n", device->interrupt_pin );
	klog( "Bar0: 0x%08X\n", device->bar0 );
	klog( "Bar1: 0x%08X\n", device->bar1 );
	klog( "Bar2: 0x%08X\n", device->bar2 );
	klog( "Bar3: 0x%08X\n", device->bar3 );
	klog( "Bar4: 0x%08X\n", device->bar4 );
	klog( "Bar5: 0x%08X\n", device->bar5 );
	//klog( "Command:\n" );
	//debugf_bit_array( device->command );
	//debugf( "Status:\n" );
	//debugf_bit_array( device->status );
	//debugf( "Capabilities:\n" );
	//debugf_bit_array( device->capabilities_pointer );
	#endif

	log_entry_exit();
}