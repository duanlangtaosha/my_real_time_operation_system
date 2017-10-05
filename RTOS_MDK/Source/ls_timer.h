#ifndef __LS_TIMER_H
#define __LS_TIMER_H

#include "stdint.h"
#include "ls_task.h"

void tSetSysTickPeriod(uint32_t ms);
void ls_init_delay_list (void);
void ls_delayms(uint32_t systick);

/*
 *	从延时队列中移除
 */
void ls_task_time_remove(ls_task_t *p_task);

/*
 *	\brief 任务进入等待状态
 */
void ls_task_timer_wait(ls_task_t *p_task, uint32_t delay_ticks);

/*
 *	\brief 任务被从等待态中唤醒 
 */
void ls_task_timer_weakup(ls_task_t *p_task);



#endif
