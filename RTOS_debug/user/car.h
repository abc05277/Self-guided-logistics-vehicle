#ifndef __CAR__H__
#define __CAR__H__


#include "Car_Thread.h"
#include "my_PID.h"
#include "User_show.h"
#include "my_ultrasonic_ADC.h"
#include "trilateration.h"
#include "my_servo.h"
#include "my_Buzzer.h"

#include "debug.h"

typedef enum{
	car_free = 0,
	car_run,
	car_ATurn,
	car_RTurn,
	door_set,
	car_Coll,
	car_goto,
	car_goto_Coll,
	car_always
}do_ID_T;

typedef struct 
{
	uint8_t channel;
	const timer_instance_t *name;
	void *ctrl;
	uint32_t Max;

}CarT_Config_t;


//自定义编码器名称
typedef enum 
{
	CarT1 =0,
//	En_LQ,
//	En_RQ,
	CarT_max    //编码器最大数标志位，位于末端
}CarTType_e;


#define RUN_H		200
#define RUN_L		2








#define CarT_1_channel   					5			//GPT通道
#define CarT_1_name          	    g_timer5      //GPT名称
#define CarT_1_ctrl          	    g_timer5_ctrl      //配置块，控制句柄
#define CarT_1_OVER_value    	    0x10000      //溢出值
#define CarT_1_callback      	    g_timer5_callback      //回调函数

extern SemaphoreHandle_t do_flag_useflag;
extern do_ID_T do_flag;
extern bool goto_flag;
extern float angle;
extern bool sum_flag;


void car_init(CarTType_e);
void car_sum(float speed,float sum, float yaw);
void car_A_always_run(float speed,float yaw);
void car_R_always_run(float speed,float yaw);
void car_RTurning(float yaw);
void car_ATurning(float yaw);
void car_Collision(float yaw,float speed);
void car_door_set(bool state);
void car_goto_do(float x,float y,float speed);
void car_goto_coll_do(float x,float y,float speed);


#endif
