#include "my_adc.h"

extern ADC_HandleTypeDef hadc1;
extern volatile uint16_t Joystick_Value[JOYSTICK_DMA_LENGTH];


void My_ADC_Init()
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)Joystick_Value, JOYSTICK_DMA_LENGTH);

}