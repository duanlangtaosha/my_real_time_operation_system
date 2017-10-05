#include "ls_rtos.h"
#include "ls_mbox.h"

uint32_t temp = 0;

/*
 *	\brief ��ʼ������
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
 *	\brief ������Ϣ, (��Ϣ��������read������ָ�򼴽�Ҫ���ģ�
 *         write��ָ����һ��Ҫд��λ�ã�read��������writeָ�����û������)
 */
ls_error_t ls_mbox_send_msg (ls_mbox_t *p_mbox, void* p_msg, uint32_t send_option)
{
	uint32_t count  = 0;
	
	ls_task_enter_critical();
	
	count = ls_event_wait_count(&p_mbox->event);
	
	/* �������ȴ�������������ȴ� */
	if (count) {
		
		ls_task_t *task;
		
		/* �������ѣ�������Ϣ���������е�event_msg */
		task = ls_event_wakeup(&p_mbox->event, p_msg, event_no_error);
		
		if (task->task_pro < current_task->task_pro) {
			ls_task_schedule();
		}	
	} else {
		/* ���û�������ڵȴ� */
		
		/* �������� */
		if (p_mbox->msg_count >= p_mbox->msg_max_count) {
			ls_task_exit_critical();
			return -LS_MBOX_FULL;
		}
		
		/* ��Ϣ���� */
		if (send_option == LS_MSG_URGENCY) {
			
			if (p_mbox->read_index <= 0) {
				p_mbox->read_index = p_mbox->read_index + p_mbox->msg_max_count - 1;
				
				p_mbox->buffer[p_mbox->read_index] = p_msg; 
			} else {
				p_mbox->buffer[--(p_mbox->read_index)] = p_msg;
			}
			
		} else {  /* ��Ϣ��ͨ */
			
				temp = (uint32_t)&p_mbox->buffer[p_mbox->write_index];
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
 *	\brief ��ȡ��Ϣ
 */
ls_error_t ls_mbox_recieve_msg(ls_mbox_t *p_mbox, void** msg, uint32_t timeout)
{
	ls_task_enter_critical();
	
	if (p_mbox->msg_count) {
		
		temp = (uint32_t)&p_mbox->buffer[p_mbox->read_index];
		/* �����Ϣ����������Ϣ���������Ϣ */
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
		
		/* ���´��ٵ��������֤����Ϣ�Ѿ������������event_msg�� */
		*msg = current_task->event_msg;
		
		ls_task_exit_critical();
		
		return current_task->event_result;
	}
}

/*
 *	\brief ��ȡ��Ϣ�޵ȴ�
 */
ls_error_t ls_mbox_recieve_msg_nowait(ls_mbox_t *p_mbox, void** msg)
{
	ls_task_enter_critical();
	
	if (p_mbox->msg_count) {
		
		/* �����Ϣ����������Ϣ���������Ϣ */
		*msg = p_mbox->buffer[p_mbox->read_index++];
		
		p_mbox->msg_count--;
		
		ls_task_exit_critical();
		
		return LS_OK;
	} else {
		
		ls_task_exit_critical();
		
		return event_resource_unavailable_error;
	}
}

