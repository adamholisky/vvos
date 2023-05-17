#include <kernel.h>
#include <device.h>
#include <terminal_class.h>

TerminalDevice::TerminalDevice() {
	printf( "In const terminal\n" );
	buffer = (byte *)kmalloc( DEVICE_DEFAULT_BUFFER_SIZE );
}

TerminalDevice::~TerminalDevice() {
	printf( "In dest\n" );
 	kfree( buffer );
}

void TerminalDevice::open( void ) {
	printf( "Opening Terminal...\n" );
}

void TerminalDevice::close( void ) {

}

void TerminalDevice::read( void ) {

}

void TerminalDevice::write( void ) {

}