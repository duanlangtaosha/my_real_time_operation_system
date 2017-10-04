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

	/** \brief ����Ķ�ջָ�� */
	uint32_t *p_stack;
	
	/** \brief �������ʱtick��  */
	uint32_t  task_delay_ticks;
	
	/** \brief ������ʱ���нڵ� */
	ls_node_t task_delay_node;
	
	/** \brief �������ȼ� */
	uint32_t task_pro;
	
	/** \brief �����״̬ */
	uint32_t task_state;
	
	/** \brief ��ͬ���ȼ���������£���������ռʱ��Ƭ������ */
	uint32_t task_slice;
	
	/** \brief ����ʱ��Ƭ�ڵ� */
	ls_node_t task_time_slice_node;
	
//	/** \brief ��¼��ϵͳ����ĵ�ǰ����ڵ� */
//	ls_node_t task_myself_node;
	
	/** \brief ��¼����Ĺ�������� */
	uint32_t ls_task_suspend_count;
	
}ls_task_t;

typedef uint32_t ls_stack_t;
extern ls_task_t *current_task;
extern ls_task_t *next_task;

extern ls_list_t task_table[LS_TASK_COUNT];

///**< \brief ��¼��ǰϵͳ�д��ڵ����� */
//extern ls_list_t ls_rtos_task_list;


void ls_task_init(ls_task_t *p_task, ls_stack_t * p_task_stack, uint8_t prio, void* func_entry, void *p_param);

/*
 *	\brief ��ȡ��ǰ��������������ȼ�������
 *  \prame[in]  none
 *  \ret   ����������ȼ����������ָ��
 */
ls_task_t* ls_task_high_redy(void);

/*
 *	\brief �������ʧ��
 *  \prame[in]  none
 *  \ret   none
 */
void ls_task_schedule_disable (void);

/*
 *	\brief �������ʹ��
 *  \prame[in]  none
 *  \ret   none
 */
void ls_task_schedule_enable (void);


/*
 *	\brief �����Ϊ����״̬
 *  \prame[in]  Ҫ�ָ�Ϊ����״̬�������ָ��
 *  \ret   none
 */
void ls_task_sched_rdy(ls_task_t *p_task);

/*
 *	\brief ����Ӿ���״̬��Ϊ�Ǿ���״̬
 *  \prame[in]  Ҫ�������������ɾ�������������ָ��
 *  \ret   none
 */
void ls_task_sched_unrdy(ls_task_t *p_task);


/*
 *	\brief ��ʼ���������
 *  \prame[in]  none
 *  \ret   none
 */
void ls_task_sched_init (void);

/*
 *	\brief ��ȡ��ǰϵͳ�������
 *  \prame[in]  none
 *  \ret   ���ص�ǰ����ϵͳ��ʹ�õ����ȼ�����
 */
uint32_t ls_get_rtos_task_count (void);

///*
// *	\brief ��ʼ��ϵͳ����ͳ������
// *  \prame[in]  none
// *  \ret   none
// */
//void ls_rtos_task_list_init (void);


ls_error_t ls_task_suspend(ls_task_t* p_task);

void ls_task_resume(ls_task_t* p_task);

#endif

