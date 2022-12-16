#ifndef __PID_CONTROLLER_H
#define __PID_CONTROLLER_H
#include "gpio.h"

/**
  * @brief  	PID控制器
  * @details 	保存一些可配置常量和变化量
  */
struct PIDController
{
	// 常量
	float Kp; 			/**< 比例系数 */
	float Ki; 			/**< 积分系数 */
	float Kd; 			/**< 微分系数 */
	float d_Time;  	/**< 测量时间间隔	*/
	
	// 变化量
	float ll_Error; 	/**< 上上一次误差 */
	float l_Error; 	/**< 上一次误差 */
	float c_Error; 	/**< 当前误差 */
	float s_Error; 	/**< 累计误差 */
	float d_Error; 	/**< 误差微分 */
};

/**
  * @brief  	判断浮点数是否小于某一小量
  */
float SmallEnough(float value);
#endif
