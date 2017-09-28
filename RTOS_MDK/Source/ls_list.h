#ifndef __LS_LIST_H
#define __LS_LIST_H

#include "stdint.h"

#define LS_TASK_RDY			(1UL)
#define LS_TASK_DELAY   (2UL)





#define LS_LIST_FIRST_NODE  head_node.next_node
#define LS_LIST_LAST_NODE   head_node.pre_node

#define LS_GET_PARENT_STRUCT_ADDR(node, parent_struct, son)  \
				(parent_struct*)((uint32_t)node - (uint32_t)(&(((parent_struct*)0)->son)))

typedef struct __ls_node {

	struct __ls_node *pre_node;
	struct __ls_node *next_node;
}ls_node_t;


typedef struct __ls_list {

	ls_node_t  head_node;
	uint32_t   node_count;
	
}ls_list_t;


void ls_node_init (ls_node_t *p_node);
void ls_list_init (ls_list_t *p_list);

ls_node_t* ls_list_first_node (ls_list_t *p_list);

ls_node_t* ls_list_last_node (ls_list_t *p_list);

uint32_t  ls_list_get_node_count (ls_list_t *p_list);

void ls_list_insert_node_first (ls_list_t *p_list, ls_node_t *p_node);

void ls_list_insert_node_last (ls_list_t *p_list, ls_node_t *p_node);

void ls_list_insert_node_pre (ls_list_t *p_list, ls_node_t *p_current_node, ls_node_t *p_insert_node);

void ls_list_insert_node_next (ls_list_t *p_list, ls_node_t *p_current_node, ls_node_t *p_insert_node);

ls_node_t* ls_list_remove_first (ls_list_t *p_list);

void ls_list_remove_last (ls_list_t *p_list);

void ls_list_remove_node (ls_list_t *p_list, ls_node_t *p_node);

void ls_list_remove_all (ls_list_t *p_list);

#endif
