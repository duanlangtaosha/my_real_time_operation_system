#include "ls_rtos.h"
#include "string.h"


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

ls_flag_group_t flag_group;
uint32_t result;
uint32_t result_inof;
ls_flag_group_info_t group_info;

void task1_func()
{
	extern void tSetSysTickPeriod(uint32_t ms);
	tSetSysTickPeriod(10);
	
	ls_flag_group_init(&flag_group, 0x00);

	for (; ;) {
		
		ls_flag_group_take(&flag_group, LS_FLAG_SET_ALL, 0x03, &result, 0);
		ls_flag_group_get_flag(&flag_group, LS_FLAG_SET_ALL, 0x01, &result_inof);
		
		flag1 = 1;
		ls_delayms(1);
		flag1 = 0;
		ls_delayms(1);
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
		

		flag2 = 1;
		ls_delayms(2);
		ls_flag_group_get_info(&flag_group, &group_info);
//	  ls_flag_group_give(&flag_group, LS_FLAG_SET, 0x03);
		ls_flag_group_delete(&flag_group);
		flag2 = 0;
		ls_delayms(2);
	
		
	}
}

void task3_func()
{
	for (; ;) {

		flag3 = 1;
		ls_delayms(2);
		flag3 = 0;
		ls_delayms(2);
	}
}

void task4_func()
{
	for (; ;) {

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
	
	ls_timer_module_init ();
	/* 初始化任务调度 */
	ls_task_sched_init();
	
	ls_init_delay_list ();
	
	ls_task_init(&task1, &task1_stack[1024], 0, task1_func, (void*)0x11111111);
	
	ls_task_init(&task2, &task2_stack[1024], 1, task2_func, (void*)0x22222222);
	
	ls_task_init(&task3, &task3_stack[1024], 1, task3_func, (void*)0x33333333);
	
	ls_task_init(&task4, &task4_stack[1024], 1, task4_func, (void*)0x44444444);

	ls_task_init(&task_idle, &task_idle_stack[1024], 31, task_idle_func, (void*)0x22222222);
	
	next_task = ls_task_high_redy();

	ls_first_tast_entry();
	
/* 不可能执行到返回 */
	return 0;
}
