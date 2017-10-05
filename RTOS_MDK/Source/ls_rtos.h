#ifndef __LS_RTOS_H
#define __LS_RTOS_H

#include "stdint.h"

//#include "ls_task.h"
//#include "ls_list.h"

/** \brief RTOS֧�ֵ�������������� */
#define LS_TASK_COUNT      32

/** \brief ÿ������ִ�е����ʱ��Ƭ   */
#define TASK_TIME_SLICE_MAX     10

/**< \brief ��ǰʵʱ����ϵͳ���������� */
extern uint32_t g_rtos_task_count;

#endif

