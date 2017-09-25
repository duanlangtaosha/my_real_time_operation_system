#include "ARMCM3.h"
#include "rtos.h"

__asm void PendSV_Handler () {

	IMPORT current_task  /*���൱�ڣ����Ե�extern */
	IMPORT next_task

	MRS R0, PSP         /* �ѵ�ǰ��PSPָ�븳ֵ��R0 */
	
	CBZ R0, first_start /* �����Ǹ�λ�����ĵ�һ�����񣬲�ִ��ѹջ�Ĳ��� */
	
	STMDB   R0!, {R4-R11}  /* �����Ҫ�ӵ�ǰ�����л�����һ��������ô��Ҫ���棬��ǰ�����ϵͳ�Ĵ���
												  * xPSR��8���Ĵ����ڽ����ж�ǰ���Ѿ���ѹ����PSP��ջ��PSP�ǵ�ǰ���������ջ��
													*/

	LDR R1, =current_task  /* ȡ����ǰ����ָ���ָ�������ַ */
	LDR R1, [R1]           /* ��current_taskָ��ĵ�ַ��ֵ�� R1 */
	
	STR R0, [R1]    /*��R1��ʱ����&taskn,Ҳ��&task_stack, ��R0��ʱ��ѹ��R4-R11���ջ��ָ�룬Ҳ����˵����ʱ��
									 *  ��ǰ��ջ��ָ�룬��ֵ����ǰ������ṹ���ջָ��(������¼��ǰ����ջ��λ��)���´��л���
						       *  �������󣬷����ջָ����ȡ����ǰ������ջ�������
									 */
	
	
first_start							/* �ӿ��������ĵ�һ������ */
	
		LDR R0, =current_task
		LDR R1, =next_task  /* ���һ��=������ȡ��ַ��Ҳ���൱��C�����е�&,������ȡnext_task�ĵ�ַ���൱��&next_task */
		LDR R1, [R1]	      /* ����е�[]������ĳ����ַ��ȡֵ������ĳ����ַ����ֵ, 
												 * �е�������C�����е� *p = 1,��p��ַ��д��1,
												 *  �����R1�ǵ��� next_task,  *(&next_task)
                       	 */
	  STR R1, [R0]  /* ��R1��ֵ���� R0���ڵĵ�ַ��, �൱�� *R0 = R1��C�����е� *p = 1��
									 * �����Ч������ current_task = next_task, Ҳ�������¸�������µ�ǰ����
									 * �⼸��Ĳ����� �磺 current_task = A , next_task = B,ִ�й��󣬱�� current_task = B
									 */
									
	
		LDR R0, [R1]  /* ��ʱ��R1�Ǹ�ֵ���current_task,��Ϊcurrent_task��һ��ָ�������current_task = &taskn,
									 * ����˵ ִ�е��� *current_task,���൱�� *(&taskn),��taskn���������һ��task����ṹ��
									 * ����ṹ���׵�ַ���Ƕ���Ķ�ջ uint32_t *p_stack������&taskn == &p_stack����˵������
									 * ������Ч������ *(&p_stack) = R0, Ҳ����˵��ǰ������ջָ��ָ������Ҫִ�е�����ָ�룬ͬ
									 * ʱҲ��ָ���˽�Ҫִ�е�����������ջ��
									 */

	  LDMIA  R0!, {R4-R11}  /* ����Cortex M3Ӳ���Զ���ջ��˳���� xPSR,PC,LR,R12,R3,R2,R1,R0(����Ĳ���),
													 * ��8���Ĵ�������ջ��ջ����Ӳ���Զ�ִ�еġ�����˵���м���Ӳ����ѹջ��,�˼Ĵ���
												   * ����ͨ���ֹ��ķ�ʽѹջ����ʵ��ѹ����R0ָ��ĵ�ַ,ѹ��һ��R0-4���պ�R0��ָ��
													 * ����Ҫִ�е����������˽�ж�ջ��
													 */
		
		MSR PSP, R0  /* ֻ�����쳣�в��ܹ��ı�SP��ģʽ�������MSPģʽ��ΪPSP����ģʽ�����е��쳣������MSPģʽ,
									* ��������쳣ǰ���õ�PSPջ����ô�ڽ����쳣ǰ�ͻ����8���Ĵ���ѹ��PSPջ��,�쳣����ִ����
							    * ����Զ��Ĵ�PSP�е�����Ӧ��ջ��ע����û�г��쳣ǰ����ͨ������PSP��ֵ�������ʵ��RTOS����
									* �л��ĺ��ģ���֮��ջ���Ǵ�ջ��������Ӧ�����ݣ�Ȼ���ٸ�ֵ����8���Ĵ�����
									*/
	
		ORR LR, LR, #0x04  /* ����ѡ���쳣�˳�ʱ�ǵ�������MSP��ջ����PSP��ջ������ѡ�񵯳�ʱѡ��PSP��ջ��*/
												
		BX LR  /* ִ���쳣����, ִ��������Ӳ����ִ�е�����8���Ĵ����Ĳ��� */
}


__asm void task_enter_critical(void)
{
	CPSID I
	BX LR
}

__asm void task_exit_critical(void)
{
	CPSIE I
	BX LR
}

void first_tast_entry(void)
{
	/* ����PSPջΪ0�������ж��ǲ��Ǹ�λ��ſ�ʼ */
	__set_PSP(0);

	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;   /* ����PENDSV Ϊ������ȼ� 255����Խ�����ȼ�Խ�� */

	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;    /* �����λPENDSV���жϽ��������л� */
}

void task_witch (void)
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;    /* �����λPENDSV���жϽ��������л� */
}

extern rtos_task_t	 task1;
extern rtos_task_t	 task2;
extern rtos_task_t	 task_idle;

extern rtos_task_t *next_task;


void task_schedule(void)
{
	if ((task1.task_delay != 0) && (task2.task_delay != 0)) {
		
		next_task = &task_idle;
		
	} else if (task1.task_delay != 0) {
		
		next_task = &task2;
		
	} else if (task2.task_delay != 0) {
	
		next_task = &task1;
	} else {
		next_task = &task1;
	}
	
	task_witch ();
}
