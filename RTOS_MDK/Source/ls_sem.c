#include "ls_rtos.h"

/*
 *	\brief 初始化信号量
 */
void ls_sem_init(ls_sem_t *p_sem, uint32_t init_count, uint32_t max_count)
{
	/* 初始化事件控制块队列 */
	ls_list_init(&p_sem->event.event_list);
	
	p_sem->event.event_type = ls_event_type_sem;
	
	p_sem->count = init_count;
	
	p_sem->max_count = max_count;
}

/*
 *	\brief 获取信号量
 */
uint32_t ls_sem_take(ls_sem_t *p_sem, uint32_t timeout)
{
//	uint32_t count = 0;
	
	ls_task_enter_critical();
	
//count = ls_list_get_node_count(&p_sem->event.event_list);
	
	/* 如果有信号量可以获取 */
	if (p_sem->count) {
		
		p_sem->count--;
		ls_task_exit_critical();
		return LS_OK;
		
		/* 没有信号量可以获取 */
	}else {
		ls_event_wait(current_task, &p_sem->event, (void*)0, ls_event_type_sem, timeout);
		
		ls_task_schedule();
		
		return current_task->event_result;
	}
}

/*
 *	\brief 无等待获取信号量
 */
uint32_t ls_sem_take_no_wait(ls_sem_t *p_sem)
{
//	uint32_t count = 0;
	
	ls_task_enter_critical();
	
//count = ls_list_get_node_count(&p_sem->event.event_list);
	
	/* 如果有信号量可以获取 */
	if (p_sem->count) {
		
		p_sem->count--;
		ls_task_exit_critical();
		return LS_OK;
		
		/* 没有信号量可以获取 */
	}else {
		ls_task_exit_critical();
		return event_resource_unavailable_error;
	}
}

/*
 *	\brief 释放信号量
 */
void ls_sem_give (ls_sem_t *p_sem)
{
	uint32_t count = 0;
	ls_task_enter_critical();
	
	count = ls_list_get_node_count(&p_sem->event.event_list);
	
	if (count) {
		ls_task_t *task;
		
		task = ls_event_wakeup(&p_sem->event, (void*)0, event_no_error);
		
		if (task->task_pro < current_task->task_pro) {
			ls_task_schedule();
		}
	} else {
		p_sem->count++;
		
		if (p_sem->max_count) {
			if (p_sem->count > p_sem->max_count) {
				p_sem->count = p_sem->max_count;
			}
	  }else {
			if (p_sem->count == 0xffffffff) {
				p_sem->count = 0xffffffff;
			}
		}
	}
	
	ls_task_exit_critical();
}

/*
 *	\brief 获取信号量的信息
 */
void ls_sem_get_info(ls_sem_t *p_sem, ls_sem_info_t *info)
{
	ls_task_enter_critical();
	
	info->count = p_sem->count;
	info->max_count = p_sem->max_count;
	info->task_count = ls_event_wait_count(&p_sem->event);
	
	ls_task_exit_critical();
}

/*
 * \brief 删除信号量
 */
void ls_sem_delete(ls_sem_t *p_sem)
{
	uint32_t count = 0;
	ls_task_enter_critical();
	
	count = ls_event_remove_all(&p_sem->event, (void*)0, event_no_error);
	p_sem->count = 0;
	
	ls_task_exit_critical();
	
	if (count) {
		ls_task_schedule();
	}
}
