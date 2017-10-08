#include "ls_rtos.h"

void ls_mutex_init (ls_mutex_t *p_mutex)
{
	ls_event_init(&p_mutex->event, ls_event_type_mutex);
	p_mutex->lock_count = 0;
	p_mutex->origin_prio = LS_TASK_COUNT;
	p_mutex->owner_task = (ls_task_t *)0;
}

ls_error_t ls_mutex_take (ls_mutex_t *p_mutex, uint32_t timeout)
{
	ls_task_enter_critical();
	
	if (p_mutex->lock_count <= 0) {
		p_mutex->owner_task = current_task;
		p_mutex->origin_prio = current_task->task_pro;
		p_mutex->lock_count++;
		
		ls_task_exit_critical();
		return event_no_error;
	} else {
	
		/* ��������������ٴ����� */
		if (p_mutex->owner_task == current_task) {
			p_mutex->lock_count++;
			ls_task_exit_critical();
			
			return event_no_error;
		} else {
			/* ����������������� */
			
			/* �����ǰ��������ȼ������������ߵ����ȼ�������Ҫ���ȼ����� */
			if (p_mutex->owner_task->task_pro > current_task->task_pro) {
				
				/* ��������е�״̬�Ǿ����� */
				if (p_mutex->owner_task->task_state == LS_TASK_RDY) {
				
				/* �����ӵ���ߵ������������������ */
				
				ls_task_sched_unrdy(p_mutex->owner_task);
				
				/* ����ӵ���ߵ����ȼ���Ϊcrrent_task�����ȼ� */
				p_mutex->owner_task->task_pro = current_task->task_pro;
				
				/* ���·���������� */
				ls_task_sched_rdy(p_mutex->owner_task);
			  } else {
					
					/* ���������״̬ */
					p_mutex->owner_task->task_pro = current_task->task_pro;
				}
		  }
			/* �ѵ�ǰ��������¼��ȴ������� */
			ls_event_wait(current_task, &p_mutex->event, (void*)0, event_no_error, timeout);
			
			ls_task_exit_critical();
			
			/* ���ѵ�ǰ��������¼��ȴ��б��к�ִ��һ��������� */
			ls_task_schedule();
			return event_no_error;
		
		}
	}
}

ls_error_t ls_mutex_give (ls_mutex_t *p_mutex)
{
	ls_task_enter_critical();
	
	/* �жϿ���û������ֱ�ӵ����ͷŻ����ź��� */
	if (p_mutex->lock_count <= 0) {
		
		/* ����������Ϊ0������δ������ֱ���˳� */
		ls_task_exit_critical();
		return event_no_error;
	}
	
	/* ����ǵ�ǰ�������ͷŻ����ź��� */
	if (p_mutex->owner_task == current_task) { 
		if (--p_mutex->lock_count > 0) {
			ls_task_exit_critical();
			return event_no_error;
		}
		
//		/* ������������0������¼����ƿ����еȴ������� */
//		if (ls_event_wait_count(&p_mutex->event)) {
			
			/* ����Ǽ̳��˸����ȼ������˳�ʱ��Ҫ�ָ����ȼ� */
			if (p_mutex->origin_prio != p_mutex->owner_task->task_pro) {
				
				/* ����������������Ϊ����״̬ */
				if (p_mutex->owner_task->task_state == LS_TASK_RDY) {
					ls_task_sched_unrdy(p_mutex->owner_task);
					p_mutex->owner_task->task_pro = p_mutex->origin_prio;
					ls_task_sched_rdy(p_mutex->owner_task);
				} else {
					/* ���������״̬ */
					p_mutex->owner_task->task_pro = p_mutex->origin_prio;
				}
			}
			
			if (ls_event_wait_count(&p_mutex->event)) {
				ls_task_t *task = ls_event_wakeup(&p_mutex->event, (void*)0, event_no_error);
				
				/* ��������󣬸�������һ�������������Ի���Ҫִ�� */
				p_mutex->lock_count++;
				p_mutex->owner_task = task;
				p_mutex->origin_prio = task->task_pro;
				
				if (task->task_pro < current_task->task_pro) {
					
					ls_task_exit_critical();
					ls_task_schedule();
					
//					return LS_OK;
				}
			}
			return LS_OK;
//		}
		
		
	} else {
	/* ����ڷ����������������ͷžͻ���� */
		
		ls_task_exit_critical();
		
		return LS_OWNER_ERROR;
	
	}
	
//	ls_task_exit_critical();
}

/*
 *	\brief ɾ���ź���
 */
uint32_t ls_mutex_delete (ls_mutex_t *p_mutex)
{
	
	uint32_t count = 0;
	ls_task_enter_critical();
	
	/* ������������� */
	if (p_mutex->lock_count > 0) {
		
		/* ������������ȼ��̳� */
		if (p_mutex->origin_prio != p_mutex->owner_task->task_pro) {
			
			if (p_mutex->owner_task->task_state == LS_TASK_RDY) {
			
					ls_task_sched_unrdy(p_mutex->owner_task);
				
					p_mutex->owner_task->task_pro = p_mutex->origin_prio;
					ls_task_sched_rdy(p_mutex->owner_task);
			}else {
				p_mutex->owner_task->task_pro = p_mutex->origin_prio;
			}
		}
	}
	
	count = ls_event_remove_all(&p_mutex->event, (void*)0, event_no_error);
	
	if (count > 0) {
		ls_task_schedule();
	}
	
	ls_task_exit_critical();
	
	return count;
}

/*
 *	\brief ��ȡ��ǰ�����ź�������Ϣ 
 */
void ls_mutex_get_info (ls_mutex_t *p_mutex, ls_mutex_info_t *info)
{
	
	ls_task_enter_critical();
	
	info->lock_count = p_mutex->lock_count;
	info->task_count = ls_event_wait_count(&p_mutex->event);
	info->owner_prio = p_mutex->owner_task->task_pro;
	
	if (p_mutex->origin_prio != p_mutex->owner_task->task_pro) {
		info->inherited_prio = p_mutex->owner_task->task_pro;
	}else {
		info->inherited_prio = LS_TASK_COUNT;
	}
	
	info->owner_task = p_mutex->owner_task;
	
	ls_task_exit_critical();
}

