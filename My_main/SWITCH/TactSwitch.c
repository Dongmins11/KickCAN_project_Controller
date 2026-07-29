#include "TactSwitch.h"
#include "Bt_Com.h"
#include "stm32f4xx_hal_gpio.h"

void Tact_SwitchProgress(Protocol_DataFrame* _pOutFrame)
{
    GPIO_PinState hornState;

    if(_pOutFrame == NULL)
        return;

    hornState = HAL_GPIO_ReadPin(PA4_A2_KLAXON_GPIO_Port, PA4_A2_KLAXON_Pin);

    _pOutFrame->protocal_Id = PROTOCOL_ID_NODE_2;
    _pOutFrame->command_Id = C_HORN_SIGNAL;
    _pOutFrame->data[0] = hornState == GPIO_PIN_RESET ? 1 : 0;
}
