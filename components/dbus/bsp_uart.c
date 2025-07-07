/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : bsp_uart.c
  * @brief          : BSP UART interfaces functions
  * @author         : DM Project
  * @date           : 2025/01/07
  * @version        : v2.0
  ******************************************************************************
  * @attention      : 双缓冲区DMA接收系统实现
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "bsp_uart.h"
#include "dbus.h"
#include <string.h>

/* Private defines -----------------------------------------------------------*/
#define DBUS_UART_HANDLE    huart3      /* 使用UART3作为DBUS接口 */

/**
 * @brief MCU初始化函数
 */
void MCU_Init(void)
{
    HAL_StatusTypeDef status;

    /* 添加额外的延时，确保UART完全初始化 */
    HAL_Delay(50);

    /* 检查UART状态 */
    if (huart3.Instance == NULL) {
        // UART未初始化
        while(1);
    }

    /* 先尝试最简单的DMA接收测试 */
    status = HAL_UART_Receive_DMA(&huart3, SBUS_MultiRx_Buf[0], SBUS_RX_BUF_NUM);

    if (status == HAL_OK) {
        /* 如果标准DMA成功，再尝试空闲中断模式 */
        HAL_UART_DMAStop(&huart3);  // 停止标准DMA
        HAL_Delay(10);

        /* 启动空闲中断DMA接收 */
        status = HAL_UARTEx_ReceiveToIdle_DMA(&huart3, SBUS_MultiRx_Buf[0], SBUS_RX_BUF_NUM);
    }

    /* 检查DMA启动是否成功 */
    if (status != HAL_OK) {
        // DMA启动失败，可以在这里设置断点检查status值
        volatile HAL_StatusTypeDef debug_status = status;
        while(1) {
            // 死循环，便于��试发现问题
            (void)debug_status;
        }
    }

    /* 禁用半传输中断，只使用完整传输和空闲中断 */
    __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);
}

/**
 * @brief BSP UART初始化函数
 * @retval None
 */
void BSP_USART_Init(void)
{
    HAL_StatusTypeDef status;

    /* 直接启动HAL库的DMA接收，使用空闲中断模式 */
    status = HAL_UARTEx_ReceiveToIdle_DMA(&huart3, SBUS_MultiRx_Buf[0], SBUS_RX_BUF_NUM);

    /* 检查DMA启动是否成功 */
    if (status != HAL_OK) {
        // DMA启��失败��可以在这里设置断点检查
        while(1) {
            // 死循环，便于调试发现问题
        }
    }

    /* 禁用半传输中断，只使用完整传输和空闲中断 */
    __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);
}

/**
 * @brief UART3接收数据处理函数
 * @param huart UART句柄
 * @param Size 接收到的数据长度
 */
void USER_USART3_RxHandler(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance != USART3) return;

    /* 检查数据长度是否为遥控器标准���长 */
    if (Size == RC_FRAME_LENGTH)
    {
        /* 解析DBUS数据 */
        SBUS_TO_RC((volatile const uint8_t *)SBUS_MultiRx_Buf[0], &remote_ctrl);

    }

    /* 重新启动DMA接收，准备接收下一帧 */
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, SBUS_MultiRx_Buf[0], SBUS_RX_BUF_NUM);
}

/**
 * @brief HAL库的UART空闲中断回调函数
 * @param huart UART句柄
 * @param Size 接收到的数据长度
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    static uint32_t callback_count = 0;

    if(huart->Instance == USART3)
    {
        callback_count++;  // 计数器，可以在调试器中观察

        // 添加一个volatile变量确保调试器能正确显示
        volatile uint32_t debug_count = callback_count;
        volatile uint16_t debug_size = Size;

        // 在这里设置断点，观察是否能进入这个回调函数
        // 同时观察Size参数的值
        (void)debug_count;
        (void)debug_size;

        USER_USART3_RxHandler(huart, Size);  // 处理DBUS数据
    }
}

/**
 * @brief DMA传输完成回调函数 - 添加这个函数来测试是否有任何DMA活动
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    static uint32_t dma_complete_count = 0;

    if(huart->Instance == USART3)
    {
        dma_complete_count++;  // 在调试器中观察这个计数��

        // 重新启动DMA接收
        HAL_UARTEx_ReceiveToIdle_DMA(huart, SBUS_MultiRx_Buf[0], SBUS_RX_BUF_NUM);
    }
}

/**
 * @brief UART错误回调函数 - 检查是否��UART错误
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    static uint32_t error_count = 0;
    static uint32_t last_error_code = 0;

    error_count++;

    // 检查huart是否为NULL
    if (huart == NULL) {
        // huart为NULL是异常情况，直接返回
        return;
    }

    if(huart->Instance == USART3)
    {
        last_error_code = huart->ErrorCode;

        // 详细分析错误类型
        volatile uint8_t parity_error = (last_error_code & HAL_UART_ERROR_PE) ? 1 : 0;   // 0x01
        volatile uint8_t noise_error = (last_error_code & HAL_UART_ERROR_NE) ? 1 : 0;    // 0x02
        volatile uint8_t frame_error = (last_error_code & HAL_UART_ERROR_FE) ? 1 : 0;    // 0x04
        volatile uint8_t overrun_error = (last_error_code & HAL_UART_ERROR_ORE) ? 1 : 0; // 0x08
        volatile uint8_t dma_error = (last_error_code & HAL_UART_ERROR_DMA) ? 1 : 0;     // 0x10

        // 在调试器中观察这些变量
        (void)parity_error;
        (void)noise_error;
        (void)frame_error;
        (void)overrun_error;
        (void)dma_error;

        // 清除错误状态 - 修正标志位名称和语法
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_PE | UART_FLAG_FE | UART_FLAG_NE | UART_FLAG_ORE);

        // 重置UART状态
        huart->RxState = HAL_UART_STATE_READY;
        huart->ErrorCode = HAL_UART_ERROR_NONE;

        // 尝试重新启动接收
        HAL_UART_AbortReceive(huart);
        HAL_Delay(1);  // 短暂延时
        HAL_UARTEx_ReceiveToIdle_DMA(huart, SBUS_MultiRx_Buf[0], SBUS_RX_BUF_NUM);
    }
}
