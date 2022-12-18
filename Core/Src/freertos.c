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
#include "BLETest.h"
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
uint8_t motor_en = 0;
struct PIDController press_ctrl;    // 压力控制器

extern uint8_t SlaverAddr;
extern uint8_t Fuction;
extern uint16_t StartAddr;
extern uint16_t ValueOrLenth;

extern uint16_t monitor;

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
 * @brief  蓝牙通信任务，消息处理函数写下bletest.c中
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
 * @brief 数据管理任务获取传感器压力数值，并提供给电机控制任务和手机任务
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_DataManagerEntry */
void DataManagerEntry(void const *argument)
{
  /* USER CODE BEGIN DataManagerEntry */
  /* Infinite loop */
  for (;;)
  {
    BLECommunicationFlag = 0;
    DataManagerFlag = 1;
    osEvent eSensorValue;
    eSensorValue = osMessageGet(SensorQHandle, osWaitForever);
    if (eSensorValue.status == osEventMessage)
    {
      // PC_USART("Data Manager task read Queue: %d\n", *(uint32_t *)eSensorValue.value.p);
      // * 蓝牙消息发送思路，手机端添加指令，根据指令反馈消息
      osEvent ePhone;
      ePhone = osSignalWait(PHONE_DATA_REQUEST, 100);
      // * 只有手机端发送数据请求指令了才返回压力数值
      if (ePhone.status == osEventSignal || monitor)
      {
        if (monitor)
        {
          Msg respond_msg[RESPOND_PAIR_LEN] = {
              {"d", Number, NULL},
          };
          respond_msg[0].value.number = (int32_t)*(uint32_t*)eSensorValue.value.p;
          PB02_USART("%s", ble_to_json(respond_msg, RESPOND_PAIR_LEN));
        }
        else
          osMessagePut(BLEQHandle, *(uint32_t *)eSensorValue.value.p, osWaitForever);
      }

      osMessagePut(MotorQHandle, *(uint32_t *)eSensorValue.value.p, osWaitForever);

      osPoolFree(SensorDataPHandle, eSensorValue.value.p);
    }
    osDelay(500);
  }
  /* USER CODE END DataManagerEntry */
}

/* USER CODE BEGIN Header_SensorCommunicationEntry */
/**
 * @brief 传感器modbus通信函数在master.c中实现
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_SensorCommunicationEntry */
void SensorCommunicationEntry(void const *argument)
{
  /* USER CODE BEGIN SensorCommunicationEntry */
  /* Infinite loop */
  Modbus_RegMap();
  HAL_UART_Receive_IT(&huart2, buf, 1);

  for (;;)
  {
    modbus_rtu();

    osDelay(500);
  }
  /* USER CODE END SensorCommunicationEntry */
}

/* USER CODE BEGIN Header_MotorControlEntry */
/**
 * @brief 电机控制函数实现在motor.c文件中
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_MotorControlEntry */
void MotorControlEntry(void const *argument)
{
  /* USER CODE BEGIN MotorControlEntry */
  MS_Motor_Init(1700, 100, 2000);
  MS_Press_PID_Param_Set(0.1, 0.05, 0);
  MS_Press_PID_Param_Reset();
  /* Infinite loop */
  PC_USART("Waiting for Start command!\n");
  enum MotorStatus last_status = motor.Status;
  for (;;)
  {
    osEvent evt;
    evt = osSignalWait(PHONE_MOTOR_REQUEST, 100);
    if (evt.status == osEventSignal ||(motor_en && motor.Status != last_status))
    {
      MS_Motor_Update();
      last_status = motor.Status;
    }

    osEvent eMotorValue;
    eMotorValue = osMessageGet(MotorQHandle, osWaitForever);
    if (eMotorValue.status == osEventMessage)
    {
      MS_Motor_Press_Update(eMotorValue.value.v);
      PC_USART("Motor current press: %d\n", motor.c_Press);
      MS_Press_PID_Update();
    }
  }
  /* USER CODE END MotorControlEntry */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) // 串口接收中断回调函数
{
  if (huart->Instance == USART3) // 判断发生接收中断的串�????????
  {
  }
  if (huart->Instance == USART1) 
  {
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
  }
}

//* 串口 overrun 处理, 进入overrun红色led灯状态会反转
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART3)
  {
    HAL_GPIO_TogglePin(PB02_LED_GPIO_Port, PB02_LED_Pin);
    __HAL_UART_CLEAR_OREFLAG(huart);
    HAL_UART_Receive(huart, buf, 1, 1);
  }
}
/* USER CODE END Application */
