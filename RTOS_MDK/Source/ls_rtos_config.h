#ifndef __LS_RTOS_CONFIG_H
#define __LS_RTOS_CONFIG_H

/** \brief RTOS֧�ֵ�������������� */
#define LS_TASK_COUNT             32

/** \brief ÿ������ִ�е����ʱ��Ƭ   */
#define TASK_TIME_SLICE_MAX       10

#define LS_TIMER_TASK_PRIORITY   (1UL)
#define LS_TIMER_TASK_STACK      (1024UL)

#define LS_IDEL_TASK_PRIORITY    (LS_TASK_COUNT - 1)


/******************************************************************************************
*			�ں˲ü�
*******************************************************************************************/

#define LS_ENABLE_SEM            (0)
#define LS_ENABLE_MUTEX					 (0)
#define LS_ENABLE_FLAG_GROUP     (0)
#define LS_ENABLE_MEM_BLOCK      (0)
#define LS_ENABLE_TIMER          (0)
#define LS_ENABLE_MBOX           (0)
#define LS_ENABLE_CPU_USAGE      (0)
#define LS_ENABLE_HOOKS          (1)

#endif

