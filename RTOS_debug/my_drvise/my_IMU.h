#ifndef __MY_IMU__H__
#define __MY_IMU__H__

#include "Car_Thread.h"
#include "debug.h"


#define IUM_1_UART						g_uart9					//惯导串口名称
#define IUM_1_UART_CALLBACK 	uart9_callback	//串口中断函数名

extern SemaphoreHandle_t ol_first_useflag;
extern double ol_first[3];

extern SemaphoreHandle_t ol_yaw_useflag;
extern double ol_yaw[3];

extern SemaphoreHandle_t deg_speed_useflag;
extern double deg_speed[3];


void uart9_send(uint8_t const * const p_src, uint32_t const bytes);
void uart9_read( uint8_t * const p_dest, uint32_t const bytes);
void IMU_init(void);

int IMU_Read(void);



#endif

