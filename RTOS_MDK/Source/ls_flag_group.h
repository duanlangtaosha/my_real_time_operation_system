#ifndef __LS_FLAG_GROUP_H
#define __LS_FLAG_GROUP_H

#define LS_FLAG_CLEAR               (0x0 << 0)
#define LS_FLAG_SET                 (0x1 << 0)
#define LS_FLAG_ANY									(0x0 << 1)
#define LS_FLAG_ALL									(0x1 << 1)

#define LS_FLAG_SET_ALL             (LS_FLAG_SET | LS_FLAG_ALL)
#define LS_FLAG_SET_ANY             (LS_FLAG_SET | LS_FLAG_ANY)
#define LS_FLAG_CLEAR_ALL           (LS_FLAG_CLEAR | LS_FLAG_ALL)
#define LS_FLAG_CLEAR_ANY           (LS_FLAG_CLEAR | LS_FLAG_ANY)

#define LS_FLAG_CONSUME             (1 << 2)

typedef struct __ls_flag_group {

	ls_event_t event;
	uint32_t flag;
}ls_flag_group_t;

typedef struct ___ls_flag_group_info {
	uint32_t flag;
	uint32_t task_count;
}ls_flag_group_info_t;

void ls_flag_group_init(ls_flag_group_t *p_flag_group, uint32_t flag);

/*
 *	\brief 获取事件标志
 */
uint32_t ls_flag_group_take(ls_flag_group_t *p_flag_group,
														uint32_t wait_type,
														uint32_t request_flag,
														uint32_t *result_flag,
														uint32_t timeout);

/*
 *	\brief 获取事件标志
 */
uint32_t ls_flag_group_get_flag(ls_flag_group_t *p_flag_group,
																uint32_t wait_type,
																uint32_t request_flag,
																uint32_t *result_flag);

/*
 *	\brief 发送一个事件标志
 */
void ls_flag_group_give(ls_flag_group_t *p_flag_group, uint32_t is_set, uint32_t flag);

/*
 *	\brief 获取事件标志组的信息
 */
void ls_flag_group_get_info(ls_flag_group_t *p_flag_group, ls_flag_group_info_t *p_flag_group_info);

/*
 *	\brief 删除事件标志组
 */
uint32_t ls_flag_group_delete(ls_flag_group_t *p_flag_group);

#endif

