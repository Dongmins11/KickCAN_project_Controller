#include "ToggleSwitch.h"
#include "main.h"
#include "def.h"

static Turn_ToggleState g_previousState = TURN_TOGGLE_NONE;

static Turn_ToggleState Toggle_ReadState(void)
{
    GPIO_PinState leftState;
    GPIO_PinState rightState;

    leftState = HAL_GPIO_ReadPin(PB0_A3_TurnL_GPIO_Port, PB0_A3_TurnL_Pin );

    rightState = HAL_GPIO_ReadPin(PC1_A4_TurnR_GPIO_Port, PC1_A4_TurnR_Pin);

    if(leftState == GPIO_PIN_SET && rightState != GPIO_PIN_SET)
        return TURN_TOGGLE_LEFT;

    if(rightState == GPIO_PIN_SET && leftState != GPIO_PIN_SET)
        return TURN_TOGGLE_RIGHT;

    return TURN_TOGGLE_MIDDLE;
}

void Toggle_SwitchInit(void)
{
    g_previousState = TURN_TOGGLE_NONE;
}

void Toggle_SwitchProgress(Protocol_DataFrame* _pOutFrame)
{
    Turn_ToggleState currentState;

    if(outFrame == NULL)
        return;

    currentState = Toggle_ReadState();

    if(currentState == g_previousState)
        return;

    g_previousState = currentState;

    _pOutFrame->protocal_Id = PROTOCOL_ID_MAIN;
    _pOutFrame->command_Id = C_TURN_SIGNAL;
    _pOutFrame->data[0] = (uint8_t)currentState;
}
