#include "ARMCM3.h"
#include "rtos.h"
#include "lib.h"
#include "ls_list.h"
#include "ls_timer.h"
#include "ls_task.h"

extern ls_bitmap g_bit_map;

void tSetSysTickPeriod(uint32_t ms)
{
  SysTick->LOAD  = ms * SystemCoreClock / 1000 - 1; 
  NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
  SysTick->VAL   = 0;                           
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk; 
}


void ls_delayms(uint32_t systick)
{
	ls_task_enter_critical();
	current_task->task_delay = systick;
	ls_bitmap_clr(&g_bit_map, current_task->task_pro);
	ls_task_exit_critical();
	
	ls_task_schedule();
}

extern ls_task_t	 task1;
extern ls_task_t	 task2;
void SysTick_Handler () 
{

		if (task1.task_delay != 0) {
			task1.task_delay--;
		}else {

			ls_bitmap_set(&g_bit_map, task1.task_pro);
		}
		if (task2.task_delay != 0) {
			task2.task_delay--;
		}else {
			ls_bitmap_set(&g_bit_map, task2.task_pro);
		}
		
    ls_task_schedule();
}

