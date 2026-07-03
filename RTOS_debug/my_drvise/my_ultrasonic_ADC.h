#ifndef __MY_ULTRASONIC_H__
#define __MY_ULTRASONIC_H__
#include "Car_Thread.h"
#include "debug.h"

typedef enum{
	ult_1_ADC = 0,
	
	ult_ADC_max
}ult_ADC_Type_e;


#define ult_1_ADC_name	g_adc5
#define ult_1_ADC_HL		BSP_IO_LEVEL_LOW												//BSP_IO_LEVEL_LOW	1m,				BSP_IO_LEVEL_HIGH	3m
#define ult_1_Range 		BSP_IO_PORT_08_PIN_06
#define	ult_1_number		1


void ult_ADC_init(ult_ADC_Type_e ult);
double ult_ADC_Read(ult_ADC_Type_e ult);

#endif

