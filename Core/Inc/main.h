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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"

extern SemaphoreHandle_t xSemPage;
extern EventGroupHandle_t xEventUpdate;
extern TimerHandle_t xTimerOffline;
extern TaskHandle_t xTaskAlarmHandle;

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern IWDG_HandleTypeDef hiwdg;
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
#define MQ2DO_Pin GPIO_PIN_0
#define MQ2DO_GPIO_Port GPIOA
#define MQ2AO_Pin GPIO_PIN_1
#define MQ2AO_GPIO_Port GPIOA
#define FIREDO_Pin GPIO_PIN_2
#define FIREDO_GPIO_Port GPIOA
#define FIREAO_Pin GPIO_PIN_3
#define FIREAO_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_1
#define KEY1_GPIO_Port GPIOB
#define KEY2_Pin GPIO_PIN_11
#define KEY2_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_12
#define BUZZER_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_13
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_14
#define LED2_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_15
#define LED3_GPIO_Port GPIOB
#define DHT22_Pin GPIO_PIN_5
#define DHT22_GPIO_Port GPIOB
#define OLEDSCL_Pin GPIO_PIN_8
#define OLEDSCL_GPIO_Port GPIOB
#define OLEDSDA_Pin GPIO_PIN_9
#define OLEDSDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
