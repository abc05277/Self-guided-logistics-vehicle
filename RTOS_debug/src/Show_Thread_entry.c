#include "Show_Thread.h"
#include "User_show.h"
#include "debug.h"
#include "EEPROM.h"
#include "HC_04.h"
#include "my_LED.h"
// 外部变量声明（已在 HC_04.c 或 EEPROM.c 中定义）
extern volatile uint8_t Serial_RxFlag;
                /* Show_Thread entry function */
                /* pvParameters contains TaskHandle_t */
                void Show_Thread_entry(void * pvParameters)
                {
                    FSP_PARAMETER_NOT_USED(pvParameters);
										debug_init();           // 调试串口（printf重定向）				
										// ---------- 1. 基础硬件初始化 ----------
										show_init();            // OLED显示及按键初始化（原有功能）

										// ---------- 2. 蓝牙串口初始化 ----------
										Serial_Init();          // 初始化UART8（蓝牙模块）

										// ---------- 3. EEPROM 初始化与数据加载 ----------
										I2CDev *ep = EEPROMGetDevice();
										if (ep != NULL)
										{
														if (ep->Init(ep) == 0)
														{
																		Serial_SendString("[System] EEPROM 初始化成功。\r\n");
																		// 从 EEPROM 加载历史记录到 RAM
																		EEPROM_Load_To_RAM();
														}
														else
														{
																		Serial_SendString("[Error] EEPROM 初始化失败！\r\n");
														}
										}
										else
										{
														Serial_SendString("[Error] 未找到 EEPROM 设备！\r\n");
										}
										
										

                    /* TODO: add your own code here */
                    while(1)
                    {
											// 4.1 更新OLED显示（原有功能，保持实时刷新）
											show_all();
											
											// 4.2 处理蓝牙接收到的指令
											if (Serial_RxFlag == 1)
											{
															// 解析并执行蓝牙指令
															Protocol_Parse_And_Response();

															// 清除接收标志，准备接收下一条
															Serial_RxFlag = 0;
											}

											// 4.3 短暂延时，让出CPU给其他任务
											vTaskDelay(1);
                    }
                }