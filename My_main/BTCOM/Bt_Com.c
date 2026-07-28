#include "Bt_Com.h"
#include "cmsis_os2.h"

static SEND_STATE g_sendState = SEND_ENABLE;
static uint8_t g_IsSending = 0;
static uint8_t g_IsReceive = 0;

extern UART_HandleTypeDef huart1;

static uint8_t copyData[12] = {0,};

static void Data_Wrapper(Protocol_DataFrame* frame)
{
    uint8_t* bytes;

    if(frame == NULL)
        return;

    frame->start_byte = PROTOCOL_START_BYTE;
    frame->checkSum = 0;
    frame->end = PROTOCOL_END_BYTE;

    bytes = (uint8_t*)frame;

    for(uint32_t i = 0; i < 10; i++)
        frame->checkSum ^= bytes[i];
}

static HAL_StatusTypeDef Bluetooth_Transmit(const Protocol_DataFrame* source)
{
    Protocol_DataFrame frame;

    if(source == NULL)
        return HAL_ERROR;

    frame = *source;
    Data_Wrapper(&frame);

    return HAL_UART_Transmit(&huart1, (uint8_t*)&frame, sizeof(frame), 100);
}

HAL_StatusTypeDef Send_Data(const Protocol_DataFrame* dataFrame)
{
    if(dataFrame == NULL)
        return HAL_ERROR;

    if(!System_CanControl())
        return HAL_BUSY;

    HAL_StatusTypeDef state = Bluetooth_Transmit(dataFrame);

    memset(&dataFrame, 0, sizeof(dataFrame));
}

HAL_StatusTypeDef Send_SystemData(const Protocol_DataFrame* dataFrame)
{
    return Bluetooth_Transmit(dataFrame);
}

HAL_StatusTypeDef Receive_Data(Protocol_DataFrame* outDataFrame)
{
    if(outDataFrame == NULL)
        return HAL_ERROR;

    return HAL_UART_Receive(&huart1, (uint8_t*)outDataFrame, sizeof(*outDataFrame), 300);
}


void Send_Data(Protocol_DataFrame _DataFrame)
{
    if(g_sendState == SEND_DISABLE || g_IsSending == 1)
        return;

    g_IsSending = 1;
    
    // uint8_t copyData[10] = {0,};

    // ADDR:0022:08:310F5C
    memset(&copyData, 0, sizeof(copyData));
    memcpy(copyData, &_DataFrame, sizeof(_DataFrame));
    // HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, (uint8_t*)copyData, sizeof(copyData), 300);

    for(int i =0; i < 10; ++i)
    {
       copyData[10] ^= copyData[i];
    }

    for(int i =0; i < sizeof(copyData); ++i)
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)(copyData + i), 1, 300);
        osDelay(1);
    }
    memset(&copyData, 0, sizeof(copyData));

    // if(status != HAL_OK)
    // {
    //     const char* error_Msg = "[Send Error] Unknown Error\r\n";
    
    //     switch(status) 
    //     {
    //         case HAL_ERROR:   
    //             error_Msg = "[Send Error] UART Hardware Error\r\n";
    //             break;
    //         case HAL_BUSY:    
    //             error_Msg = "[Send Error] UART is Busy\r\n";
    //             break;
    //         case HAL_TIMEOUT:
    //             error_Msg = "[Send Error] UART Timeout\r\n";
    //             break;
    //         default: 
    //         break;
    //     }

    //     HAL_UART_Transmit(&huart2, (const uint8_t*)error_Msg, strlen(error_Msg) ,100);
    // }

    g_IsSending = 0;
}

void Send_Data_Ten(Protocol_DataFrame _DataFrame)
{
    if(g_sendState == SEND_DISABLE || g_IsSending == 1)
    return;

    g_IsSending = 1;
    
    uint8_t copyData_stack[10] = {0,};
    size_t copyDataSize = sizeof(copyData_stack);
    memset(&copyData_stack, 0, copyDataSize);
    memcpy(copyData_stack, &_DataFrame, sizeof(_DataFrame));

    for(int i =0; i < 10; ++i)
    {
       copyData_stack[10] ^= copyData_stack[i];
    }

    for(int i =0; i < 10; ++i)
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)copyData_stack, copyDataSize, 300);
        osDelay(1);
    }

    memset(&copyData_stack, 0, copyDataSize);

    g_IsSending = 0;
}


void Set_SendEnable(SEND_STATE _State)
{
    if(g_sendState != _State)
        g_sendState = _State;
}