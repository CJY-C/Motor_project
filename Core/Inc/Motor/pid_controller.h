#ifndef __PID_CONTROLLER_H
#define __PID_CONTROLLER_H
#include "gpio.h"

/**
  * @brief  	PID������
  * @details 	����һЩ�����ó����ͱ仯��
  */
struct PIDController
{
	// ����
	float Kp; 			/**< ����ϵ�� */
	float Ki; 			/**< ����ϵ�� */
	float Kd; 			/**< ΢��ϵ�� */
	float d_Time;  	/**< ����ʱ����	*/
	
	// �仯��
	float ll_Error; 	/**< ����һ����� */
	float l_Error; 	/**< ��һ����� */
	float c_Error; 	/**< ��ǰ��� */
	float s_Error; 	/**< �ۼ���� */
	float d_Error; 	/**< ���΢�� */
};

/**
  * @brief  	�жϸ������Ƿ�С��ĳһС��
  */
float SmallEnough(float value);
#endif
