#ifndef VV_LIST_HEADER
#define VV_LIST_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <kmalloc.h>

#define vv_malloc(x) kmalloc(x)
#define vv_free(x) kfree(x)

typedef struct vv_node vv_node;
typedef struct vv_list vv_list;

struct vv_node {
	vv_node * next;
	vv_node * prev;
	void * data;
};

struct vv_list {
	vv_node 	* head;
	vv_node 	* tail;
	uint32_t	size;
};

vv_list * vv_list_initalize( void );
vv_node * vv_list_append( vv_list * list, void * node_data );
void vv_list_remove( vv_list * list, vv_node * node );
void vv_list_iterate( vv_list * list, void (*func)(vv_node *) );

void main_vv_list( void );

#ifdef __cplusplus
}
#endif

#endif