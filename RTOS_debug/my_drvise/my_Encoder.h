#ifndef __ENCODER_H__
#define __ENCODER_H__
//多用线程头文件声明
#include "Car_Thread.h"
extern	bool En_speed_useflag;
//调试文件
#include <debug.h>
//统一配置结构体
typedef struct 
{
	uint8_t channel;
	const timer_instance_t *name;
	void *ctrl;
	uint64_t Max;
	uint32_t    En_val_1;
	uint32_t    En_val_2;
	int64_t     En_delta;
}Encoder_Config_t;


//自定义编码器名称
typedef enum 
{
	En_L =0,
	En_R,
//	En_LQ,
//	En_RQ,
	Encoder_max    //编码器最大数标志位，位于末端
}EncoderType_e;
/*

#define GPT_1_channel   								//GPT通道
#define GPT_1_name          	          //GPT名称
#define GPT_1_ctrl          	          //配置块，控制句柄
#define GPT_1_Delay_ms      	          //阻塞时长
#define GPT_1_OVER_value    	          //溢出值
#define GPT_1_callback      	          //回调函数
*/

#define PAI 3.14
//编码器参数配置
//编码器参数0
#define GPT_Delay_ms       6                     //阻塞时长


#define GPT_1_channel        0                        //GPT通道
#define GPT_1_name           g_timer0                 //GPT名称
#define GPT_1_ctrl           g_timer0_ctrl            //配置块，控制句柄
#define GPT_1_OVER_value     0x80000000 							//溢出值


//编码器1
#define GPT_2_channel        1                        //GPT通道
#define GPT_2_name           g_timer1                 //GPT名称
#define GPT_2_ctrl           g_timer1_ctrl            //配置块，控制句柄
#define GPT_2_OVER_value     0x80000000 							//溢出值

//#define GPT_3_channel        2                        //GPT通道
//#define GPT_3_name           g_timer2                 //GPT名称
//#define GPT_3_ctrl           g_timer2_ctrl            //配置块，控制句柄
//#define GPT_3_OVER_value     0x80000000 							//溢出值

//#define GPT_4_channel        3                        //GPT通道
//#define GPT_4_name           g_timer3                 //GPT名称
//#define GPT_4_ctrl           g_timer3_ctrl            //配置块，控制句柄
//#define GPT_4_OVER_value     0x80000000 							//溢出值

void Encoder_Init(EncoderType_e Encoder);
void Encoder_Read_speed(void);
extern double En_speed[Encoder_max];
#endif
