#include <stdarg.h>
#include "stdlib.h"
#include "cmsis_os.h"

#include "BLEConfig.h"

struct STRUCT_USART_Fram PB02_Fram_Record_Struct = {0}; // 定义数据帧结构体

uint8_t huart_switch = 1;

uint8_t rr = 0x0d;
uint8_t nn = 0x0a;

char ok[4] = "AT\r\n";

void PB02_Init(void)
{
    //* 改变cJSON的内存管理方式，影响不大
    // cJSON_Hooks cJSONhooks_freeRTOS;
    // cJSONhooks_freeRTOS.malloc_fn = pvPortMalloc;
    // cJSONhooks_freeRTOS.free_fn   = vPortFree;
    // cJSON_InitHooks(&cJSONhooks_freeRTOS);

    PB02_Rst(); // 复位蓝牙模块
    PB02_Fram_Record_Struct.messageProcess = &msgProcess;
}

bool PB02_Send_AT_Cmd(char *cmd, char *ack1, char *ack2, uint32_t time)
{
    if (strcmp(cmd, "+++") == 0)
    {
        PB02_USART("%s", cmd);
    }
    else
    {
        PB02_USART("%s\r\n", cmd);
    }

    if (ack1 == NULL && ack2 == NULL) 
    {
        return true;
    }
    // PC_USART("%s\r\n", "delay some time to get receive data");
    // todo: 可调整为信号控制，而不是延时。
    osDelay(time);
    PB02_Fram_Record_Struct.Data_RX_BUF[PB02_Fram_Record_Struct.InfBit.FramLength] = '\0';
    PB02_Fram_Record_Struct.InfBit.FramLength = 0; //* 覆盖接收，可以添加一个清除函数
    // PC_USART("PB02_data:%s",PB02_Fram_Record_Struct .Data_RX_BUF);
    // ����ֵ����
    if (ack1 != NULL && ack2 != NULL)
    {
        return ((bool)strstr(PB02_Fram_Record_Struct.Data_RX_BUF, ack1) ||
                (bool)strstr(PB02_Fram_Record_Struct.Data_RX_BUF, ack2));
    }
    else if (ack1 != NULL)
        return ((bool)strstr(PB02_Fram_Record_Struct.Data_RX_BUF, ack1));

    else if (ack2 != NULL)
        return ((bool)strstr(PB02_Fram_Record_Struct.Data_RX_BUF, ack2));
    else
        return false;
}

void PB02_Rst(void)
{
    PB02_RST_Pin_SetL; // 复位低电平有效
    // HAL_Delay(500);
    osDelay(500);
    PB02_RST_Pin_SetH;
    // osDelay(5000);
}

bool PB02_AT_Test(void)
{
    char count = 0;
    // HAL_Delay(1000);
    osDelay(100); //* 等待蓝牙复位
    while (count < 10)
    {
        PC_USART("%s\r\n", "AT Test");
        if (PB02_Send_AT_Cmd("AT", "OK", NULL, 500))
        {
            PC_USART("%s\r\n", "OK");
            return true;
        }
        ++count;
    }
    return false;
}

bool PB02_Get_BLEMAC(bool stepSwitch)
{
    if (stepSwitch)
    {
        if (PB02_Send_AT_Cmd("AT+BLEMAC?", PB02_BLE_MAC_EXAM, 0, 200))
            return false; //* 如果已经配置过MAC地址就不用再修改了。
        else
            return true;
    }
    return false;
}

bool PB02_Set_BLEMAC(char *addr, bool stepSwitch)
{
    if (stepSwitch)
    {
        char cStr[30];
        sprintf(cStr, "AT+BLEMAC=%s", addr);
        if (PB02_Send_AT_Cmd(cStr, "OK", 0, 200))
            return true;
        else
            return false;
    }
    else
        return false;
}

bool PB02_transmission_exit(void)
{
    if (PB02_Send_AT_Cmd("+++", NULL, NULL, 100))
    // if (PB02_Send_AT_Cmd("+++", "OK", 0, 100))
        return true;
    else
        return false;
}

// XXX: 注意蓝牙模块的部分参数只能通过烧录修改
void PB02_Set_Slave_Broadcast(void)
{
    bool switchFlag = 0;
    switchFlag = PB02_AT_Test();
    switchFlag = PB02_Get_BLEMAC(switchFlag);
    switchFlag = PB02_Set_BLEMAC(PB02_BLE_MAC, switchFlag);
    if (switchFlag)
        PB02_Rst(); //* 调整了MAC地址才需要再复位一下。
}

void PB02_Clear_Data(void)
{
    int a;
    for (a = 0; a < PB02_Fram_Record_Struct.InfBit.FramLength; a++)
    // for (a = 0; a < 1024; a++)
        PB02_Fram_Record_Struct.Data_RX_BUF[a] = '\0';
    PB02_Fram_Record_Struct.InfBit.FramLength = 0;
    PB02_Fram_Record_Struct.InfBit.FramFinishFlag = 0;
}

//XXX: 不使用了
void USART_printf(UART_HandleTypeDef *huart, char *format, ...)
{
    char buf[512];

    va_list args;
    va_start(args, format);
    uint16_t len = vsnprintf((char *)buf, sizeof(buf), (char *)format, args);
    va_end(args);
    HAL_UART_Transmit_IT(huart, (uint8_t *)buf, len);
}
