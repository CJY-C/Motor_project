#include "master.h"
#include "usart.h"
#include <stdio.h>
#include <stdlib.h>

#include "cmsis_os.h"


// TODO: 接收缓冲区大小调整
uint8_t RS485_RX_BUFF[2048] = {0}; // 接收缓冲区2048字节
uint16_t RS485_RX_CNT = 0;				 // 接收计数器
uint8_t RS485_RxFlag = 0;					 // 接收一帧结束标记

uint8_t RS485_TX_BUFF[2048]; // 发送缓冲区
uint16_t RS485_TX_CNT = 0;	 // 发送计数器

/////////////////////////////////////////////////////////////////////////////////////
// 主机命令区
uint8_t SlaverAddr = 0x01;		// 从机地址
uint8_t Fuction = 0x03;				// 功能码
uint16_t StartAddr = 0x9C40;	// 起始地址
uint16_t ValueOrLenth = 0x02; // 数据or长度

//////////////////////////////////////////////////////////////////////////////////////////

/// @brief 发送，接受命令切换。 0 发送模式 1接受模式
uint8_t TX_RX_SET = 0;

/// @brief  0 代表通讯正常 1代表CRC错误 2代表功能码错误 现在默认的是0x03 7为通讯超时 8代表未开始
uint8_t ComErr = 8;

/// @brief 1代表tim7串口待机时间过长，放弃数据帧 2检测到噪音、帧错误或校验错误
uint8_t errpace = 0;

/// @brief 1发送 2接收 下一个从机地址
uint8_t state = 0;

//* 操作系统相关变量
extern osMessageQId SensorQHandle;
extern osPoolId			SensorDataPHandle;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Master寄存器和单片机寄存器的映射关系,届时需要在主函数进行调用或者显示
uint16_t Master_InputIO[100];	 // 输入开关量寄存器(这里使用的是位带操作)   注意： 这里储存从机返回的数据。    开关量的数据只能是0，1 例如 Master_InputIO[5]=0；Master_InputIO[8]=1；
uint16_t Master_OutputIO[100]; // 输出开关量寄存器(这里使用的是位带操作)    功能码 05 15

uint16_t Master_ReadReg[1000];	// 只读寄存器----存储从机返回的数据          功能码 03 目前是只用这一个
uint16_t Master_WriteReg[1000]; // 写寄存器-------将寄存器中的数据送给从机   功能码 06 16

void Modbus_RegMap(void) // 功能码0x06使用
{
	Master_WriteReg[0] = 1;
	Master_WriteReg[1] = 8;
	Master_WriteReg[2] = 9;
	Master_WriteReg[3] = 235;
	Master_WriteReg[4] = 8690;
	Master_WriteReg[5] = 23578;
	Master_WriteReg[6] = 125;

	Master_OutputIO[20] = 1;
	Master_OutputIO[21] = 0;
	Master_OutputIO[22] = 1;
	Master_OutputIO[23] = 1;
	Master_OutputIO[24] = 0;
	Master_OutputIO[25] = 0;
	Master_OutputIO[26] = 1;
	Master_OutputIO[27] = 1;

	Master_OutputIO[28] = 1;
	Master_OutputIO[29] = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// CRC校验 

const uint8_t auchCRCHi[] = {
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40};

const uint8_t auchCRCLo[] = {
		0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
		0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
		0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
		0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
		0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
		0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
		0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
		0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
		0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
		0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
		0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
		0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
		0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40};

uint16_t CRC_Compute(uint8_t *puchMsg, uint16_t usDataLen)
{
	uint8_t uchCRCHi = 0xFF;
	uint8_t uchCRCLo = 0xFF;
	uint32_t uIndex;
	while (usDataLen--)
	{
		uIndex = uchCRCHi ^ *puchMsg++;
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}
	return ((uchCRCHi << 8) | (uchCRCLo));
} // uint16 crc16(uint8 *puchMsg, uint16 usDataLen)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 初始化USART2
void RS485_Init(void)
{
	EN_485_TX_L; 
}

//////////////////////////////////////////////////////////////////////////////
// 发送n个字节数据 主机将数据进行发送
// buff:发送区首地址
// len：发送的字节数
void RS485_SendData(uint8_t *buff, uint8_t len)
{
	EN_485_TX_H; // 切换为发送模式
	while (len--)
	{
		// todo: 待修改
		RS485_USART("%c", *(buff++));
		// HAL_UART_Transmit(&huart2, (uint8_t *)(buff++), 1, 0xFF);
		// while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) != SET);
	}
	TX_RX_SET = 1; // 发送命令完成，定时器T4处理接收到的数据
	EN_485_TX_L; // 切换为接收模式
}

/////////////////////////////////////////////////////////////////////////////////////
// 通讯策略
void modbus_rtu(void)
{
	static uint8_t i = 0;
	static uint8_t j = 0;
	switch (i)
	{
	case 0: // modbus执行命令第一步。
		// RS485_TX_Service();  //向从机发送一个请求。就在此时发送完成TX_RX_SET=1 发送命令完成，定时器T4处理接收到的数据
		// 在此处也可以直接写Master_Service( SlaverAddr, Fuction, StartAddr, ValueOrLenth);
		// 多次通讯结果可以按照类似的封装进行填写

		RS485_TX_Service();
		if (TX_RX_SET)
			i = 1; // 发送，接受命令切换。 0 发送模式 1 接受模式
		// if(TX_RX_SET) i=1; //发送，接受命令切换。 0 发送模式 1 接受模式

		state = 1;
		break;
	case 1:								// modbus命令执行第二步。
		RS485_RX_Service(); // 执行数据接收
		state = 2;
		if (ComErr == 0) // 如果什么错误都没有发生
		{
			i = 0; // 完成命令更换功能码！
		}				 // 一次通讯已经完成
		else		 // 错误接收后再次准备接收
		{
			i = 1; //
			j++;	 // 一个命令发送3次没有应答切换下一个命令
			if (j >= 2)
			{
				j = 0;
				i = 0;
				ComErr = 7; // 通讯超时
			}
		}
		break;
	default:
		break;
	}
}

// Modbus功能码03处理程序///////////////////////////////////////////////////////////////////////////////////////
// 读保持寄存器
void Master_03_Slove(uint8_t board_adr, uint16_t start_address, uint16_t lenth)
{
	uint16_t calCRC;

	RS485_TX_BUFF[0] = board_adr;
	RS485_TX_BUFF[1] = READ_HLD_REG; // modbus 指令码03
	RS485_TX_BUFF[2] = HI(start_address);
	RS485_TX_BUFF[3] = LOW(start_address);
	RS485_TX_BUFF[4] = HI(lenth);
	RS485_TX_BUFF[5] = LOW(lenth);
	calCRC = CRC_Compute(RS485_TX_BUFF, 6);
	RS485_TX_BUFF[6] = (calCRC >> 8) & 0xFF;
	RS485_TX_BUFF[7] = (calCRC)&0xFF;

	RS485_SendData(RS485_TX_BUFF, 8);
}
// Modbus功能码04处理程序/////////////////////////////////////////////////////////////////////////////////////// 未使用
// 读输入寄存器
void Master_04_Slove(uint8_t board_adr, uint16_t start_address, uint16_t lenth)
{
	uint16_t calCRC;
	RS485_TX_BUFF[0] = board_adr;
	RS485_TX_BUFF[1] = READ_AI; // modbus 指令码04
	RS485_TX_BUFF[2] = HI(start_address);
	RS485_TX_BUFF[3] = LOW(start_address);
	RS485_TX_BUFF[4] = HI(lenth);
	RS485_TX_BUFF[5] = LOW(lenth);
	calCRC = CRC_Compute(RS485_TX_BUFF, 6);
	RS485_TX_BUFF[6] = (calCRC >> 8) & 0xFF;
	RS485_TX_BUFF[7] = (calCRC)&0xFF;
	RS485_SendData(RS485_TX_BUFF, 8);
}

// Modbus功能码06处理程序   //////////////////////////////////////////////////////////////////////////////////
// 写单个保持寄存器
void Master_06_Slove(uint8_t board_adr, uint16_t start_address, uint16_t value)
{
	uint16_t calCRC;
	if (value == 1)
	{
		RS485_TX_BUFF[0] = board_adr;
		RS485_TX_BUFF[1] = SET_HLD_REG; // modbus 指令码06
		RS485_TX_BUFF[2] = HI(start_address);
		RS485_TX_BUFF[3] = LOW(start_address);
		RS485_TX_BUFF[4] = HI(Master_WriteReg[start_address]);
		RS485_TX_BUFF[5] = LOW(Master_WriteReg[start_address]);
		calCRC = CRC_Compute(RS485_TX_BUFF, 6);
		RS485_TX_BUFF[6] = (calCRC >> 8) & 0xFF;
		RS485_TX_BUFF[7] = (calCRC)&0xFF;
		RS485_SendData(RS485_TX_BUFF, 8);
	}
}
void Master_Service(uint8_t SlaverAddr, uint8_t Fuction, uint16_t StartAddr, uint16_t ValueOrLenth)
{

	switch (Fuction)
	{
	case 03:
		Master_03_Slove(SlaverAddr, StartAddr, ValueOrLenth);
		break;
	case 06:
		Master_06_Slove(SlaverAddr, StartAddr, ValueOrLenth);
		break;
	}
}

void RS485_TX_Service(void)
{
	Master_Service(SlaverAddr, Fuction, StartAddr, ValueOrLenth);
}

/////////////////////////////////////////////////////////////////////////////////////
// RS485服务程序，用于处理接收到的数据(请在主函数中循环调用)

void RS485_RX_Service(void)
{
	uint16_t calCRC;
	uint16_t recCRC;

	// osSignalWait(SENSOR_DATA_RECEIVED, osWaitForever);
	if (RS485_RxFlag == 1)
	{
		// PC_USART("enter rx service!\r\n");
		if (RS485_RX_BUFF[0] == SlaverAddr) // 地址正确
		{
			if ((RS485_RX_BUFF[1] == 01) || (RS485_RX_BUFF[1] == 02) || (RS485_RX_BUFF[1] == 03) || (RS485_RX_BUFF[1] == 05) || (RS485_RX_BUFF[1] == 06) || (RS485_RX_BUFF[1] == 15) || (RS485_RX_BUFF[1] == 16)) // 功能码正确
			{
				calCRC = CRC_Compute(RS485_RX_BUFF, RS485_RX_CNT - 2);																				 // 计算所接收数据的CRC
				recCRC = RS485_RX_BUFF[RS485_RX_CNT - 1] | (((uint16_t)RS485_RX_BUFF[RS485_RX_CNT - 2]) << 8); // 接收到的CRC(低字节在前，高字节在后)

				if (calCRC == recCRC) // CRC校验正确
				{
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					switch (RS485_RX_BUFF[1]) // 根据不同的功能码进行处理
					{
					case 03: // 读单个寄存器，现在目前只用这一个先，只能读一个。
					{
						Modbus_03_Solve();
						break;
					}

					case 06:
					{
						Modbus_06_Solve();
						break;
					}

					case 16:
					{
						Modbus_16_Solve();
						break;
					}
					}
				}

				else // CRC校验错误
				{
					PC_USART("crc check incorrect!\r\n");
					ComErr = 14;
				}
			}

			else // 功能码错误
			{
				PC_USART("function code incorrect!\r\n");
				if ((RS485_RX_BUFF[1] == 0x81) || (RS485_RX_BUFF[1] == 0x82) || (RS485_RX_BUFF[1] == 0x83) || (RS485_RX_BUFF[1] == 0x85) || (RS485_RX_BUFF[1] == 0x86) || (RS485_RX_BUFF[1] == 0x8F) || (RS485_RX_BUFF[1] == 0x90))
				{
					switch (RS485_RX_BUFF[2])
					{
					case 0x01:
						ComErr = 11;
						break;
					case 0x02:
						ComErr = 12;
						break;
					case 0x03:
						ComErr = 13;
						break;
					case 0x04:
						ComErr = 14;
						break;
					}
					TX_RX_SET = 0; // 命令完成
				}
			}
		}
		else
		{
			PC_USART("slave addr incorrect!\r\n");
		}

		RS485_RxFlag = 0; // 复位帧结束标志
		RS485_RX_CNT = 0; // 接收计数器清零
		EN_485_TX_H; //开启发送模式
		TX_RX_SET = 0; // 命令完成
	}
}

// Modbus功能码03处理程序
// 读保持寄存器
// 排除了寄存器的字节验证机制，采用默认的轮询周期清空缓存区
void Modbus_03_Solve(void)
{
	uint8_t i;
	uint8_t RegNum;
	RegNum = RS485_RX_BUFF[2] / 2; // 获取字节数 2个字节
	uint16_t relativeAddr = StartAddr - 0x9C40;

	if ((relativeAddr + RegNum) < 1000) // 寄存器地址+数量在范围内
	{
		for (i = 0; i < RegNum; i++)
		{
			// * 计算方式待定
			Master_ReadReg[relativeAddr + i] = RS485_RX_BUFF[3 + i * 2];																					 /////////高8位
			Master_ReadReg[relativeAddr + i] = RS485_RX_BUFF[4 + i * 2] + (Master_ReadReg[relativeAddr + i] << 8); // 低8位+高8位
		}
		int32_t value_high = Master_ReadReg[relativeAddr];
		int32_t value_low = Master_ReadReg[relativeAddr + 1];

		uint32_t* sensorData = osPoolAlloc(SensorDataPHandle);

		*sensorData = (value_high << 16) + value_low;

		osMessagePut(SensorQHandle, (uint32_t)sensorData, osWaitForever);

		ComErr = 0;
	}
	else
	{
		ComErr = 3;
	}

	TX_RX_SET = 0; // 命令完成
}

// Modbus功能码05处理程序   ///////////////////////////////////////////////////////程序已验证OK
// 写单个输出开关量
void Modbus_05_Solve(void)
{
	uint16_t i;
	i = ValueOrLenth;
	if ((i > 0 && RS485_RX_BUFF[4] == 0XFF && RS485_RX_BUFF[5] == 0X00) || (i == 0 && RS485_RX_BUFF[4] == 0X00 && RS485_RX_BUFF[5] == 0X00))
	{
		ComErr = 0;
	}
	else
	{
		ComErr = 5;
	}
	TX_RX_SET = 0; // 命令完成
}

// Modbus功能码06处理程序   //////////////////////////////////////////////////////////////////////////////////已验证程序OK
// 写单个保持寄存器
void Modbus_06_Solve(void)
{
	uint16_t i;																									// 数据返回校验用
	i = (((uint16_t)RS485_RX_BUFF[4]) << 8) | RS485_RX_BUFF[5]; // 获取寄存器数量
	if (i == Master_WriteReg[StartAddr])
	{
		ComErr = 0;
	}
	else
	{
		ComErr = 6;
	}
	TX_RX_SET = 0; // 命令完成
}
// Modbus功能码15处理程序   //////////////////////////////////////////////////////程序已验证OK
// 写多个输出开关量
void Modbus_15_Solve(void)
{
	uint16_t i;																									// 数据返回校验用
	i = (((uint16_t)RS485_RX_BUFF[4]) << 8) | RS485_RX_BUFF[5]; // 获取寄存器数量
	if (i == ValueOrLenth)
	{
		ComErr = 0;
	}
	else
	{
		ComErr = 15;
	}
	TX_RX_SET = 0; // 命令完成
}

// Modbus功能码16处理程序 /////////////////////////////////////////////////////////////////////////////////////////////////已验证程序OK
// 写多个保持寄存器
void Modbus_16_Solve(void)
{
	uint16_t i;																											// 数据返回校验用
	i = (((uint16_t)RS485_RX_BUFF[4]) << 8) | ((RS485_RX_BUFF[5])); // 获取寄存器数量
	if (i == ValueOrLenth)
	{
		ComErr = 0;
	}
	else
	{
		ComErr = 16;
	}
	TX_RX_SET = 0; // 命令完成
}

uint16_t erroeback()
{
	return ComErr;
}

uint16_t errpaceback()
{
	return errpace;
}

uint16_t stateback()
{
	return state;
}
