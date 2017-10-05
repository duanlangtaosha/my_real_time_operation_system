#ifndef __LS_RTOS_H
#define __LS_RTOS_H

#include "stdint.h"

//#include "ls_task.h"
//#include "ls_list.h"

/** \brief RTOS支持的最大的任务的数量 */
#define LS_TASK_COUNT      32

/** \brief 每个任务执行的最大时间片   */
#define TASK_TIME_SLICE_MAX     10

/**< \brief 当前实时操作系统中任务总数 */
extern uint32_t g_rtos_task_count;

#endif

