#ifndef __LS_EVENT_H
#define __LS_EVENT_H

#include "ls_rtos.h"


typedef enum __ls_event_error {
	event_no_error,
	event_timeout_error,
}ls_event_error_t;

typedef enum __ls_event_type {

	event_type_no,

}ls_event_type_t;

typedef struct __ls_event {

	ls_event_type_t event_type;
	ls_list_t  event_list;

}ls_event_t;



/*
 *	\brief 初始化任务等待事件
 */
void ls_event_init (ls_event_t *p_event, ls_event_type_t event_type);

/*
 *	\brief 任务产生一个等待事件
 */
void ls_event_wait (ls_task_t *p_task, ls_event_t *p_event, void *p_msg, uint32_t state, uint32_t timeout);

/*
 *	\brief 将任务从事件等待队列中唤醒
 */
ls_task_t *ls_event_wakeup(ls_event_t *p_event, void *p_msg, uint32_t result);

/*
 *	\brief 将任务从事件等待队列中移除
 */
void ls_event_rmove_task (ls_task_t * p_task, void * p_msg, uint32_t result);




#endif

