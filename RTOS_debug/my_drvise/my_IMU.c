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
	惯导模块
	在rasc里配置uart

	pins->SCI->SCIx
	mode		asy...	uart

	stacks New stack->connect...->r_sci_uart

	module
	->general
	name  		g_uartx		名字
	channel 	同pins		通道
	bits			8					数据位
	paritiy		none			
	stop	bits	1bit		停止位

	->Baud
	Baud Rate	961200		波特率
*/

#include "my_IMU.h"


//	R 601--
//	T 602--

//发送数组
uint8_t IMU_T[270] = {0};
//读取数组
uint8_t IMU_R[270]= {0};
//校验位数组
uint8_t	IMU_X[2] = {0};
//校准数组
uint8_t IMU_RET[2][25] = {
{0x59,0x53,0x01,0x09,0x00,0x02,0x0C,0x21},
{0x59,0x53,0x05,0x32,0x00,0x11,0x04,0x00,0x00,0x00,0x00,0x4C,0x37}
};

SemaphoreHandle_t ol_yaw_useflag;
SemaphoreHandle_t deg_speed_useflag;
//欧拉角存储pitch,roll,yaw		俯仰角,翻滚角,偏航角
double ol_yaw[3];

SemaphoreHandle_t ol_first_useflag;
double ol_first[3];

//角速度存储	x,y,z	逆时针正
double deg_speed[3];

static volatile int g_uart9_tx_complete = 0;
static volatile int g_uart9_rx_complete = 0;

//中断回调
void IUM_1_UART_CALLBACK(uart_callback_args_t * p_args)
{
	switch (p_args->event)
	{
		case UART_EVENT_TX_COMPLETE:
		{
			g_uart9_tx_complete = 1;
			break;
		}
		case UART_EVENT_RX_COMPLETE:
		{
			g_uart9_rx_complete = 1;
			break;
		}
		default:
		{
			break;
		}
	}
}

////等待发送阻塞
void imu_wait_for_tx(void)
{
	while (!g_uart9_tx_complete);
	g_uart9_tx_complete = 0;
}



//等待读取阻塞
void imu_wait_for_rx(void)
{
	while (!g_uart9_rx_complete);

	g_uart9_rx_complete = 0;
}


/*
发送函数
p_src 发送数组首地址
bytes	发送数据位数

*/
void imu_uart_send(uint8_t const * const p_src, uint32_t const bytes)
{
	IUM_1_UART.p_api->write(IUM_1_UART.p_ctrl,p_src,bytes);
	imu_wait_for_tx();
}


/*
读取函数
p_src 读取数组首地址
bytes	读取数据位数

*/
void imu_uart_read( uint8_t * const p_dest, uint32_t const bytes)
{
	IUM_1_UART.p_api->read(IUM_1_UART.p_ctrl, p_dest, bytes);
	imu_wait_for_rx();
}


//初始化
void IMU_init(void)
{
	ol_yaw_useflag = xSemaphoreCreateMutex();
	ol_first_useflag = xSemaphoreCreateMutex();
	deg_speed_useflag = xSemaphoreCreateMutex();
	
	IUM_1_UART.p_api->open(IUM_1_UART.p_ctrl, IUM_1_UART.p_cfg);
}


/*
校验函数,自适应数据包长
p_src		数据包头地址
*/
void IMU_XY(uint8_t const * const p_src)
{
	int i;
	uint8_t CK1 = 0,CK2 = 0;
	bool bag_f = false;
	if(p_src[0] == 0x59&&p_src[1] == 0x53)
		bag_f = true;
	uint8_t bag_len = p_src[4];
	for(i = 0;i < bag_len+3;	i++)
	{
		CK1 = CK1 + p_src[2+i];
		CK2 = CK2 + CK1;
	}
	IMU_X[0] = CK1;
	IMU_X[1] = CK2;
}


//读取函数,并解析数据存入对应数组
int IMU_Read(void)
{
	xSemaphoreTake(ol_yaw_useflag,portMAX_DELAY);
	xSemaphoreTake(deg_speed_useflag,portMAX_DELAY);
	int i,j;
	uint8_t ID = 0x00,ID_len,ID_f = 5;
	do{
		imu_uart_read(&IMU_R[0],2);
	}while(!((IMU_R[0] == 0x59)&&(IMU_R[1] == 0x53)));

	imu_uart_read(&IMU_R[2],3);
	uint8_t bag_len = IMU_R[4];
	imu_uart_read(&IMU_R[5],bag_len+2);
	IMU_XY(IMU_R);
	if(IMU_R[4+bag_len+1]== IMU_X[0]&&IMU_R[4+bag_len+2]== IMU_X[1])
	{
//		printf("read_ok\r\n");
		
		do{
			ID = IMU_R[ID_f];
			ID_len = IMU_R[ID_f+1];
			ID_f = ID_f+ID_len+2;
		}while(ID != 0x40);
		
		
		ID_f = ID_f-ID_len-2+2;
		
		for(j = 0;j<3;j++)
		{
			for(i = 0;i < 4;i++)
			{
				ol_yaw[j] += ((int32_t)IMU_R[ID_f]<<(8*i)	);
				ID_f++;
			}
			ol_yaw[j] = ol_yaw[j] * 0.000001;
//			printf("shu%d:%.2f\r\n",j,shu[j]);
		}
		
		ID_f = 5;
			do{
			ID = IMU_R[ID_f];
			ID_len = IMU_R[ID_f+1];
			ID_f = ID_f+ID_len+2;
		}while(ID != 0x20);
		ID_f = ID_f-ID_len-2+2;

		for(j = 0;j<3;j++)
		{
			for(i = 0;i < 4;i++)
			{
				deg_speed[j] += ((int32_t)IMU_R[ID_f]<<(8*i)	);
				ID_f++;
			}
			deg_speed[j] = deg_speed[j] * 0.000001;
//			printf("shu%d:%.2f\r\n",j,shu[j]);
		}
		xSemaphoreGive(ol_yaw_useflag);
		xSemaphoreGive(deg_speed_useflag);

		return 0;
	}
	else
	{
		xSemaphoreGive(ol_yaw_useflag);
		xSemaphoreGive(deg_speed_useflag);
		printf("read_err");
		return -1;
	}
}

