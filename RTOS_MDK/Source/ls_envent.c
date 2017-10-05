#include "ls_rtos.h"
#include "ls_event.h"
#include "ls_task.h"
#include "ls_timer.h"

#include "rtos.h"

/*
 *	\brief 初始化任务等待事件
 */
void ls_event_init (ls_event_t *p_event, ls_event_type_t event_type)
{
	p_event->event_type = event_type;
	ls_list_init(&p_event->event_list);
	
}

/*
 *	\brief 任务产生一个等待事件
 */
void ls_event_wait (ls_task_t *p_task, ls_event_t *p_event, void *p_msg, uint32_t state, uint32_t timeout)
{
	ls_task_enter_critical();
	
	p_task->event = p_event;
	p_task->event_msg = p_msg;
	p_task->task_state |= state;
	p_task->event_result = event_no_error;
	
	/* 从就绪表中移除 */
//	ls_task_sched_remove(p_task);
	ls_task_sched_unrdy(p_task);
	
	
	/* 插入等待事件表中 */
	ls_list_insert_node_last(&p_event->event_list, &p_task->task_time_slice_node);
	
	/* 如果超时不为0，则将任务插入延时队列 */
	if (timeout) {
		
		ls_task_timer_wait(p_task, timeout);
	}
	
	ls_task_exit_critical();
}

/*
 *	\brief 将任务从事件等待队列中唤醒
 */
ls_task_t *ls_event_wakeup(ls_event_t *p_event, void *p_msg, uint32_t result)
{
	ls_task_t *task = (ls_task_t *)0;
	
	ls_node_t *temp_node;
	
	ls_task_enter_critical();
	
	temp_node = ls_list_remove_first(&p_event->event_list);
	
	if (temp_node != (ls_node_t*)0) {
		task = LS_GET_PARENT_STRUCT_ADDR(temp_node, ls_task_t, task_time_slice_node);

		task->event = p_event;
		task->event_msg = p_msg;
		task->task_state &= ~LS_TASK_WAIT_MASK;
		task->event_result = event_no_error;

		if (task->task_delay_ticks != 0) {
			ls_task_timer_weakup(task);
		}
		
		ls_task_sched_rdy(task);
	}

	
	ls_task_exit_critical();

	return task;
}

/*
 *	\brief 将任务从事件等待队列中移除
 */
void ls_event_rmove_task (ls_task_t * p_task, void * p_msg, uint32_t result)
{
	ls_task_enter_critical();
	
	ls_list_remove_node(&p_task->event->event_list, &p_task->task_time_slice_node);
	
	p_task->event = (ls_event_t*)0;
	p_task->event_result = result;
	p_task->event_msg = p_msg;
	p_task->task_state &= ~LS_TASK_WAIT_MASK;
	
	ls_task_exit_critical();
}



