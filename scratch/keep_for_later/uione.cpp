#include <uione.h>
#include <kernel.h>
#include <multiboot.h>
#include <stdint.h>
#include <vga.h>
#include <string.h>

void uione_test( void ) {
	SIScreen *screen = new SIScreen();
	
	screen->x = 0;
	screen->y = 0;
	screen->width = 1280;
	screen->height = 720;

	SIWindow *w = new SIWindow();
	w->set_title( "UI One" );

	screen->add_child( w );

	SILabel *l = new SILabel();
	SIButton *b1 = new SIButton();
	SIButton *b2 = new SIButton();

	w->add_child(l);
	w->add_child(b1);
	w->add_child(b2);

	error_log_ui_family( screen, 0 );

	//SIAlert *a = new SIAlert( "UI Test", "This is a UI test..." );
}

void error_log_ui_family( SIObject *obj, int indent ) {
	SIObject *element = obj;
	char indent_string[20];
	int i;

	strcpy( indent_string, "" );

	for( i = 0; i < indent; i++ ) {
		indent_string[i] = ' ';
	}

	indent_string[i] = '\0';

	if( element != NULL ) {
		klog( "0x%08X : %s%s\n", element, indent_string, element->get_name() );
	
		// cover children
		SIObjectList *possible_children = NULL;

		possible_children = element->children;

		while( possible_children != NULL ) {
			error_log_ui_family( possible_children->obj, indent + 2 );
			possible_children = possible_children->next;
		}
	}
}

void si_draw_each_child( SIObject *obj ) {
	SIObject *element = obj;

	/* if( element != NULL ) {
		element->draw();

		element = ;
	} */
}

// SIObject

SIObject::SIObject( void ) {
	parent = NULL;
	children = NULL;
}

void SIObject::add_child( SIObject *obj ) {
	SIObjectList *objlist = this->children;

	if( objlist == NULL ) {
		this->children = new SIObjectList( obj );
	} else {
		SIObjectList *possible_tail = objlist;

		while( possible_tail->next != NULL ) {
			possible_tail = possible_tail->next;
		}

		possible_tail->next = new SIObjectList( obj );
	}
}

// SIObjectList

SIObjectList::SIObjectList( void ) {
	next = NULL;
	prev = NULL;
}

SIObjectList::SIObjectList( SIObject *si_object ) {
	next = NULL;
	prev = NULL;
	obj = si_object;
}

// SIScreen

SIScreen::SIScreen( void ) {
	is_active = true;
}

void SIScreen::draw( void ) {
	si_draw_each_child( this );
}

char * SIScreen::get_name( void ) {
	return "SIScreen";
}

// SIWindow

SIWindow::SIWindow() {
	x = 50;
	y = 50;
	width = 500;
	height = 300;
}

SIWindow::~SIWindow() {

}

void SIWindow::set_title( char *t ) {
	strcpy( title, t );
}

char * SIWindow::get_name( void ) {
	return "SIWindow";
}

void SIWindow::draw( void ) {
	rect r;

	r.x = this->x;
	r.y = this->y;
	r.w = this->width;
	r.h = this->height;
	draw_rect( r, UIONE_ACCENT1 );

	r.h = 24;
	r.w = this->width - 2;
	r.x = this->x + 1;
	r.y = this->y + 1;
	draw_rect( r, UIONE_TITLEBAR );

	r.h = this->height - 25;
	r.y = this->y + 24;
	r.x = this->x + 1;
	r.w = this->width - 2;
	draw_rect( r, UIONE_PRIMARY );

	if( show_close_box ) {
		r.h = 14;
		r.w = 14;
		r.x = this->x + this->width - 19;
		r.y = this->y + 5;
		draw_rect( r, UIONE_ACCENT2 );
	}

	vga_draw_screen();

	draw_string( title, this->x + 5, this->y + 5, UIONE_TEXT, UIONE_TITLEBAR );
}

// SILabel

SILabel::SILabel( void ) {
	//
}

SILabel::~SILabel( void ) {
	//
}

void SILabel::set_text( char * s ) {
	this->text = (char *)kmalloc( strlen(s) );
	strcpy( this->text, s );
}

char * SILabel::get_text( void ) {
	return this->text;
}

void SILabel::draw( void ) {

}

char * SILabel::get_name( void ) {
	return "SILabel";
}

// SIButton

SIButton::SIButton( void ) {
	//
}

SIButton::~SIButton( void ) {
	//
}

void SIButton::set_text( char * s ) {
	this->text = (char *)kmalloc( strlen(s) );
	strcpy( this->text, s );
}

char * SIButton::get_text( void ) {
	return this->text;
}

void SIButton::draw( void ) {

}

char * SIButton::get_name( void ) {
	return "SIButton";
}

// SIAlert

SIAlert::SIAlert( char *title, char *text ) {
	win = new SIWindow();

	win->set_title( title );
	win->x = 100;
	win->y = 100;
	win->width = 250;
	win->height = 100;
	win->show_close_box = false;

	win->draw();
}

SIAlert::SIAlert( char *text ) {
	SIAlert( "Version V", text );
}