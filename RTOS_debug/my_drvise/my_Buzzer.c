#include "my_Buzzer.h"

const uint16_t music_data[7];


void Morse(uint8_t data)
{
	vTaskDelay(500);
	int i;
	for(i = 0 ;i < 5;i++)
	{
		R_IOPORT_PinWrite(g_ioport.p_ctrl,BUZZER_PIN,1);
		if(Morse_data[data][i])
			vTaskDelay(HIGH_TIME);
		else
			vTaskDelay(LOW_TIME);
		R_IOPORT_PinWrite(g_ioport.p_ctrl,BUZZER_PIN,0);
		vTaskDelay(FEER_TIME);
	}
}

// void	music_one(uint8_t data)
// {
//	 uint16_t i;
//			for(i = 0;i < 200;i++)
//			{
//				R_IOPORT_PinWrite(g_ioport.p_ctrl,BUZZER_PIN,1);
//				R_BSP_SoftwareDelay(music_data[data], BSP_DELAY_UNITS_MICROSECONDS);
//				R_IOPORT_PinWrite(g_ioport.p_ctrl,BUZZER_PIN,0);
//				R_BSP_SoftwareDelay(music_data[data], BSP_DELAY_UNITS_MICROSECONDS);
//			}
// }

//const uint16_t music_data[7]={956,851,758,716,637,568,506};

const uint8_t Morse_data[10][5]={
	{1,1,1,1,1},
	{0,1,1,1,1},
	{0,0,1,1,1},
	{0,0,0,1,1},
	{0,0,0,0,1},
	{0,0,0,0,0},
	{1,0,0,0,0},
	{1,1,0,0,0},
	{1,1,1,0,0},
	{1,1,1,1,0}
};




