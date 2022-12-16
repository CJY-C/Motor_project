/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f1xx_it.c
 * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "usart.h"
#include "tim.h"
#include "cmsis_os.h"
#include "BLEConfig.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
extern osThreadId BLECommunicationHandle;
// todo: 清理不必要的全局flag
extern uint8_t TX_RX_SET;
extern uint8_t ComErr;
extern uint8_t errpace;
extern uint8_t state;
extern uint8_t RS485_RX_BUFF[2048];
extern uint16_t RS485_RX_CNT;
extern uint8_t RS485_RxFlag;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern TIM_HandleTypeDef htim1;

/* USER CODE BEGIN EV */
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM1 update interrupt.
  */
void TIM1_UP_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_IRQn 0 */

  /* USER CODE END TIM1_UP_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_IRQn 1 */

  /* USER CODE END TIM1_UP_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
  // xxx: 转移至回调函数
  // uint8_t res;
  // uint8_t err;

  // if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)
  // {
  //   if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE))
  //   {
  //     err = 1;
  //     errpace = 2;
  //   } // �??测到噪音、帧错误或校验错�??
  //   else
  //     err = 0;
  //   res = huart2.Instance->DR & (uint8_t)0x00FF; // 读接收到的字节，同时相关标志自动清除

  //   if ((RS485_RX_CNT < 2047) && (err == 0))
  //   {
  //     RS485_RX_BUFF[RS485_RX_CNT] = res;
  //     RS485_RX_CNT++;

  //     __HAL_TIM_SET_COUNTER(&htim2,0);
  //     HAL_TIM_Base_Start_IT(&htim2);
  //     // __HAL_TIM_CLEAR_FLAG(TIM2, TIM_FLAG_UPDATE);
  //     // TIM_ClearITPendingBit(TIM7, TIM_IT_Update); // 清除定时器溢出中�??
  //     // TIM_SetCounter(TIM7, 0);                    // 当接收到�??个新的字节，将定时器7复位�??0，重新计时（相当于喂狗）
  //     // TIM_Cmd(TIM7, ENABLE);                      // �??始计�??
  //   }
  // }
  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
  // 	HAL_TIM_Base_Start_IT(&htim2);
  //   if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_ORE) == SET)
  //   {
  //     __HAL_UART_CLEAR_OREFLAG(&huart3);
  //     PB02_Fram_Record_Struct.Data_RX_BUF[PB02_Fram_Record_Struct.InfBit.FramLength++] =
  //     huart3.Instance->DR & (uint8_t)0x00FF;
  //   }
  //XXX: 重新使用Cube生成代码后需要注释掉下面的中断处理函数
  /* USER CODE END USART3_IRQn 0 */
  // HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */
  if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE) == SET)
  {
    //XXX: IDLE中断无法在Keil仿真环境中检测到，只能上板调试！！！
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
    if (PB02_Fram_Record_Struct.InfBit.FramLength < RX_BUF_MAX_LEN - 1)
    {
      //* 读一个字节的数据
      PB02_Fram_Record_Struct.Data_RX_BUF[PB02_Fram_Record_Struct.InfBit.FramLength++] =
          huart3.Instance->DR & (uint8_t)0x00FF;
    }
    __HAL_UART_CLEAR_NEFLAG(&huart3);
    // __HAL_TIM_SET_COUNTER(&htim2, 0);
  }
  if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) == SET)
  {
    // HAL_GPIO_TogglePin(PB02_LED_GPIO_Port, PB02_LED_Pin);
    __HAL_UART_CLEAR_IDLEFLAG(&huart3);
    PB02_Fram_Record_Struct.InfBit.FramFinishFlag = 1;
    PB02_Fram_Record_Struct.Data_RX_BUF[PB02_Fram_Record_Struct.InfBit.FramLength] = '\0'; //* 添加字符串结尾
    if (BLECommunicationHandle == NULL)
    {
      PC_USART("ERROR!\n");
    }
    else
      osSignalSet(BLECommunicationHandle, BLE_DATA_RECEIVED); //* 检测到总线空闲，就表示一帧数据接收完毕
    // __HAL_TIM_SET_COUNTER(htim,0);
    // HAL_TIM_Base_Stop_IT(htim);
  }
  /* USER CODE END USART3_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
