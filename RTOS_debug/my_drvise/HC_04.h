#ifndef _HC_04_H
#define _HC_04_H
//多用线程头文件声明
#include "Show_Thread.h"
//调试文件
#include <debug.h>

#include "User_show.h"

//统一配置结构体
typedef struct 
{
	uint8_t channel;
	const uart_instance_t *name;
	
	
}HC_04_Config_t;

//蓝牙串口模块配置
//#define uart_2_Name       名称
//#define uart_2_Channel    通道
//#define uart_2_Baud_Rate  波特率
//#define uart_2_Callback   回调函数名
//#define uart_2_RXD2       接收引脚
//#define uart_2_TXD2       发送引脚

//蓝牙串口模块配置
#define uart_8_Name       g_uart8
#define uart_8_Channel    8
#define uart_8_Baud_Rate  460800
#define uart_8_Callback   uart8_callback
#define uart_8_RXD2       P607
#define uart_8_TXD2       PA00



// 定义记录结构体
/**
 * @brief 简易单条记录结构体
 * @param timestamp 32位无符号时间戳，Unix时间/自定义日期数字
 * @param description[50] 备注字符数组，最大有效49字符+末尾'\0'
 */
typedef struct {
    uint32_t timestamp;
    char description[50];
} SimpleRecord_t;


// 每台机器独立的环形缓冲区与状态字典映射结构体
#define MAX_RECORDS 10  // 【优化】单台机器历史记录深度扩大到 10 条

typedef struct {
    char name;                                 // 机器代号：'A', 'B', 'C'
    SimpleRecord_t records[MAX_RECORDS];       // 独立的记录存储区
    int head_idx;                              // 环形队列写入头指针
    int valid_count;                           // 当前有效记录总数（最大为 MAX_RECORDS）
    uint16_t ee_base_addr;                     // 【新增】每台机器在EEPROM内部的物理起始基地址
    // 状态量与中文映射字典
    struct {
        uint8_t code;
        const char *desc;
    } dict[4];                                 // 每台机器对应 4 种状态
} Machine_Control_t;


void Serial_Init(void);
void uart_8_Callback(uart_callback_args_t *p_args);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void StoreRecord(uint32_t ts, const char *desc);
void ShowRecords(void);
// 声明外部可调用的流程函数
void Protocol_Parse_And_Response(void);
void EEPROM_Load_To_RAM(void);
void StoreRecord_Sync(Machine_Control_t *m, uint32_t ts, uint8_t code, const char *desc);
#endif
