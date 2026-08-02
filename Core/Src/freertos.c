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
#include "sysconfigs.h"

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
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Bluetooth */
osThreadId_t BluetoothHandle;
const osThreadAttr_t Bluetooth_attributes = {
  .name = "Bluetooth",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Control */
osThreadId_t ControlHandle;
const osThreadAttr_t Control_attributes = {
  .name = "Control",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for UART_MUTEX */
osMutexId_t UART_MUTEXHandle;
const osMutexAttr_t UART_MUTEX_attributes = {
  .name = "UART_MUTEX"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void JoystickTask(void *argument);
void RFIDTask(void *argument);
void BluetoothTask(void *argument);
void ControlTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  My_ADC_Init();
  My_SPI_Init();
  My_UART_Init();
  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of UART_MUTEX */
  UART_MUTEXHandle = osMutexNew(&UART_MUTEX_attributes);

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
  // BluetoothHandle = osThreadNew(BluetoothTask, NULL, &Bluetooth_attributes);

  /* creation of Control */
  ControlHandle = osThreadNew(ControlTask, NULL, &Control_attributes);

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

  /* Infinite loop */
  for(;;)
  {

    if(System_CanControl())
      Joystick_Progress();

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
    RFID_Process();

    osDelay(100);
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

/* USER CODE BEGIN Header_ControlTask */
/**
* @brief Function implementing the Control thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ControlTask */
void ControlTask(void *argument)
{
  /* USER CODE BEGIN ControlTask */
  /* Infinite loop */
  /* USER CODE BEGIN SwtichTesk */
  uint32_t lastLedTick = 0;

  uint32_t tactDebounceTick = 0;
  uint32_t toggleDebounceTick = 0;

  uint8_t tactPending_flag = 0;
  uint8_t togglePending_flag = 0;

  /* Infinite loop */
  Toggle_SwitchInit();
  System_Init();

  for(;;)
  {
    uint32_t now;
    uint32_t flags;

    now = osKernelGetTickCount();

    flags = osThreadFlagsWait(CONTROL_FLAG_ALL, osFlagsWaitAny, 1);

    if((flags & osFlagsError) == 0)
    {

      if(flags & CONTROL_FLAG_RFID_AUTHORIZED)
          System_HandleRfidAuthorized();

          
      if(flags & CONTROL_FLAG_RFID_UNKNOWN)
          System_HandleRfidUnknown();


      if(flags & CONTROL_FLAG_BT_STATE_CHANGED)
      {
          uint8_t activated = System_HandleBluetoothStateChanged();

          if(activated)
              Control_SendCurrentToggle();
      }

      if(flags & CONTROL_FLAG_TACT_SWITCH)
      {
          tactPending_flag = 1;
          tactDebounceTick = now;
      }

      if(flags & CONTROL_FLAG_TOGGLE_SWITCH)
      {
          togglePending_flag = 1;
          toggleDebounceTick = now;
      }
    }

    now = osKernelGetTickCount();

    if(tactPending_flag && (now - tactDebounceTick) >= TACT_DEBOUNCE_MS)
    {
      Protocol_DataFrame frame = {0};

      tactPending_flag = 0;

      if(System_CanControl())
      {
          Tact_SwitchProgress(&frame);

          if(frame.protocal_Id != 0)
              Send_MultipleDataFrame(&frame, COMMAND);
      }
    }

    if(togglePending_flag && (now - toggleDebounceTick) >= TOGGLE_DEBOUNCE_MS)
    {
        Protocol_DataFrame frame = {0};

        togglePending_flag = 0;

        if(System_CanControl())
        {
            Toggle_SwitchProgress(&frame);

            if(frame.protocal_Id != 0)
                Send_MultipleDataFrame(&frame, COMMAND);
        }
      }

      /* LED 로직 함수에는 반복문이 없고 여기서 100ms마다 호출한다. */
      if((now - lastLedTick) >= LED_UPDATE_PERIOD_MS)
      {
          lastLedTick = now;
          System_LedProgress(now);
      }


    osDelay(1);
  }
  /* USER CODE END ControlTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */

