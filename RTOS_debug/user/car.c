#include "car.h"
/*
	小车行走控制
	//无需配置pin
	stack线程里New Stacks -> timers -> (r_gpt)

	common
	pin output enabled


	General
	设置name如g_timer0					
	Channel和pin内一致					通道
	Mod			periodic						计数模式
	period  0x10000							周期
	period 	Raw	conunts					单位原始数据

	Output
	pinA、B 		enabled  true
	levelA、B 	Low

*/
SemaphoreHandle_t do_flag_useflag;
do_ID_T do_flag = car_free;
//自身角度
float	angle = 0;
CarT_Config_t CarT_list[CarT_max];

uint16_t TX_flag;
bool sum_flag = true;
uint8_t run_flag = 1;

bool goto_flag =	false;
bool stop_flag = false;


//小车初始化
void car_init(CarTType_e CarT)
{
	tril_init();
	do_flag_useflag = xSemaphoreCreateMutex();

	CarT_list[CarT1] = (CarT_Config_t){5,&g_timer5,&g_timer5_ctrl,0x1000};
	
	PID_init();
	ult_ADC_init(ult_1_ADC);
	my_servo_init(yun_u);
	my_servo_init(yun_d);
	my_servo_init(door);	
	
	CarT_Config_t *cfg = &CarT_list[CarT];
	fsp_err_t err = FSP_SUCCESS;           						//定义FSP标准错误码变量，初始化为成功状态 （FSP_SUCCESS=0）                									
	err = R_GPT_Open(cfg->name->p_ctrl, cfg->name->p_cfg);// 调用GPT驱动的Open函数，初始化定时器（绑定控制块与配置结构体）																					
	err = R_GPT_CounterSet(cfg->name->p_ctrl,(uint16_t)0x00);			// 设置GPT0的初始计数值为0x80000000（2^31，32位定时器的中间值）
	if(FSP_SUCCESS != err)														// 判断Open操作是否失败（错误码不等于FSP_SUCCESS时进入分支）
	{
			printf("Encoder_Init_Error : %d\r\n", err);								// 串口打印定时器初始化失败的错误码，方便定位配置/硬件问题
	}
	else													// 判断Open操作是否失败（错误码不等于FSP_SUCCESS时进入分支）
	{
			printf("Encoder_Init_OK : %d\r\n", err);								// 串口打印定时器初始化失败的错误码，方便定位配置/硬件问题
	}
	my_servo_set(yun_u,10);
	my_servo_set(yun_d,135);
	my_servo_set(door,0);
	Morse(0);
}

//跑一定距离
/*
	speed 	速度
	sum			距离
	yaw 		角度
*/
void car_sum(float speed,float sum, float yaw)
{
	sum_flag = true;
	angle = yaw;
	
	double O_T;
	double timer = sum/speed;
	timer_info_t info;
	R_GPT_InfoGet(g_timer5.p_ctrl, &info);
	
	O_T = (double)info.period_counts/(double)info.clock_frequency;
	TX_flag = (uint16_t)(timer/O_T);
	uint32_t conunter = (uint32_t)((timer - TX_flag*O_T) * info.clock_frequency);
	R_GPT_CounterSet(g_timer5.p_ctrl,0x1000 - conunter);
	R_GPT_Start(g_timer5.p_ctrl);
	while(sum_flag)	PID_Set(speed,speed,yaw);
	R_GPT_Stop(g_timer5.p_ctrl);
	Motor_Set(M_L,0);
	Motor_Set(M_R,0);
	PID_Rest();
}
//一直跑绝对
/*
	speed		速度
	yaw			角度
*/
void car_A_always_run(float speed,float yaw)
{
	angle = yaw;

	PID_Set(speed,speed,yaw);
}

//一直跑相对
/*
	speed		速度
	yaw			角度
*/
void car_R_always_run(float speed,float yaw)
{
	float temp = angle + yaw;
	while(temp > 180 || temp < -180)
	{
		if(temp > 180)	temp = temp-360;
		else if(temp < -180) temp = temp + 360;
	}
	angle = temp;
	PID_Set(speed,speed,temp);
}


//转向,yaw相对角度
void car_RTurning(float yaw)
{
	float temp = angle + yaw;
	
	if(temp > 180)	temp = temp-360;
	else if(temp < -180) temp = temp + 360;
	angle = temp;
	do{
		PID_Set(0,0,temp);
	}while(fabsf(Y_Error0) >0.5);
	Motor_Set(M_L,0);
	Motor_Set(M_R,0);
	PID_Rest();
}
//转向,yaw绝对角度
void car_ATurning(float yaw)
{
	angle = yaw;
	do{
		PID_Set(0,0,yaw);
	}while(fabsf(Y_Error0) >0.5);
	Motor_Set(M_L,0);
	Motor_Set(M_R,0);
	PID_Rest();
}

vec3d car_Actual;
int sum[3] = {0,0,0};
bool lu_flag[3] = {true,true,true};
int sum_temp;

//避障
void car_Collision(float yaw, float speed)
{
	int i;
	int stopgo_flag;
	goto_flag = false;
	stop_flag = false;
	
	my_servo_set(yun_u,10);
	my_servo_set(yun_d,135);
	car_ATurning(yaw);
	
	while(!goto_flag)//前进循环
	{
		
		sum[1] = (int)ult_ADC_Read(ult_1_ADC);
		if(sum[1] < 20)	{stop_flag = true; lu_flag[1] = false;}
		if(stop_flag)	//找路进入
		{
			for(i = 0;i < 3; i++)
			{
				sum[i] = 0;
				lu_flag[i] = true;
			}
			stopgo_flag = 0;
			for(i =0 ;i < 3;i++)
			{
				my_servo_set(yun_d, 225 - i*90);
				vTaskDelay(1000);
				sum[i] = (int)ult_ADC_Read(ult_1_ADC);
				if(sum[i] < 20)
					lu_flag[i] = false;
			}
			for(i = 1;i < 3;i++)
			{
				if(sum[i] >= sum[stopgo_flag])
					stopgo_flag = i;
			}
			if(sum[stopgo_flag] < 20)	//无路可走,跳出函数
			{
				Morse(1);
				return;
			}
			else//可能有路,找
			{
				while(stop_flag)
				{
					my_servo_set(yun_d,45 + (float)stopgo_flag * 90);
					car_ATurning((float)90.0 - (float)stopgo_flag * 90 + yaw);//转向走
					vTaskDelay(1000);
					for(i = 0;i < 15;i++)//走15cm,边走边测
					{
						car_sum(15,1,(float)90.0 - (float)stopgo_flag * 90 + yaw);
						sum_temp = (int)ult_ADC_Read(ult_1_ADC);

						if(sum_temp > 20)//找到路跳出
						{
							stop_flag = false;
							break;
						}
					}
					my_servo_set(yun_d,135);
					vTaskDelay(1000);
					sum_temp = (int)ult_ADC_Read(ult_1_ADC);
					if(stop_flag == false)//检测前方能否转弯
					{
						if(sum_temp > 15)
						{
							car_sum(15,15,(float)90.0 - (float)stopgo_flag * 90 + yaw);
							car_ATurning(0 + yaw);
							
							break;
						}
						else//此路不通
						{
							lu_flag[stopgo_flag] = false;
							stop_flag = true;
							if(lu_flag[0] == false && lu_flag[2] == false)
							{
								Morse(1);
								return;
							}
							if(stopgo_flag == 0)
								stopgo_flag = 2;
							else if(stopgo_flag == 2)
								stopgo_flag = 0;
							my_servo_set(yun_d,225 - (float)stopgo_flag * 90);
							car_ATurning(90 - (float)stopgo_flag * 90 + yaw);//转向走
							vTaskDelay(1000);
						}
					}
					else if(sum_temp < 15)//此路不通
					{
							lu_flag[stopgo_flag] = false;
							stop_flag = true;
							if(lu_flag[0] == false && lu_flag[2] == false)
							{
								Morse(1);
								return;
							}
							if(stopgo_flag == 0)
								stopgo_flag = 2;
							else if(stopgo_flag == 2)
								stopgo_flag = 0;
							my_servo_set(yun_d,225 - (float)stopgo_flag * 90);
							car_ATurning(90 - (float)stopgo_flag * 90 + yaw);//转向走
							vTaskDelay(1000);						
					}
				}
			}
		}
		car_sum(speed,1,0 + yaw);
	}
}

//门
void car_door_set(bool state)
{
	if(state)
		my_servo_set(door,110);
	else
		my_servo_set(door,0);
}

//去某地
void car_goto_do(float x,float y,float speed)
{
	goto_flag = false;
	int temp;
	
	float degree;
	vec3d Error,car_Target;
	float distance1;
	float distance_use;
	
	my_servo_set(yun_u,10);
	my_servo_set(yun_d,135);
	
	car_Target.x = x*100;
	car_Target.y = y*100;
	car_Target.z = 0;
	
	while(!goto_flag)
	{
		temp = tril_read();
		if(temp < 0)
		{
			Morse(2);//定位失败
		}
		xSemaphoreTake(UWB_data_useflag,portMAX_DELAY);
		car_Actual	=	report;
		xSemaphoreGive(UWB_data_useflag);

		car_Actual.x = car_Actual.x*100;
		car_Actual.y = car_Actual.y*100;
		car_Actual.z = car_Actual.z*100;
		
		
		Error.x = car_Target.x - car_Actual.x;
		Error.y = car_Target.y - car_Actual.y;
		degree = (float)(atan2(Error.y,Error.x)/(2*PAI)*360);
		distance1 = (float)pow(pow(Error.x,2) + pow(Error.y,2),0.5);
		
		
		distance_use = 7*logf(distance1 - 25)+5;
		
		if(distance_use > RUN_H) distance_use = RUN_H;
		else if(distance_use < RUN_L) distance_use = RUN_L;
		if(distance1 < 15)
		{
			goto_flag = true;
			break;
		}
		if(temp >= 0)
		{
			car_sum(speed,distance_use,degree);
		}
	}
	return;
}


//避障去某地专用
void car_Collision_gouse(float speed,float yaw,float juli)
{
	int i = (int)juli;
	int stopgo_flag;
	stop_flag = false;
	
	my_servo_set(yun_u,10);
	my_servo_set(yun_d,135);
	while(i--)//前进循环
	{
		sum[1] = (int)ult_ADC_Read(ult_1_ADC);
		if(sum[1] < 20)	{stop_flag = true; lu_flag[1] = false;}
		if(stop_flag)	//找路进入
		{
			for(i = 0;i < 3; i++)
			{
				sum[i] = 0;
				lu_flag[i] = true;
			}
			stopgo_flag = 0;
			for(i =0 ;i < 3;i++)
			{
				my_servo_set(yun_d, 225 - i*90);
				vTaskDelay(1000);
				sum[i] = (int)ult_ADC_Read(ult_1_ADC);
				if(sum[i] < 20)
					lu_flag[i] = false;
			}
			for(i = 1;i < 3;i++)
			{
				if(sum[i] >= sum[stopgo_flag])
					stopgo_flag = i;
			}
			if(sum[stopgo_flag] < 20)	//无路可走,跳出函数
			{
				Morse(1);
				goto_flag = true;
				return;
			}
			else//可能有路,找
			{
				while(stop_flag)
				{
					my_servo_set(yun_d,45 + (float)stopgo_flag * 90);
					car_ATurning((float)90.0 - (float)stopgo_flag * 90 + yaw);//转向走
					vTaskDelay(1000);
					for(i = 0;i < 15;i++)//走15cm,边走边测
					{
						car_sum(speed,1,(float)90.0 - (float)stopgo_flag * 90 + yaw);
						sum_temp = (int)ult_ADC_Read(ult_1_ADC);

						if(sum_temp > 20)//找到路跳出
						{
							stop_flag = false;
							break;
						}
					}
					my_servo_set(yun_d,135);
					vTaskDelay(1000);
					sum_temp = (int)ult_ADC_Read(ult_1_ADC);
					if(stop_flag == false)//检测前方能否转弯
					{
						if(sum_temp > 15)
						{
							car_sum(speed,15,(float)90.0 - (float)stopgo_flag * 90 + yaw);
							car_ATurning(0 + yaw);
							car_sum(speed,15,0	+ yaw);
							i = 0;
							break;
						}
						else//此路不通
						{
							lu_flag[stopgo_flag] = false;
							stop_flag = true;
							if(lu_flag[0] == false && lu_flag[2] == false)
							{
								Morse(1);
								goto_flag = true;
								return;
							}
							if(stopgo_flag == 0)
								stopgo_flag = 2;
							else if(stopgo_flag == 2)
								stopgo_flag = 0;
							my_servo_set(yun_d,225 - (float)stopgo_flag * 90);
							car_ATurning(90 - (float)stopgo_flag * 90 + yaw);//转向走
							vTaskDelay(1000);
						}
					}
					else if(sum_temp < 15)//此路不通
					{
							lu_flag[stopgo_flag] = false;
							stop_flag = true;
							if(lu_flag[0] == false && lu_flag[2] == false)
							{
								Morse(1);
								goto_flag = true;
								return;
							}
							if(stopgo_flag == 0)
								stopgo_flag = 2;
							else if(stopgo_flag == 2)
								stopgo_flag = 0;
							my_servo_set(yun_d,225 - (float)stopgo_flag * 90);
							car_ATurning(90 - (float)stopgo_flag * 90 + yaw);//转向走
							vTaskDelay(1000);						
					}
				}
			}
		}
		car_sum(speed,1,0 + yaw);
	}
}


//去某地避障
void car_goto_coll_do(float x,float y,float speed)
{
	goto_flag = false;
	int temp;
	
	float degree;
	vec3d Error,car_Target;
	float distance1;
	float distance_use;
	
	my_servo_set(yun_u,10);
	my_servo_set(yun_d,135);
	
	car_Target.x = x*100;
	car_Target.y = y*100;
	car_Target.z = 0;
	
	while(!goto_flag)
	{
		temp = tril_read();
		if(temp < 0)
		{
			Morse(2);//定位失败
		}
		xSemaphoreTake(UWB_data_useflag,portMAX_DELAY);
		car_Actual	=	report;
		xSemaphoreGive(UWB_data_useflag);

		car_Actual.x = car_Actual.x*100;
		car_Actual.y = car_Actual.y*100;
		car_Actual.z = car_Actual.z*100;
		
		
		Error.x = car_Target.x - car_Actual.x;
		Error.y = car_Target.y - car_Actual.y;
		degree = (float)(atan2(Error.y,Error.x)/(2*PAI)*360);
		distance1 = (float)pow(pow(Error.x,2) + pow(Error.y,2),0.5);
		
		
		distance_use = 7*logf(distance1 - 25)+5;
		
		if(distance_use > RUN_H) distance_use = RUN_H;
		else if(distance_use < RUN_L) distance_use = RUN_L;
		if(distance1 < 15)
		{
			goto_flag = true;
			break;
		}
		if(temp >= 0)
		{
			car_Collision_gouse(speed,degree,distance_use);
		}
	}
	return;
}





void g_timer5_callback(timer_callback_args_t * p_args)
{
	if(p_args->event == TIMER_EVENT_CYCLE_END)
	{
			if(TX_flag > 0)
				TX_flag--;
			else
				sum_flag = false;
	}
}

