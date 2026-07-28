#include "TactSwitch.h"

void Tact_SwitchProgress(Protocol_DataFrame* _pOutFrame)
{
    GPIO_PinState hornState;

    if(outFrame == NULL)
        return;

    hornState = HAL_GPIO_ReadPin(PA4_A2_KLAXON_GPIO_Port, PA4_A2_KLAXON_Pin);

    _pOutFrame->protocal_Id = PROTOCOL_ID_MAIN;
    _pOutFrame->command_Id = C_HORN_SIGNAL;
    _pOutFrame->data[0] = hornState == GPIO_PIN_RESET ? 1 : 0;
}
