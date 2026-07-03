#include "HC_04.h"
#include "HC_04Protocol.h"
#include "my_servo.h"
#include "EEPROM.h"
#include "car.h"

/*蓝牙串口模块
在RASC中PINS配置uart
stack线程里New stacks->Connectivity->(r_sci_uart)

General
设置name为g_uart2
Channel     2   						 通道
Data Bits   8bit     				数据位
Baud        115200   				波特率
Callback    uart2_callback  回调函数名称

Pins
RXD2   				p301   
TXD2   				p302   
*/

HC_04_Config_t cfg;

// 全局变量定义
volatile uint8_t Serial_RxFlag = 0; // 串口接收完成标志，1表示已收到一个完整数据包
char Serial_RxPacket[100];          // 存储接收到的数据包内容（不含起始符'@'和结束符'#'），以'\0'结尾，最大长度100字节

volatile uint16_t rx_data = 999; // 默认给个999代表还没有事件

// 接收状态机变量（静态）
uint8_t rx_state = 0;          // 0:等待'@'; 1:接收数据包内容
uint16_t rx_index = 0;					//接收缓冲区下标

// 蓝牙串口初始化
void Serial_Init(void)
{
	cfg = (HC_04_Config_t){uart_8_Channel,&uart_8_Name};
	fsp_err_t err;
	err=g_uart8.p_api->open(cfg.name->p_ctrl, cfg.name->p_cfg);
	if (err != FSP_SUCCESS)
  {
     printf("err = %d\n", err);      // 打印为十进制整数
  }

}


static volatile bool uart_tx_complete = false;// 串口发送完成标志（用于阻塞发送，如果需要）
//蓝牙串口中断回调函数
void uart_8_Callback(uart_callback_args_t *p_args)
{
	if(p_args->event==UART_EVENT_TX_COMPLETE)
	{
		uart_tx_complete=true;
	}
	else if(p_args->event==UART_EVENT_RX_CHAR)
	{
		
		rx_data = (uint16_t)p_args->data;
		if (rx_state == 0)
		{
				
				if (rx_data == '[' && Serial_RxFlag == 0)
				{
						rx_state = 1;
						rx_index = 0;
				}
		}
		else if (rx_state == 1)
		{
				if (rx_data ==']')
				{
						rx_state = 0;
						Serial_RxPacket[rx_index] = '\0';
						Serial_RxFlag = 1;
//						Protocol_Parse_And_Response();
//						Serial_RxFlag = 0;
				}
				else
				{
						if (rx_index < sizeof(Serial_RxPacket) - 1)
						{
								Serial_RxPacket[rx_index++] =(char)rx_data;
						}
						else
						{
								// 防止溢出，丢弃包
								rx_state = 0;
								Serial_RxFlag = 0;
						}
				}
		}
	}
}

// 发送单字节（阻塞直到发送完成）
void Serial_SendByte(uint8_t Byte)
{
    uart_tx_complete = false;
    fsp_err_t err = R_SCI_UART_Write(cfg.name->p_ctrl, &Byte, 1);
    if (err == FSP_SUCCESS)
    {
        while (!uart_tx_complete); // 等待发送完成回调
    }
}

// 发送数组   参数1：Array：要发送的字节数组（指向 uint8_t 类型的指针）
						//参数2 ：Length：要发送的字节数
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    for (uint16_t i = 0; i < Length; i++)
    {
        Serial_SendByte(Array[i]);
    }
}

// 发送字符串
void Serial_SendString(char *String)
{
    for (uint8_t i = 0; String[i] != '\0'; i++)
    {
        Serial_SendByte((uint8_t)String[i]);
    }
}



// 实体初始化（增加物理基地址字段，解耦系统架构）
static Machine_Control_t machines[] = {
    {
        .name = 'A', .head_idx = 0, .valid_count = 0, .ee_base_addr = 0x00,
        .dict = {
            {CODE_A_CLEANED, "已清洁"}, {CODE_A_REPAIRED, "已维修"},
            {CODE_A_MAINTAIN, "已保养"}, {CODE_A_SYS_ERROR, "设备异常！"}
        }
    },
    {
        .name = 'B', .head_idx = 0, .valid_count = 0, .ee_base_addr = 0x50,
        .dict = {
            {CODE_B_CLEANED, "已清洁"}, {CODE_B_REPAIRED, "已维修"},
            {CODE_B_MAINTAIN, "已保养"}, {CODE_B_SYS_ERROR, "设备异常！"}
        }
    },
    {
        .name = 'C', .head_idx = 0, .valid_count = 0, .ee_base_addr = 0xA0,
        .dict = {
            {CODE_C_CLEANED, "已清洁"}, {CODE_C_REPAIRED, "已维修"},
            {CODE_C_MAINTAIN, "已保养"}, {CODE_C_SYS_ERROR, "设备异常！"}
        }
    }
};
#define MACHINE_NUM (sizeof(machines) / sizeof(machines[0]))



// 存储一条新记录
/**
 * @brief 新增一条数据记录至 records
 * @param ts 传入时间戳
 * @param desc 备注描述字符串
 * @note 存储空间满则直接返回提示，不覆盖旧数据
 */
void StoreRecord_Ring(Machine_Control_t *m,uint32_t ts, const char *desc)
{
    // 在当前头指针位置写入新数据
    m->records[m->head_idx].timestamp = ts;
    strncpy(m->records[m->head_idx].description, desc, sizeof(m->records[0].description) - 1);
    m->records[m->head_idx].description[sizeof(m->records[0].description) - 1] = '\0';

    // 环形递增头指针
    m->head_idx = (m->head_idx + 1) % MAX_RECORDS;

    // 更新当前有效数据计数（满了之后维持 MAX_RECORDS，实现自动顶替旧数据逻辑）
    if (m->valid_count < MAX_RECORDS) {
        m->valid_count++;
        Serial_SendString("记录已存储成功.\r\n");
    } else {
        Serial_SendString("缓冲区已满! 最旧的数据已被覆盖.\r\n");
    }
}

/**
 * @brief 显示单台机器的所有历史记录
 */
static void ShowRecords_Ring(Machine_Control_t *m)
{
    char buf[80];
    sprintf(buf, "\r\n=== 机器 %c 记录 (%d/%d) ===\r\n", m->name, m->valid_count, MAX_RECORDS);
    Serial_SendString(buf);

    if (m->valid_count == 0) {
        Serial_SendString("此机器无相关记录.\r\n");
        return;
    }

    // 算法核心：由于环形队列满后会覆盖，最老的数据就在当前的 head_idx 位置
    // 通过从旧到新的顺序将数据倒腾出来显示
    int start_idx = (m->valid_count == MAX_RECORDS) ? m->head_idx : 0;

    for (int i = 0; i < m->valid_count; i++) {
        int curr_idx = (start_idx + i) % MAX_RECORDS;
        uint32_t ts = m->records[curr_idx].timestamp;

        uint8_t y  = (uint8_t)((ts >> 16) & 0xFF); // 完美对应
				uint8_t m_code = (uint8_t)((ts >> 8) & 0xFF); // 完美对应
				uint8_t d  = (uint8_t)(ts  & 0xFF); 

        sprintf(buf, "[%02d] 20%02x年%02x月%02x日 : 机器%c%s\r\n", 
                i, y, m_code, d, m->name, m->records[curr_idx].description);
        Serial_SendString(buf);
    }
    Serial_SendString("===============================\r\n");
}

/**
 * @brief 同步存储记录（内存 Ring + EEPROM 物理硬件）
 * @param m    目标机器指针
 * @param ts   时间戳（打包为 (年<<16)|(月<<8)|日）
 * @param code 状态码（如 0xA1）
 * @param desc 状态描述字符串（如 "已清洁"）
 * @note  只有 EEPROM 写入并校验通过后，才会更新 RAM 指针和计数，
 *        确保掉电恢复后数据一致。
 */
void StoreRecord_Sync(Machine_Control_t *m, uint32_t ts, uint8_t code, const char *desc)
{
    // ---------- 1. 参数检查 ----------
    if (m == NULL) {
        Serial_SendString("[ERROR] 机器指针为空！\r\n");
        return;
    }

    I2CDev *ep = EEPROMGetDevice();
    if (ep == NULL) {
        Serial_SendString("[ERROR] EEPROM 设备未初始化！\r\n");
        return;
    }

    // ---------- 2. 准备数据 ----------
    uint8_t slot = (uint8_t)m->head_idx;   // 当前写入槽位
    uint8_t pack_buf[5];
    pack_buf[0] = 0x00;                           // 保留字节
    pack_buf[1] = (uint8_t)((ts >> 16) & 0xFF);   // 年
    pack_buf[2] = (uint8_t)((ts >> 8) & 0xFF);    // 月
    pack_buf[3] = (uint8_t)(ts & 0xFF);           // 日
    pack_buf[4] = code;                           // 状态码

    // ---------- 3. 计算 EEPROM 物理地址（完整计算，检测溢出） ----------
    uint16_t raw_addr = m->ee_base_addr + slot * 5;
    if (raw_addr + 5 > EE_TOTAL_SIZE) {
        char err_msg[60];
        sprintf(err_msg, "[ERROR] EEPROM 地址溢出! raw_addr=%d\r\n", raw_addr);
        Serial_SendString(err_msg);
        return;   // 不更新任何内容
    }
    uint8_t ee_offset = (uint8_t)raw_addr;

    // ---------- 4. 写入 EEPROM ----------
    // 单字节写入 5 字节
		int write_ok = 1;
		for (int i = 0; i < 5; i++) {
				if (EEPROMDrvWriteByte(ep, ee_offset + i, pack_buf[i]) != 0) {
						write_ok = 0;
						break;
				}
				R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
		}
		if (!write_ok) {
				Serial_SendString("[EEPROM] 写入失败\r\n");
				return;
		}
		// 额外等待（可选）
		R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);
		
    // ---------- 5. 回读校验 ----------
    uint8_t verify[5];
    for (int i = 0; i < 5; i++) {
        EEPROMDrvRead(ep, ee_offset + i, &verify[i], 1);
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
    }
    int verify_ok = (memcmp(pack_buf, verify, 5) == 0);
    if (verify_ok) {
        Serial_SendString("[验证] 成功\r\n");
    } else {
        Serial_SendString("[验证] 失败，数据未固化\r\n");
        // 可在此增加重试逻辑（可选）
        return;
    }

    // ---------- 6. 只有写入成功且校验通过，才更新 RAM ----------
    // 6.1 将数据存入 RAM 环形缓冲区
    m->records[slot].timestamp = ts;
    strncpy(m->records[slot].description, desc,
            sizeof(m->records[slot].description) - 1);
    m->records[slot].description[sizeof(m->records[slot].description) - 1] = '\0';
         // 6.2 移动环形指针
    m->head_idx = (m->head_idx + 1) % MAX_RECORDS;
    if (m->valid_count < MAX_RECORDS) {
        m->valid_count++;
    }

    Serial_SendString("[系统] EEPROM 写入成功，RAM 已更新。\r\n");
}
/**
 * @brief 从 EEPROM 加载所有机器的历史记录到 RAM 环形缓冲区（上电时调用）
 */
void EEPROM_Load_To_RAM(void)
{
    I2CDev *ep = EEPROMGetDevice();
    if (ep == NULL) return;

    // ?? 核心修复：上电必须死等 50ms，给 I2C 总线和 EEPROM 内部电荷泵充足的硬件初始化时间
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    uint8_t read_buf[8] = {0}; // 扩大缓冲大小，防止任何底层溢出
    char log_buf[100];

    Serial_SendString("[系统]: 开始从 EEPROM 加载设备持久化历史...\r\n");

    for (size_t i = 0; i < MACHINE_NUM; i++) 
    {
        Machine_Control_t *m = &machines[i];
        m->valid_count = 0;
        m->head_idx = 0;

        int last_valid_k = -1; 

        // 检索读取硬件槽
        for (int k = 0; k < MAX_RECORDS; k++) 
        {
					
						uint16_t raw_addr = m->ee_base_addr + (k * 5);
            // 地址溢出检查（基本不会触发，但防御）
            if (raw_addr + 5 > EE_TOTAL_SIZE) {
                char err[60];
                sprintf(err, "[警告] 地址 %d 超出范围，跳过槽 %d\r\n", raw_addr, k);
                Serial_SendString(err);
                continue;
            }
						
             uint8_t ee_offset = (uint8_t)raw_addr; 
            
            // 清空单步物理缓存
            memset(read_buf, 0, sizeof(read_buf));

            // 从硬件读取 5 字节
            if(ep->Read(ep, ee_offset, read_buf, 5) != 0)
						{
							 Serial_SendString("[警告] EEPROM 读取失败，跳过该槽\r\n");
								continue;
						}
													
            // ?? 核心修复：单步物理读取后，强制给硬件 5-10ms 的休息时间，防止连续高频读取导致总线挂死
            R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);

            uint32_t ts = ((uint32_t)read_buf[1] << 16) | 
                          ((uint32_t)read_buf[2] << 8)  | 
                          read_buf[3];
            
            uint8_t status_code = read_buf[4];

            // 基础防御：如果是空扇区(0xFF)或坏数据(0)，直接跳过
            if (status_code == 0xFF || status_code == 0x00) continue;

            // 过滤掉明显不合理的 BCD 年份
            uint8_t year_check = read_buf[1]; 
            if (year_check == 0x00 || year_check > 0x99) continue;

            // 搜索字典匹配文本
            const char *status_text = NULL;
            for (int j = 0; j < 4; j++) {
                if (m->dict[j].code == status_code) {
                    status_text = m->dict[j].desc;
                    break;
                }
            }
						if (status_text == NULL) continue;

             // 填充 RAM
            m->records[k].timestamp = ts;
            strncpy(m->records[k].description, status_text,
                    sizeof(m->records[0].description) - 1);
            m->records[k].description[sizeof(m->records[0].description) - 1] = '\0';

            m->valid_count++;
            last_valid_k = k;
        }

        // 环形状态机指针恢复
        if(m->valid_count >= 0)
				{
						m->head_idx = 0;
				}
				else
				{
						m->head_idx =m->valid_count;
				}
        sprintf(log_buf, " -> 机器 %c 成功加载: %d 条记录，下次写入槽位: [%d]\r\n", m->name, m->valid_count, m->head_idx);
        Serial_SendString(log_buf);
    }
    Serial_SendString("[System]: 硬件 EEPROM 内存同步加载成功。\r\n");
}






/**
 * @brief 文本大闭环解析响应核心
 * 支持文本输入：
 * 存储数据格式: "save:A,26,03,19,A1" -> 含义：存储 机器A 2026年03月19日 状态A1
 * 查询数据格式: "query"
 * 舵机控制格式: "yun_u:90"
 */
void Protocol_Parse_And_Response(void)
{
	int i;
    if (Serial_RxPacket[0] == '\0') return;

    // ------------------ 【文本分支 1】：保存数据命令（存入EEPROM） ------------------
    // 手机发送文本例如: "save:A,26,03,19,A1"
    if (strncmp((char *)Serial_RxPacket, "save:", 5) == 0)
    {
        char m_name;
        int y, m, d, c_hex;
        // 解析格式化文本
        int parsed = sscanf((char *)Serial_RxPacket, "save:%c,%x,%x,%x,%x", &m_name, &y, &m, &d, &c_hex);
        if (parsed != 5) {
            Serial_SendString("[Error]: Parse failure. Format must be 'save:Name,YY,MM,DD,Code'\r\n");
            return;
        }

        uint8_t status_code = (uint8_t)c_hex;
        Machine_Control_t *target_machine = NULL;
        const char *status_text = NULL;

        // 匹配机器实体与字典描述
        for (size_t i = 0; i < MACHINE_NUM; i++) {
            if (machines[i].name == m_name) {
                for (int j = 0; j < 4; j++) {
                    if (machines[i].dict[j].code == status_code) {
                        target_machine = &machines[i];
                        status_text = machines[i].dict[j].desc;
                        break;
                    }
                }
            }
            if (target_machine) break;
        }

        if (target_machine && status_text) {
            char log_buf[100];
            sprintf(log_buf, "\r\n[Notify]: 收到文本存盘指令 ->20%02x年%02x月%02x日 机器%c: %s\r\n", y, m, d, m_name, status_text);
            Serial_SendString(log_buf);

            // 组装 BCD 时间戳
            uint32_t packed_ts = ((uint32_t)y << 16) | ((uint32_t)m << 8) | (uint32_t)d;
            
            // 执行同步写入（内部触发写入 EEPROM）
            StoreRecord_Sync(target_machine, packed_ts, status_code, status_text);
        } else {
            Serial_SendString("[Error]: Unknown Machine Name or Invalid Status Code!\r\n");
        }
        return;
    }

    // ------------------ 【文本分支 2】：统一查询（只读，不写EEPROM） ------------------
    if (strcmp((char *)Serial_RxPacket, "query") == 0)
		{
				Serial_SendString("\r\n============= 历史记录查询结果 =============\r\n");
				for (size_t i = 0; i < MACHINE_NUM; i++) {
						ShowRecords_Ring(&machines[i]);   // 直接复用已有的环形显示函数
				}
				Serial_SendString("===========================================\r\n");
				return;
		}
   // ------------------ 【文本分支 3】：一键数据擦除指令（新增） ------------------
    if (strcmp((char *)Serial_RxPacket, "clear") == 0)
		{
				I2CDev *ep = EEPROMGetDevice();
				if (ep != NULL) 
				{
						Serial_SendString("\r\n[System]: 开始擦除 EEPROM 硬件全盘...\r\n");
						
						int res = EEPROM_Erase_Chip(ep);
						if (res == 0)
						{
								for (size_t i = 0; i < MACHINE_NUM; i++) {
										machines[i].valid_count = 0;
										machines[i].head_idx = 0;
										memset(machines[i].records,0,sizeof(machines[i].records));
								}
								Serial_SendString("[Success]: EEPROM 扇区与本地 RAM 均已完全清空！\r\n");
						}
						else
						{
								char err_log[64];
								sprintf(err_log, "[Error]: EEPROM 硬件写入失败！错误码: %d\r\n", res);
								Serial_SendString(err_log);
						}
				}
				else
				{
						Serial_SendString("[Error]: 未检测到挂载的 EEPROM 硬件设备。\r\n");
				}
				return;
		}

    // ------------------ 【小车控制指令】 ------------------
		// 格式：run speed sum yaw
		if (strncmp((char *)Serial_RxPacket, "run ", 4) == 0) {
				float speed, sum, yaw;
				if (sscanf((char *)Serial_RxPacket, "run %f %f %f", &speed, &sum, &yaw) == 3) {
						// 参数限幅
						if (speed < 0) speed = 0; else if (speed > 999.99) speed = 999.99;
						if (sum < 0) sum = 0; else if (sum > 999.99) sum = 999.99;
						if (yaw < -180) yaw = -180;else if (yaw > 180) yaw = 180;
						
						xSemaphoreTake(run_useflag, portMAX_DELAY);
						run[0] = speed;
						run[1] = sum;
						run[2] = yaw;
						xSemaphoreGive(run_useflag);
						{
							uint8_t Depth_temp[5] = {3,2,0,0,0};
							uint8_t Old_Depth_temp[5] = {3,0,0,0,0};
							for(i = 0;i < 5;i++)
							{
								Depth_list[i] = Depth_temp[i];
								Old_Depth_list[i] = Old_Depth_temp[i];
							}
							Depth_flag = 3;
							Page_flag = 1;
							Depth_flag = 3;
							Old_Depth_flag = 2;
							Page_flag = 1;
							Old_Page_flag = 1;
							new_flag = false;
						}
						xSemaphoreTake(do_flag_useflag, portMAX_DELAY);
						do_flag = car_run;
						xSemaphoreGive(do_flag_useflag);
						Serial_SendString("[OK] 小车运行指令已下达\r\n");
				} else {
						Serial_SendString("[Error] 格式错误，应为: run speed sum yaw\r\n");
				}
				return;
		}
		// 一直跑指令
        if (strncmp((char *)Serial_RxPacket, "always ", 7) == 0) {
                float speed, yaw;
                if (sscanf((char *)Serial_RxPacket, "always %f %f", &speed, &yaw) == 2) {
                        // 参数限幅：速度 0~100，角度 0~360
                        if (speed < 0) speed = 0;
                        if (speed > 100) speed = 100;
                        if (yaw < -180) yaw = -180;
                        if (yaw > 180) yaw = 180;

                        xSemaphoreTake(run_useflag, portMAX_DELAY);
                        run[0] = speed;
                        run[2] = yaw;
                        xSemaphoreGive(run_useflag);
									
                        xSemaphoreTake(do_flag_useflag, portMAX_DELAY);
                        do_flag = car_always;
                        xSemaphoreGive(do_flag_useflag);

                        Serial_SendString("[OK] 一直跑指令已下达\r\n");
                } else {
                        Serial_SendString("[Error] 格式错误，应为: always speed yaw\r\n");
                }
                return;
        }
		// 绝对转向
		if (strncmp((char *)Serial_RxPacket, "aturn ", 6) == 0) {
				float yaw;
				if (sscanf((char *)Serial_RxPacket, "aturn %f", &yaw) == 1) {
						if (yaw < -180) yaw = -180; if (yaw > 180) yaw = 180;
						yaw_set = yaw;
						{
							uint8_t Depth_temp[5] = {3,3,0,0,0};
							uint8_t Old_Depth_temp[5] = {3,0,0,0,0};
							for(i = 0;i < 5;i++)
							{
								Depth_list[i] = Depth_temp[i];
								Old_Depth_list[i] = Old_Depth_temp[i];
							}
							Depth_flag = 3;
							Old_Depth_flag = 2;
							Page_flag = 1;
							Old_Page_flag = 1;
							new_flag = false;
						}
						xSemaphoreTake(do_flag_useflag, portMAX_DELAY);
						do_flag = car_ATurn;
						xSemaphoreGive(do_flag_useflag);
						Serial_SendString("[OK] 绝对转向指令已下达\r\n");
				} else {
						Serial_SendString("[Error] 格式错误，应为: aturn yaw\r\n");
				}
				return;
		}

		// 相对转向
		if (strncmp((char *)Serial_RxPacket, "rturn ", 6) == 0) {
				float yaw;
				if (sscanf((char *)Serial_RxPacket, "rturn %f", &yaw) == 1) {
						if (yaw < -180) yaw = -180; if (yaw > 180) yaw = 180;
						yaw_set = yaw;
						{
							uint8_t Depth_temp[5] = {3,4,0,0,0};
							uint8_t Old_Depth_temp[5] = {3,0,0,0,0};
							for(i = 0;i < 5;i++)
							{
								Depth_list[i] = Depth_temp[i];
								Old_Depth_list[i] = Old_Depth_temp[i];
							}
							Depth_flag = 3;
							Old_Depth_flag = 2;
							Page_flag = 1;
							Old_Page_flag = 1;
							new_flag = false;
						}
						xSemaphoreTake(do_flag_useflag, portMAX_DELAY);
						do_flag = car_RTurn;
						xSemaphoreGive(do_flag_useflag);
						Serial_SendString("[OK] 相对转向指令已下达\r\n");
				} else {
						Serial_SendString("[Error] 格式错误，应为: rturn yaw\r\n");
				}
				return;
		}

		// 车门控制
		if (strncmp((char *)Serial_RxPacket, "door ", 5) == 0) {
				char state[10];
				if (sscanf((char *)Serial_RxPacket, "door %s", state) == 1) {
						if (strcmp(state, "open") == 0) {
								door_state = true;
						} else if (strcmp(state, "close") == 0) {
								door_state = false;
						} else {
								Serial_SendString("[Error] 状态应为 open 或 close\r\n");
								return;
						}
						{
							uint8_t Depth_temp[5] = {3,5,0,0,0};
							uint8_t Old_Depth_temp[5] = {3,0,0,0,0};
							for(i = 0;i < 5;i++)
							{
								Depth_list[i] = Depth_temp[i];
								Old_Depth_list[i] = Old_Depth_temp[i];
							}
							Depth_flag = 3;
							Old_Depth_flag = 2;
							Page_flag = 1;
							Old_Page_flag = 1;
							new_flag = false;
						}
						xSemaphoreTake(do_flag_useflag, portMAX_DELAY);
						do_flag = door_set;
						xSemaphoreGive(do_flag_useflag);
						Serial_SendString("[OK] 车门指令已下达\r\n");
				} else {
						Serial_SendString("[Error] 格式错误，应为: door open/close\r\n");
				}
				return;
		}

		// 避障前进
		if (strncmp((char *)Serial_RxPacket, "coll ", 5) == 0) {
				float yaw;
				if (sscanf((char *)Serial_RxPacket, "coll %f", &yaw) == 1) {
						if (yaw < -180) yaw = -180; if (yaw > 180) yaw = 180;
						yaw_set = yaw;
						{
							uint8_t Depth_temp[5] = {3,6,0,0,0};
							uint8_t Old_Depth_temp[5] = {3,0,0,0,0};
							for(i = 0;i < 5;i++)
							{
								Depth_list[i] = Depth_temp[i];
								Old_Depth_list[i] = Old_Depth_temp[i];
							}
							Depth_flag = 3;
							Old_Depth_flag = 2;
							Page_flag = 1;
							Old_Page_flag = 1;
							new_flag = false;
						}
						xSemaphoreTake(do_flag_useflag, portMAX_DELAY);
						do_flag = car_Coll;
						xSemaphoreGive(do_flag_useflag);
						Serial_SendString("[OK] 避障指令已下达\r\n");
				} else {
						Serial_SendString("[Error] 格式错误，应为: coll yaw\r\n");
				}
				return;
		}

		// 前往坐标
		if (strncmp((char *)Serial_RxPacket, "goto ", 5) == 0) {
				float x, y;
				if (sscanf((char *)Serial_RxPacket, "goto %f %f", &x, &y) == 2) {
						if (x < 0) x = 0; if (x > 9.99) x = 9.99;
						if (y < 0) y = 0; if (y > 9.99) y = 9.99;
						xSemaphoreTake(go_useflag, portMAX_DELAY);
						go[0] = x;
						go[1] = y;
						xSemaphoreGive(go_useflag);
						{
							uint8_t Depth_temp[5] = {3,7,0,0,0};
							uint8_t Old_Depth_temp[5] = {3,0,0,0,0};
							for(i = 0;i < 5;i++)
							{
								Depth_list[i] = Depth_temp[i];
								Old_Depth_list[i] = Old_Depth_temp[i];
							}
							Depth_flag = 3;
							Old_Depth_flag = 2;
							Page_flag = 1;
							Old_Page_flag = 1;
							new_flag = false;
						}					
						xSemaphoreTake(do_flag_useflag, portMAX_DELAY);
						do_flag = car_goto;
						xSemaphoreGive(do_flag_useflag);
						Serial_SendString("[OK] 前往坐标指令已下达\r\n");
				} else {
						Serial_SendString("[Error] 格式错误，应为: goto x y\r\n");
				}
				return;
		}

		// 前往坐标（带避障）
		if (strncmp((char *)Serial_RxPacket, "gotocoll ", 9) == 0) {
				float x, y;
				if (sscanf((char *)Serial_RxPacket, "gotocoll %f %f", &x, &y) == 2) {
						if (x < 0) x = 0; if (x > 9.99) x = 9.99;
						if (y < 0) y = 0; if (y > 9.99) y = 9.99;
						xSemaphoreTake(go_useflag, portMAX_DELAY);
						go[0] = x;
						go[1] = y;
						xSemaphoreGive(go_useflag);
						{
							uint8_t Depth_temp[5] = {3,8,0,0,0};
							uint8_t Old_Depth_temp[5] = {3,0,0,0,0};
							for(i = 0;i < 5;i++)
							{
								Depth_list[i] = Depth_temp[i];
								Old_Depth_list[i] = Old_Depth_temp[i];
							}
							Depth_flag = 3;
							Old_Depth_flag = 2;
							Page_flag = 1;
							Old_Page_flag = 1;
							new_flag = false;
						}
						xSemaphoreTake(do_flag_useflag, portMAX_DELAY);
						do_flag = car_goto_Coll;
						xSemaphoreGive(do_flag_useflag);
						Serial_SendString("[OK] 前往坐标（带避障）指令已下达\r\n");
				} else {
						Serial_SendString("[Error] 格式错误，应为: gotocoll x y\r\n");
				}
				return;
		}

		// 停止
		if (strcmp((char *)Serial_RxPacket, "stop") == 0) {
						{
							uint8_t Depth_temp[5] = {0,0,0,0,0};
							uint8_t Old_Depth_temp[5] = {0,0,0,0,0};
							for(i = 0;i < 5;i++)
							{
								Depth_list[i] = Depth_temp[i];
								Old_Depth_list[i] = Old_Depth_temp[i];
							}
							Depth_flag = 1;
							Old_Depth_flag = 1;
							Page_flag = 1;
							Old_Page_flag = 1;
							new_flag = false;
						}
				goto_flag =	true;
				xSemaphoreTake(do_flag_useflag, portMAX_DELAY);
				do_flag = car_free;
				xSemaphoreGive(do_flag_useflag);
				// 可选：强制停止电机（直接调用Motor_Set(0,0)）
				Serial_SendString("[OK] 停止指令已执行\r\n");
				return;
		}

    // 未知文本指令防错
    Serial_SendString("[System Error]: Unknown Text Command.\r\n");
}
