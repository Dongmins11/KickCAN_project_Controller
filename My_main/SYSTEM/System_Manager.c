#include "System_Manager.h"
#include "cmsis_os2.h"
#include "main.h"

extern osThreadId_t ControlHandle;

static volatile SystemState g_systemState = SYSTEM_LOCKED;
static uint32_t g_btLedTick = 0;
static uint8_t g_btLedState = 0;

static void System_SetBluetoothPower(uint8_t powerOn);
static uint8_t System_IsBluetoothConnected(void);
static void System_SetRgb(uint8_t redOn, uint8_t greenOn, uint8_t blueOn);
static void System_SetBluetoothLed(uint8_t ledOn);
static void System_EnterLocked(void);
static HAL_StatusTypeDef System_SendAuthorization(uint8_t authorized);

void System_Init(void)
{
    g_systemState = SYSTEM_LOCKED;
    g_btLedTick = osKernelGetTickCount();
    g_btLedState = 0;

    System_SetBluetoothPower(0);
    System_SetRgb(1, 0, 0);
    System_SetBluetoothLed(0);
}

void System_PostFlag(uint32_t flag)
{
    if(ControlHandle == NULL)
        return;

    osThreadFlagsSet(ControlHandle, flag);
}

uint8_t System_CanControl(void)
{
    return g_systemState == SYSTEM_ACTIVE;
}

SystemState System_GetState(void)
{
    return g_systemState;
}

void System_HandleRfidAuthorized(void)
{
    if(g_systemState != SYSTEM_LOCKED)
        return;

    g_systemState = SYSTEM_WAIT_BT;
    g_btLedTick = osKernelGetTickCount();
    g_btLedState = 0;

    System_SetBluetoothPower(1);
    System_SetRgb(0, 1, 0);
    System_SetBluetoothLed(0);
}

void System_HandleRfidUnknown(void)
{
    SystemState previousState = g_systemState;

    if(previousState == SYSTEM_LOCKED || previousState == SYSTEM_LOCKING || previousState == SYSTEM_WAIT_BT)
        return;

    /* 먼저 상태를 바꿔 조이스틱과 스위치 송신을 즉시 차단한다. */
    g_systemState = SYSTEM_LOCKING;

    /* 연결된 상태에서만 RC카에 잠금 명령을 한 번 보낸다. */
    if(previousState == SYSTEM_ACTIVE && System_IsBluetoothConnected())
    {
        System_SendAuthorization(AUTH_LOCKED);
    }

    /* Blocking UART 송신이 끝난 뒤 HC-05 전원을 차단한다. */
    System_EnterLocked();
}

uint8_t System_HandleBluetoothStateChanged(void)
{
    uint8_t connected = System_IsBluetoothConnected();

    if(connected)
    {
        if(g_systemState != SYSTEM_WAIT_BT)
            return 0;

        if(System_SendAuthorization(AUTH_UNLOCKED) != HAL_OK)
            return 0;

        g_systemState = SYSTEM_ACTIVE;
        System_SetBluetoothLed(1);
        return 1;
    }

    if(g_systemState == SYSTEM_ACTIVE)
    {
        g_systemState = SYSTEM_WAIT_BT;
        g_btLedTick = osKernelGetTickCount();
        g_btLedState = 0;
        System_SetBluetoothLed(0);
    }

    return 0;
}

void Control_SendCurrentToggle(void)
{
    Protocol_DataFrame frame = {0};

    Toggle_SwitchInit();
    Toggle_SwitchProgress(&frame);

    if(frame.protocal_Id != 0)
        Send_SwitchDataFrame(&frame);
}

void System_LedProgress(uint32_t now)
{
    switch(g_systemState)
    {
        case SYSTEM_LOCKED:
            System_SetRgb(1, 0, 0);
            System_SetBluetoothLed(0);
            break;

        case SYSTEM_WAIT_BT:
            System_SetRgb(0, 1, 0);

            if((now - g_btLedTick) >= 500)
            {
                g_btLedTick = now;
                g_btLedState = !g_btLedState;
                System_SetBluetoothLed(g_btLedState);
            }
            break;

        case SYSTEM_ACTIVE:
            System_SetRgb(0, 1, 0);
            System_SetBluetoothLed(1);
            break;

        case SYSTEM_LOCKING:
            System_SetRgb(1, 0, 0);
            System_SetBluetoothLed(0);
            break;

        default:
            break;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == PB5_D4_BSTATE_Pin)
    {
        System_PostFlag(CONTROL_FLAG_BT_STATE_CHANGED);
        return;
    }

    if(GPIO_Pin == PA4_A2_KLAXON_Pin)
    {
        System_PostFlag(CONTROL_FLAG_TACT_SWITCH);
        return;
    }

    if(GPIO_Pin == PB0_A3_TurnL_Pin || GPIO_Pin == PC1_A4_TurnR_Pin)
        System_PostFlag(CONTROL_FLAG_TOGGLE_SWITCH);
}

static void System_SetBluetoothPower(uint8_t powerOn)
{
    HAL_GPIO_WritePin(PC0_A5_RELAY_GPIO_Port, PC0_A5_RELAY_Pin, powerOn ? GPIO_PIN_SET : GPIO_PIN_RESET);


}

static uint8_t System_IsBluetoothConnected(void)
{
    return HAL_GPIO_ReadPin(PB5_D4_BSTATE_GPIO_Port, PB5_D4_BSTATE_Pin) == GPIO_PIN_SET;
}

static void System_SetRgb(uint8_t redOn, uint8_t greenOn, uint8_t blueOn)
{
    HAL_GPIO_WritePin(PC8_LEDR_GPIO_Port, PC8_LEDR_Pin, redOn ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(PC6_LEDG_GPIO_Port, PC6_LEDG_Pin, greenOn ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void System_SetBluetoothLed(uint8_t ledOn)
{
    HAL_GPIO_WritePin(PC5_LEDB_GPIO_Port, PC5_LEDB_Pin, ledOn ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void System_EnterLocked(void)
{
    System_SetBluetoothPower(0);

    g_systemState = SYSTEM_LOCKED;
    g_btLedState = 0;

    System_SetRgb(1, 0, 0);
    System_SetBluetoothLed(0);
}

static HAL_StatusTypeDef System_SendAuthorization(uint8_t authorized)
{
    Protocol_DataFrame frame = {0};

    frame.protocal_Id = PROTOCOL_ID_MAIN;
    frame.command_Id = C_AUTH_CONTROL;
    frame.data[0] = authorized;

    return Send_SystemData(&frame);
}
