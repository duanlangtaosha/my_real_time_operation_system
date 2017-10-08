#ifndef __LS_MUTEX_H
#define __LS_MUTEX_H

typedef struct __ls_mutex {
	
	/* 事件控制块 */
	ls_event_t event;
	
	/* 锁住的次数 */
	uint32_t lock_count;
	
	/* 当前锁住后的所有者任务 */
	ls_task_t* owner_task;
	
	/* 原始优先级 */
	uint32_t origin_prio;
}ls_mutex_t;


#include "ls_rtos.h"

void ls_mutex_init (ls_mutex_t *p_mutex);

ls_error_t ls_mutex_take (ls_mutex_t *p_mutex, uint32_t timeout);

ls_error_t ls_mutex_give (ls_mutex_t *p_mutex);


#endif

