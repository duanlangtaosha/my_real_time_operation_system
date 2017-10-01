#include "ARMCM3.h"
#include "rtos.h"
#include "lib.h"
#include "ls_list.h"
#include "ls_rtos.h"
#include "ls_task.h"



/**< \brief 任务调度锁，调度次数初始化 */
uint8_t  task_schedule_lock = 0;

/**< \brief 当前任务的指针 */
ls_task_t *current_task;

/**< \brief 将要执行的下一个任务的指针 */
ls_task_t *next_task;

/**< \brief 任务数组表  */
//ls_task_t *task_list[LS_TASK_COUNT];

/**< \brief 任务时间片节点 */
ls_list_t task_table[LS_TASK_COUNT];



extern ls_bitmap g_bit_map;


/*
 *	任务初始化
 */
void ls_task_init(ls_task_t *p_task, ls_stack_t * p_task_stack, uint8_t prio, void* func_entry, void *p_param)
{
	*(--p_task_stack) = (uint32_t)(1 << 24);
	*(--p_task_stack) = (uint32_t)func_entry;
	*(--p_task_stack) = (uint32_t)14;		     /* R14(LR)*/
	*(--p_task_stack) = (uint32_t)12;		     /* R14 */
	*(--p_task_stack) = (uint32_t)3;         /* R3 */
	*(--p_task_stack) = (uint32_t)2;         /* R2 */
	*(--p_task_stack) = (uint32_t)1;         /* R1 */
	*(--p_task_stack) = (uint32_t)p_param;   /* R0 */
	*(--p_task_stack) = (uint32_t)11;        /* R11 */
	*(--p_task_stack) = (uint32_t)10;        /* R10 */
	*(--p_task_stack) = (uint32_t)9;         /* R9 */
	*(--p_task_stack) = (uint32_t)8;         /* R8 */
	*(--p_task_stack) = (uint32_t)7;         /* R7 */
	*(--p_task_stack) = (uint32_t)6;         /* R6 */
	*(--p_task_stack) = (uint32_t)5;         /* R5 */
	*(--p_task_stack) = (uint32_t)4;         /* R4 */
	
	p_task->task_pro = prio;
	p_task->p_stack = p_task_stack;
	p_task->task_delay_ticks = 0;
//	task_list[prio] = p_task;
	ls_bitmap_set(&g_bit_map, prio);
	
	/* 初始化延时列表的节点，其实不初始化也可以, 但是存在一个问题，它的上一个前指针和后
	 * 后指针不是指向它自己，再以后的操作中可能遇到问题。
	 */
	ls_node_init(&p_task->task_delay_node);
	
	/* 初始化任务最大的时间片 */
	p_task->task_slice = TASK_TIME_SLICE_MAX;
	
	/* 初始化任务时间片节点 */
	ls_node_init(&p_task->task_time_slice_node);
	
	/* 初始化任务链表 */
	ls_list_insert_node_last(&task_table[prio], &p_task->task_time_slice_node);
	
}


/*
 *	获取当前优先级好的任务的任务指针
 */
ls_task_t* ls_task_high_redy(void)
{
	/* 获取就绪表中的最高优先级 */
	uint32_t temp = ls_get_bitmap_high_prio(&g_bit_map);
	
	/* 获取当前最高优先级任务链表中首个节点(在任务时间片执行完后会自动的把当前节点插入到尾节点上) */
	ls_node_t *node = ls_list_first_node(&task_table[temp]);
	
	/* 通过当前节点获取当前优先级最高的任务 */
	ls_task_t *task_high = LS_GET_PARENT_STRUCT_ADDR(node, ls_task_t, task_time_slice_node);
	
	return task_high;
}


/*
 *	\brief 禁止任务调度,打开任务调度锁
 */
void ls_task_schedule_disable (void)
{
	if (task_schedule_lock < 255) {
		task_schedule_lock ++;
	} 
}


/*
 *	\brief 使能任务调度，取消任务调度锁
 */
void ls_task_schedule_enable (void)
{
	if (task_schedule_lock > 0) {
		task_schedule_lock--;
	}
}

/*
 *	设置任务为就绪态
 */
void ls_task_sched_rdy(ls_task_t *p_task)
{
	ls_bitmap_set(&g_bit_map, p_task->task_pro);
}


/*
 *	设置任务非就绪态
 */
void ls_task_sched_unrdy(ls_task_t *p_task)
{
	ls_bitmap_clr(&g_bit_map, p_task->task_pro);
}

/*
 *	初始化任务调度
 */
void ls_task_sched_init (void) 
{
	uint32_t i;
	
	task_schedule_lock = 0;
	
	ls_bitmap_init(&g_bit_map);
	
	/* 
	 * 把能支持的所有的任务的优先级都注册初始化为链表
	 */
	for (i = 0; i < LS_TASK_COUNT; i++) {
	
		ls_list_init(&task_table[i]);
	}
	
	
}

