#include "ls_rtos.h"

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

//		task->event = p_event;
		/* 唤醒后的任务事件指针清0 */
		task->event = (ls_event_t*)0;
		task->event_msg = p_msg;
		task->task_state &= ~LS_TASK_WAIT_MASK;
		task->event_result = result;

		if (task->task_delay_ticks != 0) {
			ls_task_timer_weakup(task);
		}
		
		ls_task_sched_rdy(task);
	}

	ls_task_exit_critical();

	return task;
}

/*
 *	\brief 将特定的任务从事件等待队列中唤醒
 */
ls_task_t *ls_event_wakeup_task(ls_event_t *p_event, ls_task_t *p_task, void *p_msg, uint32_t result)
{
	ls_task_enter_critical();
	
	ls_list_remove_node(&p_event->event_list, &p_task->task_time_slice_node);
	
		/* 唤醒后的任务事件指针清0 */
		p_task->event = (ls_event_t*)0;
		p_task->event_msg = p_msg;
		p_task->task_state &= ~LS_TASK_WAIT_MASK;
		p_task->event_result = result;

		if (p_task->task_delay_ticks != 0) {
			ls_task_timer_weakup(p_task);
		}
		
		ls_task_sched_rdy(p_task);

	ls_task_exit_critical();

	return p_task;
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

/*
 *	\brief 获取当前事件块队列中的任务数量
 */
uint32_t ls_event_wait_count(ls_event_t *p_event)
{
	uint32_t count = 0;
	ls_task_enter_critical();
	
	count = ls_list_get_node_count(&p_event->event_list);  
	
	ls_task_exit_critical();
	
	return count;
}

/*
 *	\brief 从事件控制块中移除所有的事件
 *  \ret  返回移除事件的数量
 */
uint32_t ls_event_remove_all (ls_event_t *p_event, void* p_msg, uint32_t result)
{
	uint32_t count = 0;
	
	ls_node_t *p_node = (ls_node_t*)0;
	
	ls_task_t *p_task = (ls_task_t*)0;
	
	ls_task_enter_critical();
	
	count = ls_list_get_node_count(&p_event->event_list);
	
	while ((p_node = ls_list_remove_first(&p_event->event_list)) != (ls_node_t*)0) {
		
		p_task = LS_GET_PARENT_STRUCT_ADDR(p_node, ls_task_t, task_time_slice_node);
		
		p_task->event = (ls_event_t*)0;
		p_task->event_msg = p_msg;
		p_task->event_result = result;
		p_task->task_state &= ~LS_TASK_WAIT_MASK;
		
		if (p_task->task_delay_ticks != 0) {
			ls_task_timer_weakup(p_task);
		}
		/* 从事件等待队列中移除后，要恢复到就绪队列中 */
		ls_task_sched_rdy(p_task);
	}
	
	ls_task_exit_critical();
	
	return count;
}




