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
 *	\brief 初始化邮箱
 */
void ls_mbox_init(ls_mbox_t *p_mbox, void** pp_buf, uint32_t max_count);

/*
 *	\brief 发送消息, (消息缓存区，read索引是指向即将要读的，
 *         write是指向下一个要写的位置，read是有数据write指向的是没有数据)
 */
ls_error_t ls_mbox_send_msg (ls_mbox_t *p_mbox, void* p_msg, uint32_t send_option);

/*
 *	\brief 获取消息
 */
ls_error_t ls_mbox_recieve_msg(ls_mbox_t *p_mbox, void** msg, uint32_t timeout);

/*
 *	\brief 获取消息无等待
 */
ls_error_t ls_mbox_recieve_msg_nowait(ls_mbox_t *p_mbox, void** msg);

/*
 *	\brief 清空邮箱
 */
void ls_mbox_flush (ls_mbox_t *p_mbox);

/*
 *	\brief 删除邮箱
 */
uint32_t ls_mbox_delete (ls_mbox_t *p_mbox);


#endif

