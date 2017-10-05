#ifndef  __LS_SEM_H
#define  __LS_SEM_H

typedef struct __ls_sem {

	ls_event_t event;
	uint32_t   count;
	uint32_t   max_count;
	
}ls_sem_t;

typedef struct __ls_sem_info {

	uint32_t count;
	uint32_t max_count;
	uint32_t task_count;
	
}ls_sem_info_t;


/*
 *	\brief ��ʼ���ź���
 */
void ls_sem_init(ls_sem_t *p_sem, uint32_t init_count, uint32_t max_count);
/*
 *	\brief ��ȡ�ź���
 */
uint32_t ls_sem_take(ls_sem_t *p_sem, uint32_t timeout);

/*
 *	\brief �޵ȴ���ȡ�ź���
 */
uint32_t ls_sem_take_no_wait(ls_sem_t *p_sem);
/*
 *	\brief �ͷ��ź���
 */
void ls_sem_give (ls_sem_t *p_sem);


/*
 *	\brief ��ȡ�ź�������Ϣ
 */
void ls_sem_get_info(ls_sem_t *p_sem, ls_sem_info_t *info);

/*
 * \brief ɾ���ź���
 */
void ls_sem_delete(ls_sem_t *p_sem);



#endif

