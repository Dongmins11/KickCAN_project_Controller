#include "HC05.h"

#define AT_COMMAND_SIZE          (64)
#define AT_RESPONSE_SIZE         (128)
#define AT_RECEIVE_TIMEOUT       (1000)
#define AT_RESPONSE_IDLE_TIME    (100)

#define FAILED_CONNECTINGTIMER (7000)

static char commandBuffer[AT_COMMAND_SIZE];
static uint16_t commandLength;
static uint8_t skipLineFeed;
static uint16_t g_failedConnectingTime = 0;

static void Bluetooth_Print(const char* text)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)text, (uint16_t)strlen(text), 100);
}

static void Bluetooth_PrintData(const char* data, uint16_t length)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)data, length, 100);
}

static void Bluetooth_ClearCommand(void)
{
    memset(commandBuffer, 0, sizeof(commandBuffer));
    commandLength = 0;
}

static uint16_t Bluetooth_ReadResponse(char* response)
{
    uint16_t length = 0;
    uint32_t startTick = HAL_GetTick();
    uint32_t lastReceiveTick = startTick;

    while (HAL_GetTick() - startTick < AT_RECEIVE_TIMEOUT)
    {
        uint8_t data;

        if (HAL_UART_Receive(&huart1, &data, 1, 20) == HAL_OK)
        {
            if (length < AT_RESPONSE_SIZE - 1)
                response[length++] = (char)data;

            lastReceiveTick = HAL_GetTick();
        }
        else if (length > 0 &&
                 HAL_GetTick() - lastReceiveTick >= AT_RESPONSE_IDLE_TIME)
        {
            break;
        }
    }

    response[length] = '\0';

    return length;
}

static void Bluetooth_PrintResponse(char* response, uint16_t length)
{
    uint16_t start = 0;
    uint16_t end = length;

    // 응답 앞뒤의 불필요한 줄바꿈을 제거한다.
    while (start < end && (response[start] == '\r' || response[start] == '\n'))
        start++;

    while (end > start && (response[end - 1] == '\r' || response[end - 1] == '\n'))
        end--;

    if (start == end)
    {
        Bluetooth_Print("[EMPTY RESPONSE]\r\n");
        return;
    }

    Bluetooth_PrintData(&response[start], end - start);
    Bluetooth_Print("\r\n");
}

static void Bluetooth_SendCommand(void)
{
    static const uint8_t lineEnd[] = "\r\n";
    char response[AT_RESPONSE_SIZE];

    if (commandLength == 0)
        return;

    HAL_UART_Transmit(&huart1, (uint8_t*)commandBuffer, commandLength, 100);

    HAL_UART_Transmit(&huart1, (uint8_t*)lineEnd, sizeof(lineEnd) - 1, 100);

    Bluetooth_ClearCommand();

    uint16_t responseLength = Bluetooth_ReadResponse(response);

    if (responseLength == 0)
        Bluetooth_Print("[NO RESPONSE]\r\n");
    else
        Bluetooth_PrintResponse(response, responseLength);

    Bluetooth_Print("> ");
}

static void Bluetooth_HandleEnter(void)
{
    Bluetooth_Print("\r\n");

    if (commandLength == 0)
    {
        Bluetooth_Print("> ");
        return;
    }

    Bluetooth_SendCommand();
}

static void Bluetooth_HandleBackspace(void)
{
    if (commandLength == 0)
        return;

    commandLength--;
    commandBuffer[commandLength] = '\0';

    // 터미널에서 마지막 문자를 지운다.
    Bluetooth_Print("\b \b");
}

void Bluetooth_ATInit(void)
{
    Bluetooth_ClearCommand();

    skipLineFeed = 0;

    Bluetooth_Print(
        "\r\n"
        "HC-05 AT Console\r\n"
        "MobaXterm Local Echo: OFF\r\n"
        "\r\n"
        "> ");
}


void Bluetooth_ATProgress(void)
{
    uint8_t data;

    if (HAL_UART_Receive(&huart2, &data, 1, 10) != HAL_OK)
        return;

    if (data == '\r')
    {
        skipLineFeed = 1;
        Bluetooth_HandleEnter();
        return;
    }

    if (data == '\n')
    {
        if (skipLineFeed)
        {
            skipLineFeed = 0;
            return;
        }

        Bluetooth_HandleEnter();
        return;
    }

    skipLineFeed = 0;

    if (data == '\b' || data == 0x7F)
    {
        Bluetooth_HandleBackspace();
        return;
    }

    // 제어 문자는 저장하지 않는다.
    if (data < 32 || data > 126)
        return;

    if (commandLength >= AT_COMMAND_SIZE - 1)
    {
        Bluetooth_ClearCommand();
        Bluetooth_Print("\r\n[COMMAND TOO LONG]\r\n> ");
        return;
    }

    commandBuffer[commandLength++] = (char)data;
    commandBuffer[commandLength] = '\0';

    // MobaXterm Local Echo 대신 STM32가 입력 문자를 표시한다.
    HAL_UART_Transmit(&huart2, &data, 1, 100);
}

// void Bluetooth_Receive(void)
// {
//     if(HAL_UART_Receive(&huart1, &pData, 1, 100) == HAL_OK)
//     {
//         if(pData == '\r' || pData == '\n')
//         {
//             HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 100);
//             HAL_UART_Transmit(&huart2, (uint8_t*)"RECEIVE: ", 9, 200);
//             HAL_UART_Transmit(&huart2, dataArr, arrIndex, 200);
//             arrIndex = 0;
//         }
//         else 
//         {
//             HAL_UART_Transmit(&huart2, &pData, 1, 200);
//             dataArr[arrIndex++] = pData;
//         }
//     }
// }


void Bluetooth_TestProgress(void)
{
    // Bluetooth_Send();
    // Bluetooth_Receive();

    uint8_t data;

    if (HAL_UART_Receive(&huart2, &data, 1, 1) == HAL_OK)
        HAL_UART_Transmit(&huart1, &data, 1, 100);

    if (HAL_UART_Receive(&huart1, &data, sizeof(uint8_t), 1) == HAL_OK)
        HAL_UART_Transmit(&huart2, &data, 1, 100);
}
