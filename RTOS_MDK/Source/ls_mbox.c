#include "ls_rtos.h"
#include "ls_mbox.h"

#if (LS_ENABLE_MBOX == 1)

/*
 *	\brief 初始化邮箱
 */
void ls_mbox_init(ls_mbox_t *p_mbox, void** pp_buf, uint32_t max_count)
{
	ls_event_init(&p_mbox->event, ls_event_type_msg);
	
	p_mbox->buffer = pp_buf;
	
	p_mbox->msg_count = 0;
	
	p_mbox->msg_max_count = max_count;
	
	p_mbox->read_index = 0;
	
	p_mbox->write_index = 0;
}

/*
 *	\brief 发送消息, (消息缓存区，read索引是指向即将要读的，
 *         write是指向下一个要写的位置，read是有数据write指向的是没有数据)
 */
ls_error_t ls_mbox_send_msg (ls_mbox_t *p_mbox, void* p_msg, uint32_t send_option)
{
	uint32_t count  = 0;
	
	ls_task_enter_critical();
	
	count = ls_event_wait_count(&p_mbox->event);
	
	/* 如果任务等待队列中有任务等待 */
	if (count) {
		
		ls_task_t *task;
		
		/* 将任务唤醒，并将消息传给任务中的event_msg */
		task = ls_event_wakeup(&p_mbox->event, p_msg, event_no_error);
		
		if (task->task_pro < current_task->task_pro) {
			ls_task_schedule();
		}	
	} else {
		/* 如果没有任务在等待 */
		
		/* 邮箱已满 */
		if (p_mbox->msg_count >= p_mbox->msg_max_count) {
			ls_task_exit_critical();
			return -LS_MBOX_FULL;
		}
		
		/* 消息紧急 */
		if (send_option == LS_MSG_URGENCY) {
			
			if (p_mbox->read_index <= 0) {
				p_mbox->read_index = p_mbox->read_index + p_mbox->msg_max_count - 1;
				
				p_mbox->buffer[p_mbox->read_index] = p_msg; 
			} else {
				p_mbox->buffer[--(p_mbox->read_index)] = p_msg;
			}
			
		} else {  /* 消息普通 */
			
				p_mbox->buffer[p_mbox->write_index++] = p_msg;
			
				if (p_mbox->write_index >= p_mbox->msg_max_count) {
					p_mbox->write_index = 0;
				}
		}		
		
		p_mbox->msg_count++;
	}
	
	ls_task_exit_critical();
	
	return LS_OK;
}

/*
 *	\brief 获取消息
 */
ls_error_t ls_mbox_recieve_msg(ls_mbox_t *p_mbox, void** msg, uint32_t timeout)
{
	ls_task_enter_critical();
	
	if (p_mbox->msg_count) {
		
		/* 如果消息缓存区有消息，则读出消息 */
		*msg = p_mbox->buffer[p_mbox->read_index++];
		
		if (p_mbox->read_index >= p_mbox->msg_max_count) {
			p_mbox->read_index = 0;
		}
		
		p_mbox->msg_count--;
		
		ls_task_exit_critical();
		
		return LS_OK;
	} else {
		
		ls_event_wait(current_task, &p_mbox->event, (void*)0, ls_event_type_msg, timeout);
		
		ls_task_schedule();
		
		/* 当下次再调这个任务，证明消息已经发到了任务的event_msg中 */
		*msg = current_task->event_msg;
		
		ls_task_exit_critical();
		
		return current_task->event_result;
	}
}

/*
 *	\brief 获取消息无等待
 */
ls_error_t ls_mbox_recieve_msg_nowait(ls_mbox_t *p_mbox, void** msg)
{
	ls_task_enter_critical();
	
	if (p_mbox->msg_count) {
		
		/* 如果消息缓存区有消息，则读出消息 */
		*msg = p_mbox->buffer[p_mbox->read_index++];
		
		p_mbox->msg_count--;
		
		ls_task_exit_critical();
		
		return LS_OK;
	} else {
		
		ls_task_exit_critical();
		
		return event_resource_unavailable_error;
	}
}

/*
 *	\brief 清空邮箱
 */
void ls_mbox_flush (ls_mbox_t *p_mbox)
{
	ls_task_enter_critical();
	
	/* 如果没有任务等待，则邮箱已可能不为空，如果有任务等待则邮箱为空 */
	if ( ls_event_wait_count(&p_mbox->event) == 0) {
		p_mbox->msg_count = 0;
		p_mbox->read_index = 0;
		p_mbox->write_index = 0;
	}
	
	ls_task_exit_critical();
}

/*
 *	\brief 删除邮箱
 */
uint32_t ls_mbox_delete (ls_mbox_t *p_mbox)
{
	uint32_t count = 0;
	ls_task_enter_critical();
	
	count = ls_event_remove_all(&p_mbox->event, (void *)0, event_no_error);
	
	ls_task_exit_critical();
	/* 如果存在释放的任务，就执行一下任务调度 */
	if (count) {
		ls_task_schedule();
	}
	
	return count;
}

/*
 *	\brief 邮箱的状态查询
 */
void ls_mbox_get_info (ls_mbox_t *p_mbox, ls_mbox_info_t *info)
{
	ls_task_enter_critical();
	
	info->msg_count = p_mbox->msg_count;
	info->max_msg_count = p_mbox->msg_max_count;
	info->task_count = ls_event_wait_count(&p_mbox->event);
	
	ls_task_exit_critical();
}


#endif

