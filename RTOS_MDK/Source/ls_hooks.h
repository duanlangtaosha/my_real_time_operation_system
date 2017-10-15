#ifndef __LS_HOOKS_H
#define __LS_HOOKS_H

void ls_task_idle_hooks(void);
void ls_systick_handle_hooks(void);
void ls_task_init_hooks(ls_task_t *p_task);
void ls_task_switch_hooks(ls_task_t *from, ls_task_t *to);

#endif


