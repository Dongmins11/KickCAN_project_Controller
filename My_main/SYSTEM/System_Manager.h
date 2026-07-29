#pragma once

#include "sysconfigs.h"

#define CONTROL_FLAG_RFID_AUTHORIZED   (1 << 0)
#define CONTROL_FLAG_RFID_UNKNOWN      (1 << 1)
#define CONTROL_FLAG_BT_STATE_CHANGED  (1 << 2)
#define CONTROL_FLAG_TACT_SWITCH       (1 << 3)
#define CONTROL_FLAG_TOGGLE_SWITCH     (1 << 4)

#define CONTROL_FLAG_ALL               \
    (CONTROL_FLAG_RFID_AUTHORIZED  |   \
     CONTROL_FLAG_RFID_UNKNOWN     |   \
     CONTROL_FLAG_BT_STATE_CHANGED |   \
     CONTROL_FLAG_TACT_SWITCH      |   \
     CONTROL_FLAG_TOGGLE_SWITCH)

typedef enum
{
    SYSTEM_LOCKED = 0,
    SYSTEM_WAIT_BT,
    SYSTEM_ACTIVE,
    SYSTEM_LOCKING
} SystemState;

void System_Init(void);
void System_PostFlag(uint32_t flag);

void System_HandleRfidAuthorized(void);
void System_HandleRfidUnknown(void);
uint8_t System_HandleBluetoothStateChanged(void);
void System_LedProgress(uint32_t now);

void Control_SendCurrentToggle(void);
uint8_t System_CanControl(void);
SystemState System_GetState(void);
