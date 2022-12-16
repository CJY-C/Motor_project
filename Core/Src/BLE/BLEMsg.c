#include <string.h>
#include "FreeRTOS.h"

#include "BLEConfig.h"

bool ble_parse_json(const char* const json, Msg* msg, const unsigned int n)
{
    int i;
    cJSON* item_ptr = NULL;
    receive_json = cJSON_Parse(json);
    if (receive_json == NULL)
        return false; // json����ʧ��
    for (i=0; i < n; i++)
    {
        item_ptr = cJSON_GetObjectItem(receive_json, msg[i].key);
        if (item_ptr == NULL)
            continue; // ����ƥ�䣬������һ��
        switch (msg[i].type)
        {
        case String:
            msg[i].value.string = item_ptr->valuestring;
            break;
        case Number:
            msg[i].value.number = item_ptr->valueint;
            break;
        // todo: ���������������
        default:
            break;
        }
    }
    return true;
}
char* ble_to_json(Msg* msg, const unsigned int n)
{
    int i;
    ble_clear_data();
    cJSON* respond_json = cJSON_CreateObject();
    for (i = 0; i < n; i++)
    {
        switch (msg[i].type)
        {
        case String:
            cJSON_AddStringToObject(respond_json, msg[i].key, msg[i].value.string);
            break;
        case Number:
            cJSON_AddNumberToObject(respond_json, msg[i].key, msg[i].value.number);
            break;
        default:
            break;
        }
    }
    respond_str = cJSON_PrintUnformatted(respond_json);
    cJSON_Delete(respond_json); // �ͷ��ڴ�
    return respond_str;
}

void ble_clear_data(void)
{
    cJSON_Delete(receive_json);
    // vPortFree(respond_str); // ʹ��FreeRTOS�����ñ��У�ע����һ��
    cJSON_free(respond_str);   

    receive_json = NULL;
    respond_str  = NULL;
}
