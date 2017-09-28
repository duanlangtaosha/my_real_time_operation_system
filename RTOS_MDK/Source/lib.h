
#ifndef  __LIB_H
#define __LIB_H

#include "stdint.h"

typedef struct _ls_bitmap{

	uint32_t bit_map;
}ls_bitmap;



void ls_bitmap_init (void *p_bitmap);

void ls_bitmap_set (void *p_bitmap, uint32_t pos);

void ls_bitmap_clr (void *p_bitmap, uint32_t pos);

uint32_t ls_bitmap_counnt (void);
uint32_t ls_get_bitmap_high_prio(void *p_bitmap);

#endif

