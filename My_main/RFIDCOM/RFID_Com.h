#pragma once

#include "sysconfigs.h"

typedef struct
{
    uint8_t uid[4];
    char* name;
} RFID_AuthorizedCard;

void RFID_Process();