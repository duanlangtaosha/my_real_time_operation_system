#include "ls_rtos.h"
#include "ls_flag_group.h"


void ls_flag_group_init(ls_flag_group_t *p_flag_group, uint32_t flag)
{
	ls_event_init(&p_flag_group->event, ls_event_type_flag_group);
	p_flag_group->flag = flag;
}

/*
 *	\brief 标志检测和消耗标志
 */
static ls_error_t flag_check_and_consume(ls_flag_group_t *p_flag_group,
																				 uint32_t wait_type,
																				 uint32_t *flag)
{
	uint32_t is_all = (wait_type & LS_FLAG_ALL);
	uint32_t rsc_flag = *flag;
	
	uint32_t is_set = (wait_type & LS_FLAG_SET);
	
	uint32_t cal_flag = is_set ? (p_flag_group->flag & rsc_flag) : (~p_flag_group->flag & rsc_flag);
	
	if (((is_all == LS_FLAG_ALL) && (cal_flag == rsc_flag)) || 
			((is_all != LS_FLAG_ALL) && (cal_flag != 0))) {
				
		if (wait_type & LS_FLAG_CONSUME) {
			if (is_set) {
				p_flag_group->flag &= ~rsc_flag;
			} else {
				p_flag_group->flag |= rsc_flag;
			}
		}
		*flag = cal_flag;
		return event_no_error;
	}
	
	*flag = cal_flag;
	return event_resource_unavailable_error;
	
}

/*
 *	\brief 获取事件标志
 */
uint32_t ls_flag_group_take(ls_flag_group_t *p_flag_group,
														uint32_t wait_type,
														uint32_t request_flag,
														uint32_t *result_flag,
														uint32_t timeout)
{
	uint32_t result;
	uint32_t error_flag = 0;
	uint32_t flag = request_flag;
	
	ls_task_enter_critical();
	
	error_flag = flag_check_and_consume(p_flag_group, wait_type, &flag);
	
	/* 没有等着相应的事件标志 */
	if (error_flag != event_no_error) {
		
		current_task->flag_group_wait_type = wait_type;
		current_task->flag_group_flag = request_flag;
		ls_event_wait(current_task, &p_flag_group->event, (void*)0, ls_event_type_flag_group, timeout);
		
		ls_task_exit_critical();
		
		ls_task_schedule();
		
		*result_flag = current_task->flag_group_flag;
		result = current_task->event_result;
		
	} else {
		*result_flag = flag;
		ls_task_exit_critical();
	}
	
	return result;
}


/*
 *	\brief 获取事件标志
 */
uint32_t ls_flag_group_get_flag(ls_flag_group_t *p_flag_group,
																uint32_t wait_type,
																uint32_t request_flag,
																uint32_t *result_flag)
{

	uint32_t error_flag = 0;
	uint32_t flag = request_flag;
	
	ls_task_enter_critical();
	
	error_flag = flag_check_and_consume(p_flag_group, wait_type, &flag);
	
	ls_task_exit_critical();
	
	*result_flag = flag;
	
	return error_flag;
}

/*
 *	\brief 发送一个事件标志
 */
void ls_flag_group_give(ls_flag_group_t *p_flag_group, uint32_t is_set, uint32_t flag)
{
	ls_list_t *waitlist;
	ls_node_t *node;
	ls_node_t *next_node;
	uint32_t sched = 0;
	
	ls_task_enter_critical();
	
	if (is_set) {
		p_flag_group->flag |= flag;
	} else {
		p_flag_group->flag &= ~flag;
	}
	
	waitlist = &p_flag_group->event.event_list;
	
	for (node = waitlist->LS_LIST_FIRST_NODE; node != &waitlist->head_node; node = next_node) {
//		ls_task_t *task = LS_GET_PARENT_STRUCT_ADDR(&node, ls_task_t, task_time_slice_node);
		ls_task_t *task = LS_GET_PARENT_STRUCT_ADDR(node, ls_task_t, task_time_slice_node);
		uint32_t flag = task->flag_group_flag;
		uint32_t error_flag = flag_check_and_consume(p_flag_group, task->flag_group_wait_type, &flag);
		
		next_node = node->next_node;
		
		if (error_flag == event_no_error) {
			task->flag_group_flag = flag;
			ls_event_wakeup_task(&p_flag_group->event, task, (void*)0, event_no_error);
			sched = 1;
		}
		
	}
	
	if (sched) {
		ls_task_exit_critical();
		ls_task_schedule();
		return;
	}
	
	ls_task_exit_critical();
}

/*
 *	\brief 获取事件标志组的信息
 */
void ls_flag_group_get_info(ls_flag_group_t *p_flag_group, ls_flag_group_info_t *p_flag_group_info)
{
	ls_task_enter_critical();
	
	p_flag_group_info->flag = p_flag_group->flag;
	p_flag_group_info->task_count = ls_event_wait_count(&p_flag_group->event);
	
	ls_task_exit_critical();
}

/*
 *	\brief 删除事件标志组
 */
uint32_t ls_flag_group_delete(ls_flag_group_t *p_flag_group)
{
	uint32_t count = 0;
	ls_task_enter_critical();
	count = ls_event_remove_all(&p_flag_group->event, (void*)0, event_deleted);
	ls_task_exit_critical();
	
	if (count > 0) {
		ls_task_schedule();
	}
	
	return count;
}

