#ifndef OBSERVER_HEADER
#define OBSERVER_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define observer_malloc(x) kmalloc(x) // We're doing this so eventually everything here can go into shared memory, not kernel space

#define MAX_OBSERVERS 100
#define MAX_SUBJECTS 100
#define MAX_OBSERVERS_PER_SUBJECT 20

typedef struct {
	void * data;
	int version;
	char * subject_name;
} event_message;

typedef void (*observer_function)(event_message *);

typedef struct {
	char name[25];
	observer_function func;
} observer;

typedef observer *observer_ptr;

typedef struct {
	char name[25];
	observer_ptr observers[20];
} subject;

typedef subject *subject_ptr;

void observer_initalize( void );
bool observer_register_subject( char * subject_name );
bool observer_register( char * observer_name, observer_function observer );
bool observer_attach_to_subject( char * subject_name, char * observer_name, int priority );
bool observer_detach_from_subject( char * subject_name, char * observer_name );
bool observer_notify( char * subject_name, event_message * message );

void observer_test( void );
void observer_a( event_message * message );
void observer_b( event_message * message );
void observer_c( event_message * message );

#ifdef __cplusplus
}
#endif

#endif