#ifndef __LS_TIMER_H
#define __LS_TIMER_H

#include "stdint.h"

void tSetSysTickPeriod(uint32_t ms);
void ls_init_delay_list (void);
void ls_delayms(uint32_t systick);


#endif
