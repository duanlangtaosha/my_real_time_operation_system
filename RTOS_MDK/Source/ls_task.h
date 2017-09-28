#ifndef __LS_TASK_H
#define __LS_TASK_H

#include "ls_list.h"
#include "stdint.h"

typedef struct __ls_task {

	uint32_t *p_stack;
	
	uint32_t  task_delay;
	
	ls_node_t delay_node;
	
	uint32_t task_pro;
	
	uint32_t task_state;
	
}ls_task_t;

typedef uint32_t ls_stack_t;
extern ls_task_t *current_task;
extern ls_task_t *next_task;


void ls_task_init(ls_task_t *p_task, ls_stack_t * p_task_stack, uint8_t prio, void* func_entry, void *p_param);

ls_task_t* ls_task_high_redy(void);

void ls_task_schedule_disable (void);

void ls_task_schedule_enable (void);

#endif

