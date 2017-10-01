#ifndef __LS_TASK_H
#define __LS_TASK_H

#include "ls_list.h"
#include "ls_rtos.h"
#include "stdint.h"

typedef struct __ls_task {

	/** \brief 任务的堆栈指针 */
	uint32_t *p_stack;
	
	/** \brief 任务的延时tick数  */
	uint32_t  task_delay_ticks;
	
	/** \brief 任务延时队列节点 */
	ls_node_t task_delay_node;
	
	/** \brief 任务优先级 */
	uint32_t task_pro;
	
	/** \brief 任务的状态 */
	uint32_t task_state;
	
	/** \brief 相同优先级任务情况下，本任务所占时间片的数量 */
	uint32_t task_slice;
	
	/** \brief 任务时间片节点 */
	ls_node_t task_time_slice_node;
	
}ls_task_t;

typedef uint32_t ls_stack_t;
extern ls_task_t *current_task;
extern ls_task_t *next_task;

extern ls_list_t task_table[LS_TASK_COUNT];


void ls_task_init(ls_task_t *p_task, ls_stack_t * p_task_stack, uint8_t prio, void* func_entry, void *p_param);

ls_task_t* ls_task_high_redy(void);

void ls_task_schedule_disable (void);

void ls_task_schedule_enable (void);

void ls_task_sched_rdy(ls_task_t *p_task);

void ls_task_sched_unrdy(ls_task_t *p_task);

void ls_task_sched_init (void);

#endif

