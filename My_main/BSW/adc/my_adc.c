#include "my_adc.h"

extern ADC_HandleTypeDef hadc1;

void My_ADC_Init()
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)Joystick_Value, JOYSTICK_DMA_LENGTH);

}