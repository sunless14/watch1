/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include <stdio.h>
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
#define key0_Pin GPIO_PIN_6
#define key0_GPIO_Port GPIOF
#define key1_Pin GPIO_PIN_7
#define key1_GPIO_Port GPIOF
#define key2_Pin GPIO_PIN_8
#define key2_GPIO_Port GPIOF
#define key3_Pin GPIO_PIN_9
#define key3_GPIO_Port GPIOF
#define LED_Pin GPIO_PIN_1
#define LED_GPIO_Port GPIOA
#define LED_RS_Pin GPIO_PIN_2
#define LED_RS_GPIO_Port GPIOA
#define LED_CS_Pin GPIO_PIN_3
#define LED_CS_GPIO_Port GPIOA
#define LED_RST_Pin GPIO_PIN_4
#define LED_RST_GPIO_Port GPIOA
#define led0_Pin GPIO_PIN_13
#define led0_GPIO_Port GPIOG
#define led1_Pin GPIO_PIN_14
#define led1_GPIO_Port GPIOG
#define led2_Pin GPIO_PIN_15
#define led2_GPIO_Port GPIOG

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
