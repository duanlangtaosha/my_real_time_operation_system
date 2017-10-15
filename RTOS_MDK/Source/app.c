#include "ls_rtos.h"

ls_task_t	 task1;
ls_task_t	 task2;
ls_task_t	 task3;
ls_task_t	 task4;
//ls_task_t	 task_idle;


ls_stack_t  task1_stack[1024];
ls_stack_t  task2_stack[1024];
ls_stack_t  task3_stack[1024];
ls_stack_t  task4_stack[1024];
//ls_stack_t  task_idle_stack[1024];

ls_task_info_t task1_info;
ls_task_info_t task2_info;
ls_task_info_t task3_info;
ls_task_info_t task4_info;


int flag1  = 0;
int flag2  = 0;
int flag3  = 0;
int flag4  = 0;
//int flag5  = 0;

ls_flag_group_t flag_group;
uint32_t result;
uint32_t result_inof;
ls_flag_group_info_t group_info;
extern float ls_get_cpu_usage(void);
void delay()
{
	uint32_t i = 0xFFFF;
	while(i--);
}
void task1_func()
{
	float usag = 0.0;
	
	ls_flag_group_init(&flag_group, 0x00);

	for (; ;) {
		
//		ls_flag_group_take(&flag_group, LS_FLAG_SET_ALL, 0x03, &result, 0);
//		ls_flag_group_get_flag(&flag_group, LS_FLAG_SET_ALL, 0x01, &result_inof);
		
		flag1 = 1;
//		ls_delayms(1);
		delay();
		flag1 = 0;
		ls_delayms(1);
		
		usag = ls_get_cpu_usage();
	}
}



void task2_func()
{
	for (; ;) {
		

		flag2 = 1;
		ls_delayms(2);
//		ls_flag_group_get_info(&flag_group, &group_info);
//	  ls_flag_group_give(&flag_group, LS_FLAG_SET, 0x03);
//		ls_flag_group_delete(&flag_group);
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

void ls_init_app()
{
	ls_task_init(&task1, task1_stack, sizeof(task1_stack), 0, task1_func, (void*)0x11111111);
	
	ls_task_init(&task2, task2_stack, sizeof(task2_stack), 1, task2_func, (void*)0x22222222);
	
	ls_task_init(&task3, task3_stack, sizeof(task3_stack), 1, task3_func, (void*)0x33333333);
	
	ls_task_init(&task4, task4_stack, sizeof(task4_stack), 1, task4_func, (void*)0x44444444);
}
