#ifndef __LS_DELAY_H
#define __LS_DELAY_H


void tSetSysTickPeriod(uint32_t ms);
void ls_init_delay_list (void);
void ls_delayms(uint32_t systick);

/*
 *	����ʱ�������Ƴ�
 */
void ls_task_time_remove(ls_task_t *p_task);

/*
 *	\brief �������ȴ�״̬
 */
void ls_task_timer_wait(ls_task_t *p_task, uint32_t delay_ticks);

/*
 *	\brief ���񱻴ӵȴ�̬�л��� 
 */
void ls_task_timer_weakup(ls_task_t *p_task);

/*
 *	����ʱ�������Ƴ�
 */
void ls_task_time_remove(ls_task_t *p_task);



#endif
