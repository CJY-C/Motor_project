#ifndef BLE_TEST_H
#define BLE_TEST_H

#include "main.h"

#define COMMAND_KEY_1 "command"
#define VALUE_KEY     "value"
#define VALUE_INDEX    1

// ����LED
#define LED_Pin_SetL HAL_GPIO_WritePin(PB02_LED_GPIO_Port, PB02_LED_Pin, GPIO_PIN_RESET)
#define LED_Pin_SetH HAL_GPIO_WritePin(PB02_LED_GPIO_Port, PB02_LED_Pin, GPIO_PIN_SET)

// �����շ���Ϣjson�м�ֵ�Եĸ���
#define RECEIVE_PAIR_LEN     2
#define RESPOND_PAIR_LEN     1

/// @brief �ֻ���ָ��enum
enum PC
{
    OPEN,        // ����
    CLOSE,       // �ص�
    UPDATE,      // ����ѹ����ֵ
    START,       // �������
    STOP,        // �رյ��
    FORWARD,     // ��ת
    BACKWARD,    // ��ת
    SPUP,        // ����
    SPDOWN,      // ����
    EPRESS,      // ����ѹ��ֵ
    NONE
};

/// @brief ���ڷ���JSON��Ϣ����ʧ��
/// @param void
void bletest_json_parsing_err(void);

/// @brief ���յ�δ�����ָ��
/// @param void
void bletest_unkonow_command(void);

/// @brief ���ڷ���LED��
/// @param void
void bletest_led_on(void);

/// @brief ���ڷ���LED�ر�
/// @param void 
void bletest_led_off(void);

/// @brief ���ֻ����͵�ǰѹ����ֵ
/// @param void
// TODO: ����ѡ����һ��ѹ����ֵ
void bletest_update(void);

/// @brief �����תָ��
/// @param void
void bletest_motor_forward(void);

/// @brief �����תָ��
/// @param void
void bletest_motor_backward(void);

/// @brief �������ָ��
/// @param void
void bletest_motor_start(void);

/// @brief ֹͣ���ָ��
/// @param void
void bletest_motor_stop(void);

/// @brief ���PWM���ָ��
/// @param void
void bletest_motor_spup(void);

/// @brief ���PWM����ָ��
/// @param void
void bletest_motor_spdown(void);

/// @brief �ֻ���������ѹ����ֵ
/// @param press �������ֻ��˷��͵�ѹ��ֵ
void bletest_motor_epress(uint16_t press);

/// @brief �ַ���תСд����
/// @param ps �ַ���
void mystrlwr(char *ps);

#endif // !BLE_TEST_H

