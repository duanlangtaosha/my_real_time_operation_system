#ifndef __MY_ROTS_H
#define __MY_ROTS_H

#include "stdint.h"

#define NVIC_INT_CTRL       0xE000ED04      /**< \brief 中断控制及状态寄存器 */
#define NVIC_PENDSVSET      0x10000000      /**< \brief 触发软件中断的值 */
#define NVIC_SYSPRI2        0xE000ED22      /**< \brief 系统优先级寄存器 */
#define NVIC_PENDSV_PRI     0x000000FF      /**< \brief 配置优先级  */

#define MEM32(addr)         *(volatile unsigned long *)(addr)
#define MEM8(addr)          *(volatile unsigned char *)(addr)
	
typedef struct __rtos_task {

	uint32_t *p_stack;
	
}rtos_task_t;

typedef uint32_t task_stack;


void first_tast_entry();

void task_witch ();

//void task_sched();
#endif