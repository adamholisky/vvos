#if !defined(KEYBOARD_INCLUDED)
#define KEYBOARD_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <observer.h>

#define keyboard_get_char() keyboard_get_char_nonblocking()

typedef struct {
    uint32_t scanecode;
    bool shift_down;
    char c;
} keyboard_event;

char get_character( unsigned int scan_code );
char get_scancode( void );
void keyboard_interrupt_handler( void );
void keyboard_initalize( void );

char *get_keyboard_input(char *s, uint32_t size);

void keyboard_event_handler( event_message * message );
char keyboard_get_char_nonblocking( void );
char keyboard_get_char_blocking( void );
char keyboard_get_char_stage2( bool blocking );

#ifdef __cplusplus
}
#endif

#endif