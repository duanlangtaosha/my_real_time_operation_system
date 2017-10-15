
#include "ls_rtos.h"
#include "ls_mem_block.h"

#if (LS_ENABLE_MEM_BLOCK == 1)

/*
 *	\brief ��ʼ���ڴ��
 */
void ls_mem_block_init (ls_mem_block_t *p_mem_block,
												void * p_mem_start_addr, 
												uint32_t block_size, 
												uint32_t block_count)
{
	uint8_t *mem_start_addr = (uint8_t*)p_mem_start_addr;
	uint8_t *mem_end_addr = (uint8_t*)p_mem_start_addr + block_size * block_count;
	
	if (block_size < sizeof (ls_node_t)) {
	
	/* ���ܴ������Ľڵ� */
	return;
	}
	
	ls_event_init(&p_mem_block->event, ls_event_type_mem_block);
	
	p_mem_block->start_addr = (void *)p_mem_start_addr;
	p_mem_block->block_size = block_size;
	p_mem_block->block_count = block_count;
	
	ls_list_init(&p_mem_block->mem_block_list);
	
	
	while (mem_start_addr < mem_end_addr) {
		ls_list_insert_node_last(&p_mem_block->mem_block_list, (ls_node_t*)mem_start_addr);
		
		mem_start_addr += block_size;
	}
	
}

/*
 *	\brief �ͷ��ڴ��
 */
ls_error_t ls_mem_block_give (ls_mem_block_t *p_mem_block, void *mem)
{
	ls_task_enter_critical();
	
	/* ����¼����еȴ����� */
	if (ls_event_wait_count(&p_mem_block->event)) {
		
		ls_task_t *task;
		
		/* �������� */
		task = ls_event_wakeup(&p_mem_block->event, mem, event_no_error);
		ls_task_exit_critical();
		
		if (task->task_pro < current_task->task_pro) {
			ls_task_schedule();
		}
		
		
		return LS_OK;
	} else {
	
		/* ���ڴ��������� */
		ls_list_insert_node_last(&p_mem_block->mem_block_list, (ls_node_t*)mem);
	}
	
	ls_task_exit_critical();
	
	return LS_OK;
}

/*
 *	\brief ��ȡ�ڴ��
 */
ls_error_t ls_mem_block_take (ls_mem_block_t *p_mem_blcok, void** mem, uint32_t timeout)
{
	ls_task_enter_critical();
	
	/*  ����ڴ������л����ڴ� */
	if (ls_list_get_node_count(&p_mem_blcok->mem_block_list)) {
		
		*mem = (void*)ls_list_remove_first(&p_mem_blcok->mem_block_list);

		ls_task_exit_critical();
		
		return event_no_error;
	} else {
		/* û���ڴ���� */
		
		/* ����ǰ�β���ȴ����� */
		ls_event_wait(current_task, &p_mem_blcok->event, (void *)0, event_no_error, timeout);
		ls_task_exit_critical();
		
		ls_task_schedule();
		
		*mem = current_task->event_msg;
		
		return current_task->event_result;
		
	}
}

/*
 *	\brief ��ȡ�ڴ���޵ȴ�
 */
ls_error_t ls_mem_block_take_nowait (ls_mem_block_t *p_mem_blcok, void** mem, uint32_t timeout)
{
	ls_task_enter_critical();
	
	/*  ����ڴ������л����ڴ� */
	if (ls_list_get_node_count(&p_mem_blcok->mem_block_list)) {
		
		*mem = (void*)ls_list_remove_first(&p_mem_blcok->mem_block_list);
		ls_task_exit_critical();
		
		return event_no_error;
	} else {
		/* û���ڴ���� */
		
		ls_task_exit_critical();
		
		return event_resource_unavailable_error;
		
	}
}


#endif

