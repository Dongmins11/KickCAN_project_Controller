#include "System_Manager.h"
#include "cmsis_os2.h"
#include "main.h"

extern osThreadId_t ControlHandle;

static volatile SystemState g_systemState = SYSTEM_LOCKED;

//데이터 보내려는걸 막으려고함
static volatile uint8_t g_controlTxBlocked = 1;
static volatile uint8_t g_authFailurePending = 0;

static uint32_t g_btLedTick = 0;
static uint8_t g_btLedState = 0;

static void System_SetBluetoothPower(uint8_t powerOn);
static uint8_t System_IsBluetoothConnected(void);
static void System_SetRgb(uint8_t redOn, uint8_t greenOn, uint8_t blueOn);
static void System_SetBluetoothLed(uint8_t ledOn);
static HAL_StatusTypeDef System_SendAuthorization(uint8_t authorized);

void System_Init(void)
{
    g_systemState = SYSTEM_LOCKED;
    g_controlTxBlocked = 1;
    g_authFailurePending = 0;

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
    return g_systemState == SYSTEM_ACTIVE && g_controlTxBlocked == 0;
}
SystemState System_GetState(void)
{
    return g_systemState;
}

static uint8_t CheckAuthFailurepending()
{
    if(g_authFailurePending)
    {
        g_systemState = SYSTEM_AUTH_FAILED;
        System_BlockControlTx();

        System_SendAuthorization(AUTH_FAILED);
        System_SetRgb(1, 0, 0);
        System_SetBluetoothLed(1);

        return 1;
    }

    return 0;
}

static void System_AllowControlTx(void)
{
    g_controlTxBlocked = 0;
}

void System_BlockControlTx(void)
{
    g_controlTxBlocked = 1;
}

void System_RequestAuthFailure(void)
{
    if(g_systemState == SYSTEM_LOCKED)
        return;

    g_authFailurePending = 1;
    g_controlTxBlocked = 1;
}

void System_HandleRfidAuthorized(void)
{
    if(g_systemState == SYSTEM_LOCKED)
    {
        g_authFailurePending = 0;
        g_controlTxBlocked = 1;

        g_systemState = SYSTEM_WAIT_BT;
        g_btLedTick = osKernelGetTickCount();
        g_btLedState = 0;

        System_SetBluetoothPower(1);
        System_SetRgb(0, 1, 0);
        System_SetBluetoothLed(0);

        return;
    }

    if(g_systemState == SYSTEM_AUTH_FAILED)
    {
        g_authFailurePending = 0;
        System_BlockControlTx();

        //이미 블르투스가 연결된거니 그냥 바로 처리헤ㅐ바랴
        if(System_IsBluetoothConnected())
        {
            if(System_SendAuthorization(AUTH_UNLOCKED) != HAL_OK)
                return;

            if(CheckAuthFailurepending())
                return;

            g_systemState = SYSTEM_ACTIVE;

            System_AllowControlTx();
            System_SetRgb(0, 1, 0);
            System_SetBluetoothLed(1);

            Control_SendCurrentToggle();

            return;
        }

        //연결이 끊긴거니 다시 연겨대기상태로
        g_systemState = SYSTEM_WAIT_BT;
        g_btLedTick = osKernelGetTickCount();
        g_btLedState = 0;

        System_SetRgb(0, 1, 0);
        System_SetBluetoothLed(0);

        return;
    }
}

void System_HandleRfidUnknown(void)
{
        SystemState previousState = g_systemState;

    if(previousState == SYSTEM_LOCKED || previousState == SYSTEM_AUTH_FAILED)
        return;

    g_systemState = SYSTEM_AUTH_FAILED;

    if(System_IsBluetoothConnected())
        System_SendAuthorization(AUTH_FAILED);

    System_SetRgb(1, 0, 0);
    System_SetBluetoothLed(System_IsBluetoothConnected());
}


uint8_t System_HandleBluetoothStateChanged(void)
{
    uint8_t connected = System_IsBluetoothConnected();

    if(connected)
    {
        if(g_systemState == SYSTEM_WAIT_BT)
        {
            if(CheckAuthFailurepending())
                return 0;

            // 인ㄴ증 성공실패 여부 처리슨
            if(System_SendAuthorization(AUTH_UNLOCKED) != HAL_OK)
                return 0;

            // 데이터 보내던와중에 미확인 카드가 올 수 있으니 한번더 체크해봅시다잉
            if(CheckAuthFailurepending())
                return 0;

            g_systemState = SYSTEM_ACTIVE;

            System_AllowControlTx();
            System_SetBluetoothLed(1);
            return 1;
        }


        if (g_systemState == SYSTEM_AUTH_FAILED)
        {
            System_BlockControlTx();
            System_SendAuthorization(AUTH_FAILED);
            System_SetBluetoothLed(1);
            return 0;
        }

        return 0;
    }

    //이게 연결이 갑자기 끊어졌을 때 방어코드
    if(g_systemState == SYSTEM_ACTIVE)
    {
        System_BlockControlTx();

        g_systemState = SYSTEM_WAIT_BT;
        g_btLedTick = osKernelGetTickCount();
        g_btLedState = 0;

        System_SetBluetoothLed(0);
        return 0;
    }

    if(g_systemState == SYSTEM_AUTH_FAILED)
    {
        System_BlockControlTx();
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
        Send_MultipleDataFrame(&frame, COMMAND);
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

        case SYSTEM_AUTH_FAILED:
            System_SetRgb(1, 0, 0);
            System_SetBluetoothLed( System_IsBluetoothConnected());
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

// static void System_EnterLocked(void)
// {
//     // System_SetBluetoothPower(0);

//     g_systemState = SYSTEM_LOCKED;
//     g_btLedState = 0;

//     System_SetRgb(1, 0, 0);
//     System_SetBluetoothLed(0);
// }

static HAL_StatusTypeDef System_SendAuthorization(uint8_t authorized)
{
    Protocol_DataFrame frame = {0};

    frame.protocal_Id = PROTOCOL_ID_NODE_2;
    frame.command_Id = C_AUTH_CONTROL;
    frame.data[0] = authorized;

    return Send_MultipleDataFrame(&frame, SYSTEM);
}
