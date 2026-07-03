#include "my_UWB.h"
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
Baud Rate	115200		波特率

*/
//R -- 502
//T -- 501





static volatile int	g_uart5_tx_complete;
static volatile int	g_uart5_rx_complete;

//状态
unsigned char UWB_Content[70];
uint8_t  UWB_Content_flag = 0;	//存入指针
uint8_t  UWB_Content_stact = 0;	//状态标志

//数据
SemaphoreHandle_t  UWB_data_useflag;
unsigned char UWB_data[80];
uint8_t  UWB_data_flag = 0;	//存入指针
uint8_t  UWB_data_stact = 0;//状态标志

//等待发送阻塞
void UWB_wait_for_tx(void)
{
	while (!g_uart5_tx_complete);
	g_uart5_tx_complete = 0;
}


//等待读取阻塞
void UWB_wait_for_rx(void)
{
	while (!g_uart5_rx_complete);
	g_uart5_rx_complete = 0;
}

/*
发送函数
p_src 发送数组首地址
bytes	发送数据位数

*/
void UWB_uart_send(uint8_t const * const p_src, uint32_t const bytes)
{
	UWB_1_UART.p_api->write(UWB_1_UART.p_ctrl,p_src,bytes);
	imu_wait_for_tx();
}

/*
读取函数
p_src 读取数组首地址
bytes	读取数据位数

*/


void UWB_uart_read( uint8_t * const p_dest, uint32_t const bytes)
{
	UWB_1_UART.p_api->read(UWB_1_UART.p_ctrl, p_dest, bytes);
	imu_wait_for_rx();
}

//初始化函数
void UWB_init(void)
{
	UWB_data_useflag = xSemaphoreCreateMutex();
	UWB_1_UART.p_api->open(UWB_1_UART.p_ctrl, UWB_1_UART.p_cfg);
}


//中断回调数据存储
void UWB_1_CALLBACK(uart_callback_args_t * p_args)
{
	switch (p_args->event)
	{		
		case UART_EVENT_TX_COMPLETE:
		{
			g_uart5_tx_complete = 1;
			break;
		}
		case UART_EVENT_RX_COMPLETE:
		{
			g_uart5_rx_complete = 1;
			break;
		}
		case UART_EVENT_RX_CHAR:
		{
			if(p_args->data == '$'	&&	UWB_data_stact == 0	&&	UWB_Content_stact == 0)
			{
				UWB_data_stact = 0;
				UWB_Content_stact = 1;
				UWB_Content_flag = 0;
				UWB_Content[UWB_Content_flag++] = (uint8_t)p_args->data;
			}
			else if(p_args->data == 'm'	&&	UWB_data_stact == 0	&&	UWB_Content_stact == 0)
			{
				UWB_Content_stact = 0;
				UWB_data_stact = 1;
				UWB_data_flag = 0;
				UWB_data[UWB_data_flag++] = (uint8_t)p_args->data;
			}
			else if(p_args->data == '\n')
			{
				if(UWB_Content_stact)
				{
					UWB_Content[UWB_Content_flag++] = (uint8_t)p_args->data;
					UWB_Content_flag = 0;
					UWB_Content_stact = 0;
				}
				else if(UWB_data_stact)
				{
					UWB_data[UWB_data_flag++] = (uint8_t)p_args->data;
					UWB_data_flag = 0;
					UWB_data_stact = 0;
				}
			}
			else
			{
				if(UWB_Content_stact)
				{
					UWB_Content[UWB_Content_flag++] = (uint8_t)p_args->data;
				}
				else if(UWB_data_stact)
				{
					UWB_data[UWB_data_flag++] = (uint8_t)p_args->data;
				}
			}
			
			break;
		}
		default:
		{
			break;
		}
	}
}

