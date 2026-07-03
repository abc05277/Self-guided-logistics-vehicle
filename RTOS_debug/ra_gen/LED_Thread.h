/* generated thread header file - do not edit */
#ifndef LED_THREAD_H_
#define LED_THREAD_H_
#include "bsp_api.h"
                #include "FreeRTOS.h"
                #include "task.h"
                #include "semphr.h"
                #include "hal_data.h"
                #ifdef __cplusplus
                extern "C" void LED_Thread_entry(void * pvParameters);
                #else
                extern void LED_Thread_entry(void * pvParameters);
                #endif
FSP_HEADER
FSP_FOOTER
#endif /* LED_THREAD_H_ */
