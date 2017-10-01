#include "ARMCM3.h"
#include "rtos.h"
#include "lib.h"
#include "ls_list.h"

/*
 *	\brief 初始化链表
 */
void ls_node_init (ls_node_t *p_node)
{
	p_node->next_node = p_node;
	p_node->pre_node  = p_node;
}

/*
 *	\brief 初始化链表
 */
void ls_list_init (ls_list_t *p_list)
{
	p_list->LS_LIST_FIRST_NODE = &p_list->head_node;
	p_list->LS_LIST_LAST_NODE  = &p_list->head_node;	
	
	p_list->node_count = 0;
}


ls_node_t* ls_list_first_node (ls_list_t *p_list)
{
	ls_node_t *node = (ls_node_t *)0;
	
	if (p_list->node_count != 0) {
		
		node = p_list->LS_LIST_FIRST_NODE;
	}
	
	return node;
}

ls_node_t* ls_list_last_node (ls_list_t *p_list)
{
	ls_node_t *node = (ls_node_t *)0;
	
	if (p_list->node_count !=0 ) {
	
		node = p_list->LS_LIST_LAST_NODE;
	}
	
	return node;
}

uint32_t  ls_list_get_node_count (ls_list_t *p_list) 
{
	return p_list->node_count;
}


void ls_list_insert_node_first (ls_list_t *p_list, ls_node_t *p_node)
{

	p_node->next_node = p_list->LS_LIST_FIRST_NODE;
	p_node->pre_node  = &p_list->head_node;
	
	p_list->LS_LIST_FIRST_NODE->pre_node = p_node;
	p_list->LS_LIST_FIRST_NODE = p_node;
	
	p_list->node_count++;
}

void ls_list_insert_node_last (ls_list_t *p_list, ls_node_t *p_node)
{
	p_node->pre_node = p_list->LS_LIST_LAST_NODE;
	p_node->next_node = &p_list->head_node;
	
	p_list->LS_LIST_LAST_NODE->next_node = p_node;
	p_list->LS_LIST_LAST_NODE = p_node;
	
	p_list->node_count++;
	
}

void ls_list_insert_node_pre (ls_list_t *p_list, ls_node_t *p_current_node, ls_node_t *p_insert_node)
{
	
	
	p_insert_node->pre_node = p_current_node->pre_node;
	p_insert_node->next_node = p_current_node;
	
	p_current_node->pre_node->next_node = p_insert_node;
	p_current_node->pre_node = p_insert_node;
	
	p_list->node_count++;
	
}

void ls_list_insert_node_next (ls_list_t *p_list, ls_node_t *p_current_node, ls_node_t *p_insert_node)
{
	p_insert_node->pre_node = p_current_node;
	p_insert_node->next_node = p_current_node->next_node;
	
	p_current_node->next_node->pre_node = p_insert_node;
	p_current_node->next_node = p_insert_node;

	p_list->node_count++;
}

/*
 *	\brief 从链表中移除第一个节点
 */
ls_node_t* ls_list_remove_first (ls_list_t *p_list)
{
	ls_node_t  *node = (ls_node_t*)0;
	
	if (p_list->node_count == 0) {
		return node;
	}
	
	node = p_list->LS_LIST_FIRST_NODE;
	
	p_list->LS_LIST_FIRST_NODE = p_list->LS_LIST_FIRST_NODE->next_node;
	
//	p_list->LS_LIST_FIRST_NODE->next_node->pre_node = &p_list->head_node;
	node->next_node->pre_node = &p_list->head_node ;
	
	p_list->node_count--;
	
	return node;

}

/*
 *	\brief 从链表中移除最后一个节点
 */
void ls_list_remove_last (ls_list_t *p_list)
{
	if (p_list->node_count == 0) {
		return ;
	}
	
	p_list->LS_LIST_LAST_NODE->pre_node->next_node = &p_list->head_node;
	p_list->LS_LIST_LAST_NODE = p_list->LS_LIST_LAST_NODE->pre_node;
	
	p_list->node_count--;

}

void ls_list_remove_node (ls_list_t *p_list, ls_node_t *p_node)
{
	if (p_list->node_count == 0) {
		return;
	}
	p_node->pre_node = p_node->next_node;
	p_node->next_node->pre_node = p_node->pre_node;
	
	p_list->node_count--;

}

void ls_list_remove_all (ls_list_t *p_list)
{
	ls_node_t *next_node;

	
	uint32_t count = 0;
	
	next_node = p_list->LS_LIST_FIRST_NODE;
	
	for (count = p_list->node_count; count != 0; count--) {
			ls_node_t *current_node = next_node;
		
			current_node->next_node = current_node;
			current_node->pre_node = current_node;
		
			next_node = next_node->next_node;
	}
	p_list->LS_LIST_FIRST_NODE = &p_list->head_node;
	p_list->LS_LIST_LAST_NODE = &p_list->head_node;
	
	p_list->node_count = 0;
}


