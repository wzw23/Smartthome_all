/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart6;

/* USER CODE BEGIN Private defines */
#define USART6_MAX_RECV_LEN		400					//最大接收缓存字节数
#define USART6_MAX_SEND_LEN		400					//最大发送缓存字节数
#define USART6_RX_EN 			1					//0,不接收;1,接收.
	
extern uint8_t  USART6_RX_BUF[USART6_MAX_RECV_LEN]; 		//接收缓冲,最大USART6_MAX_RECV_LEN字节
extern uint8_t  USART6_TX_BUF[USART6_MAX_SEND_LEN]; 		//发送缓冲,最大USART6_MAX_SEND_LEN字节
extern uint16_t USART6_RX_STA;   							//接收数据状态
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART6_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

