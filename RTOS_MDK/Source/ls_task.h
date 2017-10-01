#ifndef __LS_TASK_H
#define __LS_TASK_H

#include "ls_list.h"
#include "ls_rtos.h"
#include "stdint.h"

typedef struct __ls_task {

	/** \brief ����Ķ�ջָ�� */
	uint32_t *p_stack;
	
	/** \brief �������ʱtick��  */
	uint32_t  task_delay_ticks;
	
	/** \brief ������ʱ���нڵ� */
	ls_node_t task_delay_node;
	
	/** \brief �������ȼ� */
	uint32_t task_pro;
	
	/** \brief �����״̬ */
	uint32_t task_state;
	
	/** \brief ��ͬ���ȼ���������£���������ռʱ��Ƭ������ */
	uint32_t task_slice;
	
	/** \brief ����ʱ��Ƭ�ڵ� */
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

