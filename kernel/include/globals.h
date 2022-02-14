#ifndef GLOBALS_INCLUDED
#define GLOBALS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern bool SERIAL_CONSOLE_ACTIVE;
extern bool GRAPHICS_ACTIVE;
extern bool DF_COM4_ONLY;
extern bool READY_FOR_INPUT;

extern uint8_t paging_level_active;

extern char serial_in_com1_buffer[1024];
extern char serial_in_com2_buffer[1024];
extern char serial_in_com3_buffer[1024 * 20];
extern uint32_t serial_com1_buffer_add_loc;
extern uint32_t serial_com1_buffer_read_loc;
extern uint32_t serial_com2_buffer_add_loc;
extern uint32_t serial_com2_buffer_read_loc;
extern uint32_t serial_com3_buffer_add_loc;
extern uint32_t serial_com3_buffer_read_loc;

#ifdef __cplusplus
}
#endif

#endif