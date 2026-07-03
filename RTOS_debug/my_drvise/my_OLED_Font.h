#ifndef __MY_OLED_FONT_H
#define __MY_OLED_FONT_H
#include "stdint.h"

typedef	enum{
	cursor,
	
	
	
	Font_max
}FontT;


extern const uint8_t OLED_F8x16[][16];

extern const uint8_t my_F8x16[Font_max][16];

	
#endif
