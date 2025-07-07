/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : bsp_uart.h
  * @brief          : BSP UART interfaces functions
  * @author         : DM Project
  * @date           : 2025/01/07
  * @version        : v2.0
  ******************************************************************************
  * @attention      : 双缓冲区DMA接收系统
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BSP_UART_H
#define BSP_UART_H

/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "dma.h"
#include <stdint.h>
#include <stdbool.h>

/* Exported defines -----------------------------------------------------------*/
/**
 * @brief MCU初始化函数
 */
extern void MCU_Init(void);

/**
 * @brief BSP UART初始化函数
 */
extern void BSP_USART_Init(void);

/**
 * @brief 启动UART DMA多缓冲区接收
 * @param huart UART句柄
 * @param pSrcAddress 源地址（UART数据寄存器）
 * @param pDstAddress0 目标缓冲区0地址
 * @param pDstAddress1 目标缓冲区1地址
 * @param DataLength 数据长度
 */
extern void USART_RxDMA_MultiBufferStart(UART_HandleTypeDef *huart,
                                        uint32_t *pSrcAddress,
                                        uint32_t *pDstAddress0,
                                        uint32_t *pDstAddress1,
                                        uint32_t DataLength);

/**
 * @brief UART3接收数据处理函数
 * @param huart UART句柄
 * @param Size 接收到的数据长度
 */
extern void USER_USART3_RxHandler(UART_HandleTypeDef *huart, uint16_t Size);

#endif /* BSP_UART_H */
