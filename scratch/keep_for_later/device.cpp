#include <device.h>
#include <serial_class.h>
#include <terminal_class.h>

Device::Device() {
	printf( "In const Device\n" );
	buffer = (byte *)kmalloc( DEVICE_DEFAULT_BUFFER_SIZE );
}

Device::~Device() {
	printf( "In dest\n" );
 	kfree( buffer );
}

void Device::open( void ) {
	printf( "Device open\n" );
}

void Device::close( void ) {

}

void Device::read( void ) {

}

void Device::write( void ) {

}

void init_devices( void ) {
	SerialDevice *s;
	TerminalDevice *t;
	Device *d;

	printf( "Serial new\n" );
	s = new SerialDevice();

	printf( "\nTerminal new\n" );
	t = new TerminalDevice();

 	printf( "\nCasting\n" );
	d = s;
	d->open();
	d = t;
	d->open();
}