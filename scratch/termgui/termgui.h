#define ui_vertical_bar "\xF7"
#define ui_horizontal_bar "\xF8"
#define ui_corner_top_left "\xF9"
#define ui_corner_top_right "\xFA"
#define ui_corner_bottom_left "\xFB"
#define ui_corner_bottom_right "\xFC"
#define ui_title_line_left "\xFD"
#define ui_title_line_right "\xFE"

#define unicode_vertical_bar "\u2502"
#define unicode_horizontal_bar "\u2500"
#define unicode_corner_top_left "\u250C"
#define unicode_corner_top_right "\u2510"
#define unicode_corner_bottom_left "\u2514"
#define unicode_corner_bottom_right "\u2518"
#define unicode_title_line_left "\u251C"
#define unicode_title_line_right "\u2524"

#define normal_weight 0
#define double_weight 1

#define MAX_COMPONENTS 5
#define GT_GRID_WIDTH 4
#define GT_GRID_HEIGHT 4

#define GT_TYPE_WINDOW 1
#define GT_TYPE_TEXTBOX 2
#define GT_TYPE_BUTTON 3

#define GT_MAX -100
#define GT_HALF -50
#define GT_THIRD -33
#define GT_QUARTER -25

#define GT_MAX_ROW_WIDTH 250

typedef struct gt_component gt_component;
typedef struct gt_grid gt_grid;

// Primatives

typedef struct {
	char * top_left;
	char * top_right;
	char * bottom_left;
	char * bottom_right;
	char * vertical_bar;
	char * horizontal_bar;
	char * title_line_left;
	char * title_line_right;
} gt_ui_info;

typedef struct {
	int x_left;
	int y_top;
	int width;
	int height; 
} gt_rect;

// UI Objects

typedef struct {
	int type;
	int x;
	int y;
	int width;
	int height;
	char * title;
} gt_window;

typedef struct {
	int type;
	int x;
	int y;
	int width;
	int height;
	char * text;
} gt_textbox;

typedef struct {
	int type;
	int x;
	int y;
	int width;
	int height;
	char * name;
} gt_button;

// Meta Objects
/*

component -- a container for one or more ui components
grid -- a plane on which to place multiple components

*/
struct gt_component{
	int type;
	gt_component *children;
	gt_component *parent;

	int span_x;
	int span_y;
	
	gt_window * window;
	gt_textbox * textbox;
	gt_button * button;
};

struct gt_grid {
	gt_component * space[ GT_GRID_WIDTH ][ GT_GRID_HEIGHT ];
	
	int width;
	int height;
};

// Must be last

typedef struct {
	int	height;
	int	width;
	char *buffer;
	gt_grid grid;
} gt_term_info;


void print_at( int x, int y, char  * fmt, ... );
void print_at_term( int x, int y, char  * fmt, ... );
gt_ui_info * get_weight( int weight );
void draw_box( int x_left, int y_top, int width, int height, int weight );
void clear_screen( void );
void refresh_screen( void );
void draw_window( gt_rect * r, int weight, char * title );
void draw_title_line( int x_left, int y_top, int width, int weight );
gt_grid * new_grid( gt_grid * grid, int screen_height, int screen_width );
gt_grid * add_to_grid( gt_grid * grid, int x, int y, gt_component * component );
gt_component * new_component( gt_component * component );
gt_component * new_window( gt_window * window );
void gt_draw_grid( gt_grid * grid );
void gt_draw_component_at_grid( gt_grid * grid, int x, int y );
char * gt_str_replace( char * s, char find, char replace );