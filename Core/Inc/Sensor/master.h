#ifndef __MASTER_H
#define __MASTER_H
// #include "sys.h"
#include "main.h"

#define READ_COIL     01
#define READ_DI       02
#define READ_HLD_REG  03
#define READ_AI       04
#define SET_COIL      05
#define SET_HLD_REG   06
#define WRITE_COIL    15
#define WRITE_HLD_REG 16


#define HI(n) ((n)>>8)
#define LOW(n) ((n)&0xff)

#define EN_485_TX_H    HAL_GPIO_WritePin(EN_485_TX_GPIO_Port, EN_485_TX_Pin, GPIO_PIN_SET)
#define EN_485_TX_L    HAL_GPIO_WritePin(EN_485_TX_GPIO_Port, EN_485_TX_Pin, GPIO_PIN_RESET)
// #define RS485_TX_EN PDout(7)

extern uint8_t huart_switch;
#define RS485_USART(fmt, ...)        huart_switch = 2; printf(fmt, ##__VA_ARGS__)    

void RS485_Init(void);
void Timer4_enable(uint16_t arr);

void Modbus_RegMap(void);

void Modbus_01_Solve(void);
void Modbus_02_Solve(void);
void Modbus_03_Solve(void);
void Modbus_05_Solve(void);
void Modbus_06_Solve(void);
void Modbus_15_Solve(void);
void Modbus_16_Solve(void);
	
void Timer7_Init(void);
void RS485_TX_Service(void);
void Master_Service(uint8_t SlaverAddr,uint8_t Fuction,uint16_t StartAddr,uint16_t ValueOrLenth);
void RS485_RX_Service(void);
void modbus_rtu(void);

uint16_t Master_ReadReginput(uint8_t teamcounter);
uint16_t Master_sendReginput(uint8_t teamcounter);
uint16_t RS485_RX_BUFFback(uint8_t count);
uint16_t volback(void); //��ѹֵ���أ�������Ƕ��λ��λ�ڽ��������У�
uint16_t erroeback(void);//������Ϣ
uint16_t errpaceback(void);// ���մ�����Ϣ
uint16_t stateback(void);//ͨѶ�������

// * ҵ���߼�����
// float parsews(uint16_t *buf);//�������
// void parsewx(uint16_t *buf);//�жϷ���
// void printfwx(int a);//��ӡ��ʾ����
// uint8_t judge_wsorwx(uint8_t SlaverAddr);//�жϷ��ٷ���
#endif









