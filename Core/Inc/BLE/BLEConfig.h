#ifndef BLE_Config_H
#define BLE_Config_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "usart.h"

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#include "PB02.h"
#include "BLEMsg.h"

#define PB02_BLE_MAC "FFDDFFCCFFBB"
#define PB02_BLE_MAC_EXAM "FF DD FF CC FF BB"

#define PB02_RST_Pin_SetH     HAL_GPIO_WritePin(PB02_RST_GPIO_Port, PB02_RST_Pin, GPIO_PIN_SET)
#define PB02_RST_Pin_SetL     HAL_GPIO_WritePin(PB02_RST_GPIO_Port, PB02_RST_Pin, GPIO_PIN_RESET)


extern uint8_t huart_switch;
// #define PB02_USART(fmt, ...)        USART_printf (&huart3, fmt, ##__VA_ARGS__)    
#define PB02_USART(fmt, ...)        huart_switch = 3; printf(fmt, ##__VA_ARGS__)    

#endif // !BLE_Config_H
