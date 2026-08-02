#include "Joystick.h"
#include "Bt_Com.h"
#include <math.h>

#define JOYSTICK_MAX_VALUE (1023)

static EMA_FilterStruct g_pFilter_X = {0,};
static EMA_FilterStruct g_pFilter_Y = {0,};
volatile uint16_t Joystick_Value[JOYSTICK_DMA_LENGTH] = {0,0};


// void Joystick_Init()
// {
//   HAL_ADC_Start_DMA(&hadc1, (uint32_t*)Joystick_Value, JOYSTICK_DMA_LENGTH);
// }


void Clamp_JoystickValue()
{
  if(MAX_VALUE_CHECK < Joystick_Value[X])
    Joystick_Value[X] = MAX_VALUE;
  else if(MIN_VALUE_CHECK > Joystick_Value[X])
    Joystick_Value[X] = MIN_VALUE;
  
  if(MAX_VALUE_CHECK < Joystick_Value[Y])
    Joystick_Value[Y] = MAX_VALUE;
  else if(MIN_VALUE_CHECK > Joystick_Value[Y])
    Joystick_Value[Y] = MIN_VALUE;

    //10미만 0으로 때림
    //1000초과 1023으로 떄림
}

static uint16_t Joystick_ClampValue(uint16_t _FilterValue)
{
  if(MAX_VALUE_CHECK < _FilterValue)
      return MAX_VALUE;
  else if(MIN_VALUE_CHECK > _FilterValue)
      return MIN_VALUE;

    return _FilterValue;
}


uint16_t Filter_JoystickValue(EMA_FilterStruct* filter, uint16_t input)
{
    int32_t scale = 256;
    int32_t divisor = 8;

    if (!filter->initialized)
    {
        filter->value = (int32_t)input * scale;
        filter->initialized = 1;
    }
    else
    {
        int32_t scaledInput = (int32_t)input * scale;
        filter->value += (scaledInput - filter->value) / divisor;
    }

    return (uint16_t)(filter->value / scale);
}

static uint16_t Joystick_InvertValue(uint16_t value)
{
    if(value > JOYSTICK_MAX_VALUE)
        value = JOYSTICK_MAX_VALUE;

    return JOYSTICK_MAX_VALUE - value;
}

void Joystick_Progress(void)
{
    Protocol_DataFrame data = {0};

    uint16_t originalX = Joystick_Value[X];
    uint16_t originalY = Joystick_Value[Y];

    // uint16_t invertedX = Joystick_InvertValue(originalX);
    // uint16_t invertedY = Joystick_InvertValue(originalY);

    uint16_t filteredX = Filter_JoystickValue(&g_pFilter_X, originalX);
    uint16_t filteredY = Filter_JoystickValue(&g_pFilter_Y, originalY);

    filteredX = Joystick_ClampValue(filteredX);
    filteredY = Joystick_ClampValue(filteredY);

    // printf("X Original:%u, Inverted:%u, Filtered:%u\r\n", originalX, invertedX, filteredX );
    // printf("Y Original:%u, Inverted:%u, Filtered:%u\r\n", originalY, invertedY, filteredY);

    // printf("X Original:%u \r\n", originalX); 
    // printf("Y Original:%u \r\n", originalY); 

    data.protocal_Id = PROTOCOL_ID_MAIN;
    data.command_Id = C_STEERING_CONTROL;

    data.data[0] = (uint8_t)((filteredY >> 8) & 0xFF);
    data.data[1] = (uint8_t)(filteredY & 0xFF);

    data.data[2] = (uint8_t)((filteredX >> 8) & 0xFF);
    data.data[3] = (uint8_t)(filteredX & 0xFF);

    Send_DataFrame(&data);
}



// 필터 적용 후 중앙 값 (연산이 들어감)
//X 오차 535 ~ 538 +- 3
//Y 오차 517 ~ 520 +- 3

// 필터 적용 전 중앙 값
//X 오차 534 ~ 539 +- 5
//Y 오차 515 ~ 520 +- 5