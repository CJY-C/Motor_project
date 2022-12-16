#ifndef __MOTOR_H
#define __MOTOR_H
#include "pid_controller.h"

#define MOTOR_PARAM 0.416

/**
  * @brief  	电机状态枚举值
  */
enum MotorStatus
{
	MOTOR_STOP,			/**< 停止 */
	MOTOR_FOREWARD,	/**< 正转 */
	MOTOR_BACKWARD	/**< 反转 */
};

/**
  * @brief  	电机模型
  * @details 	保存一些可配置常量和变化量
  */
struct Motor
{
	// 常量
	float Min;											/**< 最小转速 */
	float Max;											/**< 最大转速 */
	uint16_t e_Press; 							/**< 期望压力	 */
	
	// 变化量
	int16_t c_Press; 									/**< 当前压力 */
	uint16_t PWM;  									/**< PWM输入 */
	enum MotorStatus Status; 	      /**< 电机状态 */
};


extern struct Motor motor; /**< 在主函数中定义电机模型 */
extern struct PIDController press_ctrl; /**< 在主函数中定义压力控制器 */

/**
  * @brief  		电机初始化
  * @param[in] 	e_press 期望压力
	* @param[in] 	min			最小压力
	* @param[in] 	max			最大压力
  */
void MS_Motor_Init(uint16_t e_press, float min, float max);

/**
  * @brief  		改变电机的转动方向
  * @param[in] 	status 转动方向枚举值
  */
void MS_Motor_Direction(enum MotorStatus status);

/**
  * @brief  		更新电机的控制引脚（方向）
  */
void MS_Motor_Update(void);

/**
  * @brief  		获取控制电机转速的PWM占空比
	* @return			uint16_t
  */
uint16_t MS_Motor_GetPWM(void);

/**
  * @brief  			更新压力
	* @param[in]		press 测量到的压力数值
  */
void MS_Motor_Press_Update(int16_t pess);

/**
  * @brief  			更新期望压力数值
	* @param[in]		epress 期望的压力数值
  * @return       在设定范围内返回原数值，超出范围返回-1
  */
int16_t MS_Motor_Set_ePress(uint16_t epress);

/**
  * @brief  			提高电机的输入PWM
  * @param[in]    value PWM增量
  */
void MS_Motor_PWM_Up(uint8_t value);

/**
  * @brief  			降低电机的输入PWM
  * @param[in]    value PWM增量
  */
void MS_Motor_PWM_Down(uint8_t value);

/**
  * @brief  		配置PID参数
	* @param[in] 	kp 比例系数
	* @param[in] 	ki 积分系数
	* @param[in] 	kd 微分系数
  */
void MS_Press_PID_Param_Set(float kp, float ki, float kd);

/**
  * @brief  		重置PID参数，置零
  */
void MS_Press_PID_Param_Reset(void);

/**
  * @brief  		更新PID控制量
	* @details		这里是通过压力误差控制电机的转速，输出即为电机的转速
  */
void MS_Press_PID_Update(void);

#endif // motor.h
