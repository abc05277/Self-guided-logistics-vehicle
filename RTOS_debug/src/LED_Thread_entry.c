#include "LED_Thread.h"
#include "my_LED.h"
#include "my_Buzzer.h"
                /* LED_Thread entry function */
                /* pvParameters contains TaskHandle_t */
                void LED_Thread_entry(void * pvParameters)
                {
                    FSP_PARAMETER_NOT_USED(pvParameters);
										LED_Init();
                    /* TODO: add your own code here */
                    while(1)
                    {
												LED_do();

                    }
                }
