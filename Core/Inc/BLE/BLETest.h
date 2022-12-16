#ifndef BLE_TEST_H
#define BLE_TEST_H

#include "main.h"

#define COMMAND_KEY_1 "command"
#define VALUE_KEY     "value"
#define VALUE_INDEX    1

// 测试LED
#define LED_Pin_SetL HAL_GPIO_WritePin(PB02_LED_GPIO_Port, PB02_LED_Pin, GPIO_PIN_RESET)
#define LED_Pin_SetH HAL_GPIO_WritePin(PB02_LED_GPIO_Port, PB02_LED_Pin, GPIO_PIN_SET)

// 配置收发消息json中键值对的个数
#define RECEIVE_PAIR_LEN     2
#define RESPOND_PAIR_LEN     1

/// @brief 手机端指令enum
enum PC
{
    OPEN,        // 开灯
    CLOSE,       // 关灯
    UPDATE,      // 更新压力数值
    START,       // 开启电机
    STOP,        // 关闭电机
    FORWARD,     // 正转
    BACKWARD,    // 反转
    SPUP,        // 提速
    SPDOWN,      // 减速
    EPRESS,      // 期望压力值
    NONE
};

/// @brief 串口发送JSON消息解析失败
/// @param void
void bletest_json_parsing_err(void);

/// @brief 接收到未定义的指令
/// @param void
void bletest_unkonow_command(void);

/// @brief 串口发送LED打开
/// @param void
void bletest_led_on(void);

/// @brief 串口发送LED关闭
/// @param void 
void bletest_led_off(void);

/// @brief 向手机发送当前压力数值
/// @param void
// TODO: 可以选择发送一组压力数值
void bletest_update(void);

/// @brief 电机正转指令
/// @param void
void bletest_motor_forward(void);

/// @brief 电机正转指令
/// @param void
void bletest_motor_backward(void);

/// @brief 启动电机指令
/// @param void
void bletest_motor_start(void);

/// @brief 停止电机指令
/// @param void
void bletest_motor_stop(void);

/// @brief 电机PWM提高指令
/// @param void
void bletest_motor_spup(void);

/// @brief 电机PWM降低指令
/// @param void
void bletest_motor_spdown(void);

/// @brief 手机发送期望压力数值
/// @param press 解析到手机端发送的压力值
void bletest_motor_epress(uint16_t press);

/// @brief 字符串转小写函数
/// @param ps 字符串
void mystrlwr(char *ps);

#endif // !BLE_TEST_H

