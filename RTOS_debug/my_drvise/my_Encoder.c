/*编码器控制模块
在RASC中PINS配置GPT
stack线程里New stacks->timers->(r_gpt)

General
设置name为g_timer0、g_timer1
channel和pin内一致    通道
Mode     periodic     周期模式
period   0x100000000  溢出值

input  up     A上升B低
       down     A下降B低
       clock select     1分频
			GTIOCA	GTIOCB
GPT0  512			511
GPT1	109			110
GPT2	113			114
GPT3	111			112
*/

#include "my_Encoder.h"
//配置列表
Encoder_Config_t Encoder_list[Encoder_max];

//GPT高级定时器初始化与启动配置模块，Encoder自定义名称
void Encoder_Init(EncoderType_e Encoder)
{
		Encoder_list[En_L]=(Encoder_Config_t) {GPT_1_channel,&GPT_1_name,&GPT_1_ctrl,GPT_1_OVER_value,0,0,0};
		Encoder_list[En_R]=(Encoder_Config_t) {GPT_2_channel,&GPT_2_name,&GPT_2_ctrl,GPT_2_OVER_value,0,0,0};
//		Encoder_list[En_LQ]=(Encoder_Config_t){GPT_3_channel,&GPT_3_name,&GPT_3_ctrl,GPT_3_OVER_value,0,0,0};
//		Encoder_list[En_RQ]=(Encoder_Config_t){GPT_4_channel,&GPT_4_name,&GPT_4_ctrl,GPT_4_OVER_value,0,0,0};
		
		
		Encoder_Config_t *cfg = &Encoder_list[Encoder];
		fsp_err_t err = FSP_SUCCESS;           						//定义FSP标准错误码变量，初始化为成功状态 （FSP_SUCCESS=0）                									
		err = R_GPT_Open(cfg->name->p_ctrl, cfg->name->p_cfg);// 调用GPT驱动的Open函数，初始化定时器（绑定控制块与配置结构体）																					
		err = R_GPT_CounterSet(cfg->name->p_ctrl,(uint32_t)(cfg->Max)/2);			// 设置GPT0的初始计数值为0x80000000（2^31，32位定时器的中间值）
		if(FSP_SUCCESS != err)														// 判断Open操作是否失败（错误码不等于FSP_SUCCESS时进入分支）
    {
        printf("Encoder_Init_Error : %d\r\n", err);								// 串口打印定时器初始化失败的错误码，方便定位配置/硬件问题
    }
		else													// 判断Open操作是否失败（错误码不等于FSP_SUCCESS时进入分支）
    {
        printf("Encoder_Init_OK : %d\r\n", err);								// 串口打印定时器初始化失败的错误码，方便定位配置/硬件问题
    }
}

bool En_speed_useflag = false;
double En_speed[Encoder_max];

//获取编码器旋转方向与速度函数，Encoder自定义名称，返回码盘值
void Encoder_Read_speed(void)
{
		int i;
		double speed;
		fsp_err_t err = FSP_SUCCESS;											//定义FSP标准错误码变量
		timer_status_t timer_status;											//定义编码寄存器结构体变量，此时变量值=0
		for(i = 0;i < Encoder_max;i++)
		{
			R_GPT_CounterSet(Encoder_list[i].name->p_ctrl,(uint32_t)(Encoder_list[i].Max/2));
			R_GPT_Start(Encoder_list[i].name->p_ctrl);
			err=R_GPT_StatusGet(Encoder_list[i].name->p_ctrl, &timer_status);
			Encoder_list[i].En_val_1 = timer_status.counter;	// 从定时器状态结构体中读取当前计数器值，赋值给全局变量En_val_1			
		}
	
		vTaskDelay(GPT_Delay_ms);
	
		for(i = 0;i < Encoder_max;i++)
		{
			R_GPT_StatusGet(Encoder_list[i].name->p_ctrl, &timer_status);			//读取最新值
			Encoder_list[i].En_val_2 = timer_status.counter;					//赋值给全局变量En_val_2
			R_GPT_Stop(Encoder_list[i].name->p_ctrl);								
			Encoder_list[i].En_delta=(int64_t)((int64_t)Encoder_list[i].En_val_2 - (int64_t)Encoder_list[i].En_val_1);
			speed = ((double)Encoder_list[i].En_delta/(500.0*28.0)*2*PAI*3.4)	/((double)GPT_Delay_ms/1000.0)*1.155;
			En_speed[i] = speed;
			//printf("En%d:%lld\r\n",Encoder,En_delta);
			if(i == 1)
			printf("%f\r\n",speed);
		}
	}

