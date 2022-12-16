#ifndef __MOTOR_H
#define __MOTOR_H
#include "pid_controller.h"

#define MOTOR_PARAM 0.416

/**
  * @brief  	���״̬ö��ֵ
  */
enum MotorStatus
{
	MOTOR_STOP,			/**< ֹͣ */
	MOTOR_FOREWARD,	/**< ��ת */
	MOTOR_BACKWARD	/**< ��ת */
};

/**
  * @brief  	���ģ��
  * @details 	����һЩ�����ó����ͱ仯��
  */
struct Motor
{
	// ����
	float Min;											/**< ��Сת�� */
	float Max;											/**< ���ת�� */
	uint16_t e_Press; 							/**< ����ѹ��	 */
	
	// �仯��
	int16_t c_Press; 									/**< ��ǰѹ�� */
	uint16_t PWM;  									/**< PWM���� */
	enum MotorStatus Status; 	      /**< ���״̬ */
};


extern struct Motor motor; /**< ���������ж�����ģ�� */
extern struct PIDController press_ctrl; /**< ���������ж���ѹ�������� */

/**
  * @brief  		�����ʼ��
  * @param[in] 	e_press ����ѹ��
	* @param[in] 	min			��Сѹ��
	* @param[in] 	max			���ѹ��
  */
void MS_Motor_Init(uint16_t e_press, float min, float max);

/**
  * @brief  		�ı�����ת������
  * @param[in] 	status ת������ö��ֵ
  */
void MS_Motor_Direction(enum MotorStatus status);

/**
  * @brief  		���µ���Ŀ������ţ�����
  */
void MS_Motor_Update(void);

/**
  * @brief  		��ȡ���Ƶ��ת�ٵ�PWMռ�ձ�
	* @return			uint16_t
  */
uint16_t MS_Motor_GetPWM(void);

/**
  * @brief  			����ѹ��
	* @param[in]		press ��������ѹ����ֵ
  */
void MS_Motor_Press_Update(int16_t pess);

/**
  * @brief  			��������ѹ����ֵ
	* @param[in]		epress ������ѹ����ֵ
  * @return       ���趨��Χ�ڷ���ԭ��ֵ��������Χ����-1
  */
int16_t MS_Motor_Set_ePress(uint16_t epress);

/**
  * @brief  			��ߵ��������PWM
  * @param[in]    value PWM����
  */
void MS_Motor_PWM_Up(uint8_t value);

/**
  * @brief  			���͵��������PWM
  * @param[in]    value PWM����
  */
void MS_Motor_PWM_Down(uint8_t value);

/**
  * @brief  		����PID����
	* @param[in] 	kp ����ϵ��
	* @param[in] 	ki ����ϵ��
	* @param[in] 	kd ΢��ϵ��
  */
void MS_Press_PID_Param_Set(float kp, float ki, float kd);

/**
  * @brief  		����PID����������
  */
void MS_Press_PID_Param_Reset(void);

/**
  * @brief  		����PID������
	* @details		������ͨ��ѹ�������Ƶ����ת�٣������Ϊ�����ת��
  */
void MS_Press_PID_Update(void);

#endif // motor.h
