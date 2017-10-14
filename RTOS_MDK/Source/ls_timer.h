#ifndef __LS_TIMER_H
#define __LS_TIMER_H

#define LS_TIMER_HARD   (0 < 0)
#define LS_TIMER_SOFT		(1 < 0)

typedef enum __ls_timer_state {

	LS_TIMER_CREATED,
	LS_TIMER_STARTED,
	LS_TIMER_RUNING,
	LS_TIMER_STOP,
	LS_TIMER_DELETED
}ls_timer_state_t;

typedef void (*timer_callback_t)(void *param);

typedef struct __ls_timer {

	ls_node_t link_node;
	uint32_t  start_delay_ticks;
	uint32_t  duration_delay_ticks;
	
	/* 实际的延时tick数  */
	uint32_t delay_ticks;
	
	timer_callback_t timer_callback;
	
	void * timer_callback_param;
	
	uint32_t config;
	
	ls_timer_state_t state;
	
}ls_timer_t;

/* 初始化定时器 */
void ls_timer_init (ls_timer_t *p_timer,
										uint32_t start_delay_ticks,
										uint32_t duration_delay_ticks,
										timer_callback_t callback,
										void *callback_param,
										uint32_t config);
										
										
/* 开启定时器 */
void ls_timer_start (ls_timer_t *p_timer);
										
/* 停止定时器 */
void ls_timer_stop (ls_timer_t *p_timer);

/* 滴答定时器中处理定时器的函数, 放在滴答定时器中断中 */
void systick_handle_timer_deal(void);

/* 定时器模块的初始化 */
void ls_timer_module_init (void);

#endif



