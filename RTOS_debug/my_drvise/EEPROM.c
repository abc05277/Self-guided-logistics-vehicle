#include "EEPROM.h"

//回调函数与等待机制
static volatile bool gI2CTxCplt = false; 
static volatile bool gI2CRxCplt = false; 
void sci_i2c4_master_callback(i2c_master_callback_args_t *p_args) 
{ 
		if(I2C_MASTER_EVENT_RX_COMPLETE == p_args->event) 
		{ 
				gI2CRxCplt = true; 
		} 
		else if(I2C_MASTER_EVENT_TX_COMPLETE == p_args->event) 
		{ 
				gI2CTxCplt = true; 
		} 
} 

//等待发送完成
void I2C4WaitTxCplt(void) 
{ 
		uint16_t wTimeout = 50; 
		while(!gI2CTxCplt && wTimeout) 
		{ 
				R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS); 
				wTimeout--; 
		} 
		gI2CTxCplt = false; 
} 
//等待接收完成
void I2C4WaitRxCplt(void) 
{ 
		uint16_t wTimeout = 50; 
		while(!gI2CRxCplt && wTimeout) 
		{ 
				R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS); 
				wTimeout--; 
		} 
		gI2CRxCplt = false; 
} 

//I2C设备初始化函数
int EEPROMDrvInit(struct I2CDev* ptDev) 
{ 
		if(NULL == ptDev->name) return -1; 
		fsp_err_t err = g_i2c4.p_api->open(g_i2c4.p_ctrl, g_i2c4.p_cfg); 
		if(FSP_SUCCESS == err) 
		{ 
				printf("Success to open device: i2c4!\r\n"); 
				return 0; 
		} 
		else 
		printf("Failed to open device: i2c4!\r\n"); 

		return -1; 
}

static I2CDev gEepromDev = 
{ 
		.name = "EEPROM", 
		.Init = EEPROMDrvInit, 
		.Write = EEPROMDrvWriteBuff, 
		.Read = EEPROMDrvRead 
};

struct I2CDev* EEPROMGetDevice(void) 
{ 
		return &gEepromDev; 
}

//EEPROM 单字节写函数
int EEPROMDrvWriteByte(struct I2CDev* ptDev, unsigned char ucAddr, unsigned char const ucData) 
{ 
		if(NULL == ptDev->name) return -1; 

		uint16_t wData = (uint16_t)((ucData<<8) | ucAddr); 
		/* 发送一个字节的地址数据 */ 
		g_i2c4.p_api->write(g_i2c4.p_ctrl, (unsigned char*)&wData, (unsigned int)2, 
		false); 
		I2C4WaitTxCplt(); 

		return 0; 
} 

/*
////EEPROM 页写函数
//int EEPROMDrvWritePage(struct I2CDev* ptDev, unsigned char ucAddr, unsigned char const* wbuf, unsigned int dwSize) 
//{ 
//		if(NULL == ptDev->name) return -1; 
//		unsigned char data[9] = {0}; 
//		data[0] = ucAddr; 
//		for(unsigned char i=0; i<dwSize; i++) 
//		{ 
//				data[i+1] = wbuf[i]; 
//		} 
//		g_i2c4.p_api->write(g_i2c4.p_ctrl, (unsigned char*)data, (unsigned int)(dwSize+1), false); 
//		I2C4WaitTxCplt(); 
//		R_BSP_SoftwareDelay((dwSize+1)*5, BSP_DELAY_UNITS_MILLISECONDS); 
//		return 0; 
//} 
*/
// EEPROM 页写函数 - 终极安全重构版
int EEPROMDrvWritePage(struct I2CDev* ptDev, unsigned char ucAddr, unsigned char const* wbuf, unsigned int dwSize) 
{ 
    if(NULL == ptDev->name) return -1; 
    
    // 1. 严格防御编程：单页物理写入绝对不能超过 EEPROM 页大小（24C02最大为8字节）
    if (dwSize > EE_PAGE_SIZE)
    {
        dwSize = EE_PAGE_SIZE; 
    }

    // 2. 将缓冲区扩大到安全边界（1字节地址 + 8字节数据 = 9字节，定义16字节防止任何溢出风险）
    unsigned char data[16] = {0}; 
    
    data[0] = ucAddr; 
    for(unsigned char i = 0; i < dwSize; i++) 
    { 
        data[i+1] = wbuf[i]; 
    } 
    
    // 3. 执行物理写入
    fsp_err_t err = g_i2c4.p_api->write(g_i2c4.p_ctrl, (unsigned char*)data, (unsigned int)(dwSize + 1), false); 
    if (err != FSP_SUCCESS)
    {
        return -1;
    }
    
    // 4. 等待硬件发送完毕
    I2C4WaitTxCplt(); 
    
    // 5. 换用标准、稳固的死延时，规避参数相乘被编译器优化的 Bug
    R_BSP_SoftwareDelay(8, BSP_DELAY_UNITS_MILLISECONDS); 
    
    return 0; 
}

//综合写函数
int EEPROMDrvWriteBuff(struct I2CDev* ptDev, unsigned char ucAddr, unsigned char const* wbuf, unsigned int dwSize) 
{ 
		if(NULL == ptDev->name) return -1; 

		if(dwSize == 1) 
		return EEPROMDrvWriteByte(ptDev, ucAddr, *wbuf); 

		/* 如果从当前地址开始写 size 字节会超过 EEPROM 的容量则返回错误值-1 */ 
		if((ucAddr+dwSize) > 256) return -1; 

		unsigned char nAddr = ucAddr; 
		/* 不管从何处开始，都将从起始地址开始把所在页写满 */ 
		/* 或者不会写满的情况下，从起始位置开始写 size 个字节 */ 
		unsigned char ucSize; /* 当前写入的数据个数 */ 
		if( (ucAddr==0 || (ucAddr/EE_PAGE_SIZE)!=0) && (dwSize<=EE_PAGE_SIZE)) 
		ucSize = (unsigned char)dwSize;
		else 
		ucSize = EE_PAGE_SIZE - (ucAddr % EE_PAGE_SIZE); 
		EEPROMDrvWritePage(ptDev, nAddr, wbuf, ucSize); 

		/* 写满起始位置开始的那一页之后，要将位置、数据地址和个数进行偏移计算 */ 
		nAddr += ucSize; 
		wbuf += ucSize; 
		dwSize -= ucSize; 

		/* 如果写满起始地址所在页后还有数据，就进行下一步处理 */ 
		while(dwSize != 0) 
		{ 
				/* 如果剩余数据个数大于一页的个数，就写满下一页 */ 
				/* 否则将剩余数据全部写到下一页 */ 
				if(dwSize <= EE_PAGE_SIZE) 
				ucSize = (unsigned char)dwSize; 
				else 
				ucSize = EE_PAGE_SIZE; 

				EEPROMDrvWritePage(ptDev, nAddr, wbuf, ucSize); 
				/* 继续偏移 */ 
				nAddr += ucSize; 
				wbuf += ucSize; 
				dwSize -= ucSize; 
		} 
		return 0; 
} 

//EEPROM 读函数
int EEPROMDrvRead(struct I2CDev* ptDev, unsigned char ucAddr, unsigned char* rbuf, unsigned int dwSize) 
{ 
		if(NULL == ptDev->name) return -1; 

		/* 发送一个字节的地址数据 */ 
		g_i2c4.p_api->write(g_i2c4.p_ctrl, (unsigned char*)&ucAddr, (unsigned int)1, true); 
		I2C4WaitTxCplt(); 
		/* 读取该地址的一个字节数据 */ 
		R_BSP_SoftwareDelay(300, BSP_DELAY_UNITS_MICROSECONDS); 
		g_i2c4.p_api->read(g_i2c4.p_ctrl, (unsigned char*)rbuf, (unsigned int)dwSize, false); 
		I2C4WaitRxCplt(); 
		return 0; 
} 


//EEPROM 读写测试程序
void EEPROMAppTest(void) 
{ 
		I2CDev *ptEepromDev = EEPROMGetDevice(); 
		if(NULL == ptEepromDev) return; 
		if(0 != ptEepromDev->Init(ptEepromDev)) return; 
		uint8_t ucCount = 10; 
		printf("Start Test Simple I2C Write/Read a Byte From AT24C02\r\n"); 
		printf("\t| Write | Read | Result | Count |\r\n"); 
		while(ucCount) 
		{ 
				uint8_t ucWData = (uint8_t)rand(); 
				uint8_t ucRData = 0; 
				uint8_t addr = (uint8_t)rand(); 
				ptEepromDev->Write(ptEepromDev, addr, &ucWData, 1); 
				R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS); 
				ptEepromDev->Read(ptEepromDev, addr, &ucRData, 1); 
				R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS); 
				printf("\t| %.3d | %.3d |", ucWData, ucRData); 
				if(ucWData == ucRData) 
						printf(" %s | %.2d |\r\n", "Success", ucCount); 
				else 
						printf(" %s | %.3d |\r\n", "Error", ucCount); 
						ucCount--; 
		} 
				printf("Start Test Simple I2C Write/Read n Bytes From AT24C02\r\n"); 
				printf("\t| Address | Size | Result | Count |\r\n"); 
				ucCount = 5; 
				uint8_t wbuf[256]; 
				uint8_t rbuf[256]; 
				while(ucCount) 
				{ 
						uint8_t ucAddr = (uint8_t)rand(); 
						uint8_t size = (uint8_t)rand(); 
						for(uint16_t i=0; i<256; i++) 
						{ 
								wbuf[i] = (uint8_t)i; 
						} 
						size = (uint8_t)(((ucAddr+size)>256)?(256-ucAddr):size); 
						printf("\t| %.3d | %.3d |", ucAddr, size); 
						ptEepromDev->Write(ptEepromDev, ucAddr, wbuf, size); 
						R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS); 
						ptEepromDev->Read(ptEepromDev, ucAddr, rbuf, size); 
						uint16_t err_c = 0; 
						for(uint16_t i=0; i<size; i++) 
						{ 
								if(rbuf[i] != wbuf[i]) 
								{ 
										err_c++; 
								} 
						} 
						if(0 == err_c) 
						printf(" %s | %.2d |\r\n", "Success", ucCount); 
						else
						printf(" %s | %.3d |\r\n", "Error", err_c); 
						ucCount--; 
						R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS); 
		} 
} 




/**
 * @brief  擦除 EEPROM 指定地址开始的指定长度空间
 * @param  ptDev  : I2C 设备指针
 * @param  ucAddr : 起始地址
 * @param  dwSize : 擦除的字节大小
 * @return int    : 0 成功，-1 失败
 */
int EEPROM_Erase_Range(struct I2CDev* ptDev, unsigned char ucAddr, unsigned int dwSize)
{
    if (ptDev == NULL || dwSize == 0) return -1;

    // 分配临时缓冲区
    unsigned char *erase_buf = (unsigned char *)malloc(dwSize);
    if (erase_buf == NULL) return -1; 
    
    memset(erase_buf, 0xFF, dwSize);

    // 调用驱动层写入函数。注意：由于底层有 (ucAddr + dwSize) >= 256 的拦截限制，
    // 确保传入的单个区段地址+大小不突破 255 物理边界。
    int status = ptDev->Write(ptDev, ucAddr, erase_buf, dwSize);
    
    free(erase_buf);
    return status;
}

/**
 * @brief  一键清空整片 EEPROM（全芯片擦除模拟）
 * @param  ptDev  : I2C 设备指针
 * @return int    : 0 成功，-1 失败
 */
int EEPROM_Erase_Chip(struct I2CDev* ptDev)
{
    if (ptDev == NULL) return -1;

    unsigned char erase_data[8];
    memset(erase_data, 0xFF, 8); // AT24C02 单页最大 8 字节

    // 256 字节 = 32 页 (每页 8 字节)
    for (unsigned char page = 0; page < 32; page++)
    {
        unsigned char ucAddr = (unsigned char)(page * 8);

        // 直接调用最底层的页写函数，彻底绕过高级 Buff 驱动的各种判断
        if (EEPROMDrvWritePage(ptDev, ucAddr, erase_data, 8) != 0)
        {
            // 如果在某一页写入时失败，直接返回具体的页码（Debug 用）
            return (int)(-100 - page); 
        }
    }

    return 0; // 完美擦除
}














