#include "ls_rtos.h"


extern ls_bitmap g_bit_map;

ls_task_t	 task1;
ls_task_t	 task2;
ls_task_t	 task3;
ls_task_t	 task4;
ls_task_t	 task_idle;


ls_stack_t  task1_stack[1024];
ls_stack_t  task2_stack[1024];
ls_stack_t  task3_stack[1024];
ls_stack_t  task4_stack[1024];
ls_stack_t  task_idle_stack[1024];


int flag1  = 0;
int flag2  = 0;
int flag3  = 0;
int flag4  = 0;
int flag5  = 0;

ls_event_t event_timeout;
ls_event_t event_nomal;

void clean(void * param)
{
	flag1 = 1;
}

ls_task_info_t task_info;

void task1_func()
{
	tSetSysTickPeriod(10);
	ls_event_init(&event_nomal, event_type_no);
	ls_event_init(&event_timeout, event_type_no);
	
	for (; ;) {
		
		ls_event_wait(current_task, &event_timeout, 0, 0, 5);
		ls_task_schedule();
		flag1 = 1;
		ls_delayms(2);
		flag1 = 0;
		ls_delayms(2);
		
	}
}

void delay()
{
	uint32_t i = 0xFFFF;
	while(i--);
}
void task2_func()
{
	for (; ;) {
	
		ls_event_wait(current_task, &event_nomal, 0, 0, 0);
		ls_task_schedule();
		flag2 = 1;
		ls_delayms(2);

		flag2 = 0;
		ls_delayms(2);
	}
}

void task3_func()
{
	for (; ;) {

//		ls_event_wakeup(&event_timeout, (void*)0, 0);
		ls_task_schedule();
		flag3 = 1;
		ls_delayms(2);
		flag3 = 0;
		ls_delayms(2);
	}
}

void task4_func()
{

//	uint32_t request = 0;
	for (; ;) {

		ls_event_wakeup(&event_nomal, (void*)0, 0);
		ls_task_schedule();
		flag4 = 1;
		ls_delayms(2);
		flag4 = 0;
		ls_delayms(2);
		
	}
	
}


void task_idle_func()
{
	for (; ;) {
	flag5 = !flag5;
	}
}


int test = 0;

int main(){
	
	/* 初始化任务调度 */
	ls_task_sched_init();
	
	ls_init_delay_list ();
	
	ls_task_init(&task1, &task1_stack[1024], 0, task1_func, (void*)0x11111111);
	
	ls_task_init(&task2, &task2_stack[1024], 1, task2_func, (void*)0x22222222);
	
	ls_task_init(&task3, &task3_stack[1024], 1, task3_func, (void*)0x33333333);
	
	ls_task_init(&task4, &task4_stack[1024], 0, task4_func, (void*)0x44444444);

	ls_task_init(&task_idle, &task_idle_stack[1024], 31, task_idle_func, (void*)0x22222222);
	
	next_task = ls_task_high_redy();

	ls_first_tast_entry();
	
/* 不可能执行到返回 */
	return 0;
}
