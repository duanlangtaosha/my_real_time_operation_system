#include "ARMCM3.h"
#include "rtos.h"
#include "lib.h"
#include "ls_list.h"
#include "ls_rtos.h"
#include "ls_task.h"

uint8_t  task_schedule_lock = 0;

ls_task_t *current_task;
ls_task_t *next_task;
ls_task_t *task_list[LS_TASK_COUNT];


extern ls_bitmap g_bit_map;

void ls_task_init(ls_task_t *p_task, ls_stack_t * p_task_stack, uint8_t prio, void* func_entry, void *p_param)
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
	p_task->task_delay_ticks = 0;
	task_list[prio] = p_task;
	ls_bitmap_set(&g_bit_map, prio);
}

ls_task_t* ls_task_high_redy(void)
{
	uint32_t temp = ls_get_bitmap_high_prio(&g_bit_map);
	
	return task_list[temp];
}

void ls_task_schedule_disable (void)
{
	if (task_schedule_lock < 255) {
		task_schedule_lock ++;
	} 
}

void ls_task_schedule_enable (void)
{
	if (task_schedule_lock > 0) {
		task_schedule_lock--;
	}
}

void ls_task_sched_rdy(ls_task_t *p_task)
{
	ls_bitmap_set(&g_bit_map, p_task->task_pro);
}

void ls_task_sched_unrdy(ls_task_t *p_task)
{
	ls_bitmap_clr(&g_bit_map, p_task->task_pro);
}

