#ifndef __LS_MUTEX_H
#define __LS_MUTEX_H

typedef struct __ls_mutex {
	
	/* �¼����ƿ� */
	ls_event_t event;
	
	/* ��ס�Ĵ��� */
	uint32_t lock_count;
	
	/* ��ǰ��ס������������� */
	ls_task_t* owner_task;
	
	/* ԭʼ���ȼ� */
	uint32_t origin_prio;
}ls_mutex_t;

typedef struct __ls_mutex_info {
	/* �ȴ���������� */
	uint32_t task_count;
	
	/* �����ߵ����ȼ� */
	uint32_t owner_prio;
	
	/* �̳����ȼ� */
	uint32_t inherited_prio;
	
	/* ���������� */
	ls_task_t *owner_task;
	
	/* ��ǰ�����Ĵ��� */
	uint32_t lock_count;
}ls_mutex_info_t;


#include "ls_rtos.h"

void ls_mutex_init (ls_mutex_t *p_mutex);

ls_error_t ls_mutex_take (ls_mutex_t *p_mutex, uint32_t timeout);

ls_error_t ls_mutex_give (ls_mutex_t *p_mutex);

/*
 *	\brief ɾ���ź���
 */
uint32_t ls_mutex_delete (ls_mutex_t *p_mutex);

/*
 *	\brief ��ȡ��ǰ�����ź�������Ϣ 
 */
void ls_mutex_get_info (ls_mutex_t *p_mutex, ls_mutex_info_t *info);


#endif

