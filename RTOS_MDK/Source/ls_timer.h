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



#endif
