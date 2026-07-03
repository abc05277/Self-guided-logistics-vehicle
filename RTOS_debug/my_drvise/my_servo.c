
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
舵机控制模块
在rasc内pins配置GPT，
stack线程里New Stacks -> timers -> (r_gpt)

common
pin output enabled


General
设置name如g_timer0					
Channel和pin内一致					通道
Mod			saw-wave PWM				pwm模式
period  20									周期20
period unit milliseconds		单位ms

Output
pinA、B 		enabled  true
levelA、B 	Low


*/


#include "my_servo.h"

//统一配置结构体，勿动!!!
typedef struct{
	uint8_t channel;
	const timer_instance_t*name;
	void* ctrl;
	gpt_io_pin_t Pin;
	double MAX;
	double PWM_MAX;
	double PWM_MIN;
}Servo_Config_t;


//配置列表，勿动!!!
 Servo_Config_t Servo_list[Servo_max];


/*
初始化舵机函数，servo舵机自定义名称
*/
void my_servo_init(ServoType_e Servo)
{
	//初始化配置列表，	根据.h内自定义舵机名称在下面添加一行，!!复制后只改数字!!，	可修改处
	//Servo_list[yun_u] = (Servo_Config_t){GPT_1_channel,&GPT_1_name,&GPT_1_ctrl,GPT_1_PIN,Servo_1_Degree_MAX,Servo_1_PWM_MAX,Servo_1_PWM_MIN};
	
	
	Servo_list[yun_u] = (Servo_Config_t){Servo_GPT_1_channel,&Servo_GPT_1_name,&Servo_GPT_1_ctrl,Servo_GPT_1_PIN,Servo_1_Degree_MAX,Servo_1_PWM_MAX,Servo_1_PWM_MIN};
	Servo_list[yun_d] = (Servo_Config_t){Servo_GPT_2_channel,&Servo_GPT_2_name,&Servo_GPT_2_ctrl,Servo_GPT_2_PIN,Servo_2_Degree_MAX,Servo_2_PWM_MAX,Servo_2_PWM_MIN};
	Servo_list[door] = (Servo_Config_t){Servo_GPT_3_channel,&Servo_GPT_3_name,&Servo_GPT_3_ctrl,Servo_GPT_3_PIN,Servo_3_Degree_MAX,Servo_3_PWM_MAX,Servo_3_PWM_MIN};
	Servo_Config_t temp_servo = Servo_list[Servo];
	fsp_err_t err;
	
	
		err = R_GPT_Open(temp_servo.name->p_ctrl,temp_servo.name->p_cfg);
	
	
	if(err == FSP_SUCCESS)
	printf("GPT%d,Open_OK\r\n",temp_servo.channel);
	else
	printf("GPT%d,Open_ERR\r\n",temp_servo.channel);
	
	err = temp_servo.name->p_api->start(temp_servo.name->p_ctrl);
	
	
	
	if(err == FSP_SUCCESS)
		printf("GPT%d,start_OK\r\n",temp_servo.channel);
	else
		printf("GPT%d,start_ERR\r\n",temp_servo.channel);
}

/*
舵机角度设置

servo 	舵机自定义名称
degree 	舵机定位角度

*/
void my_servo_set(ServoType_e Servo,	double degree)
{
	
	Servo_Config_t temp_servo = Servo_list[Servo];
	fsp_err_t err;
	timer_info_t info;
	double duty = ((degree/(double)temp_servo.MAX)*(temp_servo.PWM_MAX - temp_servo.PWM_MIN) + temp_servo.PWM_MIN);
	R_GPT_InfoGet(temp_servo.ctrl, &info);
	uint32_t current_period_counts = info.period_counts;
	uint32_t duty_cycle_counts =
	 	(uint32_t)(current_period_counts * duty)/100;
	
	
	err = R_GPT_DutyCycleSet(temp_servo.ctrl,duty_cycle_counts,temp_servo.Pin);
	if(err == FSP_SUCCESS)
		printf("servo%.1f,set_OK\r\n",degree);
	else
		printf("servo%.1f,set_ERR\r\n",degree);
}


