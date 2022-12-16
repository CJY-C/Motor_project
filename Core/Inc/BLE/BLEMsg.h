#ifndef BLE_MSG_H
#define BLE_MSG_H

#include <stdbool.h>
#include "cJSON.h"

#define bool _Bool

// * ��Ҫ�û�ʵ�ֵ�ö��ֵ��ת������

/// @brief �û������ֻ���ָ��enum
extern enum PC Phone_Command;

/// @brief ��Ҫ�û���ʵ��ָ���ַ���->ָ��enum��ת������
/// @param phoneCommand ָ���ַ���
/// @return ָ��enum
extern enum PC ble_CommandToEnum(char* phoneCommand);


/// @brief json��������enum
typedef enum
{
    String,
    Number,
    Array,
    Item
}Msg_Type;

/// @brief ������Ϣ��ֵ�� 
#define MAX_KEY_LEN         39
typedef struct
{
    char key[MAX_KEY_LEN];
    Msg_Type type;
    union
    {
        char*       string;
        double      number;
    }value;
}Msg;


static cJSON* receive_json = NULL;
static char*  respond_str  = NULL;

/// @brief ��ȡ�����յ���json����
/// @param json ���յ���json�ַ���
/// @param msg  ��Ϣ�ṹ������
/// @param n    ���鳤��
/// @return true->��ȡ�ɹ�, false->��ȡʧ��
bool ble_parse_json(const char* const json, Msg* msg, const unsigned int n);

/// @brief bleת��json��Ϣ��������ʽΪ{"d":"..."}������...������������ģ��̼�����������ֻ�ܷ���12���ַ�
/// @param msg  ��Ϣ�ṹ������
/// @param n    ��Ա����
/// @return ����δ��ʽ����json�ַ���
char* ble_to_json(Msg* msg, const unsigned int n);


/// @brief ���cJSON����������
/// @param void 
void ble_clear_data(void);

#endif
