/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define PC0_A5_TGS_Pin GPIO_PIN_0
#define PC0_A5_TGS_GPIO_Port GPIOC
#define PC1_A4_TurnR_Pin GPIO_PIN_1
#define PC1_A4_TurnR_GPIO_Port GPIOC
#define PC1_A4_TurnR_EXTI_IRQn EXTI1_IRQn
#define PA2_PD1_TX_Pin GPIO_PIN_2
#define PA2_PD1_TX_GPIO_Port GPIOA
#define PA3_D0_RX_Pin GPIO_PIN_3
#define PA3_D0_RX_GPIO_Port GPIOA
#define PA4_A2_KLAXON_Pin GPIO_PIN_4
#define PA4_A2_KLAXON_GPIO_Port GPIOA
#define PA4_A2_KLAXON_EXTI_IRQn EXTI4_IRQn
#define PB0_A3_TurnL_Pin GPIO_PIN_0
#define PB0_A3_TurnL_GPIO_Port GPIOB
#define PB0_A3_TurnL_EXTI_IRQn EXTI0_IRQn
#define PB10_D6_BEN_Pin GPIO_PIN_10
#define PB10_D6_BEN_GPIO_Port GPIOB
#define PC7_D9_RFRST_Pin GPIO_PIN_7
#define PC7_D9_RFRST_GPIO_Port GPIOC
#define PA8_D7_RFCC_Pin GPIO_PIN_8
#define PA8_D7_RFCC_GPIO_Port GPIOA
#define PA9_D8_BTX_Pin GPIO_PIN_9
#define PA9_D8_BTX_GPIO_Port GPIOA
#define PA10_D2_BRX_Pin GPIO_PIN_10
#define PA10_D2_BRX_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define PB6_D10_RFIRQ_Pin GPIO_PIN_6
#define PB6_D10_RFIRQ_GPIO_Port GPIOB
#define PB6_D10_RFIRQ_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
