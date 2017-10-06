#ifndef __LS_MEM_BLOCK_H
#define __LS_MEM_BLOCK_H

#include "stdint.h"
typedef struct __ls_mem_block {

	ls_event_t  event;
	void *     start_addr;
	uint32_t   block_size;
	uint32_t   block_count;
	ls_list_t  mem_block_list;
	
}ls_mem_block_t;



#include "ls_rtos.h"
#include "ls_mem_block.h"

/*
 *	\brief 初始化内存块
 */
void ls_mem_block_init (ls_mem_block_t *p_mem_block,
												void * p_mem_start_addr, 
												uint32_t block_size, 
												uint32_t block_count);

/*
 *	\brief 释放内存块
 */

ls_error_t ls_mem_block_give (ls_mem_block_t *p_mem_block, void *mem);

/*
 *	\brief 获取内存块
 */
ls_error_t ls_mem_block_take (ls_mem_block_t *p_mem_blcok, void** mem, uint32_t timeout);

/*
 *	\brief 获取内存块无等待
 */
ls_error_t ls_mem_block_take_nowait (ls_mem_block_t *p_mem_blcok, void** mem, uint32_t timeout);


#endif

