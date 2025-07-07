/**
 * @file bsp_can.c
 * @brief CAN底层驱动实现
 * @author RainboDZW
 * @date 2025-07-07
 */

#include "bsp_can.h"

/* 外部CAN句柄声明 */
extern CAN_HandleTypeDef hcan1;

/* 回调函数指针 */
static can_rx_callback_t g_can_rx_callback = NULL;

/**
 * @brief CAN初始化
 * @retval CAN_INIT_OK: 初始化成功
 * @retval CAN_INIT_ERROR: 初始化失败
 */
can_init_status_t bsp_can_init(void)
{
    /* 配置CAN过滤器 */
    bsp_can_filter_config();

    /* 启动CAN */
    if (HAL_CAN_Start(&hcan1) != HAL_OK) {
        return CAN_INIT_ERROR;
    }

    /* 启用CAN接收中断 */
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
        return CAN_INIT_ERROR;
    }

    return CAN_INIT_OK;
}

/**
 * @brief 配置CAN过滤器
 */
void bsp_can_filter_config(void)
{
    CAN_FilterTypeDef can_filter_st;

    can_filter_st.FilterActivation = ENABLE;
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter_st.FilterIdHigh = 0x0000;
    can_filter_st.FilterIdLow = 0x0000;
    can_filter_st.FilterMaskIdHigh = 0x0000;
    can_filter_st.FilterMaskIdLow = 0x0000;
    can_filter_st.FilterBank = 0;
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;

    if (HAL_CAN_ConfigFilter(&hcan1, &can_filter_st) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief CAN1标准帧发送
 * @param std_id: 标准帧ID
 * @param data: 发送数据指针
 * @param len: 数据长度
 * @retval CAN发送状态
 */
can_tx_status_t can1_std_transmit(uint32_t std_id, uint8_t *data, uint8_t len)
{
    CAN_TxHeaderTypeDef tx_header;
    uint32_t tx_mailbox;

    /* 配置发送消息头 */
    tx_header.StdId = std_id;
    tx_header.ExtId = 0;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.IDE = CAN_ID_STD;
    tx_header.DLC = len;
    tx_header.TransmitGlobalTime = DISABLE;

    /* 发送CAN消息 */
    if (HAL_CAN_AddTxMessage(&hcan1, &tx_header, data, &tx_mailbox) != HAL_OK) {
        return CAN_TX_ERROR;
    }

    return CAN_TX_OK;
}

/**
 * @brief 注册CAN接收回调函数
 * @param callback: 回调函数指针
 */
void bsp_can_register_rx_callback(can_rx_callback_t callback)
{
    g_can_rx_callback = callback;
}

/**
 * @brief CAN1接收消息回调（驱动层回调）
 * @param rx_message: 接收到的消息
 */
void can1_msg_rec(can_rx_message_t *rx_message)
{
    /* 调用协议层处理函数 */
    if (g_can_rx_callback != NULL) {
        g_can_rx_callback(rx_message);
    }
}

/**
 * @brief CAN接收中断回调函数（硬件中断处理）
 * @param hcan: CAN句柄指针
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx_header;
    can_rx_message_t rx_message;

    /* 获取接收消息 */
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_message.Data) == HAL_OK) {
        /* 填充接收消息结构体 */
        rx_message.StdId = rx_header.StdId;
        rx_message.ExtId = rx_header.ExtId;
        rx_message.IDE = rx_header.IDE;
        rx_message.RTR = rx_header.RTR;
        rx_message.DLC = rx_header.DLC;
        rx_message.FMI = rx_header.FilterMatchIndex;

        /* 根据CAN实例调用对应的处理函数 */
        if (hcan->Instance == CAN1) {
            can1_msg_rec(&rx_message);
        }
    }
}
