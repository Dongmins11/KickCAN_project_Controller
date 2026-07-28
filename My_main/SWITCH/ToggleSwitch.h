#pragma once

#include "sysconfigs.h"

typedef enum
{
    TURN_TOGGLE_LEFT = 0,
    TURN_TOGGLE_MIDDLE = 1,
    TURN_TOGGLE_RIGHT = 2,
    TURN_TOGGLE_NONE = 4
} Turn_ToggleState;

volatile uint8_t g_turnToggle_flag = 0;


void Toggle_SwitchInit(void);
void Toggle_SwitchProgress(Protocol_DataFrame* outFrame);
