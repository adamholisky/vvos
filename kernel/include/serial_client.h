#ifndef HEADER_SERIALCLIENT
#define HEADER_SERIALCLIENT

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel.h"

void serial_client_initalize( void );
void handle_data_ready( uint32_t size );
void ssvv_send( char * cmd );
uint32_t ssvv_read( uint8_t * buff );
uint32_t ssvv_send_ask( char * cmd, uint8_t * buff, uint32_t size );
uint32_t ssvv_read_file( char * full_path_to_file, uint8_t * buff );
void ssvv_read_file_test( void );
void ssvv_exec_test_app( void );

#ifdef __cplusplus
}
#endif

#endif