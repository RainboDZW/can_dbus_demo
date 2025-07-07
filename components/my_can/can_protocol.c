/**
 * @file can_protocol.c
 * @brief CAN通信协议实现
 * @author RainboDZW
 * @date 2025-07-07
 */

#include "can_protocol.h"

/* 电机数据存储 */
static motor_data_t chassis_motor_data[4];  /* 底盘4个电机数据 */
static motor_data_t gimbal_motor_data[2];   /* 云台2个电机数据 */

/**
 * @brief 协议层初始化
 */
void protocol_can_init(void)
{
    /* 注册CAN接收回调函数 */
    bsp_can_register_rx_callback(protocol_can_rcv_data);

    /* 初始化CAN驱动 */
    bsp_can_init();
}

/**
 * @brief 协议层CAN数据接收处理
 * @param rx_message: 接收到的CAN消息
 */
void protocol_can_rcv_data(can_rx_message_t *rx_message)
{
    /* 调用数据解析函数 */
    protocol_rcv_data(rx_message->StdId, rx_message->Data, rx_message->DLC);
}

/**
 * @brief 协议数据解析
 * @param can_id: CAN ID
 * @param data: 数据指针
 * @param len: 数据长度
 */
void protocol_rcv_data(uint32_t can_id, uint8_t *data, uint8_t len)
{
    motor_data_t motor_data;

    /* 解析电机数据 */
    motor_data.angle = (uint16_t)(data[0] << 8 | data[1]);
    motor_data.speed = (int16_t)(data[2] << 8 | data[3]);
    motor_data.current = (int16_t)(data[4] << 8 | data[5]);
    motor_data.temperature = data[6];

    /* 根据CAN ID更新对应电机数据 */
    switch (can_id) {
        case MOTOR_ID_CHASSIS_1:
            chassis_motor_data[0] = motor_data;
            break;
        case MOTOR_ID_CHASSIS_2:
            chassis_motor_data[1] = motor_data;
            break;
        case MOTOR_ID_CHASSIS_3:
            chassis_motor_data[2] = motor_data;
            break;
        case MOTOR_ID_CHASSIS_4:
            chassis_motor_data[3] = motor_data;
            break;
        case MOTOR_ID_GIMBAL_YAW:
            gimbal_motor_data[0] = motor_data;
            break;
        case MOTOR_ID_GIMBAL_PITCH:
            gimbal_motor_data[1] = motor_data;
            break;
        default:
            break;
    }
}

/**
 * @brief 协议接口发送数据
 * @param can_id: CAN ID
 * @param data: 发送数据数组
 * @param len: 数据个数（int16_t个数）
 */
void protocol_interface_send_data(uint32_t can_id, int16_t *data, uint8_t len)
{
    uint8_t tx_data[8];
    uint8_t i;

    /* 将int16_t数据转换为字节数组 */
    for (i = 0; i < len && i < 4; i++) {
        tx_data[i * 2] = (uint8_t)(data[i] >> 8);
        tx_data[i * 2 + 1] = (uint8_t)(data[i] & 0xFF);
    }

    /* 发送CAN数据 */
    can1_std_transmit(can_id, tx_data, len * 2);
}

/**
 * @brief 获取底盘电机数据
 * @param motor_index: 电机索引 (0-3)
 * @retval 电机数据指针
 */
motor_data_t* get_chassis_motor_data(uint8_t motor_index)
{
    if (motor_index < 4) {
        return &chassis_motor_data[motor_index];
    }
    return NULL;
}

/**
 * @brief 获取云台电机数据
 * @param motor_index: 电机索引 (0: YAW, 1: PITCH)
 * @retval 电机数据指针
 */
motor_data_t* get_gimbal_motor_data(uint8_t motor_index)
{
    if (motor_index < 2) {
        return &gimbal_motor_data[motor_index];
    }
    return NULL;
}
