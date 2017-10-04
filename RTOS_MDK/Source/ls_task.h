#ifndef __LS_TASK_H
#define __LS_TASK_H

#include "ls_list.h"
#include "ls_rtos.h"
#include "stdint.h"
#include "ls_error.h"

#define LS_TASK_RDY			(1 << 0)
#define LS_TASK_DELAY   (1 << 1)
#define LS_TASK_SUSPEND (1 << 2)

typedef struct __ls_task {

	/** \brief 任务的堆栈指针 */
	uint32_t *p_stack;
	
	/** \brief 任务的延时tick数  */
	uint32_t  task_delay_ticks;
	
	/** \brief 任务延时队列节点 */
	ls_node_t task_delay_node;
	
	/** \brief 任务优先级 */
	uint32_t task_pro;
	
	/** \brief 任务的状态 */
	uint32_t task_state;
	
	/** \brief 相同优先级任务情况下，本任务所占时间片的数量 */
	uint32_t task_slice;
	
	/** \brief 任务时间片节点 */
	ls_node_t task_time_slice_node;
	
	/** \brief 记录任务的挂起的数量 */
	uint32_t ls_task_suspend_count;
	
	/** \brief 任务删除函数 */
	void (*clean) (void*);
	
	/** \breif 任务删除函数的参数 */
	void *clean_param;
	
	/** \brief 请求删除标志 */
	uint32_t request_delete_flag;
	
}ls_task_t;


typedef struct __ls_task_info {
	uint32_t task_pro;
	uint32_t task_state;
	uint32_t task_delay_ticks;
	uint32_t task_suspend_count;
	uint32_t task_slice;

}ls_task_info_t;

typedef uint32_t ls_stack_t;
extern ls_task_t *current_task;
extern ls_task_t *next_task;

extern ls_list_t task_table[LS_TASK_COUNT];

/*
 *	\brief 初始化任务
 *  \prame[in]  p_task       ：任务的实体指针
 *  \prame[in]  p_task_stack : 任务堆栈
 *  \prame[in]  prio         ：任务优先级
 *  \prame[in]  func_entry   : 任务的入口函数
 *  \prame[in]  p_param      : 任务入口函数的参数
 *  \ret   none
 */
void ls_task_init(ls_task_t *p_task, ls_stack_t * p_task_stack, uint8_t prio, void* func_entry, void *p_param);

/*
 *	\brief 获取当前就绪表中最高优先级的任务
 *  \prame[in]  none
 *  \ret   返回最高优先级任务的任务指针
 */
ls_task_t* ls_task_high_redy(void);

/*
 *	\brief 任务调度失能
 *  \prame[in]  none
 *  \ret   none
 */
void ls_task_schedule_disable (void);

/*
 *	\brief 任务调度使能
 *  \prame[in]  none
 *  \ret   none
 */
void ls_task_schedule_enable (void);


/*
 *	\brief 任务变为就绪状态
 *  \prame[in]  要恢复为就绪状态的任务的指针
 *  \ret   none
 */
void ls_task_sched_rdy(ls_task_t *p_task);

/*
 *	\brief 任务从就绪状态变为非就绪状态
 *  \prame[in]  要从任务就绪表中删除的任务的任务指针
 *  \ret   none
 */
void ls_task_sched_unrdy(ls_task_t *p_task);


/*
 *	\brief 初始化任务调度
 *  \prame[in]  none
 *  \ret   none
 */
void ls_task_sched_init (void);

/*
 *	\brief 获取当前系统任务个数
 *  \prame[in]  none
 *  \ret   返回当前操作系统中使用的优先级个数
 */
uint32_t ls_get_rtos_task_count (void);

/*
 *	\brief 挂起任务
 *  \prame[in]  p_task  ：任务实体指针
 *  \ret   LS_INCORRECT_STA 当前任务处于延时状态
 *         LS_OK    挂起成功
 */
ls_error_t ls_task_suspend(ls_task_t* p_task);

/*
 *	\brief 恢复任务
 *  \prame[in]  p_task  ：任务实体指针
 *  \ret   none
 */
void ls_task_resume(ls_task_t* p_task);


/*
 *	\brief 强制删除
 */
void ls_task_force_delete(ls_task_t *p_task);

/*
 *	\brief 请求删除
 */
void ls_task_request_delete(ls_task_t *p_task);
/*
 *	\brief 判断是否有请求删除标志
 */
uint32_t ls_check_task_request_flag(ls_task_t *p_task);

/*
 * \brief 删除自己
 */
void ls_task_delete_self (ls_task_t *p_task);

/*
 *	\brief 设置清理函数的回调函数
 */
void ls_task_set_clean_callback(ls_task_t *p_task, void (*p_clean)(void*), void *p_clean_param);

/*
 *	\brief 获取任务的状态
 */
void ls_task_get_info(ls_task_t *p_task, ls_task_info_t *p_info);
#endif

