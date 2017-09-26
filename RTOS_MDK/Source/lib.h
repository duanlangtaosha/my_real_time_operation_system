
#ifndef  __LIB_H
#define __LIB_H

#include "stdint.h"

typedef struct _bitmap{

	uint32_t bit_map;
}bitmap;



void bitmap_init (void *p_bitmap);

void bitmap_set (void *p_bitmap, uint32_t pos);

void bitmap_clr (void *p_bitmap, uint32_t pos);

uint32_t bitmappos_counnt ();
uint32_t get_bitmap_high_prio(void *p_bitmap);

#endif

