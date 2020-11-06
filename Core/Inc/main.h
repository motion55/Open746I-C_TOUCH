/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal.h"

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
#define LED4_Out_Pin GPIO_PIN_8
#define LED4_Out_GPIO_Port GPIOI
#define LED3_Out_Pin GPIO_PIN_4
#define LED3_Out_GPIO_Port GPIOH
#define LCD_BL_Pin GPIO_PIN_3
#define LCD_BL_GPIO_Port GPIOA
#define TP_RST_Pin GPIO_PIN_11
#define TP_RST_GPIO_Port GPIOD
#define TP_SCL_Pin GPIO_PIN_12
#define TP_SCL_GPIO_Port GPIOD
#define TP_SDA_Pin GPIO_PIN_13
#define TP_SDA_GPIO_Port GPIOD
#define TP_INT_Pin GPIO_PIN_7
#define TP_INT_GPIO_Port GPIOD
#define TP_INT_EXTI_IRQn EXTI9_5_IRQn
#define LED2_Out_Pin GPIO_PIN_7
#define LED2_Out_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
