#ifndef __EEPROM_H__
#define __EEPROM_H__

//调试文件勿动!!!!
#include "debug.h"
#include "Show_Thread.h"



// EEPROM 页大小（字节），例如 AT24C02 为 8，AT24C32 为 32
#ifndef EE_PAGE_SIZE
#define EE_PAGE_SIZE    8
#endif

// EEPROM 容量（字节），用于地址范围检查（默认 256 字节，即 24C02）
#ifndef EE_TOTAL_SIZE
#define EE_TOTAL_SIZE   256
#endif

// EEPROM 设备地址（7 位地址，通常为 0x50，实际 I2C 发送时为 0xA0/0xA1）
#ifndef EE_DEV_ADDR
#define EE_DEV_ADDR     0x50
#endif

// ================================
//  设备结构体定义
// ================================
typedef struct I2CDev
{ 
		char *name; 
		int (*Init)(struct I2CDev *ptDev); 
		int (*Write)(struct I2CDev *ptDev, unsigned char ucAddr, unsigned char const *wBuf, unsigned int dwSize); 
		int (*Read)(struct I2CDev *ptDev, unsigned char ucAddr, unsigned char *rBuf, 
		unsigned int dwSize); 
}I2CDev, *PI2CDev;
void sci_i2c4_master_callback(i2c_master_callback_args_t *p_args);


// 获取设备对象
struct I2CDev* EEPROMGetDevice(void);
void I2C4WaitTxCplt(void);
void I2C4WaitRxCplt(void) ;
int EEPROMDrvInit(struct I2CDev* ptDev) ;
int EEPROMDrvWriteByte(struct I2CDev* ptDev, unsigned char ucAddr, unsigned char const ucData);
int EEPROMDrvWritePage(struct I2CDev* ptDev, unsigned char ucAddr, unsigned char const* wbuf, unsigned int dwSize) ;
int EEPROMDrvWriteBuff(struct I2CDev* ptDev, unsigned char ucAddr, unsigned char const* wbuf, unsigned int dwSize) ;
int EEPROMDrvRead(struct I2CDev* ptDev, unsigned char ucAddr, unsigned char* rbuf, unsigned int dwSize) ;
void EEPROMAppTest(void);
int EEPROM_Erase_Range(struct I2CDev* ptDev, unsigned char ucAddr, unsigned int dwSize);
int EEPROM_Erase_Chip(struct I2CDev* ptDev);

#endif