#include "ARMCM3.h"
#include "rtos.h"
#include "lib.h"




void bitmap_init (void *p_bitmap)
{
	((bitmap *)p_bitmap)->bit_map = 0;
}

void bitmap_set (void *p_bitmap, uint32_t pos)
{
	((bitmap *)p_bitmap)->bit_map |= (1 << pos);
}

void bitmap_clr (void *p_bitmap, uint32_t pos)
{
	((bitmap *)p_bitmap)->bit_map &= ~(1 << pos);
}

uint32_t bitmappos_counnt ()
{
	return 32;
}

uint32_t get_bitmap_high_prio(void *p_bitmap)
{
		static const uint8_t quickFindTable[] =     
	{
	    /* 00 */ 0xff, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 10 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 20 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 30 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 40 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 50 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 60 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 70 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 80 */ 7,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 90 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* A0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* B0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* C0 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* D0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* E0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* F0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
	};

	if (((bitmap *)p_bitmap)->bit_map & 0xff)
    {
        return quickFindTable[((bitmap *)p_bitmap)->bit_map & 0xff];         
    }
    else if (((bitmap *)p_bitmap)->bit_map & 0xff00)
    {
        return quickFindTable[(((bitmap *)p_bitmap)->bit_map >> 8) & 0xff] + 8;        
    }
    else if (((bitmap *)p_bitmap)->bit_map & 0xff0000)
    {
        return quickFindTable[(((bitmap *)p_bitmap)->bit_map >> 16) & 0xff] + 16;        
    }
    else if (((bitmap *)p_bitmap)->bit_map & 0xFF000000)
    {
        return quickFindTable[(((bitmap *)p_bitmap)->bit_map >> 24) & 0xFF] + 24;
    }
    else
    {
        return bitmappos_counnt();
    }
}

