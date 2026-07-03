#include "my_ultrasonic_ADC.h"

typedef struct{
	const adc_instance_t * ADC_name;
	double sum;
}ult_ADC_Config_t;

ult_ADC_Config_t ult_ADC_list[ult_ADC_max];

void ADCWaitConvCplt(ult_ADC_Type_e ult)
{
	adc_status_t status;
	status.state = ADC_STATE_SCAN_IN_PROGRESS;
	while (ADC_STATE_SCAN_IN_PROGRESS == status.state)
	{
		ult_ADC_list[ult].ADC_name->p_api->scanStatusGet(ult_ADC_list[ult].ADC_name->p_ctrl, &status);
	}
	
}

int ADCDrvRead(ult_ADC_Type_e ult,uint16_t *value, unsigned short num)
{
		for(uint16_t i=0; i<num; i++)
		{
			ult_ADC_list[ult].ADC_name->p_api->scanStart(ult_ADC_list[ult].ADC_name->p_ctrl);
			ADCWaitConvCplt(ult);
			ult_ADC_list[ult].ADC_name->p_api->read(ult_ADC_list[ult].ADC_name->p_ctrl, ADC_CHANNEL_5, &value[i]); 
		}
	return 0;
}

void ult_ADC_init(ult_ADC_Type_e ult)
{
	ult_ADC_list[ult_1_ADC] = (ult_ADC_Config_t){&ult_1_ADC_name,0};
	
	ult_ADC_list[ult].ADC_name->p_api->open(ult_ADC_list[ult].ADC_name->p_ctrl, ult_ADC_list[ult].ADC_name->p_cfg);
	ult_ADC_list[ult].ADC_name->p_api->scanCfg(ult_ADC_list[ult].ADC_name->p_ctrl, ult_ADC_list[ult].ADC_name->p_channel_cfg);
	R_IOPORT_PinWrite(g_ioport.p_ctrl, ult_1_Range,ult_1_ADC_HL);
}

double ult_ADC_Read(ult_ADC_Type_e ult)
{
	uint16_t ult_ADC_sum[ult_1_number];
	double Mean = 0.0;
	double range = 0.0;
	
	int i;
	ADCDrvRead(ult,ult_ADC_sum,ult_1_number);
	for(i = 0;i < ult_1_number;i++)
	{
		Mean += ult_ADC_sum[i]/(double)ult_1_number;
	}
	if(ult_1_ADC_HL == BSP_IO_LEVEL_HIGH)
		range = (Mean/4096*3.3)/5.0*3.0*100;
	else if(ult_1_ADC_HL == BSP_IO_LEVEL_LOW)
		range = (Mean/4096*3.3)/5.0*1.0*100;
	return range;
}





