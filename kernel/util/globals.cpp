#include <globals.h>

// Runtime Modification
bool SERIAL_CONSOLE_ACTIVE = false;
bool GRAPHICS_ACTIVE = false;
bool DF_COM4_ONLY = false;
bool READY_FOR_INPUT = false;

// Memory
uint8_t paging_level_active;

// Serial
char serial_in_com1_buffer[1024];
char serial_in_com2_buffer[1024];
char serial_in_com3_buffer[1024 * 20];
uint32_t serial_com1_buffer_add_loc;
uint32_t serial_com1_buffer_read_loc;
uint32_t serial_com2_buffer_add_loc;
uint32_t serial_com2_buffer_read_loc;
uint32_t serial_com3_buffer_add_loc;
uint32_t serial_com3_buffer_read_loc;