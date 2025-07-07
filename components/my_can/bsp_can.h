/**
 * @file bsp_can.h
 * @brief CAN驱动层头文件
 * @author Your Name
 * @date 2025-01-04
 */

#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "can.h"

/* CAN消息结构体定义 */
typedef struct {
    uint32_t StdId;     /* 标准帧ID (11位) */
    uint32_t ExtId;     /* 扩展帧ID (29位) */
    uint8_t IDE;        /* ID类型: CAN_ID_STD或CAN_ID_EXT */
    uint8_t RTR;        /* 帧类型: CAN_RTR_DATA或CAN_RTR_REMOTE */
    uint8_t DLC;        /* 数据长度 */
    uint8_t Data[8];    /* 数据 */
} can_tx_message_t;

typedef struct {
    uint32_t StdId;
    uint32_t ExtId;
    uint8_t IDE;
    uint8_t RTR;
    uint8_t DLC;
    uint8_t Data[8];
    uint8_t FMI;        /* 过滤器匹配索引 */
} can_rx_message_t;

/* CAN初始化状态 */
typedef enum {
    CAN_INIT_OK = 0,
    CAN_INIT_ERROR = 1
} can_init_status_t;

/* CAN发送状态 */
typedef enum {
    CAN_TX_OK = 0,
    CAN_TX_ERROR = 1,
    CAN_TX_TIMEOUT = 2
} can_tx_status_t;

/* 回调函数类型定义 */
typedef void (*can_rx_callback_t)(can_rx_message_t *rx_message);

/* 驱动层函数声明 */
can_init_status_t bsp_can_init(void);
void bsp_can_filter_config(void);
can_tx_status_t can1_std_transmit(uint32_t std_id, uint8_t *data, uint8_t len);
can_tx_status_t can2_std_transmit(uint32_t std_id, uint8_t *data, uint8_t len);

/* 中断回调函数 */
void can1_msg_rec(can_rx_message_t *rx_message);
void can2_msg_rec(can_rx_message_t *rx_message);

/* 回调注册函数 */
void bsp_can_register_rx_callback(can_rx_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_CAN_H__ */
