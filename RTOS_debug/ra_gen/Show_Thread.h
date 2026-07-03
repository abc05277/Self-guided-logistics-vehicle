/* generated thread header file - do not edit */
#ifndef SHOW_THREAD_H_
#define SHOW_THREAD_H_
#include "bsp_api.h"
                #include "FreeRTOS.h"
                #include "task.h"
                #include "semphr.h"
                #include "hal_data.h"
                #ifdef __cplusplus
                extern "C" void Show_Thread_entry(void * pvParameters);
                #else
                extern void Show_Thread_entry(void * pvParameters);
                #endif
#include "r_sci_uart.h"
            #include "r_uart_api.h"
#include "r_sci_i2c.h"
#include "r_i2c_master_api.h"
#include "r_iic_master.h"
#include "r_i2c_master_api.h"
FSP_HEADER
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart8;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_uart_instance_ctrl_t     g_uart8_ctrl;
            extern const uart_cfg_t g_uart8_cfg;
            extern const sci_uart_extended_cfg_t g_uart8_cfg_extend;

            #ifndef uart8_callback
            void uart8_callback(uart_callback_args_t * p_args);
            #endif
extern const i2c_master_cfg_t g_i2c4_cfg;
/* I2C on SCI Instance. */
extern const i2c_master_instance_t g_i2c4;
#ifndef sci_i2c4_master_callback
void sci_i2c4_master_callback(i2c_master_callback_args_t * p_args);
#endif

extern const sci_i2c_extended_cfg_t g_i2c4_cfg_extend;
extern sci_i2c_instance_ctrl_t g_i2c4_ctrl;
/* I2C Master on IIC Instance. */
extern const i2c_master_instance_t g_i2c_1;

/** Access the I2C Master instance using these structures when calling API functions directly (::p_api is not used). */
extern iic_master_instance_ctrl_t g_i2c_1_ctrl;
extern const i2c_master_cfg_t g_i2c_1_cfg;

#ifndef i2c2_callback
void i2c2_callback(i2c_master_callback_args_t * p_args);
#endif
FSP_FOOTER
#endif /* SHOW_THREAD_H_ */
