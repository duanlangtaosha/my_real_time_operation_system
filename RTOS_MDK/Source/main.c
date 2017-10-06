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

ls_mem_block_t mem_block1;
ls_mem_block_t mem_block2;

uint8_t mem1[20][100];
uint8_t mem1[20][100];

typedef uint8_t (*mem_block)[100];

void clean(void * param)
{
	flag1 = 1;
}

ls_task_info_t task_info;

uint32_t temp = 0;
void task1_func()
{
	mem_block block[20];
	uint32_t i;
	
	extern void tSetSysTickPeriod(uint32_t ms);
	
	tSetSysTickPeriod(10);
	
	
	
	ls_mem_block_init(&mem_block1, mem1, 100, 20);
	
			
	for (i = 0; i < 20; i++) {
		ls_mem_block_take(&mem_block1, (void **)&block[i], 0);
	}
	
	ls_delayms(2);
	
	for (i = 0; i < 20; i++) {
		memset(block[i], i, 100);
		ls_mem_block_give(&mem_block1, (uint8_t *)block[i]);
		
		/* 要想看的阶梯则这个延时是必须的 */
		ls_delayms(2);
	}
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
		
		mem_block block;
		
		temp = (uint32_t)block;
		ls_mem_block_take(&mem_block1, (void **)&block, 0);
		temp = (uint32_t)block;
		
		flag2 = *(uint8_t*)block;
		
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
