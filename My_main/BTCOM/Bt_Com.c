#include "Bt_Com.h"
#include "cmsis_os2.h"

#define SWITCH_SEND_REPEAT_COUNT (10)

static SEND_STATE g_sendState = SEND_ENABLE;
static uint8_t g_IsSending = 0;

extern UART_HandleTypeDef huart1;
extern osMutexId_t UART_MUTEXHandle;

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
    HAL_StatusTypeDef status;

    if(source == NULL)
        return HAL_ERROR;

    if(osMutexAcquire(UART_MUTEXHandle, 100) != osOK)
        return HAL_BUSY;

    if(!System_CanControl())
    {
        osMutexRelease(UART_MUTEXHandle);
        return HAL_BUSY;
    }

    frame = *source;
    Data_Wrapper(&frame);

    status = HAL_UART_Transmit(&huart1, (uint8_t*)&frame, sizeof(frame), 100);

    osMutexRelease(UART_MUTEXHandle);

    return status;
}

HAL_StatusTypeDef Send_DataFrame(const Protocol_DataFrame* dataFrame)
{
    if(dataFrame == NULL)
        return HAL_ERROR;

    if(!System_CanControl())
        return HAL_BUSY;

    return Bluetooth_Transmit(dataFrame);
}

HAL_StatusTypeDef Send_MultipleDataFrame(const Protocol_DataFrame* dataFrame, MESSAGE_DATA_TYPE messageType)
{
    Protocol_DataFrame frame;
    HAL_StatusTypeDef status = HAL_OK;

    if(dataFrame == NULL)
        return HAL_ERROR;

    
    if(COMMAND == messageType && !System_CanControl())
        return HAL_BUSY;

    if(osMutexAcquire(UART_MUTEXHandle, 100) != osOK)
        return HAL_BUSY;

    frame = *dataFrame;
    Data_Wrapper(&frame);

    for(uint8_t i = 0; i < SWITCH_SEND_REPEAT_COUNT; i++)
    {
        if(messageType == COMMAND && !System_CanControl())
        {
            status = HAL_BUSY;
            break;
        }

        status = HAL_UART_Transmit(&huart1, (uint8_t*)&frame, sizeof(frame), 100);

        if(status != HAL_OK)
            break;

        osDelay(1);
    }

    osMutexRelease(UART_MUTEXHandle);

    return status;
}


HAL_StatusTypeDef Receive_DataFrame(Protocol_DataFrame* outDataFrame)
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
    
    // ADDR:0022:08:310F5C
    memset(&copyData, 0, sizeof(copyData));
    memcpy(copyData, &_DataFrame, sizeof(_DataFrame));

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

void Set_SendEnable(SEND_STATE _State)
{
    if(g_sendState != _State)
        g_sendState = _State;
}