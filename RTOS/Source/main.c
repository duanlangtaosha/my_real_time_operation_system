#include "ARMCM3.h"
#include "rtos.h"

rtos_task_t	 task1;
rtos_task_t	 task2;

rtos_task_t *current_task;
rtos_task_t *next_task;

task_stack  task1_stack[1024];
task_stack  task2_stack[1024];

void delay(uint32_t time)
{
	while(time--);
}


void tSetSysTickPeriod(uint32_t ms)
{
  SysTick->LOAD  = ms * SystemCoreClock / 1000 - 1; 
  NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
  SysTick->VAL   = 0;                           
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk; 
}

void task_init(rtos_task_t *p_task, task_stack * p_task_stack, void* func_entry, void *p_param)
{
	*(--p_task_stack) = (uint32_t)(1 << 24);
	*(--p_task_stack) = (uint32_t)func_entry;
	*(--p_task_stack) = (uint32_t)14;		/* R14(LR)*/
	*(--p_task_stack) = (uint32_t)12;		/* R14 */
	*(--p_task_stack) = (uint32_t)3;    /* R3 */
	*(--p_task_stack) = (uint32_t)2;    /* R2 */
	*(--p_task_stack) = (uint32_t)1;    /* R1 */
	*(--p_task_stack) = (uint32_t)p_param;    /* R0 */
	*(--p_task_stack) = (uint32_t)11;    /* R11 */
	*(--p_task_stack) = (uint32_t)10;    /* R10 */
	*(--p_task_stack) = (uint32_t)9;    /* R9 */
	*(--p_task_stack) = (uint32_t)8;    /* R8 */
	*(--p_task_stack) = (uint32_t)7;    /* R7 */
	*(--p_task_stack) = (uint32_t)6;    /* R6 */
	*(--p_task_stack) = (uint32_t)5;    /* R5 */
	*(--p_task_stack) = (uint32_t)4;    /* R4 */
	
	p_task->p_stack = p_task_stack;
}

int flag1  = 0;
int flag2  = 0;

void task1_func()
{
	tSetSysTickPeriod(10);
	for (; ;) {
		
		flag1 = 1;
		delay(100);
		flag1 = 0;
		delay(100);
		next_task = &task2;
	}
}

void task2_func()
{

	for (; ;) {
		flag2 = 1;
		delay(100);
		flag2 = 0;
		delay(100);
		next_task = &task1;

	}
	
}

int main(){
	
	task_init(&task1, &task1_stack[1024], task1_func, (void*)0x11111111);
	
	
	task_init(&task2, &task2_stack[1024], task2_func, (void*)0x22222222);
	
	next_task = &task1;

	first_tast_entry();
	
/* 不可能执行到返回 */
	return 0;
}

void task_sched()
{
	if (current_task == &task1) {
		next_task = &task2;
	} else {
		next_task = &task1;
	}
	task_witch ();
}


void SysTick_Handler () 
{
    task_sched();
}
