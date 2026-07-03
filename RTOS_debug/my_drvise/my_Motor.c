#include "my_Motor.h"
//
//                       _oo0oo_
//                      o8888888o
//                      88" . "88
//                      (| -_- |)
//                      0\  =  /0
//                    ___/`---'\___
//                  .' \\|     |// '.
//                 / \\|||  :  |||// \
//                / _||||| -:- |||||- \
//               |   | \\\  - /// |   |
//               | \_|  ''\---/''  |_/ |
//               \  .-\__  '-'  ___/-. /
//             ___'. .'  /--.--\  `. .'___
//          ."" '<  `.___\_<|>_/___.' >' "".
//         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
//         \  \ `_.   \_ __\ /__ _/   .-` /  /
//     =====`-.____`.___ \_____/___.-`___.-'=====
//                       `=---='
//
//
//     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//        佛祖保佑       永不修改     永无BUG
//
/*
电机驱动函数

在rasc内pins配置GPT，
stack线程里New Stacks -> timers -> (r_gpt)


common
pin output enabled

General
设置name如g_timer0					
Channel和pin内一致					通道
Mod			saw-wave PWM				pwm模式
period  10									周期20
period 	kilohertz						单位khz
Duty 0											占空比
Output
pinA、B 		enabled true
levelA、B 	Low

在rasc内pins配置GPIO
pins->Ports->px->pxxx
mode output
driver low
output type cmos

*/



//配置列表，勿动!!!
 Motor_Config_t Motor_list[Motor_max];


/*
初始化电机,
Motor			电机名称

*/
void Motor_Init(MotorType_e Motor)
{
	static	bool init_flag = false;

	if(!init_flag)
	{
		
	//初始化列表	
		Motor_list[M_L]  = (Motor_Config_t){M_1_GPT_channel,&M_1_GPT_name,&M_1_GPT_ctrl,M_1_GPT_PIN,M_1_PIN1,M_1_PIN2};
		Motor_list[M_R]  = (Motor_Config_t){M_2_GPT_channel,&M_2_GPT_name,&M_2_GPT_ctrl,M_2_GPT_PIN,M_2_PIN1,M_2_PIN2};
	//	Motor_list[M_LQ] = (Motor_Config_t){M_3_GPT_channel,&M_3_GPT_name,&M_3_GPT_ctrl,M_3_GPT_PIN,M_3_PIN1,M_3_PIN2};
	//	Motor_list[M_RQ] = (Motor_Config_t){M_4_GPT_channel,&M_4_GPT_name,&M_4_GPT_ctrl,M_4_GPT_PIN,M_4_PIN1,M_4_PIN2};
		init_flag = true;
	}
	
	Motor_Config_t temp_Motor = Motor_list[Motor];
	//初始化模块
	fsp_err_t err = FSP_SUCCESS;
	err = R_GPT_Open(temp_Motor.ctrl, temp_Motor.name->p_cfg);
	if(FSP_SUCCESS != err)
	{
			//打印错误码
			printf("Error : %d\r\n", err);
	}
	//启动定时器，开始输出PWM
  R_GPT_Start(temp_Motor.name->p_ctrl);
}



/*
电机PWM设置
Motor 				电机名称
duty					占空比
Direction			转向
*/
void Motor_PWM_Set(MotorType_e Motor,double duty, DirectionType_e Direction)
{
	Motor_Config_t temp_Motor = Motor_list[Motor];
	
	//此处可更改正反转,替换case后即可
	switch(Direction)
	{
		case stop:
			R_IOPORT_PinWrite(g_ioport.p_ctrl,temp_Motor.pin1,0);
			R_IOPORT_PinWrite(g_ioport.p_ctrl,temp_Motor.pin2,0);
		break;
		case back:
			R_IOPORT_PinWrite(g_ioport.p_ctrl,temp_Motor.pin1,1);
			R_IOPORT_PinWrite(g_ioport.p_ctrl,temp_Motor.pin2,0);
		break;
		case front:
			R_IOPORT_PinWrite(g_ioport.p_ctrl,temp_Motor.pin1,0);
			R_IOPORT_PinWrite(g_ioport.p_ctrl,temp_Motor.pin2,1);
		break;
	}
	
	timer_info_t info;
	R_GPT_InfoGet(temp_Motor.ctrl, &info);
	uint32_t current_period_counts = info.period_counts;
	uint32_t duty_cycle_counts =
	 (uint32_t)(current_period_counts * duty)/100;

	 R_GPT_DutyCycleSet(temp_Motor.ctrl, duty_cycle_counts, temp_Motor.PWM_Pin);
}

/*
电机设置,转速转向
Motor 电机名称
PWM		正,正转
			反反转
			0停转
*/
void Motor_Set(MotorType_e Motor,float PWM)
{
	if(PWM == 0.0)
	{
		Motor_PWM_Set(Motor,PWM,stop);
	}
	else if(PWM > 0)
	{
		Motor_PWM_Set(Motor,PWM,front);
	}
	
	else if(PWM < 0)
	{
		PWM = -PWM;
		Motor_PWM_Set(Motor,PWM,back);
	}
	
}















