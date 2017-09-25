/*******************************************************************************
*                                 AnyWhere
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      anywhere.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief lpc17xx cm3 c startup file for gcc and arm compiler
 *
 * The startup vectors are defined in this file.
 *
 * \internal
 * \par modification history:
 * - 1.01 13-04-03  zyr, modify it to fit cm4 core on lpc43xx
 * - 1.00 13-04-03  zyr, first implementation(copy from lpc13xx)
 * \endinternal
 */

#include "cortex_m3_exc.h"

/** attribute weak and alias */
#define __weak_alias(name)  __attribute__((weak, alias(name)))

/** All weak exception handlers */
__weak_alias("__dumy_handler") void reset_handler (void);
__weak_alias("__dumy_handler") void armv7m_exc_nmi_handler (void);
__weak_alias("__dumy_handler") void armv7m_exc_hardfault_handler (void);
__weak_alias("__dumy_handler") void armv7m_exc_memmanage_handler (void);
__weak_alias("__dumy_handler") void armv7m_exc_busfault_handler (void);
__weak_alias("__dumy_handler") void armv7m_exc_usagefault_handler (void);
__weak_alias("__dumy_handler") void armv7m_exc_svcall_handler (void);
__weak_alias("__dumy_handler") void armv7m_exc_debugmon_handler (void);
#if 0
__weak_alias("__dumy_handler") void armv7m_exc_pendsv_handler (void);
#endif
__weak_alias("__dumy_handler") void armv7m_exc_systick_handler (void);
__weak_alias("__dumy_handler") void armv7m_exc_eint_handler (void);

/** All extern data */
#ifdef __GNUC__
extern unsigned long   _estack;
#define __initial_sp   ((unsigned long)&_estack - 32)
#else
extern void           __main (void);
#define __initial_sp    (0x02000000 + 0x10000 - 32)
#endif
extern void           PendSV_Handler (void);       /** 该函数的实现在汇编中实现 */

#define Sign_Value (void (*) (void))0x5A5A5A5A

extern void ResetHandler(void);

/** \brief This is the vector table */
__attribute__((section(".vectors"), used)) void (* const gc_vectors[]) (void) =
{
    (void (*)(void))((unsigned long)__initial_sp),
    ResetHandler,
    armv7m_exc_nmi_handler,
    armv7m_exc_hardfault_handler,
    0,
    0,
    0,
    0, 0, 0, 0,
    armv7m_exc_svcall_handler,
    0,
    0,
    PendSV_Handler,
    0,
    armv7m_exc_systick_handler,

    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler,
    armv7m_exc_eint_handler
};

/** \brief Here are the weak interrupt handler */
void __dumy_handler (void)
{
    volatile int go_out = 0;

    while (0 == go_out) {
    }
}

/* end of file */
