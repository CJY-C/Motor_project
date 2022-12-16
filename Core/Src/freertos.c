/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tim.h"
#include "BLEConfig.h"
#include "master.h"

#include "motor.h"
#include "pid_controller.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

uint8_t BLECommunicationFlag = 0;
uint8_t DataManagerFlag = 0;
uint8_t buf[1] = {0};

osPoolId SensorDataPHandle;

struct Motor motor;									// 电机模型
struct PIDController press_ctrl; // 压力控制器

/* USER CODE END Variables */
osThreadId BLECommunicationHandle;
osThreadId DataManagerHandle;
osThreadId SensorCommunicationHandle;
osThreadId MotorControlHandle;
osMessageQId SensorQHandle;
osMessageQId BLEQHandle;
osMessageQId MotorQHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void BLECommunicationEntry(void const *argument);
void DataManagerEntry(void const *argument);
void SensorCommunicationEntry(void const *argument);
void MotorControlEntry(void const *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of SensorQ */
  osMessageQDef(SensorQ, 16, uint32_t);
  SensorQHandle = osMessageCreate(osMessageQ(SensorQ), NULL);

  /* definition and creation of BLEQ */
  osMessageQDef(BLEQ, 16, uint32_t);
  BLEQHandle = osMessageCreate(osMessageQ(BLEQ), NULL);

  /* definition and creation of MotorQ */
  osMessageQDef(MotorQ, 16, uint32_t);
  MotorQHandle = osMessageCreate(osMessageQ(MotorQ), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  osPoolDef(SensorDataP, 16, uint32_t);
  SensorDataPHandle = osPoolCreate(osPool(SensorDataP));
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of BLECommunication */
  osThreadDef(BLECommunication, BLECommunicationEntry, osPriorityAboveNormal, 0, 256);
  BLECommunicationHandle = osThreadCreate(osThread(BLECommunication), NULL);

  /* definition and creation of DataManager */
  osThreadDef(DataManager, DataManagerEntry, osPriorityHigh, 0, 128);
  DataManagerHandle = osThreadCreate(osThread(DataManager), NULL);

  /* definition and creation of SensorCommunication */
  osThreadDef(SensorCommunication, SensorCommunicationEntry, osPriorityRealtime, 0, 128);
  SensorCommunicationHandle = osThreadCreate(osThread(SensorCommunication), NULL);

  /* definition and creation of MotorControl */
  osThreadDef(MotorControl, MotorControlEntry, osPriorityNormal, 0, 128);
  MotorControlHandle = osThreadCreate(osThread(MotorControl), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_BLECommunicationEntry */
/**
 * @brief  Function implementing the BLECommunication thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_BLECommunicationEntry */
void BLECommunicationEntry(void const *argument)
{
  /* USER CODE BEGIN BLECommunicationEntry */
  PB02_Init();
  PB02_Set_Slave_Broadcast();
  /* Infinite loop */
  // int a;
  for (;;)
  {
    BLECommunicationFlag = 1;
    DataManagerFlag = 0;
    osSignalWait(0x0001, osWaitForever);
    if (PB02_Fram_Record_Struct.InfBit.FramFinishFlag)
    {
      if (strstr(PB02_Fram_Record_Struct.Data_RX_BUF, "{")) // get command
      {
        PB02_Fram_Record_Struct.messageProcess(); // proccess command
      }
      PB02_Clear_Data();
    }
    osDelay(1000);
  }
  /* USER CODE END BLECommunicationEntry */
}

/* USER CODE BEGIN Header_DataManagerEntry */
/**
 * @brief Function implementing the DataManager thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_DataManagerEntry */
void DataManagerEntry(void const *argument)
{
  /* USER CODE BEGIN DataManagerEntry */
  /* Infinite loop */
  // int i=0;

  // HAL_UART_Receive_IT(&huart1, buf, 1);
  // int gccinterval = 0;
  for (;;)
  {
    BLECommunicationFlag = 0;
    DataManagerFlag = 1;
    osEvent eSensorValue;
    eSensorValue = osMessageGet(SensorQHandle, osWaitForever);
    if (eSensorValue.status == osEventMessage)
    {
      PC_USART("Data Manager task read Queue: %d\n", *(uint32_t *)eSensorValue.value.p);
      // * 蓝牙消息发送思路，手机端添加指令，根据指令反馈消息
      osEvent ePhone;
      ePhone = osSignalWait(PHONE_DATA_REQUEST, 100);

      if (ePhone.status == osEventSignal)
      {
        osMessagePut(BLEQHandle, *(uint32_t *)eSensorValue.value.p, osWaitForever);
      }

      osMessagePut(MotorQHandle, *(uint32_t *)eSensorValue.value.p, osWaitForever);

      osPoolFree(SensorDataPHandle, eSensorValue.value.p);
    }
    // PC_USART("TODO: Data Managing!%d\n", i++);
    osDelay(900);
  }
  /* USER CODE END DataManagerEntry */
}

/* USER CODE BEGIN Header_SensorCommunicationEntry */
/**
 * @brief 电机控制思路：0. 保证调速和正反转 1. 手机发送信号启停电机
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_SensorCommunicationEntry */
void SensorCommunicationEntry(void const *argument)
{
  /* USER CODE BEGIN SensorCommunicationEntry */
  /* Infinite loop */
  HAL_UART_Receive_IT(&huart2, buf, 1);
  for (;;)
  {
    // PC_USART("TODO: Sensor\n");
    modbus_rtu();

    osDelay(1000);
  }
  /* USER CODE END SensorCommunicationEntry */
}

/* USER CODE BEGIN Header_MotorControlEntry */
/**
 * @brief 电机控制思路：1. 读队列，获取当前压力数值 2. PID控制器 3. 读取手机端期望压力数值，提供一个改变期望压力数值的函数 4. 根据压力误差
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_MotorControlEntry */
void MotorControlEntry(void const *argument)
{
  /* USER CODE BEGIN MotorControlEntry */
  MS_Motor_Init(1700, 100, 2000);
  MS_Press_PID_Param_Set(0.05, 0.1, 0);
  MS_Press_PID_Param_Reset();
  /* Infinite loop */
  PC_USART("Waiting for Start command!\n");
  for (;;)
  {
    osEvent evt;
    evt = osSignalWait(PHONE_MOTOR_REQUEST, 100);
    if (evt.status == osEventSignal)
    {
      MS_Motor_Update();
    }

    // evt = osSignalWait(MOTOR_STOP_REQUEST, osWaitForever);
    // if (evt.status == osEventSignal)
    // {
    //   HAL_GPIO_WritePin(Motor_In1_GPIO_Port, Motor_In1_Pin, GPIO_PIN_RESET);
    //   HAL_GPIO_WritePin(Motor_In2_GPIO_Port, Motor_In2_Pin, GPIO_PIN_RESET);
    //   HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
    //   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
    // }
    osEvent eMotorValue;
    eMotorValue = osMessageGet(MotorQHandle, osWaitForever);
    if (eMotorValue.status == osEventMessage)
    {
      MS_Motor_Press_Update(eMotorValue.value.v);
      PC_USART("Motor current press: %d\n", motor.c_Press);
      MS_Press_PID_Update();
    }
    // osDelay(1000);
  }
  /* USER CODE END MotorControlEntry */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) // 串口接收中断回调函数
{
  if (huart->Instance == USART3) // 判断发生接收中断的串�????????
  {
    // HAL_TIM_Base_Start_IT(&htim2);
    // if (PB02_Fram_Record_Struct.InfBit.FramLength < RX_BUF_MAX_LEN - 1)
    // {
    //   PB02_Fram_Record_Struct.Data_RX_BUF[PB02_Fram_Record_Struct.InfBit.FramLength++] =
    //       buf[0];
    // }
    // HAL_UART_Receive_IT(&huart3, buf, 1);
    // // __HAL_UART_CLEAR_NEFLAG(&huart3);
    // __HAL_TIM_SET_COUNTER(&htim2, 0);
  }
  if (huart->Instance == USART1) 
  {
    // HAL_TIM_Base_Start_IT(&htim2);
    // if (PB02_Fram_Record_Struct.InfBit.FramLength < RX_BUF_MAX_LEN - 1)
    // {
    //   PB02_Fram_Record_Struct.Data_RX_BUF[PB02_Fram_Record_Struct.InfBit.FramLength++] =
    //       buf[0];
    // }
    // HAL_UART_Receive_IT(&huart1, buf, 1);
    // // __HAL_UART_CLEAR_NEFLAG(&huart3);
    // __HAL_TIM_SET_COUNTER(&htim2, 0);
  }
  if (huart->Instance == USART2)
  {
    uint8_t err;
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE))
    {
      err = 1;
      errpace = 2;
    } // 检测到噪音、帧错误或校验错误
    else
      err = 0;

    if ((RS485_RX_CNT < 2047) && (err == 0))
    {
      RS485_RX_BUFF[RS485_RX_CNT] = buf[0];
      RS485_RX_CNT++;

      __HAL_TIM_SET_COUNTER(&htim2, 0);
      HAL_TIM_Base_Start_IT(&htim2);
    }
    __HAL_UART_CLEAR_NEFLAG(&huart3);
    HAL_UART_Receive_IT(&huart2, buf, 1);
  }
}

//* 串口发送中断回调，未使用
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
}

//* HAL库自带的空闲中断检测，没测试成功
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) // 串口接收中断回调函数
{
  if (huart->Instance == USART3)
  {
    // 	PB02_Fram_Record_Struct.InfBit.FramFinishFlag=1;
    // 	if (BLECommunicationHandle == NULL)
    // 	{
    // 		PC_USART("ERROR!\n");
    // 	}
    // 	else
    // 		osSignalSet(BLECommunicationHandle, 0x0001);
    // 	// HAL_UART_Receive_IT(&huart3, (uint8_t *)RxBuffer,LENGTH);
    //  PB02_Fram_Record_Struct.Data_RX_BUF[Size] = '\0';
    // 	HAL_UARTEx_ReceiveToIdle_IT(&huart3, (uint8_t*)PB02_Fram_Record_Struct.Data_RX_BUF, 1024);
    // //  HAL_UARTEx_ReceiveToIdle_IT(&huart3, (uint8_t *)RxBuffer,LENGTH);
    // 	//HAL_UART_Receive_IT(&huart3, (uint8_t *)RxBuffer,LENGTH);
    // 	PB02_USART("receive:%d", Size);
    //  //__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
  }
}

//* 串口 overrun 处理
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART3)
  {
    HAL_GPIO_WritePin(PB02_LED_GPIO_Port, PB02_LED_Pin, GPIO_PIN_SET);
    // __HAL_UNLOCK(huart);
    __HAL_UART_CLEAR_OREFLAG(huart);
    HAL_UART_Receive(huart, buf, 1, 1);
  }
}
/* USER CODE END Application */
