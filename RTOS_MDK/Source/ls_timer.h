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
	
	/* ʵ�ʵ���ʱtick��  */
	uint32_t delay_ticks;
	
	timer_callback_t timer_callback;
	
	void * timer_callback_param;
	
	uint32_t config;
	
	ls_timer_state_t state;
	
}ls_timer_t;

/* ��ʼ����ʱ�� */
void ls_timer_init (ls_timer_t *p_timer,
										uint32_t start_delay_ticks,
										uint32_t duration_delay_ticks,
										timer_callback_t callback,
										void *callback_param,
										uint32_t config);
										
										
/* ������ʱ�� */
void ls_timer_start (ls_timer_t *p_timer);
										
/* ֹͣ��ʱ�� */
void ls_timer_stop (ls_timer_t *p_timer);

/* �δ�ʱ���д���ʱ���ĺ���, ���ڵδ�ʱ���ж��� */
void systick_handle_timer_deal(void);

/* ��ʱ��ģ��ĳ�ʼ�� */
void ls_timer_module_init (void);

#endif



