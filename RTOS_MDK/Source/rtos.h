#ifndef __MY_ROTS_H
#define __MY_ROTS_H

#include "stdint.h"
#include "ls_list.h"

#define NVIC_INT_CTRL       0xE000ED04      /**< \brief �жϿ��Ƽ�״̬�Ĵ��� */
#define NVIC_PENDSVSET      0x10000000      /**< \brief ��������жϵ�ֵ */
#define NVIC_SYSPRI2        0xE000ED22      /**< \brief ϵͳ���ȼ��Ĵ��� */
#define NVIC_PENDSV_PRI     0x000000FF      /**< \brief �������ȼ�  */

#define MEM32(addr)         *(volatile unsigned long *)(addr)
#define MEM8(addr)          *(volatile unsigned char *)(addr)
	





void ls_first_tast_entry(void);

void ls_task_witch (void);

void ls_task_schedule(void);

void ls_task_enter_critical(void);

void ls_task_exit_critical(void);
#endif
