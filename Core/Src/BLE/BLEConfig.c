#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "BLEConfig.h"	  

//extern osThreadId BLECommunicationHandle;
///**
//  * @brief  Reception Event Callback (Rx event notification called after use of advanced reception service).
//  * @param  huart UART handle
//  * @param  Size  Number of data available in application reception buffer (indicates a position in
//  *               reception buffer until which, data are available)
//  * @retval None
//  */
//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
//{
//  /* Prevent unused argument(s) compilation warning */
//  // UNUSED(huart);
//  // UNUSED(Size);

////	if (huart == &huart3)
////	{
//		 if (BLECommunicationHandle == NULL)
//		{
//			PC_USART("ERROR!\n");
//    // 6襽通褏袩詫未源莰
//		}
//		else
//			osSignalSet(BLECommunicationHandle, 0x0001);
//		PB02_Fram_Record_Struct.InfBit.FramFinishFlag = 1;
////	}
//	// HAL_UART_Receive(&huart3, (uint8_t*)PB02_Fram_Record_Struct.Data_RX_BUF, Size, Size*2);

//  /* NOTE : This function should not be modified, when the callback is needed,
//            the HAL_UARTEx_RxEventCallback can be implemented in the user file.
//   */
//}

// void USART2_IRQHandler(void)
// {   
//     u8 ucCh;

//     if(USART_GetITStatus( USART2, USART_IT_RXNE ) != RESET )
//     {
//         ucCh  = USART_ReceiveData( USART2 );
// 		// USART_SendData(USART1,ucCh);
// 		// USART_SendData(USART3,ucCh);
// 		if(PB02_Fram_Record_Struct.InfBit.FramLength < ( RX_BUF_MAX_LEN - 1 ) ) 
// 		{
//             PB02_Fram_Record_Struct.Data_RX_BUF[ PB02_Fram_Record_Struct.InfBit.FramLength ++ ]  = ucCh;
// 	    }
//     }

    // if( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )                                         //如果总线空闲，即代表发送结束
//     {
//         PB02_Fram_Record_Struct.InfBit.FramFinishFlag = 1;
// 		// todo: 理解一下代码

//         ucCh = USART_ReceiveData(USART2);                                                      //由软件序列清除中断标志位（先读USART_SR,然后读USART_DR）
// 		// USART_SendData(USART1,ucCh);
//     }
// }
