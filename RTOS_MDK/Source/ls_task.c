
#include "ls_rtos.h"
#include <string.h>

#if (LS_ENABLE_CPU_USAGE == 1)
 uint32_t idel_count = 0;
 uint32_t idel_count_max = 0;
 uint32_t systick_count = 0;
 uint32_t first_systick = 0;
 uint32_t start_check_cpu_usage = 0;
static float cpu_usage = 0.0;
#endif


/**< \brief ��������������ȴ�����ʼ�� */
uint8_t  task_schedule_lock = 0;

/**< \brief ��ǰ�����ָ�� */
ls_task_t *current_task;

/**< \brief ��Ҫִ�е���һ�������ָ�� */
ls_task_t *next_task;

/**< \brief ���������  */
//ls_task_t *task_list[LS_TASK_COUNT];

/**< \brief ����ʱ��Ƭ�ڵ� */
ls_list_t task_table[LS_TASK_COUNT];

extern ls_bitmap g_bit_map;



/*
 *	�����ʼ��
 */
void ls_task_init(ls_task_t *p_task, ls_stack_t * p_task_stack, uint32_t stack_size, uint8_t prio, void* func_entry, void *p_param)
{
	ls_stack_t *stack_top;
	p_task->stack_base = p_task_stack;
	p_task->stack_size = stack_size;
	
	memset(p_task_stack, 0, stack_size);
	
	stack_top = p_task_stack + stack_size / sizeof(ls_stack_t);
	*(--stack_top) = (uint32_t)(1 << 24);
	*(--stack_top) = (uint32_t)func_entry;
	*(--stack_top) = (uint32_t)14;		     /* R14(LR)*/
	*(--stack_top) = (uint32_t)12;		     /* R14 */
	*(--stack_top) = (uint32_t)3;         /* R3 */
	*(--stack_top) = (uint32_t)2;         /* R2 */
	*(--stack_top) = (uint32_t)1;         /* R1 */
	*(--stack_top) = (uint32_t)p_param;   /* R0 */
	*(--stack_top) = (uint32_t)11;        /* R11 */
	*(--stack_top) = (uint32_t)10;        /* R10 */
	*(--stack_top) = (uint32_t)9;         /* R9 */
	*(--stack_top) = (uint32_t)8;         /* R8 */
	*(--stack_top) = (uint32_t)7;         /* R7 */
	*(--stack_top) = (uint32_t)6;         /* R6 */
	*(--stack_top) = (uint32_t)5;         /* R5 */
	*(--stack_top) = (uint32_t)4;         /* R4 */
	
	p_task->task_pro = prio;
	p_task->p_stack = stack_top;
	p_task->task_delay_ticks = 0;

	ls_bitmap_set(&g_bit_map, prio);
	
	p_task->ls_task_suspend_count = 0;
	
	p_task->task_state = LS_TASK_RDY;
	
	/* ��ʼ����ʱ�б�Ľڵ㣬��ʵ����ʼ��Ҳ����, ���Ǵ���һ�����⣬������һ��ǰָ��ͺ�
	 * ��ָ�벻��ָ�����Լ������Ժ�Ĳ����п����������⡣
	 */
	ls_node_init(&p_task->task_delay_node);
	
	/* ��ʼ����������ʱ��Ƭ */
	p_task->task_slice = TASK_TIME_SLICE_MAX;
	
	/* ��ʼ������ʱ��Ƭ�ڵ� */
	ls_node_init(&p_task->task_time_slice_node);
	
	/* ��ʼ���������� */
	ls_list_insert_node_last(&task_table[prio], &p_task->task_time_slice_node);
	
	/* ��ʼ��cleanָ�� */
	p_task->clean = (void *)0;
	
	/* ��ʼ��clean�����Ĳ��� */
	p_task->clean_param = (void *)0;
	
	/* ����ɾ����־λ��0 */
	p_task->request_delete_flag = 0;
	
	/* �¼���Ϣ���»�Ϊ�� */
	p_task->event_msg = (void*)0;
	
#if (LS_ENABLE_HOOKS == 1)
	ls_task_init_hooks(p_task);
#endif
	
}


/*
 *	��ȡ��ǰ���ȼ��õ����������ָ��
 */
ls_task_t* ls_task_high_redy(void)
{
	/* ��ȡ�������е�������ȼ� */
	uint32_t temp = ls_get_bitmap_high_prio(&g_bit_map);
	
	/* ��ȡ��ǰ������ȼ������������׸��ڵ�(������ʱ��Ƭִ�������Զ��İѵ�ǰ�ڵ���뵽β�ڵ���) */
	ls_node_t *node = ls_list_first_node(&task_table[temp]);
	
	/* ͨ����ǰ�ڵ��ȡ��ǰ���ȼ���ߵ����� */
	ls_task_t *task_high = LS_GET_PARENT_STRUCT_ADDR(node, ls_task_t, task_time_slice_node);
	
	return task_high;
}


/*
 *	\brief ��ֹ�������,�����������
 */
void ls_task_schedule_disable (void)
{
	if (task_schedule_lock < 255) {
		task_schedule_lock ++;
	} 
}


/*
 *	\brief ʹ��������ȣ�ȡ�����������
 */
void ls_task_schedule_enable (void)
{
	if (task_schedule_lock > 0) {
		task_schedule_lock--;
	}
}

/*
 *	��������Ϊ����̬
 */
void ls_task_sched_rdy(ls_task_t *p_task)
{
//	ls_task_enter_critical();
	
	/* �ѽڵ����¼����������ȼ������� */
	ls_list_insert_node_last(&task_table[p_task->task_pro], &p_task->task_time_slice_node);
	
	ls_bitmap_set(&g_bit_map, p_task->task_pro);
	
//	ls_task_exit_critical();
}


/*
 *	��������Ǿ���̬
 */
void ls_task_sched_unrdy(ls_task_t *p_task)
{
//	ls_task_enter_critical();
	
	ls_list_remove_node(&task_table[p_task->task_pro], &p_task->task_time_slice_node);
	
	if (ls_list_get_node_count(&task_table[p_task->task_pro]) == 0) {
	
		ls_bitmap_clr(&g_bit_map, p_task->task_pro);
	}
	
//	ls_task_exit_critical();
}

/*
 *	�Ӿ��������Ƴ�
 */
void ls_task_sched_remove(ls_task_t *p_task)
{
	ls_task_enter_critical();

	ls_list_remove_node(&task_table[p_task->task_pro], &p_task->task_time_slice_node);

	if (ls_list_get_node_count(&task_table[p_task->task_pro]) == 0) {

		ls_bitmap_clr(&g_bit_map, p_task->task_pro);
	}

	ls_task_exit_critical();
}


/*
 *	��ʼ���������
 */
void ls_task_sched_init (void) 
{
	uint32_t i;
	
	task_schedule_lock = 0;
	
	ls_bitmap_init(&g_bit_map);
	
	/* 
	 * ����֧�ֵ����е���������ȼ���ע���ʼ��Ϊ����
	 */
	for (i = 0; i < LS_TASK_COUNT; i++) {
	
		ls_list_init(&task_table[i]);
	}	
}

/*
 *	\brief �������
 */
ls_error_t ls_task_suspend(ls_task_t* p_task)
{
	ls_task_enter_critical();
	
	if (p_task->task_state & LS_TASK_DELAY) {
		return -LS_INCORRECT_STA;
	}
	
	if (++(p_task->ls_task_suspend_count) <= 1) {
		p_task->task_state |= LS_TASK_SUSPEND;
		ls_task_sched_unrdy(p_task);
	}
	
	ls_task_exit_critical();
	
	if (current_task == p_task) {
		ls_task_schedule();
	}
	
	return LS_OK;
}

/*
 *	\brief ����ָ�
 */
void ls_task_resume(ls_task_t* p_task)
{
	
	ls_task_enter_critical();
	
	if (p_task->task_state & LS_TASK_SUSPEND) {
		
		if (--(p_task->ls_task_suspend_count) == 0) {

			ls_task_sched_rdy(p_task);
			p_task->task_state &= ~LS_TASK_SUSPEND;

			if (p_task->task_pro < current_task->task_pro) {
				ls_task_schedule();
			}
		}
  }
	
	ls_task_exit_critical();
}

/*
 *	\brief ǿ��ɾ��
 */
void ls_task_force_delete(ls_task_t *p_task)
{
	ls_task_enter_critical();
	
	/* �����������ʱ״̬ */
	if (p_task->task_state & LS_TASK_DELAY) {
		ls_task_time_remove(p_task);
		
		/* ��������ڹ���״̬���������ھ�����������״̬ */
	} else if (!(p_task->task_state & LS_TASK_SUSPEND)) {
		ls_task_sched_remove(p_task);
	}
	
	/* �����������Ϊ�� */
	if (p_task->clean) {
		p_task->clean(p_task->clean_param);
	}
	
	/* ���ɾ�������Լ���ֱ���л� */
	if (p_task == current_task) {
	
		ls_task_schedule();
	}
	
	ls_task_exit_critical();
}

/*
 *	\brief ����ɾ��
 */
void ls_task_request_delete(ls_task_t *p_task)
{
	p_task->request_delete_flag = 1;
}

/*
 *	\brief �ж��Ƿ�������ɾ����־
 */
uint32_t ls_check_task_request_flag(ls_task_t *p_task)
{
	return p_task->request_delete_flag;

}

/*
 * \brief ɾ���Լ�
 */
void ls_task_delete_self (ls_task_t *p_task)
{
	ls_task_enter_critical();
	
	/* ��Ϊ��ɾ���Լ�������˵�Լ�Ӧ��������̬ */
	ls_task_sched_remove(p_task);
	
	if (p_task->clean) {
		p_task->clean(p_task->clean_param);
	}
	
	ls_task_schedule();
	
	ls_task_exit_critical();
	
}

/*
 *	\brief �����������Ļص�����
 */
void ls_task_set_clean_callback(ls_task_t *p_task, void (*p_clean)(void*), void *p_clean_param)
{
	ls_task_enter_critical();
	
	p_task->clean = p_clean;
	p_task->clean_param = p_clean_param;
	
	
	ls_task_exit_critical();

}


/*
 *	\brief ��ȡ�����״̬
 */
void ls_task_get_info(ls_task_t *p_task, ls_task_info_t *p_info)
{
	ls_stack_t *stack_top;
	ls_task_enter_critical();
	
	p_info->task_pro = p_task->task_pro;
	p_info->task_state = p_task->task_state;
	p_info->task_slice = p_task->task_slice;
	p_info->task_delay_ticks = p_task->task_delay_ticks;
	p_info->task_suspend_count = p_task->ls_task_suspend_count;
	p_info->stack_size = p_task->stack_size;
	
	p_info->stack_free = 0;
	stack_top = p_task->stack_base;
	while ((*stack_top++ == 0) && stack_top <= p_task->stack_base + p_task->stack_size / sizeof(ls_stack_t)) {
		p_info->stack_free++;
	}
	
	p_info->stack_free *= sizeof(ls_stack_t);
	ls_task_exit_critical();
}
#if (LS_ENABLE_CPU_USAGE == 1)
static void wait_asys_systick(void);
#endif

extern void ls_init_app(void);
int flag5  = 0;
static void task_idle_func()
{

#if (LS_ENABLE_TIMER == 1)
	
	/* ��ʼ����ʱ������ */
	ls_timer_task_init();
#endif
	
	ls_init_app();
	
	tSetSysTickPeriod(LS_RTOS_SYSTICK_PERIOD);

#if (LS_ENABLE_CPU_USAGE == 1)
	/* �ȴ�ʱ��ͬ�� */
	wait_asys_systick();
#endif
	
	for (; ;) {
//	flag5 = !flag5;
#if (LS_ENABLE_CPU_USAGE == 1)
		ls_task_enter_critical();
		idel_count++;
		ls_task_exit_critical();
#endif
		
#if (LS_ENABLE_HOOKS == 1)
		ls_task_idle_hooks();
#endif
	}
}

extern void tSetSysTickPeriod(uint32_t ms);

static ls_stack_t  task_idle_stack[1024];
static ls_task_t	 task_idle;

void ls_rtos_init(void)
{
#if (LS_ENABLE_TIMER == 1)
	ls_timer_module_init ();
#endif
	
	/* ��ʼ��������� */
	ls_task_sched_init();
	
	ls_init_delay_list ();
	
	ls_task_init(&task_idle, task_idle_stack, sizeof(task_idle_stack), LS_IDEL_TASK_PRIORITY, task_idle_func, (void*)0x22222222);
	
	next_task = ls_task_high_redy();

	ls_first_tast_entry();
	
}

#if (LS_ENABLE_CPU_USAGE == 1)
void ls_cpu_usage_check ()
{
	/* ��Ҫͬ��,����һ��SYSTICKʱ�Ӻ��ͬ�� */
	if (0 == first_systick) {
		first_systick = 1;
		start_check_cpu_usage = 1;
		ls_task_schedule_disable();
	} 
	if (systick_count == LS_TICKS_PER_SEC) {
		/* ���ִֻ��һ�� */
		static uint32_t flag = 0;
		
		if (flag == 0) {
			flag = 1;
		  idel_count_max = idel_count ;
			idel_count = 0;
			ls_task_schedule_enable();
		}
	} else if (systick_count % LS_TICKS_PER_SEC == 0) {
		cpu_usage = 100 - (100.0 * idel_count) / idel_count_max;
		idel_count = 0;
	}
}

static void wait_asys_systick()
{
	while (start_check_cpu_usage == 0) {};
}

float ls_get_cpu_usage()
{
	float usage = 0.0;
	ls_task_enter_critical();
	usage = cpu_usage;
	ls_task_exit_critical();
	
	return usage;
}

#endif

