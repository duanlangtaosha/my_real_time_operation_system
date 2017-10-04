#include "ARMCM3.h"
#include "rtos.h"
#include "lib.h"
#include "ls_list.h"
#include "ls_rtos.h"
#include "ls_task.h"
#include "ls_error.h"



/**< \brief ��������������ȴ�����ʼ�� */
uint8_t  task_schedule_lock = 0;

/**< \brief ��ǰ�����ָ�� */
ls_task_t *current_task;

/**< \brief ��Ҫִ�е���һ�������ָ�� */
ls_task_t *next_task;

/**< \brief ���������  */
//ls_task_t *task_list[LS_TASK_COUNT];

/**< \brief ����ʱ��Ƭ�ڵ� */
ls_list_t task_table[LS_TASK_COUNT];

/**< \brief ��ǰʵʱ����ϵͳ���������� */
uint32_t g_rtos_task_count = 0;

/**< \brief ��¼��ǰϵͳ�д��ڵ����� */
ls_list_t ls_rtos_task_list;

extern ls_bitmap g_bit_map;

//uint32_t ls_bit_pro_check = 0;


/*
 *	�����ʼ��
 */
void ls_task_init(ls_task_t *p_task, ls_stack_t * p_task_stack, uint8_t prio, void* func_entry, void *p_param)
{
	
	*(--p_task_stack) = (uint32_t)(1 << 24);
	*(--p_task_stack) = (uint32_t)func_entry;
	*(--p_task_stack) = (uint32_t)14;		     /* R14(LR)*/
	*(--p_task_stack) = (uint32_t)12;		     /* R14 */
	*(--p_task_stack) = (uint32_t)3;         /* R3 */
	*(--p_task_stack) = (uint32_t)2;         /* R2 */
	*(--p_task_stack) = (uint32_t)1;         /* R1 */
	*(--p_task_stack) = (uint32_t)p_param;   /* R0 */
	*(--p_task_stack) = (uint32_t)11;        /* R11 */
	*(--p_task_stack) = (uint32_t)10;        /* R10 */
	*(--p_task_stack) = (uint32_t)9;         /* R9 */
	*(--p_task_stack) = (uint32_t)8;         /* R8 */
	*(--p_task_stack) = (uint32_t)7;         /* R7 */
	*(--p_task_stack) = (uint32_t)6;         /* R6 */
	*(--p_task_stack) = (uint32_t)5;         /* R5 */
	*(--p_task_stack) = (uint32_t)4;         /* R4 */
	
	p_task->task_pro = prio;
	p_task->p_stack = p_task_stack;
	p_task->task_delay_ticks = 0;

	ls_bitmap_set(&g_bit_map, prio);
	
	p_task->ls_task_suspend_count = 0;
	
	p_task->task_state = LS_TASK_RDY;
	
	/* ��ʼ����ʱ�б�Ľڵ㣬��ʵ����ʼ��Ҳ����, ���Ǵ���һ�����⣬������һ��ǰָ��ͺ�
	 * ��ָ�벻��ָ�����Լ������Ժ�Ĳ����п����������⡣
	 */
	ls_node_init(&p_task->task_delay_node);
	
	/* ��ʼ����������ʱ��Ƭ */
	p_task->task_slice = TASK_TIME_SLICE_MAX;
	
	/* ��ʼ������ʱ��Ƭ�ڵ� */
	ls_node_init(&p_task->task_time_slice_node);
	
	/* ��ʼ���������� */
	ls_list_insert_node_last(&task_table[prio], &p_task->task_time_slice_node);
	
	/**< \brief ��ǰʵʱ����ϵͳ���������� */
  g_rtos_task_count++;
	
	
//	if (!(ls_bit_pro_check & (1 << prio))) {
//		
//		ls_bit_pro_check |= (1 << prio);
//		
//		/**< \brief ��¼��ǰ�������Ϣ */
//		ls_list_insert_node_first(&ls_rtos_task_list, &p_task->task_myself_node);
//	}
}


/*
 *	��ȡ��ǰ���ȼ��õ����������ָ��
 */
ls_task_t* ls_task_high_redy(void)
{
	/* ��ȡ�������е�������ȼ� */
	uint32_t temp = ls_get_bitmap_high_prio(&g_bit_map);
	
	/* ��ȡ��ǰ������ȼ������������׸��ڵ�(������ʱ��Ƭִ�������Զ��İѵ�ǰ�ڵ���뵽β�ڵ���) */
	ls_node_t *node = ls_list_first_node(&task_table[temp]);
	
	/* ͨ����ǰ�ڵ��ȡ��ǰ���ȼ���ߵ����� */
	ls_task_t *task_high = LS_GET_PARENT_STRUCT_ADDR(node, ls_task_t, task_time_slice_node);
	
	return task_high;
}


/*
 *	\brief ��ֹ�������,�����������
 */
void ls_task_schedule_disable (void)
{
	if (task_schedule_lock < 255) {
		task_schedule_lock ++;
	} 
}


/*
 *	\brief ʹ��������ȣ�ȡ�����������
 */
void ls_task_schedule_enable (void)
{
	if (task_schedule_lock > 0) {
		task_schedule_lock--;
	}
}

/*
 *	��������Ϊ����̬
 */
void ls_task_sched_rdy(ls_task_t *p_task)
{
	ls_task_enter_critical();
	
	/* �ѽڵ����¼����������ȼ������� */
	ls_list_insert_node_last(&task_table[p_task->task_pro], &p_task->task_time_slice_node);
	
	ls_bitmap_set(&g_bit_map, p_task->task_pro);
	
	ls_task_exit_critical();
}


/*
 *	��������Ǿ���̬
 */
void ls_task_sched_unrdy(ls_task_t *p_task)
{
	ls_task_enter_critical();
	
	ls_list_remove_node(&task_table[p_task->task_pro], &p_task->task_time_slice_node);
	
	if (ls_list_get_node_count(&task_table[p_task->task_pro]) == 0) {
	
	ls_bitmap_clr(&g_bit_map, p_task->task_pro);
	}
	
	ls_task_exit_critical();
}

/*
 *	��ʼ���������
 */
void ls_task_sched_init (void) 
{
	uint32_t i;
	
	task_schedule_lock = 0;
	
	ls_bitmap_init(&g_bit_map);
	
	/* 
	 * ����֧�ֵ����е���������ȼ���ע���ʼ��Ϊ����
	 */
	for (i = 0; i < LS_TASK_COUNT; i++) {
	
		ls_list_init(&task_table[i]);
	}	
}

/*
 *	��ȡ��ǰϵͳ�������
 */
uint32_t ls_get_rtos_task_count (void)
{
	return g_rtos_task_count;
}

/*
 *	��ʼ��ϵͳ����ͳ������
 */
void ls_rtos_task_list_init (void)
{
	ls_node_init(&ls_rtos_task_list.head_node);
	
	ls_list_init(&ls_rtos_task_list);
}

/*
 *	\brief �������
 */
ls_error_t ls_task_suspend(ls_task_t* p_task)
{
	ls_task_enter_critical();
	
	if (p_task->task_state & LS_TASK_DELAY) {
		return -LS_INCORRECT_STA;
	}
	
	if (++p_task->ls_task_suspend_count <= 1) {
		p_task->task_state |= LS_TASK_SUSPEND;
		ls_task_sched_unrdy(p_task);
	}
	
	ls_task_exit_critical();
	
	if (current_task == p_task) {
		ls_task_schedule();
	}
	
	return LS_OK;
}

/*
 *	\brief ����ָ�
 */
void ls_task_resume(ls_task_t* p_task)
{
	
	ls_task_enter_critical();
	
	if (p_task->task_state & LS_TASK_SUSPEND) {
		
		ls_task_sched_rdy(p_task);
	  p_task->task_state &= ~LS_TASK_SUSPEND;
	
		if (p_task->task_pro < current_task->task_pro) {
			ls_task_schedule();
		}
  }
	
	ls_task_exit_critical();
}

