//#include "kernel.h"
//#include "vv_dev_header_wrap.h"
#include "vv_list.h"

/* void my_list_iterator( vv_node * node );

vv_dev_optional_main(vv_list) {
	vv_list * my_list = vv_list_initalize();
	vv_node * one, * two, * node, * three;

	one = vv_list_append( my_list, (void *)"One" );
	two = vv_list_append( my_list, "Two" );
	three = vv_list_append( my_list, "Three" );
	vv_list_append( my_list, "Four" );

	vv_printf( "size: %d\n", my_list->size );

	vv_list_iterate( my_list, my_list_iterator );

	node = my_list->tail;
	vv_printf( "removing: %s\n", (char *)node->data );
	vv_list_remove( my_list, node );
	vv_list_iterate( my_list, my_list_iterator );

	vv_printf( "removing: %s\n", (char *)two->data );
	vv_list_remove( my_list, two );
	vv_list_iterate( my_list, my_list_iterator );

	vv_printf( "removing: %s\n", (char *)one->data );
	vv_list_remove( my_list, one );
	vv_list_iterate( my_list, my_list_iterator );

	vv_printf( "removing: %s\n", (char *)three->data );
	vv_list_remove( my_list, three );
	vv_list_iterate( my_list, my_list_iterator );
}

#ifndef hide_from_kernel

#endif

void my_list_iterator( vv_node * node ) {
	vv_printf( "%s\n", (char *)(node->data) );
} */

vv_list * vv_list_initalize( void ) {
	vv_list * list = vv_malloc( sizeof( vv_list ) );

	list->head = NULL;
	list->tail = NULL;
	list->size = 0;

	return list;
}

vv_node * vv_list_append( vv_list * list, void * node_data ) {
	vv_node * node = vv_malloc( sizeof( vv_node ) );

	if( list == NULL ) {
		return NULL;
	}

	if( node ==  NULL ) {
		return NULL;
	}

	if( list->head == NULL ) {
		list->head = node;
		list->tail = node;
		node->next = NULL;
		node->prev = NULL;
	} else {
		node->next = NULL;
		node->prev = list->tail;
		list->tail->next = node;
		list->tail = node;
	}

	node->data = node_data;
	list->size++;

	return node;
}

void vv_list_remove( vv_list * list, vv_node * node ) {
	if( list->tail == node ) {
		if( list->head == node ) {
			list->tail = NULL;
			list->head = NULL;
		} else {
			list->tail->prev->next = NULL;
			list->tail = node->prev;
		}
	} else if( list->head == node ) {
		list->head = node->next;
		list->head->prev = NULL;
	} else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}

	vv_free( node );
}

void vv_list_iterate( vv_list * list, void (*func)(vv_node *) ) {
	vv_node * node = list->head;

	while( node != NULL ) {
		func( node );
		
		node = node->next;
	}
}
