/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Controller.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == PA4_A2_KLAXON_Pin)
  {
    // static uint8_t count1 = 0;
    static uint32_t last_exti_time = 0;
    uint32_t current_time = osKernelGetTickCount();

    if ((current_time - last_exti_time) < 100) 
      return; 

    Protocol_DataFrame dataFrame = {0,};
    dataFrame.protocal_Id = PROTOCOL_ID;
    dataFrame.command_Id = C_HORN_SIGNAL;
    Send_Data(dataFrame);

    // printf("[%d] call \r\n", count1++);
    last_exti_time = current_time;
  }

  if(GPIO_Pin == PB0_A3_TurnL_Pin)
  {
        // static uint8_t count1 = 0;
    static uint32_t last_exti_time = 0;
    uint32_t current_time = osKernelGetTickCount();

    if ((current_time - last_exti_time) < 100) 
      return; 

    printf("left \r\n");

    last_exti_time = current_time;
  } 

  if(GPIO_Pin == PC1_A4_TurnR_Pin)
  {
    static uint32_t last_exti_time = 0;
    uint32_t current_time = osKernelGetTickCount();

    if ((current_time - last_exti_time) < 100) 
      return; 

    printf("Right \r\n");

    last_exti_time = current_time;
  }

}

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Joystick */
osThreadId_t JoystickHandle;
const osThreadAttr_t Joystick_attributes = {
  .name = "Joystick",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for RFID */
osThreadId_t RFIDHandle;
const osThreadAttr_t RFID_attributes = {
  .name = "RFID",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Bluetooth */
osThreadId_t BluetoothHandle;
const osThreadAttr_t Bluetooth_attributes = {
  .name = "Bluetooth",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void JoystickTask(void *argument);
void RFIDTask(void *argument);
void BluetoothTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of Joystick */
  JoystickHandle = osThreadNew(JoystickTask, NULL, &Joystick_attributes);

  /* creation of RFID */
  RFIDHandle = osThreadNew(RFIDTask, NULL, &RFID_attributes);

  /* creation of Bluetooth */
  BluetoothHandle = osThreadNew(BluetoothTask, NULL, &Bluetooth_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  osThreadExit();
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_JoystickTask */
/**
* @brief Function implementing the Joystick thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_JoystickTask */
void JoystickTask(void *argument)
{
  /* USER CODE BEGIN JoystickTask */
  
  Joystick_Init();

  /* Infinite loop */
  for(;;)
  {
      Joystick_Progress();

    //  uint32_t start_tick = osKernelGetTickCount();
    //   if(osKernelGetTickCount() - start_tick < 5)
    // previous_button = current_button;

    osDelay(10);
  }
  /* USER CODE END JoystickTask */
}

/* USER CODE BEGIN Header_RFIDTask */
/**
* @brief Function implementing the RFID thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_RFIDTask */
void RFIDTask(void *argument)
{
  /* USER CODE BEGIN RFIDTask */
  RC522_Init();

  /* Infinite loop */
  for(;;)
  {
    uint8_t tagType[2] = { 0,};
    uint8_t serNum[5] = {0,};

    if(RC522_Request(PICC_REQIDL, tagType) == MI_OK)
    {
       if(RC522_Anticoll(serNum) == MI_OK)
       {
         printf("Tag Type : 0x%02X%02X | UID : 0x%02X %02X %02X %02X %02X \r\n", 
                tagType[0], tagType[1],
                serNum[0], serNum[1], serNum[2], serNum[3], serNum[4]);

        osDelay(500);
       }
    }

    osDelay(1);
  }
  /* USER CODE END RFIDTask */
}

/* USER CODE BEGIN Header_BluetoothTask */
/**
* @brief Function implementing the Bluetooth thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_BluetoothTask */
void BluetoothTask(void *argument)
{
  /* USER CODE BEGIN BluetoothTask */

    for (;;)
    { 
      // Bluetooth_ATProgress();
      // Bluetooth_TestProgress();
      osDelay(1);
    }
  /* USER CODE END BluetoothTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

