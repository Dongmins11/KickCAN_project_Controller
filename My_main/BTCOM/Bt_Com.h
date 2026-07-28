#pragma once

#include "sysconfigs.h"

#define MAX_DATA_INDEX (7)
#define PROTOCOL_START_BYTE     (0xAA)
#define PROTOCOL_END_BYTE       (0xFF)

typedef struct 
{
    uint8_t start_byte;
    uint8_t protocal_Id;
    uint8_t command_Id;
    uint8_t data[MAX_DATA_INDEX];
    uint8_t checkSum;
    uint8_t end;
} Protocol_DataFrame;


typedef enum
{
    SEND_DISABLE = 0,
    SEND_ENABLE = 1,
} SEND_STATE;



void Send_Data(Protocol_DataFrame _DataFrame);
void Send_Data_Ten(Protocol_DataFrame _DataFrame);
void Receive_Data(Protocol_DataFrame* _OutDataFrame);
void Set_SendEnable(SEND_STATE _State);