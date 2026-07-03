#ifndef __MY_PID_H__
#define __MY_PID_H__
//所用线程头文件声明
#include "Car_Thread.h"

#include "my_Encoder.h"
#include "my_Motor.h"
#include "my_IMU.h"
//调试文件
#include <debug.h>
#include "math.h"
//自定义电机名称， 可修改处
typedef enum{
	PID_L = 0,
	PID_R,
//	PID_LQ,
//	PID_RQ,
	PID_max					//此行为最大舵机数标志位位于末端，勿动!!!!
}PIDType_e;

#define YAW_Lim	1.5			//角度前馈阈值
#define YAW_SPEED	10		//前馈差速

#define YAW_Kp	(float)1.0		//角度环p
#define YAW_Ki	(float)0.0		//i
#define YAW_Kd	(float)0.1		//d
#define Yaw_Duty (float)0.5		//占比系数
	
#define DEGSPEED_Kp	(float)0.1	//角速度环p
#define DEGSPEED_Ki	(float)0.0	//i
#define DEGSPEED_Kd	(float)0.0	//d
#define DEGSPEED_Duty (float)1	//占比系数

#define PID_1_Motor 		M_L						//PID1轮子名称
#define PID_1_Encoder 	En_L					//PID1编码器名称
#define PID_1_Kp 				(float)1.5		//速度环p
#define PID_1_Ki 				(float)0.5		//i
#define PID_1_Kd 				(float)0.0		//d

#define PID_2_Motor 		M_R						//PID1轮子名称
#define PID_2_Encoder 	En_R					//PID1编码器名称
#define PID_2_Kp 				(float)1.5		//速度环p
#define PID_2_Ki 				(float)0.5		//i
#define PID_2_Kd 				(float)0.0		//d

//#define PID_3_Motor 		M_LQ
//#define PID_3_Encoder 	En_LQ
//#define PID_3_Kp 				(float)0.6
//#define PID_3_Ki 				(float)0.4
//#define PID_3_Kd 				(float)0.0

//#define PID_4_Motor 		M_RQ
//#define PID_4_Encoder 	En_RQ
//#define PID_4_Kp 				(float)0.6
//#define PID_4_Ki 				(float)0.4
//#define PID_4_Kd 				(float)0.0

extern float Y_Error0, Y_Error1,Y_ErrorInt;
void PID_init();
void PID_Set(float Actual0,float Actual1,float yaw);
void PID_Rest(void);

#endif
