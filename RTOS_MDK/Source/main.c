#include "ARMCM3.h"
#include "rtos.h"
#include "lib.h"
#include "ls_list.h"
#include "ls_timer.h"
#include "ls_task.h"


extern ls_bitmap g_bit_map;

ls_task_t	 task1;
ls_task_t	 task2;
ls_task_t	 task_idle;


ls_stack_t  task1_stack[1024];
ls_stack_t  task2_stack[1024];
ls_stack_t  task_idle_stack[1024];


int flag1  = 0;
int flag2  = 0;
int flag3  = 0;

void task1_func()
{
	tSetSysTickPeriod(10);
	for (; ;) {
		
		flag1 = 1;
		ls_delayms(2);
		flag1 = 0;
		ls_delayms(2);

	}
}

uint32_t test_public_varaible = 0;
void task2_func()
{

	for (; ;) {
		uint32_t i = 0xFFFF;
		uint32_t count = 0;
		

		ls_task_schedule_disable();
		count = test_public_varaible;
		while(i--);
		
		test_public_varaible = count + 1;
		ls_task_schedule_enable();

		flag2 = 1;
		ls_delayms(2);
		flag2 = 0;
		ls_delayms(2);

	}
	
}

void task_idle_func()
{
	for (; ;) {
	flag3 = !flag3;
	}
}


int test = 0;

int main(){
	
	ls_bitmap_init(&g_bit_map);
	ls_init_delay_list ();
	
	ls_task_init(&task1, &task1_stack[1024], 0, task1_func, (void*)0x11111111);
	
	ls_task_init(&task2, &task2_stack[1024], 1, task2_func, (void*)0x22222222);
	
	ls_task_init(&task_idle, &task_idle_stack[1024], 31, task_idle_func, (void*)0x22222222);
	
	next_task = ls_task_high_redy();

	ls_first_tast_entry();
	
/* 不可能执行到返回 */
	return 0;
}
