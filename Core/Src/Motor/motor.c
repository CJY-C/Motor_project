#include "motor.h"
#include "tim.h"

#include <stdio.h>


extern uint8_t huart_switch;
// 电机转动控制
void MS_Motor_Init(uint16_t e_press, float min, float max)
{
	motor.e_Press = e_press;
	motor.Min = min;
	motor.Max = max;
	
	motor.c_Press = 0;
	motor.PWM = 30;
	motor.Status = MOTOR_STOP;
}

inline void MS_Motor_Direction(enum MotorStatus status)
{
	motor.Status = status;
	// 重置PID参数
	MS_Press_PID_Param_Reset();

	switch (status)
	{
	default:
	case MOTOR_STOP:
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1); //* 关闭定时器
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
		break;
	case MOTOR_FOREWARD:
	case MOTOR_BACKWARD:
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); //* 开启定时器
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, motor.PWM);
		break;
	}
	
	// // 开启定时器
	// HAL_TIM_Base_Start_IT(&htim3);
	// HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

void MS_Motor_Update()
{
	switch (motor.Status)
	{	
		default:
		case MOTOR_STOP:
			HAL_GPIO_WritePin(Motor_In1_GPIO_Port, Motor_In1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Motor_In2_GPIO_Port, Motor_In2_Pin, GPIO_PIN_RESET);
			break;
		case MOTOR_FOREWARD:
			HAL_GPIO_WritePin(Motor_In1_GPIO_Port, Motor_In1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Motor_In2_GPIO_Port, Motor_In2_Pin, GPIO_PIN_SET);
			break;
		case MOTOR_BACKWARD:
			HAL_GPIO_WritePin(Motor_In1_GPIO_Port, Motor_In1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Motor_In2_GPIO_Port, Motor_In2_Pin, GPIO_PIN_RESET);
			break;
	}
}

inline void MS_Motor_Press_Update(int16_t press)
{
	motor.c_Press = press;
}

int16_t MS_Motor_Set_ePress(uint16_t epress)
{
	if (epress <= motor.Max && epress >= motor.Min)
	{
		motor.e_Press = epress;
		// motor.e_Press = epress - 2 * epress * (epress/(motor.Max - motor.Min));
		return epress;
	}
	return -1;
}

void MS_Motor_PWM_Up(uint8_t value)
{
	// PWM 34 以后速度变化不大
	if (motor.PWM == 20)
		motor.PWM = 24;
	motor.PWM += value;
	if (motor.PWM > 34)
		motor.PWM = 34;
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, motor.PWM);
}

void MS_Motor_PWM_Down(uint8_t value)
{
	// PWM 24以下抖动太剧烈
	motor.PWM -= value;
	if (motor.PWM < 24)
		motor.PWM = 20;
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, motor.PWM);
}

inline uint16_t MS_Motor_GetPWM()
{
	return motor.PWM;
}

// PID 参数配置
void MS_Press_PID_Param_Set(float kp, float ki, float kd)
{
	press_ctrl.Kp = kp;
	press_ctrl.Ki = ki;
	press_ctrl.Kd = kd;
}
void MS_Press_PID_Param_Reset()
{
	press_ctrl.ll_Error = 0;
	press_ctrl.l_Error = 0;
	press_ctrl.c_Error = 0;
	press_ctrl.s_Error = 0;
	press_ctrl.d_Error = 0;
	press_ctrl.d_Time  = 0.1;
}

// PID 计算更新
void MS_Press_PID_Update()
{
	// if (motor.Status == MOTOR_STOP)
	// 	return;

	// if (motor.c_Press > motor.e_Press && motor.Status != MOTOR_BACKWARD)
	// 	MS_Motor_Direction(MOTOR_BACKWARD);
	// else if (motor.Status != MOTOR_FOREWARD)
	// 	MS_Motor_Direction(MOTOR_FOREWARD);

	int16_t __abs(int16_t number);
	press_ctrl.l_Error = press_ctrl.c_Error; //* 保存上一次的误差
	press_ctrl.c_Error = motor.e_Press - motor.c_Press;	//* 计算当前误差
		// press_ctrl.c_Error = __abs(motor.e_Press - motor.c_Press);	//* 计算当前误差
	press_ctrl.d_Error = (press_ctrl.c_Error - press_ctrl.l_Error);
	// press_ctrl.d_Error = (press_ctrl.c_Error - press_ctrl.l_Error) / press_ctrl.d_Time; //* 计算误差的导数																							

	// 计算占空比
	int16_t d_PWM = press_ctrl.Kp * press_ctrl.d_Error 
						 		+ press_ctrl.Ki * press_ctrl.c_Error; 
	if (motor.Status == MOTOR_BACKWARD)
		d_PWM = - d_PWM;
	PC_USART("d_PWM: %d\n", d_PWM);
	if (motor.Status == MOTOR_STOP)
			return;
	if (d_PWM > 0)
		MS_Motor_PWM_Up(d_PWM);
	else
		MS_Motor_PWM_Down(__abs(d_PWM));
}

int16_t __abs(int16_t number)
{
	if (number >= 0)
		return number;
	else
		return -number;
}
