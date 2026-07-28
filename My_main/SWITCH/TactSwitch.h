#pragma once

#include "sysconfigs.h"

volatile uint8_t g_Horn_flag = 0;


void Tact_SwitchProgress(Protocol_DataFrame* _pOutFrame);