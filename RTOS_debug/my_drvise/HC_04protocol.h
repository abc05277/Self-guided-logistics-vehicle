#ifndef HC_04PROTOCOL_H
#define HC_04PROTOCOL_H
//所用线程头文件声明，可修改处
#include "Show_Thread.h"
//调试文件
#include <debug.h>
#include "HC_04.h"
#define EE_RECORD_SIZE  5   // 压缩存储：4字节时间戳 + 1字节状态码
#define REG_MAX_STORE   10  // 每台机器物理分配10条空间

// 状态量枚举定义
typedef enum {
    CODE_A_CLEANED  = 0xA1,  // A机器已清洁
    CODE_A_REPAIRED = 0xA2,  // A机器已维修
    CODE_A_MAINTAIN = 0xA3,  // A机器已保养
		CODE_A_SYS_ERROR  = 0xEA,  // A机器设备异常
		CODE_B_CLEANED  = 0xB1,  // B机器已清洁
    CODE_B_REPAIRED = 0xB2,  // B机器已维修
    CODE_B_MAINTAIN = 0xB3,  // B机器已保养
		CODE_B_SYS_ERROR  = 0xEB,  // B机器设备异常
		CODE_C_CLEANED  = 0xC1,  // C机器已清洁
    CODE_C_REPAIRED = 0xC2,  // C机器已维修
    CODE_C_MAINTAIN = 0xC3,  // C机器已保养
		CODE_C_SYS_ERROR  = 0xEC,  // C机器设备异常
} DataCode_e;

// 8字节严格定长数据帧结构体
typedef struct {
    uint8_t  head;         // 包头，固定 0xAA
    uint8_t  cmd;          // 指令，如 0x01 (存储数据)
    uint8_t  length;       // 长度，固定 0x08
    uint8_t  time_bcd[3];  // 3字节时间数据域  [0]=年, [1]=月, [2]=日
    uint8_t  data_bcd[1];  // 1字节状态量数据域 (大端存储的 DataCode_e)
    uint8_t  tail;         // 包尾，固定 0xCC 
} Protocol_Frame_t;










#endif
