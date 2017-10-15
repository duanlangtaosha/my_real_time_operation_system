#include "ls_rtos.h"

/*
 *	\brief ��ʼ������ȴ��¼�
 */
void ls_event_init (ls_event_t *p_event, ls_event_type_t event_type)
{
	p_event->event_type = event_type;
	ls_list_init(&p_event->event_list);
	
}

/*
 *	\brief �������һ���ȴ��¼�
 */
void ls_event_wait (ls_task_t *p_task, ls_event_t *p_event, void *p_msg, uint32_t state, uint32_t timeout)
{
	ls_task_enter_critical();
	
	p_task->event = p_event;
	p_task->event_msg = p_msg;
	p_task->task_state |= state;
	p_task->event_result = event_no_error;
	
	/* �Ӿ��������Ƴ� */
	ls_task_sched_unrdy(p_task);
	
	/* ����ȴ��¼����� */
	ls_list_insert_node_last(&p_event->event_list, &p_task->task_time_slice_node);
	
	/* �����ʱ��Ϊ0�������������ʱ���� */
	if (timeout) {
		
		ls_task_timer_wait(p_task, timeout);
	}
	
	ls_task_exit_critical();
}

/*
 *	\brief ��������¼��ȴ������л���
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
		/* ���Ѻ�������¼�ָ����0 */
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
 *	\brief ���ض���������¼��ȴ������л���
 */
ls_task_t *ls_event_wakeup_task(ls_event_t *p_event, ls_task_t *p_task, void *p_msg, uint32_t result)
{
	ls_task_enter_critical();
	
	ls_list_remove_node(&p_event->event_list, &p_task->task_time_slice_node);
	
		/* ���Ѻ�������¼�ָ����0 */
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
 *	\brief ��������¼��ȴ��������Ƴ�
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
 *	\brief ��ȡ��ǰ�¼�������е���������
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
 *	\brief ���¼����ƿ����Ƴ����е��¼�
 *  \ret  �����Ƴ��¼�������
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
		/* ���¼��ȴ��������Ƴ���Ҫ�ָ������������� */
		ls_task_sched_rdy(p_task);
	}
	
	ls_task_exit_critical();
	
	return count;
}




