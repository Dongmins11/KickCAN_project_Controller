#include "Joystick.h"
#include "Controller.h"
#include "USART_Manager.h"
#include "adc.h"
#include "stm32f4xx_hal_uart.h"

static EMA_FilterStruct pFilter_X = {0,};
static EMA_FilterStruct pFilter_Y = {0,};
volatile uint16_t Joystick_Value[JOYSTICK_DMA_LENGTH] = {0,0};

void Joystick_Init()
{
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)Joystick_Value, JOYSTICK_DMA_LENGTH);
}


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

static  Protocol_DataFrame data = {};




void Joystick_Progress()
{
  Filter_JoystickValue(&pFilter_X, Joystick_Value[X]);
  Filter_JoystickValue(&pFilter_Y, Joystick_Value[Y]);

  Clamp_JoystickValue();
  
  data.start_byte = 0xAA;
  data.protocal_Id = PROTOCOL_ID_MAIN;
  data.command_Id = C_STEERING_CONTROL;
  data.data[0] = (uint8_t)((Joystick_Value[Y] >> 8) & 0xFF);
  data.data[1] = (uint8_t)(Joystick_Value[Y] & 0xFF);

  data.data[2] = (uint8_t)((Joystick_Value[X] >> 8) & 0xFF);
  data.data[3] = (uint8_t)(Joystick_Value[X] & 0xFF);
  data.end = 0xFF;

  // 테스트로 UART 데이터 송신
  // printf("X : [%u] \r\n Y : %u \r\n", Joystick_Value[X], Joystick_Value[Y]);
  Send_Data(data);
}



// 필터 적용 후 중앙 값 (연산이 들어감)
//X 오차 535 ~ 538 +- 3
//Y 오차 517 ~ 520 +- 3

// 필터 적용 전 중앙 값
//X 오차 534 ~ 539 +- 5
//Y 오차 515 ~ 520 +- 5