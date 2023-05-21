/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum 
{
  BLE_DATA_RECEIVED = 1,  // * 蓝牙串口数据接收完成信号
  SENSOR_DATA_RECEIVED,   // * 485串口数据接收完成信号
  PHONE_DATA_REQUEST,     // * 手机数据请求信号
  PHONE_MOTOR_REQUEST,    // * 手机电机信号
}UserSignal;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
// * 定义Release_Mode会关闭usart1串口
#define Release_Mode
#ifndef Release_Mode
#define PC_USART(fmt, ...)          huart_switch = 1; printf(fmt, ##__VA_ARGS__)
#else
#define PC_USART(fmt, ...)
#endif // !Release_Mode
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PB02_LED_Pin GPIO_PIN_13
#define PB02_LED_GPIO_Port GPIOC
#define PB02_RST_Pin GPIO_PIN_4
#define PB02_RST_GPIO_Port GPIOA
#define Motor_In1_Pin GPIO_PIN_11
#define Motor_In1_GPIO_Port GPIOA
#define Motor_In2_Pin GPIO_PIN_12
#define Motor_In2_GPIO_Port GPIOA
#define EN_485_TX_Pin GPIO_PIN_8
#define EN_485_TX_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
extern uint8_t TX_RX_SET;
extern uint8_t ComErr;
extern uint8_t errpace;
extern uint8_t state;
extern uint8_t RS485_RX_BUFF[2048];
extern uint16_t RS485_RX_CNT;
extern uint8_t RS485_RxFlag;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
