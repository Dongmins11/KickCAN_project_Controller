#pragma once

#include "sysconfigs.h"

typedef struct
{
    uint8_t uid[4];
    char* name;
} RFID_AuthorizedCard;

typedef enum
{
    AUTH_UNLOCKED = 0,
    AUTH_FAILED = 1,
    AUTH_LOCKED = 2,
} AUTH_TYPE;

void RFID_Process();