#pragma once

#include "sysconfigs.h"

extern ADC_HandleTypeDef hadc1;

#define MAX_VALUE_CHECK (1000)
#define MAX_VALUE (1023)

#define MIN_VALUE_CHECK (10)
#define MIN_VALUE (0)

#define JOYSTICK_DMA_LENGTH (2)


typedef struct
{
    int32_t value;
    uint8_t initialized;
} EMA_FilterStruct;

typedef enum
{
  X = 0,
  Y = 1,
} JOYSTICK_TYPE;


void Joystick_Init();
void Clamp_JoystickValue();
uint16_t Filter_JoystickValue(EMA_FilterStruct* filter, uint16_t input);
void Joystick_Progress();
