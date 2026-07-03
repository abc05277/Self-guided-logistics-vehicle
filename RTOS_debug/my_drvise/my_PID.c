#include "my_PID.h"
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
调用编码器,电机,惯导模块,配置看相应注释
纯算法,无需启用特定硬件
*/

//统一配置结构体，勿动!!!
typedef struct{
	MotorType_e Motor;
	EncoderType_e Encoder;
	float Kp,Ki,Kd;
	float Target, Actual,Out;
	float Error0, Error1,Error2;
	float Yaw;
}PID_Config_t;



float Y_Target, Y_Actual,Y_Out;
float Y_Error0, Y_Error1,Y_ErrorInt;

float DE_Target, DE_Actual,DE_Out;
float DE_Error0, DE_Error1,DE_ErrorInt;

float Y_Q;


PID_Config_t PID_list[PID_max];


//初始化所以有关部件,
void PID_init()
{
	int i;
	IMU_init();
	Encoder_Init(En_L);
	Encoder_Init(En_R);
//	Encoder_Init(En_LQ);
//	Encoder_Init(En_RQ);
	Motor_Init(M_L);
	Motor_Init(M_R);
//	Motor_Init(M_LQ);
//	Motor_Init(M_RQ);
	PID_list[PID_L]  = (PID_Config_t){PID_1_Motor,PID_1_Encoder,PID_1_Kp,PID_1_Ki,PID_1_Kd,0,0,0,0,0,0,1};
	PID_list[PID_R]  = (PID_Config_t){PID_2_Motor,PID_2_Encoder,PID_2_Kp,PID_2_Ki,PID_2_Kd,0,0,0,0,0,0,-1};
//	PID_list[PID_LQ] = (PID_Config_t){PID_3_Motor,PID_3_Encoder,PID_3_Kp,PID_3_Ki,PID_3_Kd,0,0,0,0,0,0};
//	PID_list[PID_RQ] = (PID_Config_t){PID_4_Motor,PID_4_Encoder,PID_4_Kp,PID_4_Ki,PID_4_Kd,0,0,0,0,0,0};
	IMU_Read();
		//存储初始角度值
	xSemaphoreTake(ol_yaw_useflag,portMAX_DELAY);
	xSemaphoreTake(ol_first_useflag,portMAX_DELAY);
	for(i = 0;i < 3;i++)
	{
		ol_first[i] = ol_yaw[i];
	}
	xSemaphoreGive(ol_first_useflag);
	xSemaphoreGive(ol_yaw_useflag);
}



//清空pid所有参数
void PID_Rest(void)
{
	int i;
	DE_Target = 0, DE_Actual = 0,DE_Out = 0;
	DE_Error0 = 0, DE_Error1 = 0,DE_ErrorInt = 0;
	
	DE_Target = 0, DE_Actual = 0,DE_Out = 0;
	DE_Error0 = 0, DE_Error1 = 0,DE_ErrorInt = 0;
	
	for(i = 0;i < PID_max;i++)
	{
		PID_list[i].Actual = 0;
		PID_list[i].Error0 = 0;
		PID_list[i].Error1 = 0;
		PID_list[i].Error2 = 0;
		PID_list[i].Target = 0;
		PID_list[i].Out = 0;
	}
}


/*
设置左轮速度	Actual0	cm/s
设置右轮速度	Actual1	cm/s
以绝对角度yaw	前进
*/
void PID_Set(float Actual0,float Actual1,float yaw)
{
			int i;
			double c = 1;
			float Y_Kp = YAW_Kp,Y_Ki = YAW_Ki,Y_Kd = YAW_Kd;
	
			float DE_Kp = DEGSPEED_Kp,DE_Ki = DEGSPEED_Ki,DE_Kd = DEGSPEED_Kd;
			Encoder_Read_speed();
			float PIDSet[PID_max] = {Actual0,Actual1};
			
			Y_Target = yaw;
			IMU_Read();
			xSemaphoreTake(ol_yaw_useflag,portMAX_DELAY);
			xSemaphoreTake(ol_first_useflag,portMAX_DELAY);
			Y_Actual = (float)ol_yaw[2]	-	(float)ol_first[2];
			xSemaphoreGive(ol_first_useflag);
			xSemaphoreGive(ol_yaw_useflag);
			
			if(Y_Actual>180)Y_Actual =  Y_Actual - 360; 
			else if(Y_Actual<-180)Y_Actual =  Y_Actual + 360; 
			
			Y_ErrorInt += Y_Error0;
			Y_Error1 =Y_Error0;
			Y_Error0 =Y_Target - Y_Actual;
			
			while(Y_Error0 > 180 || Y_Error0 < -180)
			{
				if(Y_Error0>180)Y_Error0 =  Y_Error0 - 360;
				else if(Y_Error0<-180)Y_Error0 =  Y_Error0 + 360;
			}

//			if(Y_Error0 > 30)
//			{
//				Y_Ki = 0;
//				Y_ErrorInt = 0;
//			}
//			else
//			{
//				Y_Ki = YAW_Ki;
//			}
			
				if(Y_ErrorInt > 200)	Y_ErrorInt = 200;
				else if(Y_ErrorInt < -200) Y_ErrorInt = -200;
			Y_Out = Y_Kp*Y_Error0 + Y_Ki*Y_ErrorInt + Y_Kd*(Y_Error0 - Y_Error1);
			
			
			
			DE_Target = Y_Out;
			xSemaphoreTake(deg_speed_useflag,portMAX_DELAY);
			DE_Actual = (float)deg_speed[2];
			xSemaphoreGive(deg_speed_useflag);
			
			DE_ErrorInt += DE_Error0;
			DE_Error1 =DE_Error0;
			DE_Error0 =DE_Target - DE_Actual;
			
//			if(DE_Error0 > 30)
//			{
//				DE_Ki = 0;
//				DE_ErrorInt = 0;
//			}
//			else
//			{
//				DE_Ki = DEGSPEED_Ki;
//			}
			if(DE_ErrorInt > 200)	DE_ErrorInt = 200;
			else if(DE_ErrorInt < -200) DE_ErrorInt = -200;
			DE_Out = DE_Kp*DE_Error0 + DE_Ki*DE_ErrorInt + DE_Kd*(DE_Error0 - DE_Error1);
			
			if(DE_Out > 10)	DE_Out = 10;
			else if(DE_Out < -10) DE_Out = -10;
			
			for(i = 0;i < PID_max;i++)
			{
					if(Y_Error0 > YAW_Lim) Y_Q = YAW_SPEED;
					else if(Y_Error0 < -YAW_Lim) Y_Q = -YAW_SPEED;
					else Y_Q = 0;
					PID_list[i].Target = PIDSet[i]+	DEGSPEED_Duty*PID_list[i].Yaw*(DE_Out + Y_Q);
					//PID_list[i].Target = PIDSet[i];
					//printf("target%d:%.2f",i,PID_list[i].Target);
					PID_list[i].Actual = (float)En_speed[i];
					
					PID_list[i].Error2 = PID_list[i].Error1;
					PID_list[i].Error1 = PID_list[i].Error0;
					PID_list[i].Error0 = PID_list[i].Target - PID_list[i].Actual;
					
					PID_list[i].Out += (float)(PID_list[i].Kp*(PID_list[i].Error0 - PID_list[i].Error1) + c*PID_list[i].Ki*PID_list[i].Error0 
														+ PID_list[i].Kd*(PID_list[i].Error0 - 2*PID_list[i].Error1 + PID_list[i].Error2));
					
					if(PID_list[i].Out > 100) PID_list[i].Out = 100;
					if(PID_list[i].Out < -100) PID_list[i].Out = -100;
					
					Motor_Set(PID_list[i].Motor,PID_list[i].Out);
					if(i == 1)	printf("%f,%f,",c,PID_list[i].Target);

//					printf("OUT%d:%.2f\r\n",PID_list[i].Motor,PID_list[i].Out);
			}
}
	