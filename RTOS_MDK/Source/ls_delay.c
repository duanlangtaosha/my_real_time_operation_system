
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
 *	\brief 任务进入等待状态
 */
void ls_task_timer_wait(ls_task_t *p_task, uint32_t delay_ticks)
{
//	ls_list_insert_node_first(&g_delay_list, &p_task->task_delay_node);
	ls_list_insert_node_last(&g_delay_list, &p_task->task_delay_node);
	p_task->task_delay_ticks = delay_ticks;
	p_task->task_state |= LS_TASK_DELAY;

}

/*
 *	\brief 任务被从等待态中唤醒 
 */
void ls_task_timer_weakup(ls_task_t *p_task)
{

	ls_list_remove_node(&g_delay_list, &p_task->task_delay_node);
	p_task->task_state &= ~LS_TASK_DELAY;
}

/*
 *	从延时队列中移除
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

extern void ls_cpu_usage_check ();
extern uint32_t idel_count ;
extern uint32_t idel_count_max ;
extern uint32_t systick_count ;
extern  uint32_t first_systick ;
extern uint32_t start_check_cpu_usage ;
void SysTick_Handler () 
{

		ls_node_t *temp_node ;
//		ls_node_t *rtos_task_myself_node;
	
		uint32_t count = 0;
	
		/* 进入临界区 */
		ls_task_enter_critical();
	
		/* 获取出延时表中的第一个延时节点  */
		temp_node = ls_list_first_node(&g_delay_list);
		
		for (count = g_delay_list.node_count; count !=0; count--) {
			
			/* 通过延时任务节点，反推出当前任务 */
			ls_task_t *temp_task = LS_GET_PARENT_STRUCT_ADDR(temp_node, ls_task_t, task_delay_node);
			
			if (--(temp_task->task_delay_ticks) == 0 ) {
			
				if (temp_task->event) {
				
					/* 产生一个超时错误 */
					ls_event_rmove_task(temp_task, 0, event_timeout_error);
				}
				
				/* 从延时状态中唤醒 */
				ls_task_timer_weakup(temp_task);
				ls_task_sched_rdy(temp_task);
				
			}
			temp_node = temp_node->next_node;
		}
		
		
		if (--(current_task->task_slice) == 0) {
		
			current_task->task_slice = TASK_TIME_SLICE_MAX;
			
//			/* 获取某个优先级下，该任务链表中的首个任务 */
//			temp_node = ls_list_first_node(&task_table[current_task->task_pro]);
//			
//			/* 获取出任务链表中的当前处于首个节点的任务 */
//			temp_task = LS_GET_PARENT_STRUCT_ADDR(temp_node, ls_task_t, task_time_slice_node); 
			
			  /* 移除当前优先级任务链表中的首节点 */
				temp_node = ls_list_remove_first(&task_table[current_task->task_pro]);

				/* 把移除的节点添加到当前优先级链表的末尾 */
				ls_list_insert_node_last(&task_table[current_task->task_pro], temp_node);
		}

		systick_count++;
		
		ls_cpu_usage_check();
		
		/* 退出临界区 */
		ls_task_exit_critical();
		
		/* 软件定时器的处理 */
		systick_handle_timer_deal();
		
		/* 执行任务调度 */
    ls_task_schedule();
}

