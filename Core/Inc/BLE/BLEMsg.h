#ifndef BLE_MSG_H
#define BLE_MSG_H

#include <stdbool.h>
#include "cJSON.h"

#define bool _Bool

// * 需要用户实现的枚举值和转换函数

/// @brief 用户定义手机端指令enum
extern enum PC Phone_Command;

/// @brief 需要用户来实现指令字符串->指令enum的转换函数
/// @param phoneCommand 指令字符串
/// @return 指令enum
extern enum PC ble_CommandToEnum(char* phoneCommand);


/// @brief json数据类型enum
typedef enum
{
    String,
    Number,
    Array,
    Item
}Msg_Type;

/// @brief 保存消息键值对 
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

/// @brief 读取串口收到的json数据
/// @param json 接收到的json字符串
/// @param msg  消息结构体数组
/// @param n    数组长度
/// @return true->读取成功, false->读取失败
bool ble_parse_json(const char* const json, Msg* msg, const unsigned int n);

/// @brief ble转换json消息函数，格式为{"d":"..."}，其中...部分由于蓝牙模块固件库的限制最多只能发送12个字符
/// @param msg  消息结构体数组
/// @param n    成员个数
/// @return 返回未格式化的json字符串
char* ble_to_json(Msg* msg, const unsigned int n);


/// @brief 清除cJSON创建的数据
/// @param void 
void ble_clear_data(void);

#endif
