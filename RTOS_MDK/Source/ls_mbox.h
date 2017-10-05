#ifndef __LS_MSG_H
#define __LS_MSG_H

#include "stdint.h"

#define LS_MSG_NORMAL    (0)
#define LS_MSG_URGENCY   (1)

typedef struct __ls_mbox {

	ls_event_t event;
	
	uint32_t read_index;
	
	uint32_t write_index;
	
	void **buffer;
	
	uint32_t msg_count;
	
	uint32_t msg_max_count;
}ls_mbox_t;

/*
 *	\brief ��ʼ������
 */
void ls_mbox_init(ls_mbox_t *p_mbox, void** pp_buf, uint32_t max_count);

/*
 *	\brief ������Ϣ, (��Ϣ��������read������ָ�򼴽�Ҫ���ģ�
 *         write��ָ����һ��Ҫд��λ�ã�read��������writeָ�����û������)
 */
ls_error_t ls_mbox_send_msg (ls_mbox_t *p_mbox, void* p_msg, uint32_t send_option);

/*
 *	\brief ��ȡ��Ϣ
 */
ls_error_t ls_mbox_recieve_msg(ls_mbox_t *p_mbox, void** msg, uint32_t timeout);

/*
 *	\brief ��ȡ��Ϣ�޵ȴ�
 */
ls_error_t ls_mbox_recieve_msg_nowait(ls_mbox_t *p_mbox, void** msg);

/*
 *	\brief �������
 */
void ls_mbox_flush (ls_mbox_t *p_mbox);

/*
 *	\brief ɾ������
 */
uint32_t ls_mbox_delete (ls_mbox_t *p_mbox);


#endif

