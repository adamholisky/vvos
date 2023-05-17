#include <kernel.h>
#include <device.h>
#include <serial_class.h>

SerialDevice::SerialDevice() {
	printf( "In const serial\n" );
	buffer = (byte *)kmalloc( DEVICE_DEFAULT_BUFFER_SIZE );
}

SerialDevice::~SerialDevice() {
	printf( "In dest\n" );
 	kfree( buffer );
}

void SerialDevice::open( void ) {
	printf( "Opening Serial...\n" );
}

void SerialDevice::close( void ) {

}

void SerialDevice::read( void ) {

}

void SerialDevice::write( void ) {

}