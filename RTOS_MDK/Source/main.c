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

ls_mbox_t mbox1;
ls_mbox_t mbox2;
ls_mbox_info_t mbox2_info;
void *mbox1_buffer[20];
void *mbox2_buffer[20];

uint32_t testmsg[20];


void clean(void * param)
{
	flag1 = 1;
}

ls_task_info_t task_info;

void task1_func()
{
	void *p_msg;
	tSetSysTickPeriod(10);
	
	ls_mbox_init(&mbox1, mbox1_buffer, 20);
	ls_mbox_init(&mbox2, mbox2_buffer, 20);
	
	ls_mbox_recieve_msg(&mbox1, &p_msg, 0);
	
	for (; ;) {
		
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
		
		uint32_t error = 0, i;
	
		for (i = 0; i < 20 ; i++) {
			
			testmsg[i] = i;
		  ls_mbox_send_msg(&mbox2, &testmsg[i], LS_MSG_NORMAL);
		}
		
	  flag2 = 1;
		ls_delayms(2);
		flag2 = 0;
		ls_delayms(2);
		
	}
}

void task3_func()
{
	for (; ;) {

		void *msg;
		
		uint32_t temp = 0;
		ls_mbox_recieve_msg(&mbox2, &msg, 0);
		
		temp = *(uint32_t *)msg;
		
		ls_mbox_get_info(&mbox2, &mbox2_info);
		
		ls_mbox_flush (&mbox2);
		
		ls_mbox_get_info(&mbox2, &mbox2_info);
		
		flag3 = 1;
		ls_delayms(2);
		flag3 = 0;
		ls_delayms(2);
	}
}

ls_sem_info_t info;
void task4_func()
{
	uint32_t count = 0;
	for (; ;) {

		count = ls_mbox_delete(&mbox1);
		
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
	
	ls_task_init(&task4, &task4_stack[1024], 1, task4_func, (void*)0x44444444);

	ls_task_init(&task_idle, &task_idle_stack[1024], 31, task_idle_func, (void*)0x22222222);
	
	next_task = ls_task_high_redy();

	ls_first_tast_entry();
	
/* 不可能执行到返回 */
	return 0;
}
