#ifndef __MY_KEY_H__
#define __MY_KEY_H__


#include "Show_Thread.h"
#include "my_OLED.h"

typedef enum{
	Key_U = 0,
	Key_D,
	Key_L,
	Key_R,
	Key_I,
	Key_max
}KeyType_e;

typedef enum{
	Key_U_Num = 0,
	Key_D_Num,
	Key_L_Num,
	Key_R_Num,
	Key_O_Num,
	Key_I_Num,
	Key_Kong_Num,
}Key_NumT;

#define KEY_U_PIN	BSP_IO_PORT_02_PIN_08
#define KEY_D_PIN	BSP_IO_PORT_09_PIN_05
#define KEY_L_PIN	BSP_IO_PORT_03_PIN_03
#define KEY_R_PIN	BSP_IO_PORT_06_PIN_10
#define KEY_I_PIN	BSP_IO_PORT_02_PIN_10

void Key_init(void);
Key_NumT Key_Read(void);


#endif
