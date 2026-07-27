#include "USART_Manager.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_uart.h"
#include <stdint.h>
#include <string.h>

static SEND_STATE g_sendState = SEND_ENABLE;
static uint8_t g_IsSending = 0;
static uint8_t g_IsReceive = 0;

extern UART_HandleTypeDef huart1;

static volatile uint8_t copyData[10] = {0,};

void Send_Data(Protocol_DataFrame _DataFrame)
{
    if(g_sendState == SEND_DISABLE || g_IsSending == 1)
        return;

    g_IsSending = 1;
    
    // uint8_t copyData[10] = {0,};

    // ADDR:0022:08:310F5C
    memset(&copyData, 0, 10);
    memcpy(copyData, &_DataFrame, sizeof(_DataFrame));
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, (uint8_t*)copyData, sizeof(copyData), 300);
    memset(&copyData, 0, 10);

    if(status != HAL_OK)
    {
        const char* error_Msg = "[Send Error] Unknown Error\r\n";
    
        switch(status) 
        {
            case HAL_ERROR:   
                error_Msg = "[Send Error] UART Hardware Error\r\n";
                break;
            case HAL_BUSY:    
                error_Msg = "[Send Error] UART is Busy\r\n";
                break;
            case HAL_TIMEOUT:
                error_Msg = "[Send Error] UART Timeout\r\n";
                break;
            default: 
            break;
        }

        HAL_UART_Transmit(&huart2, (const uint8_t*)error_Msg, strlen(error_Msg) ,100);
    }

    g_IsSending = 0;
}

void Receive_Data(Protocol_DataFrame* _OutDataFrame)
{
    if(g_IsReceive == 1)
        return;

    g_IsReceive = 1;

    HAL_StatusTypeDef status = HAL_UART_Receive(&huart1, (uint8_t*)&_OutDataFrame, sizeof(*_OutDataFrame), 300);

    if(status != HAL_OK)
    {
        const char* error_Msg = "[Receive Error] Unknown Error\r\n";
    
        switch(status) 
        {
            case HAL_ERROR:   
                error_Msg = "[Receive Error] UART Hardware Error\r\n";
                break;
            case HAL_BUSY:    
                error_Msg = "[Receive Error] UART is Busy\r\n";
                break;
            case HAL_TIMEOUT:
                error_Msg = "[Receive Error] UART Timeout\r\n";
                break;
            default: 
            break;
        }

        HAL_UART_Transmit(&huart2, (const uint8_t*)error_Msg, strlen(error_Msg) ,100);
    }

    g_IsReceive = 0;
}

void Set_SendEnable(SEND_STATE _State)
{
    if(g_sendState != _State)
        g_sendState = _State;
}