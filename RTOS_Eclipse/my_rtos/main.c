#include "stdint.h"

#define NVIC_INT_CTRL       0xE000ED04
#define NVIC_PENDSVSET      0x10000000
#define NVIC_SYSPRI2        0xE000ED22
#define NVIC_PENDSV_PRI     0x000000FF
#define MEM32(addr)         *(volatile unsigned long *)(addr)
#define MEM8(addr)          *(volatile unsigned char *)(addr)

typedef struct _rtos_task {
    uint32_t * p_stack;
} rtos_task_t;

typedef uint32_t rtos_stack;

rtos_task_t task1 = {0};
rtos_task_t task2;

rtos_stack task1_stack[1024];
rtos_stack task2_stack[1024];

int c = 0;

extern void task_switch(void);

rtos_task_t *current_task;
rtos_task_t *next_task;

void fisrt_task_entry() {

    asm volatile (
    "MOV R0, #0\t\n"
    "MSR PSP, r0\t\n"
    );
}

void task_init(rtos_task_t *p_task, rtos_stack *p_stack, void *func_entry, void* p_param) {

    *(--p_stack) = (uint32_t)(1 << 24);
    *(--p_stack) = (uint32_t)func_entry;
    *(--p_stack) = (uint32_t)14;       /* R14(LR)*/
    *(--p_stack) = (uint32_t)12;       /* R14 */
    *(--p_stack) = (uint32_t)3;    /* R3 */
    *(--p_stack) = (uint32_t)2;    /* R2 */
    *(--p_stack) = (uint32_t)1;    /* R1 */
    *(--p_stack) = (uint32_t)p_param;    /* R0 */
    *(--p_stack) = (uint32_t)11;    /* R11 */
    *(--p_stack) = (uint32_t)10;    /* R10 */
    *(--p_stack) = (uint32_t)9;    /* R9 */
    *(--p_stack) = (uint32_t)8;    /* R8 */
    *(--p_stack) = (uint32_t)7;    /* R7 */
    *(--p_stack) = (uint32_t)6;    /* R6 */
    *(--p_stack) = (uint32_t)5;    /* R5 */
    *(--p_stack) = (uint32_t)4;    /* R4 */

    p_task->p_stack = p_stack;
}

void func_task1()
{
    while (1) {


    }

}


void task_switch()
{

}

int main()
{

    fisrt_task_entry();
    task_init(&task1, &task1_stack[1024], func_task1, (void*)0x11111111);

    next_task = &task1;

    MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;

    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
    task_switch();

}

//
//void PendSV_Handler(void) {
//
//    int a = 0;
//
//}

