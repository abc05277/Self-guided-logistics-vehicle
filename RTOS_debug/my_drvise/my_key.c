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
导航键读取函数

在rasc内pins配置GPIO
pins->Ports->px->pxxx
mode input
Pull up		input pull-up
output type CMOS

*/



#include "my_key.h"

bsp_io_port_pin_t Key_list[Key_max];
bool Key_flag[Key_max] = {false, false, false, false, false};



void Key_init(void)
{
	Key_list[Key_U] = KEY_U_PIN;
	Key_list[Key_D] = KEY_D_PIN;
	Key_list[Key_L] = KEY_L_PIN;
	Key_list[Key_R] = KEY_R_PIN;
	Key_list[Key_I] = KEY_I_PIN;
}

Key_NumT Key_Read(void)
{
	bsp_io_level_t  temp;
	Key_NumT key_num = Key_Kong_Num;
	int i;
	int delay = 500;
	
	for(i = 0;i < Key_max;i++)
	{
		R_IOPORT_PinRead(&g_ioport_ctrl,Key_list[i],&temp);
		
		if(temp != BSP_IO_LEVEL_HIGH	&&	Key_flag[i] == false)
		{
			Key_flag[i] = true;
			key_num = (Key_NumT)i;
			break;
		}
		if(temp != BSP_IO_LEVEL_HIGH	&&	Key_flag[i] == true)
		{
			temp = BSP_IO_LEVEL_HIGH;
			break;
		}

		else if(temp == BSP_IO_LEVEL_HIGH)
		{
			Key_flag[i] = false;
		}
	}
	if(!temp)
	{

		while(!temp	&&	delay>0	&&	i < Key_max)
		{
			delay--;
			R_IOPORT_PinRead(&g_ioport_ctrl,Key_list[i],&temp);
			vTaskDelay(1);
		}
	}
	if(delay <= 0	&&	key_num == Key_O_Num)
		key_num = Key_I_Num;
	
	return key_num;
}


