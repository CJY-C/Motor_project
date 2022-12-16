#ifndef __PB02_H
#define __PB02_H			   
#include "BLEConfig.h"


#define RX_BUF_MAX_LEN 1024       
extern struct STRUCT_USART_Fram   
{
    char Data_RX_BUF[RX_BUF_MAX_LEN];
    union 
    {
        volatile uint16_t InfAll;
        struct 
        {
            volatile uint16_t FramLength       :15;                               // 14:0 
            volatile uint16_t FramFinishFlag   :1;                                // 15 
        }InfBit;
    }; 
	void (*messageProcess)();
}PB02_Fram_Record_Struct;


// * 移植的函数，补充了函数说明

/// @brief 复位蓝牙模块，配置通信函数
void PB02_Init(void);

/// @brief 开机测试蓝牙模块AT指令吃否工作正常
/// @param  void
/// @return true->测试通过；false->测试不通过
bool PB02_AT_Test(void);

/// @brief 通过串口向蓝牙模块发送AT指令
/// @param cmd      待发送的AT指令
/// @param ack1     期望的响应字符串片段，与ack2是逻辑或关系
/// @param ack2     期望的响应字符串片段，与ack1是逻辑或关系
/// @param time     等待响应延时
/// @return true->ack通过(发送成功)；false->ack错误（发送失败）
bool PB02_Send_AT_Cmd(char *cmd,char *ack1,char *ack2,uint32_t time);

/// @brief  复位蓝牙模块
/// @param  void
void PB02_Rst(void);

/// @brief 向蓝牙模块请求MAC地址信息，若为ffffffff则需要配置为其他地址，保证能够和app通信
/// @param stepSwitch 标志位
/// @return false->已配置， true->未配置
bool PB02_Get_BLEMAC(bool stepSwitch);

/// @brief 修改蓝牙模块的MAC地址
/// @param addr 新MAC地址的字符串
/// @param stepSwitch 标志位
/// @return true->配置成功, false->配失败
bool PB02_Set_BLEMAC( char* addr, bool stepSwitch);

/// @brief 退出透传模式
/// @param void
/// @return true->退出成功，false->不在透传模式中
bool PB02_transmission_exit(void);

/// @brief 设置蓝牙模块为从机模式，并广播
/// @param void
void PB02_Set_Slave_Broadcast(void);

/// @brief 清除接收到的数据
/// @param void
void PB02_Clear_Data(void);

/// @brief HAL库实现多串口使用printf输出
/// @param huart    对应的串口结构体
/// @param format   格式化字符串
/// @param ...      可变长参数
void USART_printf(UART_HandleTypeDef *huart,char *format, ...);


/// @brief 蓝牙消息处理函数交给用户实现
/// @param  void
extern void msgProcess(void);
#endif
