#include "ls_rtos.h"
#include "ls_timer.h"

static ls_list_t timer_hard_list;
static ls_list_t timer_soft_list;

static ls_sem_t systick_handle_sem;
static ls_sem_t timer_protect_sem;

void ls_timer_init (ls_timer_t *p_timer,
										uint32_t start_delay_ticks,
										uint32_t duration_delay_ticks,
										timer_callback_t callback,
										void *callback_param,
										uint32_t config)
{
	
	ls_node_init(&p_timer->link_node);
	
	p_timer->start_delay_ticks = start_delay_ticks;
	p_timer->duration_delay_ticks = duration_delay_ticks;
	p_timer->timer_callback = callback;
	p_timer->timer_callback_param = callback_param;
	p_timer->config = config;
	
	p_timer->state = LS_TIMER_CREATED;
	
	p_timer->delay_ticks = ((start_delay_ticks > 0) ? start_delay_ticks : duration_delay_ticks);
}

void ls_timer_start (ls_timer_t *p_timer)
{
	switch (p_timer->state) {
		case LS_TIMER_CREATED:
		case LS_TIMER_STOP:
			p_timer->delay_ticks = (p_timer->start_delay_ticks > 0) ? p_timer->start_delay_ticks 
																															: p_timer->duration_delay_ticks ;
		
			p_timer->state = LS_TIMER_STARTED;
		
			if (p_timer->config == LS_TIMER_HARD) {
				ls_task_enter_critical();
				ls_list_insert_node_last(&timer_hard_list, &p_timer->link_node);
				ls_task_exit_critical();
		 } else if (p_timer->config == LS_TIMER_SOFT) {
				ls_sem_take(&timer_protect_sem, 0);
				ls_list_insert_node_last(&timer_soft_list, &p_timer->link_node);
				ls_sem_give(&timer_protect_sem);
		 }
			break;
		default:
			break;
	
	}
}

void ls_timer_stop (ls_timer_t *p_timer)
{
	switch (p_timer->state) {
		case LS_TIMER_STARTED:
		case LS_TIMER_RUNING:
			if (p_timer->config == LS_TIMER_HARD) {
				ls_task_enter_critical();
				ls_list_remove_node(&timer_hard_list, &p_timer->link_node);
				ls_task_exit_critical();
			} else if(p_timer->config == LS_TIMER_SOFT){
				ls_sem_take(&timer_protect_sem, 0);
				ls_list_remove_node(&timer_soft_list, &p_timer->link_node);
				ls_sem_give(&timer_protect_sem);
			}
			
		break;
		default:
			break;
	}
}

static void timer_callback_deal(ls_list_t *p_list)
{
	ls_node_t *temp_node;
	ls_timer_t *timer;
//	for (temp_node = ls_list_first_node(p_list); 
//			temp_node !=&p_list->head_node; 
//			temp_node = temp_node->next_node ) {
	for (temp_node = p_list->head_node.next_node; 
			temp_node !=&p_list->head_node; 
			temp_node = temp_node->next_node ) {
				
				timer = LS_GET_PARENT_STRUCT_ADDR(temp_node, ls_timer_t, link_node);
				
				if ((timer->delay_ticks == 0) || (--timer->delay_ticks == 0)) {
					
					timer->state = LS_TIMER_RUNING;
					
					timer->timer_callback(timer->timer_callback_param);
					
					timer->state = LS_TIMER_STARTED;
					
					if (timer->duration_delay_ticks == 0) {
//						ls_sem_take(&timer_protect_sem, 0);
//						
//						ls_timer_stop(timer);
//						
//						ls_sem_give(&timer_protect_sem);
						
						ls_list_remove_node(p_list, &timer->link_node);
						
						timer->state = LS_TIMER_STOP;
						
					} else {
						timer->delay_ticks = timer->duration_delay_ticks;
					}
					
				}
	}

}

static void soft_timer_task()
{
	for (;;) {
		
		ls_sem_take(&systick_handle_sem, 0);
		ls_sem_take(&timer_protect_sem, 0);
		
		timer_callback_deal(&timer_soft_list);
		
		ls_sem_give(&timer_protect_sem);
	}
}

void systick_handle_timer_deal()
{
	ls_task_enter_critical();
	
	timer_callback_deal(&timer_hard_list);
	
	ls_task_exit_critical();
	
	ls_sem_give(&systick_handle_sem);
}

static ls_task_t task_timer;
static ls_stack_t task_stack[LS_TIMER_TASK_STACK];


void ls_timer_module_init ()
{
	ls_task_enter_critical();
	ls_list_init(&timer_hard_list);
	ls_list_init(&timer_soft_list);
	
	/* 初始化为有一个信号量，最多也只有一个  */
	ls_sem_init(&timer_protect_sem, 1, 1);
	
	/* 初始化为0个信号量，最多有0xffffffff个信号量 */
	ls_sem_init(&systick_handle_sem, 0, 0);
	
	ls_task_exit_critical();
}

/* 初始化定时器的任务 */
void ls_timer_task_init()
{
	
	ls_task_enter_critical();
#if (LS_TIMER_TASK_PRIORITY >= LS_TASK_COUNT - 1)
#error the timer task priority must grater than (LS_TASK_COUNT - 1)!
#endif
	ls_task_init(&task_timer, task_stack, sizeof(task_stack), LS_TIMER_TASK_PRIORITY, soft_timer_task, (void*)0);
	
	ls_task_exit_critical();
}


void ls_timer_get_info(ls_timer_t *p_timer, ls_timer_info_t *p_info)
{
	ls_task_enter_critical();
	
	p_info->start_delay_ticks = p_timer->start_delay_ticks;
	p_info->duration_delay_ticks = p_timer->duration_delay_ticks;
	p_info->delay_ticks = p_timer->delay_ticks;
	p_info->timer_callback = p_timer->timer_callback;
	p_info->timer_callback_param = p_timer->timer_callback_param;
	p_info->config = p_timer->config;
	p_info->state = p_timer->state;
	
	ls_task_exit_critical();
}

void ls_timer_delete(ls_timer_t *p_timer)
{
	ls_timer_stop(p_timer);
	
	p_timer->state = LS_TIMER_DELETED;
}
