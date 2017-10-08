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
	
		/* 如果还是所有者再次置锁 */
		if (p_mutex->owner_task == current_task) {
			p_mutex->lock_count++;
			ls_task_exit_critical();
			
			return event_no_error;
		} else {
			/* 如果是其他任务置锁 */
			
			/* 如果当前任务的优先级高于锁所有者的优先级，则需要优先级重置 */
			if (p_mutex->owner_task->task_pro > current_task->task_pro) {
				
				/* 如果锁所有的状态是就绪的 */
				if (p_mutex->owner_task->task_state == LS_TASK_RDY) {
				
				/* 如果锁拥有者的任务在任务就绪表中 */
				
				ls_task_sched_unrdy(p_mutex->owner_task);
				
				/* 把锁拥有者的优先级置为crrent_task的优先级 */
				p_mutex->owner_task->task_pro = current_task->task_pro;
				
				/* 重新放入就绪表中 */
				ls_task_sched_rdy(p_mutex->owner_task);
			  } else {
					
					/* 如果是其他状态 */
					p_mutex->owner_task->task_pro = current_task->task_pro;
				}
		  }
			/* 把当前任务插入事件等待序列中 */
			ls_event_wait(current_task, &p_mutex->event, (void*)0, event_no_error, timeout);
			
			ls_task_exit_critical();
			
			/* 当把当前任务插入事件等待列表中后执行一下任务调度 */
			ls_task_schedule();
			return event_no_error;
		
		}
	}
}

ls_error_t ls_mutex_give (ls_mutex_t *p_mutex)
{
	ls_task_enter_critical();
	
	/* 判断可能没有锁而直接调用释放互斥信号量 */
	if (p_mutex->lock_count <= 0) {
		
		/* 锁定计数器为0，任务未锁定，直接退出 */
		ls_task_exit_critical();
		return event_no_error;
	}
	
	/* 如果是当前所有者释放互斥信号量 */
	if (p_mutex->owner_task == current_task) { 
		if (--p_mutex->lock_count > 0) {
			ls_task_exit_critical();
			return event_no_error;
		}
		
//		/* 当锁计数减到0后，如果事件控制块中有等待的任务 */
//		if (ls_event_wait_count(&p_mutex->event)) {
			
			/* 如果是继承了高优先级，则退出时需要恢复优先级 */
			if (p_mutex->origin_prio != p_mutex->owner_task->task_pro) {
				
				/* 如果锁所有者任务的为就绪状态 */
				if (p_mutex->owner_task->task_state == LS_TASK_RDY) {
					ls_task_sched_unrdy(p_mutex->owner_task);
					p_mutex->owner_task->task_pro = p_mutex->origin_prio;
					ls_task_sched_rdy(p_mutex->owner_task);
				} else {
					/* 如果是其他状态 */
					p_mutex->owner_task->task_pro = p_mutex->origin_prio;
				}
			}
			
			if (ls_event_wait_count(&p_mutex->event)) {
				ls_task_t *task = ls_event_wakeup(&p_mutex->event, (void*)0, event_no_error);
				
				/* 唤醒任务后，该任务还是一个互斥任务，所以还是要执行 */
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
	/* 如果在非锁的所有任务中释放就会出错 */
		
		ls_task_exit_critical();
		
		return LS_OWNER_ERROR;
	
	}
	
//	ls_task_exit_critical();
}

/*
 *	\brief 删除信号量
 */
uint32_t ls_mutex_delete (ls_mutex_t *p_mutex)
{
	
	uint32_t count = 0;
	ls_task_enter_critical();
	
	/* 如果有任务锁定 */
	if (p_mutex->lock_count > 0) {
		
		/* 如果发生了优先级继承 */
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
 *	\brief 获取当前互斥信号量的信息 
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

