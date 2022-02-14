#if !defined(SERIAL_INCLUDED)
#define SERIAL_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define serial_use_default_port 0

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

#define serial_write( c ) serial_write_port( c, COM1 ); serial_write_port( c, COM2 )
#define serial_read( ) serial_read_port( serial_use_default_port )

void initalize_serial( void );
void serial_enable_interrupts( void );
void serial_setup_port( uint32_t port );
void serial_set_default_port( uint32_t port );
void serial_write_port( char c, uint32_t port );
char serial_read_port( uint32_t port );
bool serial_buffer_is_ready( void );
char serial_buffer_get_char( void );
void set_data_ready_callback( void *f );
void set_data_buffer( char *buff );
void set_data_is_being_read( bool b );
void serial_interrupt_handler( uint32_t _port );
void serial_do_interrupt_read( uint32_t port );
void serial_clear_buffer( uint32_t port );

#ifdef __cplusplus
}
#endif

#endif