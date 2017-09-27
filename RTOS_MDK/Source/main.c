#include "ARMCM3.h"
#include "rtos.h"
#include "lib.h"
#include "ls_list.h"



#define TASK_COUNT  255

rtos_task_t	 task1;
rtos_task_t	 task2;
rtos_task_t	 task_idle;

rtos_task_t *current_task;
rtos_task_t *next_task;
rtos_task_t *task_list[TASK_COUNT];

bitmap g_bit_map;

task_stack  task1_stack[1024];
task_stack  task2_stack[1024];
task_stack  task_idle_stack[1024];

uint8_t  task_schedule_lock = 0;


void task_schedule_disable ()
{
	if (task_schedule_lock < 255) {
		task_schedule_lock ++;
	} 
}

void task_schedule_enable ()
{
	if (task_schedule_lock > 0) {
		task_schedule_lock--;
	}
}

void delay(uint32_t systick)
{
	task_enter_critical();
	current_task->task_delay = systick;
	task_exit_critical();
	
	task_schedule();
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

void task_init(rtos_task_t *p_task, task_stack * p_task_stack, uint8_t prio, void* func_entry, void *p_param)
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
	
	p_task->task_pro = prio;
	p_task->p_stack = p_task_stack;
	p_task->task_delay = 0;
	task_list[prio] = p_task;
	bitmap_set(&g_bit_map, prio);
}

int flag1  = 0;
int flag2  = 0;

void task1_func()
{
	tSetSysTickPeriod(10);
	for (; ;) {
		
		flag1 = 1;
		delay(2);
		flag1 = 0;
		delay(2);

	}
}

uint32_t test_public_varaible = 0;
void task2_func()
{

	for (; ;) {
		uint32_t i = 0xFFFF;
		uint32_t count = 0;
		

		task_schedule_disable();
		count = test_public_varaible;
		while(i--);
		
		test_public_varaible = count + 1;
		task_schedule_enable();

		flag2 = 1;
		delay(2);
		flag2 = 0;
		delay(2);

	}
	
}
int flag3 = 0;
void task_idle_func()
{
	for (; ;) {
	flag3 = !flag3;
	}
}

rtos_task_t* task_high_redy()
{
	uint32_t temp = get_bitmap_high_prio(&g_bit_map);
	
	return task_list[temp];
}

int test = 0;
int main(){
	bitmap_init(&g_bit_map);
	
	task_init(&task1, &task1_stack[1024], 1, task1_func, (void*)0x11111111);
	
	task_init(&task2, &task2_stack[1024], 2, task2_func, (void*)0x22222222);
	
	task_init(&task_idle, &task_idle_stack[1024], 255, task_idle_func, (void*)0x22222222);
	
	next_task = task_high_redy();

	first_tast_entry();
	
	
	
	
/* 不可能执行到返回 */
	return 0;
}

//void task_sched()
//{
//	
//	/* 只要访问共享变量的都要加临界区保护，防止数据被覆盖 */
//	task_enter_critical();
//	
//	if (task_schedule_lock > 0) {
//		task_exit_critical();
//		
//		return;
//	}
//	
//	if (current_task == &task1) {
//		next_task = &task2;
//	} else {
//		next_task = &task1;
//	}
//	task_witch ();
//	task_exit_critical();
//}


void SysTick_Handler () 
{
	test_public_varaible++;

		if (task1.task_delay != 0) {
			task1.task_delay--;
		}
		if (task2.task_delay != 0) {
			task2.task_delay--;
		}
	
    task_schedule();
}
