#include "ARMCM3.h"
#include "rtos.h"
#include "lib.h"
#include "ls_list.h"
#include "ls_timer.h"
#include "ls_task.h"
#include "ls_rtos.h"

extern ls_bitmap g_bit_map;

ls_list_t    g_delay_list;

void tSetSysTickPeriod(uint32_t ms)
{
  SysTick->LOAD  = ms * SystemCoreClock / 1000 - 1; 
  NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
  SysTick->VAL   = 0;                           
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk; 
}

void ls_init_delay_list (void)
{

	ls_list_init(&g_delay_list);

}

/*
 *	\brief �������ȴ�״̬
 */
void ls_task_timer_wait(ls_task_t *p_task, uint32_t delay_ticks)
{
//	ls_list_insert_node_first(&g_delay_list, &p_task->task_delay_node);
	ls_list_insert_node_last(&g_delay_list, &p_task->task_delay_node);
	p_task->task_delay_ticks = delay_ticks;
	p_task->task_state |= LS_TASK_DELAY;

}

/*
 *	\brief ���񱻴ӵȴ�̬�л��� 
 */
void ls_task_timer_weakup(ls_task_t *p_task)
{

	ls_list_remove_node(&g_delay_list, &p_task->task_delay_node);
	p_task->task_state &= ~LS_TASK_DELAY;
}

/*
 *	����ʱ�������Ƴ�
 */
void ls_task_time_remove(ls_task_t *p_task)
{
	ls_task_enter_critical();

	ls_list_remove_node(&g_delay_list, &p_task->task_delay_node);

	ls_task_exit_critical();
}

void ls_delayms(uint32_t systick)
{
	ls_task_enter_critical();
	
	ls_task_timer_wait(current_task, systick);
	
	ls_task_sched_unrdy(current_task);
	
	ls_task_exit_critical();
	
	ls_task_schedule();
}

extern ls_task_t	 task1;
extern ls_task_t	 task2;
void SysTick_Handler () 
{

		ls_node_t *temp_node ;
//		ls_node_t *rtos_task_myself_node;
	
		uint32_t count = 0;
	
		/* �����ٽ��� */
		ls_task_enter_critical();
	
		/* ��ȡ����ʱ���еĵ�һ����ʱ�ڵ�  */
		temp_node = ls_list_first_node(&g_delay_list);
		
		for (count = g_delay_list.node_count; count !=0; count--) {
			
			/* ͨ����ʱ����ڵ㣬���Ƴ���ǰ���� */
			ls_task_t *temp_task = LS_GET_PARENT_STRUCT_ADDR(temp_node, ls_task_t, task_delay_node);
			
			if (--(temp_task->task_delay_ticks) == 0 ) {
			
//				/* ���������ʱʱ�䵽��, �ʹ���ʱ�����Ƴ� */
//				ls_list_remove_first(&g_delay_list);
				
				/* ����ʱ״̬�л��� */
				ls_task_timer_weakup(temp_task);
				ls_task_sched_rdy(temp_task);
				
			}
			temp_node = temp_node->next_node;
		}
		
		
		if (--(current_task->task_slice) == 0) {
		
			current_task->task_slice = TASK_TIME_SLICE_MAX;
			
//			/* ��ȡĳ�����ȼ��£������������е��׸����� */
//			temp_node = ls_list_first_node(&task_table[current_task->task_pro]);
//			
//			/* ��ȡ�����������еĵ�ǰ�����׸��ڵ������ */
//			temp_task = LS_GET_PARENT_STRUCT_ADDR(temp_node, ls_task_t, task_time_slice_node); 
			
			  /* �Ƴ���ǰ���ȼ����������е��׽ڵ� */
				temp_node = ls_list_remove_first(&task_table[current_task->task_pro]);

				/* ���Ƴ��Ľڵ���ӵ���ǰ���ȼ������ĩβ */
				ls_list_insert_node_last(&task_table[current_task->task_pro], temp_node);
		}

		
		
		/* �˳��ٽ��� */
		ls_task_exit_critical();
		
		/* ִ��������� */
    ls_task_schedule();
}

