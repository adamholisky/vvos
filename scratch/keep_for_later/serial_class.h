#if !defined(SERIALCLASS_INCLUDED)
#define SERIALCLASS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <kernel.h>

#define DEVICE_DEFAULT_BUFFER_SIZE 4096

class SerialDevice : public Device {
	protected:
		byte *buffer;
		char full_name[25];
		char file_name[25];
	public:
		byte *get_buffer( void ) { return buffer; }
		void set_buffer( byte *b ) { buffer = b; }

		SerialDevice( );
		~SerialDevice();

		void open( void );
		void close( void );
		void read( void );
		void write( void );
};




#ifdef __cplusplus
}
#endif

#endif