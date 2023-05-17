#if !defined(DEVICE_INCLUDED)
#define DEVICE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <kernel.h>

#define DEVICE_DEFAULT_BUFFER_SIZE 4096

class Device {
	protected:
		byte *buffer;
		char full_name[25];
		char file_name[25];
	public:
		byte *get_buffer( void ) { return buffer; }
		void set_buffer( byte *b ) { buffer = b; }

		Device( );
		~Device();

		virtual void open( void );
		void close( void );
		void read( void );
		void write( void );
};

void init_devices( void );



#ifdef __cplusplus
}
#endif

#endif