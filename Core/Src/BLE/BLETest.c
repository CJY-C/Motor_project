#include "BLETest.h"

#include "BLEConfig.h"

#include "motor.h"

#include "cmsis_os.h"
extern uint8_t huart_switch;

extern osThreadId DataManagerHandle;
extern osThreadId MotorControlHandle;
extern osMessageQId BLEQHandle;

extern struct Motor motor;
extern uint8_t motor_en;

void msgProcess(void)
{
    Msg receive_msg[RECEIVE_PAIR_LEN] = {
        {COMMAND_KEY_1, String, NULL},
        {VALUE_KEY, Number, NULL}, //* value index
    };

    PB02_Fram_Record_Struct.Data_RX_BUF[PB02_Fram_Record_Struct.InfBit.FramLength] = '\0';

    PC_USART("get raw message%s\n", PB02_Fram_Record_Struct.Data_RX_BUF);

    if (ble_parse_json(PB02_Fram_Record_Struct.Data_RX_BUF, receive_msg, RECEIVE_PAIR_LEN))
    {
        int i;
        PC_USART("%s\n", "parsing json data successdully!");
        for (i = 0; i < RECEIVE_PAIR_LEN; i++)
        {
            if (receive_msg[i].type != String)
                continue;
            switch (ble_CommandToEnum(receive_msg[i].value.string))
            {
            case OPEN:
                bletest_led_on();
                break;
            case CLOSE:
                bletest_led_off();
                break;
            case UPDATE:
                bletest_update();
                break;
            case START:
                bletest_motor_start();
                break;
            case STOP:
                bletest_motor_stop();
                break;
            case SPUP:
                bletest_motor_spup();
                break;
            case SPDOWN:
                bletest_motor_spdown();
                break;
            case EPRESS:
                bletest_motor_epress(receive_msg[VALUE_INDEX].value.number);
                break;
            default:
                bletest_unkonow_command();
                break;
            }
        }
    }
    else
        bletest_json_parsing_err();
    ble_clear_data();
}

enum PC ble_CommandToEnum(char *phoneCommand)
{
    mystrlwr(phoneCommand);
    if (!strcmp(phoneCommand, "open"))
        return OPEN;
    else if (!strcmp(phoneCommand, "close"))
        return CLOSE;
    else if (!strcmp(phoneCommand, "update"))
        return UPDATE;
    else if (!strcmp(phoneCommand, "start"))
        return START;
    else if (!strcmp(phoneCommand, "stop"))
        return STOP;
    else if (!strcmp(phoneCommand, "forward"))
        return FORWARD;
    else if (!strcmp(phoneCommand, "backward"))
        return BACKWARD;
    else if (!strcmp(phoneCommand, "spup"))
        return SPUP;
    else if (!strcmp(phoneCommand, "spdown"))
        return SPDOWN;
    else if (!strcmp(phoneCommand, "epress"))
        return EPRESS;
    return NONE;
}

void bletest_json_parsing_err(void)
{
    Msg respond_msg[RESPOND_PAIR_LEN] = {
        {"d", String, NULL},
    };
    respond_msg[0].value.string = "Parsing err";
    PB02_USART("%s", ble_to_json(respond_msg, RESPOND_PAIR_LEN));
}

void bletest_unkonow_command(void)
{
    Msg respond_msg[RESPOND_PAIR_LEN] = {
        {"d", String, NULL},
    };
    respond_msg[0].value.string = "Unknow cmd!";
    PB02_USART("%s", ble_to_json(respond_msg, RESPOND_PAIR_LEN));
}

void bletest_led_on(void)
{
    Msg respond_msg[RESPOND_PAIR_LEN] = {
        {"d", String, NULL},
    };
    LED_Pin_SetL;
    respond_msg[0].value.string = "Led on";
    PB02_USART("%s", ble_to_json(respond_msg, RESPOND_PAIR_LEN));
}

void bletest_update(void)
{
    if (DataManagerHandle == NULL)
    {
        PC_USART("ERROR!\n");
    }
    else
        osSignalSet(DataManagerHandle, PHONE_DATA_REQUEST);

    osEvent eBLEValue;
    eBLEValue = osMessageGet(BLEQHandle, osWaitForever);
    if (eBLEValue.status == osEventMessage)
    {
        // PC_USART("BLE task read Queue: %d\n", (uint32_t)eBLEValue.value.v);
    }
    Msg respond_msg[RESPOND_PAIR_LEN] = {
        {"d", Number, NULL},
    };
    respond_msg[0].value.number = (double)(int32_t)eBLEValue.value.v;
    PB02_USART("%s", ble_to_json(respond_msg, RESPOND_PAIR_LEN));
}

void bletest_motor_forward(void)
{
    MS_Motor_Direction(MOTOR_FOREWARD);

}

void bletest_motor_backward(void)
{
    MS_Motor_Direction(MOTOR_BACKWARD);
}

void bletest_led_off(void)
{
    Msg respond_msg[RESPOND_PAIR_LEN] = {
        {"d", String, NULL},
    };
    LED_Pin_SetH;
    respond_msg[0].value.string = "Led off";
    PB02_USART("%s", ble_to_json(respond_msg, RESPOND_PAIR_LEN));
}

void bletest_motor_start(void)
{
    if (MotorControlHandle == NULL)
    {
        PC_USART("ERROR: No MotorControlHandle!\n");
    }
    else
    {
        MS_Motor_Direction(MOTOR_FOREWARD);
        // motor.Status = MOTOR_FOREWARD;
        osSignalSet(MotorControlHandle, PHONE_MOTOR_REQUEST);
        motor_en = 1;
        Msg respond_msg[RESPOND_PAIR_LEN] = {
            {"d", String, NULL},
        };
        respond_msg[0].value.string = "motor start";
        PB02_USART("%s", ble_to_json(respond_msg, RESPOND_PAIR_LEN));
    }
}

void bletest_motor_stop(void)
{
    if (MotorControlHandle == NULL)
    {
        PC_USART("ERROR: No MotorControlHandle!\n");
    }
    else
    {
        MS_Motor_Direction(MOTOR_STOP);
        // motor.Status = MOTOR_STOP;
        motor_en = 0;
        osSignalSet(MotorControlHandle, PHONE_MOTOR_REQUEST);
        Msg respond_msg[RESPOND_PAIR_LEN] = {
            {"d", String, NULL},
        };
        respond_msg[0].value.string = "motor stop";
        PB02_USART("%s", ble_to_json(respond_msg, RESPOND_PAIR_LEN));
    }
}

void bletest_motor_spup(void)
{
    if (MotorControlHandle == NULL)
    {
        PC_USART("ERROR: No MotorControlHandle!\n");
    }
    else
    {
        MS_Motor_PWM_Up(1);
        // osSignalSet(MotorControlHandle, PHONE_MOTOR_REQUEST);
        Msg respond_msg[RESPOND_PAIR_LEN] = {
            {"d", Number, NULL},
        };
        respond_msg[0].value.number = motor.PWM;
        PB02_USART("%s", ble_to_json(respond_msg, RESPOND_PAIR_LEN));
    }
}

void bletest_motor_spdown(void)
{
    if (MotorControlHandle == NULL)
    {
        PC_USART("ERROR: No MotorControlHandle!\n");
    }
    else
    {
        MS_Motor_PWM_Down(1);
        // osSignalSet(MotorControlHandle, PHONE_MOTOR_REQUEST);
        Msg respond_msg[RESPOND_PAIR_LEN] = {
            {"d", Number, NULL},
        };
        respond_msg[0].value.number = motor.PWM;
        PB02_USART("%s", ble_to_json(respond_msg, RESPOND_PAIR_LEN));
    }
}

void bletest_motor_epress(uint16_t press)
{
    Msg respond_msg[RESPOND_PAIR_LEN] = {
        {"d", Number, NULL},
    };
    respond_msg[0].value.number = MS_Motor_Set_ePress(press);
    if (motor.c_Press < motor.e_Press)
        bletest_motor_forward();
    else
        bletest_motor_backward();
    PB02_USART("%s", ble_to_json(respond_msg, RESPOND_PAIR_LEN));
}

void mystrlwr(char *ps)
{
    while (*ps)
    {
        if ('A' <= *ps && *ps <= 'Z')
            *ps += 'a' - 'A';
        ps++;
    }
}
