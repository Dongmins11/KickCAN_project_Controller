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
/* Definitions for Swtich */
osThreadId_t SwtichHandle;
const osThreadAttr_t Swtich_attributes = {
  .name = "Swtich",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void JoystickTask(void *argument);
void RFIDTask(void *argument);
void BluetoothTask(void *argument);
void SwtichTesk(void *argument);

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

  /* creation of Swtich */
  SwtichHandle = osThreadNew(SwtichTesk, NULL, &Swtich_attributes);

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

/* USER CODE BEGIN Header_SwtichTesk */

/**
* @brief Function implementing the Swtich thread.
* @param argument: Not used
* @retval None
*/
typedef enum
{
  TURN_TOGGLE_LEFT = 0,
  TURN_TOGGLE_MIDDLE = 1,
  TURN_TOGGLE_RIGHT = 2,
  TURN_TOGGLE_NONE = 4,
} Trun_ToggleState;


/* USER CODE END Header_SwtichTesk */
void SwtichTesk(void *argument)
{
  /* USER CODE BEGIN SwtichTesk */
  Protocol_DataFrame dataFrame_Toggle = {0,};
  Protocol_DataFrame dataFrame_Tact = {0,};

  /* Infinite loop */\
  for(;;)
  {
    Toggle_SwitchProgress(&dataFrame_Toggle);
  
    if(dataFrame_Toggle.protocal_Id != 0)
      Send_Data(dataFrame_Toggle);


    Tact_SwitchProgress(&dataFrame_Tact);

    if(dataFrame_Tact.protocal_Id != 0)
      Send_Data(dataFrame_Tact);

    osDelay(10);
  }
  /* USER CODE END SwtichTesk */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
// void ControlTask(void* argument)
// {
//     uint32_t lastJoystickTick;
//     uint32_t lastLedTick;

//     uint32_t tactDebounceTick = 0U;
//     uint32_t toggleDebounceTick = 0U;

//     uint8_t tactPending = 0U;
//     uint8_t togglePending = 0U;

//     (void)argument;

//     Joystick_Init();
//     Toggle_SwitchInit();
//     System_Init();

//     lastJoystickTick = osKernelGetTickCount();
//     lastLedTick = lastJoystickTick;

//     for(;;)
//     {
//         uint32_t now;
//         uint32_t flags;

//         /*
//          * 인터럽트/RFID 이벤트가 있으면 즉시 깨어나고,
//          * 이벤트가 없어도 최대 1ms 뒤 조이스틱과 LED를 처리한다.
//          */
//         flags = osThreadFlagsWait(
//             CONTROL_FLAG_ALL,
//             osFlagsWaitAny,
//             1U
//         );

//         now = osKernelGetTickCount();

//         if((flags & osFlagsError) == 0U)
//         {
//             if(flags & CONTROL_FLAG_RFID_AUTHORIZED)
//                 System_HandleRfidAuthorized();

//             if(flags & CONTROL_FLAG_RFID_UNKNOWN)
//                 System_HandleRfidUnknown();

//             if(flags & CONTROL_FLAG_BT_STATE_CHANGED)
//             {
//                 uint8_t activated =
//                     System_HandleBluetoothStateChanged();

//                 /* 연결 직후 현재 토글 위치를 한 번 전송한다. */
//                 if(activated)
//                     Control_SendCurrentToggle();
//             }

//             if(flags & CONTROL_FLAG_TACT_SWITCH)
//             {
//                 tactPending = 1U;
//                 tactDebounceTick = now;
//             }

//             if(flags & CONTROL_FLAG_TOGGLE_SWITCH)
//             {
//                 /*
//                  * 좌/우 핀에서 연속 엣지가 들어오면 마지막 엣지를 기준으로
//                  * 디바운싱 시간을 다시 시작한다.
//                  */
//                 togglePending = 1U;
//                 toggleDebounceTick = now;
//             }
//         }

//         now = osKernelGetTickCount();

//         /* 조이스틱은 연속 값이므로 20ms 주기 폴링한다. */
//         if((now - lastJoystickTick) >= JOYSTICK_PERIOD_MS)
//         {
//             lastJoystickTick = now;

//             if(System_CanControl())
//                 Joystick_Progress();
//         }

//         /* 택트 스위치는 EXTI 발생 후 20ms 뒤 실제 핀을 읽는다. */
//         if(tactPending &&
//            (now - tactDebounceTick) >= TACT_DEBOUNCE_MS)
//         {
//             Protocol_DataFrame frame = {0};

//             tactPending = 0U;

//             if(System_CanControl())
//             {
//                 Tact_SwitchProgress(&frame);

//                 if(frame.protocal_Id != 0U)
//                     Send_Data(&frame);
//             }
//         }

//         /*
//          * 3단 토글도 EXTI 기반이다.
//          * 마지막 Rising/Falling 엣지 후 20ms 뒤 좌/우 핀을 함께 읽어
//          * LEFT / MIDDLE / RIGHT를 확정한다.
//          */
//         if(togglePending &&
//            (now - toggleDebounceTick) >= TOGGLE_DEBOUNCE_MS)
//         {
//             Protocol_DataFrame frame = {0};

//             togglePending = 0U;

//             if(System_CanControl())
//             {
//                 Toggle_SwitchProgress(&frame);

//                 if(frame.protocal_Id != 0U)
//                     Send_Data(&frame);
//             }
//         }

//         /* LED 로직 함수에는 반복문이 없고 여기서 100ms마다 호출한다. */
//         if((now - lastLedTick) >= LED_UPDATE_PERIOD_MS)
//         {
//             lastLedTick = now;
//             System_LedProgress(now);
//         }
//     }
// }

// void RFIDTask(void* argument)
// {
//     (void)argument;

//     RC522_Init();

//     for(;;)
//     {
//         RFID_Process();
//         osDelay(100U);
//     }
// }
/* USER CODE END Application */

