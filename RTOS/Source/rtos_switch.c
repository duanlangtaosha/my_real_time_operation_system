#include "ARMCM3.h"
#include "rtos.h"

__asm void PendSV_Handler () {

	IMPORT current_task  /*　相当于Ｃ语言的extern */
	IMPORT next_task

	MRS R0, PSP         /* 把当前的PSP指针赋值给R0 */
	
	CBZ R0, first_start /* 这里是复位后进入的第一个任务，不执行压栈的操作 */
	
	STMDB   R0!, {R4-R11}  /* 如果是要从当前任务切换到下一个任务，那么就要保存，当前任务的系统寄存器
												  * xPSR等8个寄存器在进入中断前，已经被压入了PSP的栈，PSP是当前任务的任务栈。
													*/

	LDR R1, =current_task  /* 取出当前任务指针的指针变量地址 */
	LDR R1, [R1]           /* 把current_task指向的地址赋值给 R1 */
	
	STR R0, [R1]    /*　R1此时就是&taskn,也是&task_stack, 而R0此时是压如R4-R11后的栈顶指针，也就是说，此时把
									 *  当前的栈顶指针，赋值给当前的任务结构体的栈指针(用来记录当前任务栈的位置)。下次切换到
						       *  这个任务后，方便从栈指针中取出当前的任务栈顶在哪里。
									 */
	
	
first_start							/* 从开机后进入的第一个任务 */
	
		LDR R0, =current_task
		LDR R1, =next_task  /* 添加一个=，就是取地址，也就相当于C语言中的&,这里是取next_task的地址，相当于&next_task */
		LDR R1, [R1]	      /* 汇编中的[]就是向某个地址中取值或者往某个地址中填值, 
												 * 有点类似于C语言中的 *p = 1,往p地址中写入1,
												 *  这里的R1是等于 next_task,  *(&next_task)
                       	 */
	  STR R1, [R0]  /* 把R1的值放入 R0所在的地址中, 相当于 *R0 = R1，C语言中的 *p = 1。
									 * 这里的效果就是 current_task = next_task, 也就是用下个任务更新当前任务
									 * 这几句的操作是 如： current_task = A , next_task = B,执行过后，变成 current_task = B
									 */
									
	
		LDR R0, [R1]  /* 此时的R1是赋值后的current_task,因为current_task是一个指针变量，current_task = &taskn,
									 * 所以说 执行的是 *current_task,就相当于 *(&taskn),而taskn任务定义的是一个task任务结构体
									 * 任务结构的首地址就是定义的堆栈 uint32_t *p_stack。即：&taskn == &p_stack所以说这句代码
									 * 的最终效果就是 *(&p_stack) = R0, 也就是说当前的任务栈指针指向马上要执行的任务指针，同
									 * 时也是指向了将要执行的任务的任务堆栈。
									 */

	  LDMIA  R0!, {R4-R11}  /* 由于Cortex M3硬件自动入栈的顺序是 xPSR,PC,LR,R12,R3,R2,R1,R0(任务的参数),
													 * 这8个寄存器的入栈出栈都是硬件自动执行的。所以说还有几个硬件不压栈的,核寄存器
												   * 这里通过手工的方式压栈，其实是压入了R0指向的地址,压如一个R0-4。刚好R0是指向
													 * 即将要执行的任务的任务私有堆栈。
													 */
		
		MSR PSP, R0  /* 只有在异常中才能够改变SP的模式，这里从MSP模式改为PSP进程模式，所有的异常，都是MSP模式,
									* 如果进入异常前采用的PSP栈，那么在进入异常前就会把那8个寄存器压如PSP栈中,异常函数执行完
							    * 后会自动的从PSP中弹出相应的栈，注意在没有出异常前可以通过更改PSP的值，这就是实现RTOS任务
									* 切换的核心，总之出栈都是从栈顶弹出相应的数据，然后再赋值给那8个寄存器。
									*/
	
		ORR LR, LR, #0x04  /* 这里选择异常退出时是弹出的是MSP的栈还是PSP的栈，这里选择弹出时选择PSP的栈。*/
												
		BX LR  /* 执行异常返回, 执行这句后，由硬件来执行弹出那8个寄存器的操作 */
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
	/* 设置PSP栈为0，方便判断是不是复位后才开始 */
	__set_PSP(0);

	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;   /* 设置PENDSV 为最低优先级 255。数越大优先级越低 */

	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;    /* 软件置位PENDSV的中断进行任务切换 */
}

void task_witch (void)
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;    /* 软件置位PENDSV的中断进行任务切换 */
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
