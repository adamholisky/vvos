#include <kernel.h>
#include "observer.h"

observer_ptr observers[MAX_OBSERVERS];
subject_ptr subjects[MAX_SUBJECTS];

/*
typedef struct {
	void * data;
	int version;
	char * subject_name;
} event_message;

typedef void (*observer_function)(event_message *);
*/
void observer_initalize( void ) {
	int i;

	for( i = 0; i < MAX_OBSERVERS; i++ ) {
		observers[i] = NULL;
	}

	for( i = 0; i < MAX_SUBJECTS; i++ ) {
		subjects[i] = NULL;
	}
}

/**
 * @brief Registers a subject that will be observed. Subject can fire a notification to those observering it.
 * 
 * @param subject_name A name for the subject, referenced by observers
 * @return true Registration successful
 * @return false Registeration failed
 */
bool observer_register_subject( char * subject_name ) {
	int i, j;

	for( i = 0; i < MAX_SUBJECTS; i++ ) {
		if( subjects[i] == NULL ) {
			break;
		}
	}

	subjects[i] = observer_malloc( sizeof( subject ) );

	strcpy( subjects[i]->name, subject_name );

	for( j = 0; j < MAX_OBSERVERS_PER_SUBJECT; j++ ) {
		subjects[i]->observers[j] = NULL;
	}

	return true;
}

/**
 * @brief Registers an observer function that will be used to recieve messages from an observed subject.
 * 
 * @param observer_name A name for the observer
 * @param observer_fun A function that is called when the subject notifies a change has occured
 * @return true Registration successful
 * @return false Registration failed
 */
bool observer_register( char * observer_name, observer_function observer_func ) {
	int i;

	for( i = 0; i < MAX_OBSERVERS; i++ ) {
		if( observers[i] == NULL ) {
			break;
		}
	}

	observers[i] = observer_malloc( sizeof( observer ) );
	observers[i]->func = observer_func;
	strcpy( observers[i]->name, observer_name );

	return true;
}

/**
 * @brief Attached a registered observer to a registered subject
 * 
 * @param subject_name The subject name to observe. Must be registered.
 * @param observer_name The observer name the subject will notify. Must be registered.
 * @param priority An integer priority. 0 is lowest priority, 65535 is the highest.
 * @return true Attach succesful
 * @return false Attach failed
 */
bool observer_attach_to_subject( char * subject_name, char * observer_name, int priority ) {
	int i, j, k;

	for( i = 0; i < MAX_SUBJECTS; i++ ) {
		if( strcmp( subjects[i]->name, subject_name ) == 0 ) {
			break;
		}
	}

	if( i == MAX_SUBJECTS ) {
		kpanic( "MAX_SUBJECTS reached" );
		return false;
	}

	for( j = 0; j < MAX_OBSERVERS; j++ ) {
		if( strcmp( observers[j]->name, observer_name ) == 0 ) {
			break;
		}
	}

	if( j == MAX_OBSERVERS ) {
		kpanic( "MAX_OBSERVERS reached" );
		return false;
	}

	for( k = 0; k < MAX_OBSERVERS_PER_SUBJECT; k++ ) {
		if( subjects[i]->observers[k] == NULL ) {
			break;
		}
	}

	if( k == MAX_OBSERVERS_PER_SUBJECT ) {
		kpanic( "MAX_OBSERVERS_PER_SUBJECT reached" );
		return false;
	}

	subjects[i]->observers[k] = observers[j];
	
	return true;
}

/**
 * @brief Removes an observer from receiving a subject's notifications
 * 
 * @param subject_name The name of the subject being observed. Must be registered.
 * @param observer_name The name of the observer being removed. Must be registered.
 * @return true Detach successful
 * @return false Detach failed
 */
bool observer_detach_from_subject( char * subject_name, char * observer_name ) {
	
}

/**
 * @brief Notifies all observers of the registered subject of an event
 * 
 * @param subject_name The name of the subject that will have its observers notified
 * @param message An event_message containing the notification information
 * @return true Notifications successful
 * @return false Notifications failed
 */
bool observer_notify( char * subject_name, event_message * message ) {
	int i, k;
	observer_function f;

	strcpy( message->subject_name, subject_name );

	for( i = 0; i < MAX_SUBJECTS; i++ ) {
		if( strcmp( subjects[i]->name, subject_name ) == 0 ) {
			break;
		}
	}

	if( i == MAX_SUBJECTS ) {
		kpanic( "MAX_SUBJECTS reached" );
		return false;
	}

	for( k = 0; k < MAX_OBSERVERS_PER_SUBJECT; k++ ) {
		if( subjects[i]->observers[k] == NULL ) {
			break;
		} else {
			f = subjects[i]->observers[k]->func;
			f( message );
		}
	}

	if( k == MAX_OBSERVERS_PER_SUBJECT ) {
		kpanic( "MAX_OBSERVERS_PER_SUBJECT reached. Might not be a panic..." ); //TODO: Rethink this...
		return false;
	}
	
	strcpy( message->subject_name, "" );

	return true;
}

void observer_test( void ) {
	event_message event_one;
	event_message event_two;

	observer_register_subject( "keypress" );
	observer_register_subject( "mouse_click" );

	observer_register( "observer_a", observer_a );
	observer_register( "observer_b", observer_b );
	observer_register( "observer_c", observer_c );

	observer_attach_to_subject( "keypress", "observer_a", 1 );
	observer_attach_to_subject( "keypress", "observer_b", 1 );
	observer_attach_to_subject( "mouse_click", "observer_c", 1 );

	event_one.data = observer_malloc( sizeof("Event One Message") );
	strcpy( event_one.data, "Event One Message" );
	
	event_two.data = observer_malloc( sizeof("Event Two Message") );
	strcpy( event_two.data, "Event Two Message" );

	observer_notify( "keypress", &event_one );
	observer_notify( "mouse_click", &event_two );
}

void observer_a( event_message * message ) {
	//klog( "%s: %s\n", message->subject_name, message->data );
}

void observer_b( event_message * message ) {
	//klog( "%s: %s\n", message->subject_name, message->data );
}

void observer_c( event_message * message ) {
	//klog( "%s: %s\n", message->subject_name, message->data );
}