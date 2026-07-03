#ifndef __MY_UWB_H__
#define __MY_UWB_H__
//所用线程头文件声明
#include "Car_Thread.h"

//调试文件
#include <debug.h>

#define UWB_1_UART			g_uart5					//串口名称
#define UWB_1_CALLBACK	uart5_callback	//串口中断函数

extern SemaphoreHandle_t  UWB_data_useflag;

extern unsigned char UWB_Content[70];
extern unsigned char UWB_data[80];
extern uint8_t  UWB_data_stact;
extern uint8_t  UWB_Content_stact;
extern uint8_t  UWB_data_stact;
extern uint8_t  UWB_Content_stact;


void UWB_init(void);

#endif

