#pragma once

#include <stdint.h>
// #include "sysconfigs.h"

typedef enum
{
    TURN_TOGGLE_MIDDLE = 0,
    TURN_TOGGLE_RIGHT = 1,
    TURN_TOGGLE_LEFT = 2,
    TURN_TOGGLE_NONE = 4
} Turn_ToggleState;

typedef struct Protocol_DataFrame Protocol_DataFrame;


void Toggle_SwitchInit(void);
void Toggle_SwitchProgress(Protocol_DataFrame* _pOutFrame);
