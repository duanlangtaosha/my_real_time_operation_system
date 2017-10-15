#ifndef __LS_TASK_H
#define __LS_TASK_H

#include "stdint.h"
#include "ls_list.h"
#include "ls_error.h"


#define LS_TASK_RDY			(1 << 0)
#define LS_TASK_DELAY   (1 << 1)
#define LS_TASK_SUSPEND (1 << 2)
#define LS_OWNER_ERROR  (1 << 3)
#define LS_TASK_WAIT_MASK                   (0xFF << 16)

struct __ls_event;

typedef struct __ls_task {

	/** \brief ����Ķ�ջָ�� */
	uint32_t *p_stack;
	
	/** \brief ��ջ����ʼ��ַ������Ŀ�ʼλ�ã� */
	uint32_t *stack_base;
	
	/** \brief ��ջ�Ĵ�С */
	uint32_t stack_size;
	
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
	
	/** \brief ��¼����Ĺ�������� */
	uint32_t ls_task_suspend_count;
	
	/** \brief ����ɾ������ */
	void (*clean) (void*);
	
	/** \breif ����ɾ�������Ĳ��� */
	void *clean_param;
	
	/** \brief ����ɾ����־ */
	uint32_t request_delete_flag;
	
	/** \brief �¼� */
	struct __ls_event *event;
	
	/** \brief �¼����صĽ�� */
	uint32_t event_result;
	
	/** \brief �¼���Ϣ */
	void *event_msg;
	
	/** \brief �¼���־��ȴ����� */
	uint32_t flag_group_wait_type;
	
	/** \brief �¼���־��ȴ���־ */
	uint32_t flag_group_flag;
	
}ls_task_t;


typedef struct __ls_task_info {
	uint32_t task_pro;
	uint32_t stack_size;
	
	/* ���еĵ�ջ */
	uint32_t stack_free;
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
 *	\brief ��ʼ������
 *  \prame[in]  p_task       �������ʵ��ָ��
 *  \prame[in]  p_task_stack : �����ջ
 *  \prame[in]  prio         ���������ȼ�
 *  \prame[in]  func_entry   : �������ں���
 *  \prame[in]  p_param      : ������ں����Ĳ���
 *  \ret   none
 */
//void ls_task_init(ls_task_t *p_task, ls_stack_t * p_task_stack, uint8_t prio, void* func_entry, void *p_param);
/*
 *	�����ʼ��
 */
void ls_task_init(ls_task_t *p_task, ls_stack_t * p_task_stack, uint32_t stack_size, uint8_t prio, void* func_entry, void *p_param);

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

/*
 *	\brief ��������
 *  \prame[in]  p_task  ������ʵ��ָ��
 *  \ret   LS_INCORRECT_STA ��ǰ��������ʱ״̬
 *         LS_OK    ����ɹ�
 */
ls_error_t ls_task_suspend(ls_task_t* p_task);

/*
 *	\brief �ָ�����
 *  \prame[in]  p_task  ������ʵ��ָ��
 *  \ret   none
 */
void ls_task_resume(ls_task_t* p_task);


/*
 *	\brief ǿ��ɾ��
 */
void ls_task_force_delete(ls_task_t *p_task);

/*
 *	\brief ����ɾ��
 */
void ls_task_request_delete(ls_task_t *p_task);
/*
 *	\brief �ж��Ƿ�������ɾ����־
 */
uint32_t ls_check_task_request_flag(ls_task_t *p_task);

/*
 * \brief ɾ���Լ�
 */
void ls_task_delete_self (ls_task_t *p_task);

/*
 *	\brief �����������Ļص�����
 */
void ls_task_set_clean_callback(ls_task_t *p_task, void (*p_clean)(void*), void *p_clean_param);

/*
 *	\brief ��ȡ�����״̬
 */
void ls_task_get_info(ls_task_t *p_task, ls_task_info_t *p_info);

/*
 *	�Ӿ��������Ƴ�
 */
void ls_task_sched_remove(ls_task_t *p_task);
#endif

