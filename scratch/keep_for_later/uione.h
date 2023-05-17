#if !defined(UIONE_INCLUDED)
#define UIONE_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define UIONE_PRIMARY  0x282C36 
#define UIONE_ACCENT1  0x1C202D
#define UIONE_ACCENT2  0x383A3F
#define UIONE_TITLEBAR 0x111524
#define UIONE_TEXT     0xEFF2F5

/* #define UIONE_PRIMARY  0x102142
#define UIONE_ACCENT1  0x1A2F52
#define UIONE_ACCENT2  0x6689AA
#define UIONE_TITLEBAR 0x0A1636 */

class SIObjectList;

class SIObject {
    public:
        SIObject *parent;
        SIObjectList *children;

        uint32_t x;
        uint32_t y;
        uint32_t width;
        uint32_t height;

        uint32_t absolute_x;
        uint32_t absolute_y;

        uint32_t index;

        virtual void draw( void ) = 0;
        virtual char * get_name( void ) = 0;

        void add_child( SIObject *obj );

        SIObject( void );
};

class SIObjectList {
    public:
        SIObjectList *next;
        SIObjectList *prev;
        SIObject *obj;

        SIObjectList( void );
        SIObjectList( SIObject *si_object );
};

class SIScreen : public SIObject {
    public:
        bool is_active;

        SIScreen();

        void draw( void );
        char * get_name( void );
};

class SIMenubar : public SIObject {
    public:
        void draw( void );
        char * get_name( void );
};

class SIMenuItem : public SIObject {
    public:
        void draw( void );
        char * get_name( void );
};

class SIWindow: public SIObject {
    protected:
        char title[25];
        
    public:
        bool show_close_box = true;

        SIWindow();
        ~SIWindow();

        void set_title( char *t );

        void draw( void );
        char * get_name( void );
};

class SILabel: public SIObject {
    protected:
        char *text;
    
    public:
        SILabel();
        ~SILabel();

        void set_text( char * s );
        char * get_text( void );

        void draw( void );
        char * get_name( void );
};

class SIButton: public SIObject {
    protected:
        char *text;
    
    public:
        SIButton();
        ~SIButton();

        void set_text( char * s );
        char * get_text( void );

        void draw( void );
        char * get_name( void );
};

class SIAlert { 
    protected:
        SIWindow *win;

    public:
        SIAlert( char *title, char *text );
        SIAlert( char *text );
};

void uione_test( void );
void error_log_ui_family( SIObject *obj, int indent );
void si_draw_each_child( SIObject *obj );

#ifdef __cplusplus
}
#endif
#endif