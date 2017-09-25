    .thumb
    .syntax unified
    .cpu cortex-m3

    .thumb_func
    .global PendSV_Handler
    .global  current_task
    .global  next_task

PendSV_Handler:

    mrs     r0, psp

    cmp     r0, #0
    beq     first_task  //等于0就跳转

    stmdb   r0!, {r4-r11} // 当前任务压栈

    ldr     r1, =current_task //取出当前任务的指针的地址
    ldr     r1, [r1]  // 取出当前任务的地址
    str     r0, [r1]  // 把压入栈后的栈顶指针赋值给任务中任务堆栈指针



 first_task:

    ldr r0, = current_task
    ldr r1, = next_task  // 取出下一个任务的指针变量的地址

    ldr r1, [r1]  // 取出下一个任务的指针变量指向的地址，即指向的任务的地址
    str r1, [r0]  // 把当前任务的指针变量，指向下一个任务的地址

    ldr r0, [r1]  // 从当前任务中取出当前任务的栈指针指向的地址

 //   msr psp, r0   // 把当前的栈地址，赋值给psp栈

    ldmia r0!, {r4-r11} // 弹出r4-r12的寄存器

    msr psp, r0   // 把当前的栈地址，赋值给psp栈

    orr lr, lr , #0x04  //指明弹出栈是从PSP中弹出

    bx lr










