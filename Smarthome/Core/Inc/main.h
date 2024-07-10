/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#define rc522_sda_Pin GPIO_PIN_4
#define rc522_sda_GPIO_Port GPIOA
#define rc522_sck_Pin GPIO_PIN_5
#define rc522_sck_GPIO_Port GPIOA
#define rc522_mosi_Pin GPIO_PIN_6
#define rc522_mosi_GPIO_Port GPIOA
#define rc522_miso_Pin GPIO_PIN_7
#define rc522_miso_GPIO_Port GPIOA
#define key_r1_Pin GPIO_PIN_0
#define key_r1_GPIO_Port GPIOB
#define key_r2_Pin GPIO_PIN_1
#define key_r2_GPIO_Port GPIOB
#define key_r3_Pin GPIO_PIN_2
#define key_r3_GPIO_Port GPIOB
#define key_c1_Pin GPIO_PIN_12
#define key_c1_GPIO_Port GPIOB
#define key_c2_Pin GPIO_PIN_13
#define key_c2_GPIO_Port GPIOB
#define key_c3_Pin GPIO_PIN_14
#define key_c3_GPIO_Port GPIOB
#define key_c4_Pin GPIO_PIN_15
#define key_c4_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_8
#define LED_GPIO_Port GPIOA
#define rc522_rst_Pin GPIO_PIN_15
#define rc522_rst_GPIO_Port GPIOA
#define key_r4_Pin GPIO_PIN_7
#define key_r4_GPIO_Port GPIOB
#define oled_scl_Pin GPIO_PIN_8
#define oled_scl_GPIO_Port GPIOB
#define oled_sda_Pin GPIO_PIN_9
#define oled_sda_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
extern uint8_t val;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
