#ifndef __my_servo_h__
#define __my_servo_h__


//所用线程头文件声明，可修改处
#include "Car_Thread.h"

//调试文件勿动!!!!
#include "debug.h"


//自定义舵机名称， 可修改处
typedef enum{
	yun_u = 0,
	yun_d,
	door,
	
	
	
	Servo_max					//此行为最大舵机数标志位位于末端，勿动!!!!
}ServoType_e;


//舵机参数配置，只适配通过脉宽调整角度
/*修改数字后填写配置
		
	#define GPT_1_channel 										GPT通道
	#define GPT_1_name					              
	#define GPT_1_ctrl 					              控制段如	g_timer0_ctrl
	#define GPT_1_PIN 					              输出引脚如GPT_IO_PIN_GTIOCA
	#define Servo_1_Degree_MAX	              最大旋转角度如180.0
	#define Servo_1_PWM_MAX 		              最大PWM输出占空比如12.5
	#define Servo_1_PWM_MIN 		              最小PWM输出占空比如2.5
*/

#define Servo_GPT_1_channel 			6
#define Servo_GPT_1_name					g_timer6
#define Servo_GPT_1_ctrl 					g_timer6_ctrl
#define Servo_GPT_1_PIN 					GPT_IO_PIN_GTIOCA
#define Servo_1_Degree_MAX 	180.0
#define Servo_1_PWM_MAX 		12.5
#define Servo_1_PWM_MIN 		2.5

#define Servo_GPT_2_channel 			6
#define Servo_GPT_2_name					g_timer6
#define Servo_GPT_2_ctrl 					g_timer6_ctrl
#define Servo_GPT_2_PIN 					GPT_IO_PIN_GTIOCB
#define Servo_2_Degree_MAX 	270.0
#define Servo_2_PWM_MAX 		12.5
#define Servo_2_PWM_MIN 		2.5

#define Servo_GPT_3_channel 			7
#define Servo_GPT_3_name					g_timer7
#define Servo_GPT_3_ctrl 					g_timer7_ctrl
#define Servo_GPT_3_PIN 					GPT_IO_PIN_GTIOCA
#define Servo_3_Degree_MAX 	180.0
#define Servo_3_PWM_MAX 		12.5
#define Servo_3_PWM_MIN 		2.5


void my_servo_init(ServoType_e Servo);
void my_servo_set(ServoType_e Servo,	double degree);


#endif

