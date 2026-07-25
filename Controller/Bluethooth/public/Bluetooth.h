#pragma once

#include "main.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

void Bluetooth_ATInit(void);
void Bluetooth_ATProgress(void);
void Bluetooth_TestProgress(void);
void Bluetooth_Send(void);
void Bluetooth_Receive(void);