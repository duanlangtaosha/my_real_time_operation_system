#ifndef __LS_RTOS_H
#define __LS_RTOS_H

#include "ls_rtos_config.h"

#include "ARMCM3.h"

#include "stdint.h"

#include "rtos.h"

#include "lib.h"

#include "ls_list.h"

#include "ls_error.h"

#include "ls_task.h"

#include "ls_delay.h"

#include "ls_event.h"

#include "ls_sem.h"

#include "ls_mbox.h"

#include "ls_mem_block.h"

#include "ls_mutex.h"

#include "ls_flag_group.h"

#include "ls_timer.h"

#include "ls_hooks.h"

void ls_rtos_init(void);

#define LS_RTOS_SYSTICK_PERIOD      10
#define LS_TICKS_PER_SEC           (1000 / LS_RTOS_SYSTICK_PERIOD)

#endif

