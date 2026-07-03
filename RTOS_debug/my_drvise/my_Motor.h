#ifndef __MY_MOTOR_H__
#define __MY_MOTOR_H__
//所用线程头文件声明
#include "Car_Thread.h"

//调试文件
#include <debug.h>

//统一配置结构体，勿动!!!
typedef struct{
	uint8_t channel;
	const timer_instance_t*name;
	void* ctrl;
	gpt_io_pin_t PWM_Pin;
	bsp_io_port_pin_t pin1;
	bsp_io_port_pin_t pin2;

}Motor_Config_t;


//自定义电机名称， 可修改处
typedef enum{
	M_L = 0,
	M_R,
//	M_LQ,
//	M_RQ,
	Motor_max					//此行为最大舵机数标志位位于末端，勿动!!!!
}MotorType_e;

typedef enum 
{
	stop = 0,
	front,
	back,
}DirectionType_e;


#define M_1_GPT_channel 			4												//GPT通道
#define M_1_GPT_name					g_timer4								//GPTname
#define M_1_GPT_ctrl 					g_timer4_ctrl						//GPT控制段
#define M_1_GPT_PIN 					GPT_IO_PIN_GTIOCA				//p302
#define M_1_PIN1							BSP_IO_PORT_02_PIN_03		//接203
#define M_1_PIN2							BSP_IO_PORT_02_PIN_02		//接202

#define M_2_GPT_channel 			4												//GPT通道
#define M_2_GPT_name					g_timer4                //GPTname
#define M_2_GPT_ctrl 					g_timer4_ctrl           //GPT控制段
#define M_2_GPT_PIN 					GPT_IO_PIN_GTIOCB				//p301
#define M_2_PIN1							BSP_IO_PORT_02_PIN_04		//接204
#define M_2_PIN2							BSP_IO_PORT_04_PIN_05   //接405					

//#define M_3_GPT_channel 			5												//GPT通道
//#define M_3_GPT_name					g_timer5              	//GPTname
//#define M_3_GPT_ctrl 					g_timer5_ctrl         	//GPT控制段
//#define M_3_GPT_PIN 					GPT_IO_PIN_GTIOCA				//p609
//#define M_3_PIN1							BSP_IO_PORT_06_PIN_03		//接603
//#define M_3_PIN2							BSP_IO_PORT_06_PIN_04		//接604

//#define M_4_GPT_channel 			5												//GPT通道
//#define M_4_GPT_name					g_timer5                //GPTname
//#define M_4_GPT_ctrl 					g_timer5_ctrl           //GPT控制段
//#define M_4_GPT_PIN 					GPT_IO_PIN_GTIOCB				//p610
//#define M_4_PIN1							BSP_IO_PORT_06_PIN_07		//接607
//#define M_4_PIN2							BSP_IO_PORT_06_PIN_05		//接605
															

void Motor_Init(MotorType_e Motor);
void Motor_PWM_Set(MotorType_e Motor,double duty, DirectionType_e Direction);
void Motor_Set(MotorType_e Motor,float PWM);

#endif
