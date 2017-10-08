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

typedef struct __ls_mutex_info {
	/* 等待任务的数量 */
	uint32_t task_count;
	
	/* 所有者的优先级 */
	uint32_t owner_prio;
	
	/* 继承优先级 */
	uint32_t inherited_prio;
	
	/* 所有者任务 */
	ls_task_t *owner_task;
	
	/* 当前锁定的次数 */
	uint32_t lock_count;
}ls_mutex_info_t;


#include "ls_rtos.h"

void ls_mutex_init (ls_mutex_t *p_mutex);

ls_error_t ls_mutex_take (ls_mutex_t *p_mutex, uint32_t timeout);

ls_error_t ls_mutex_give (ls_mutex_t *p_mutex);

/*
 *	\brief 删除信号量
 */
uint32_t ls_mutex_delete (ls_mutex_t *p_mutex);

/*
 *	\brief 获取当前互斥信号量的信息 
 */
void ls_mutex_get_info (ls_mutex_t *p_mutex, ls_mutex_info_t *info);


#endif

