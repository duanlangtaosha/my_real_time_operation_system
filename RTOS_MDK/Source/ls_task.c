#include "ARMCM3.h"
#include "rtos.h"
#include "lib.h"
#include "ls_list.h"
#include "ls_rtos.h"
#include "ls_task.h"



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



extern ls_bitmap g_bit_map;


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
//	task_list[prio] = p_task;
	ls_bitmap_set(&g_bit_map, prio);
	
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
	ls_bitmap_set(&g_bit_map, p_task->task_pro);
}


/*
 *	��������Ǿ���̬
 */
void ls_task_sched_unrdy(ls_task_t *p_task)
{
	ls_bitmap_clr(&g_bit_map, p_task->task_pro);
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

