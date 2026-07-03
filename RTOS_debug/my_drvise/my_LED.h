#ifndef	__MY_LED_H__
#define __MY_LED_H__

#include "debug.h"
#include "LED_Thread.h"

#define STCP_PIN	BSP_IO_PORT_00_PIN_03
#define	SHCP_PIN	BSP_IO_PORT_01_PIN_06 
#define	DS_PIN		BSP_IO_PORT_06_PIN_00 


void LED_Init(void);
void LED_In_1(uint8_t temp);
void LED_Bety(uint8_t data);
void LED_L(void);
void LED_R(void);
void LED_do(void);

#endif

