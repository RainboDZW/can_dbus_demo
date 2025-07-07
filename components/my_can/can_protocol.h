/**
 * @file can_protocol.h
 * @brief CAN协议层头文件
 * @author Your Name
 * @date 2025-01-04
 */

#ifndef __CAN_PROTOCOL_H__
#define __CAN_PROTOCOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_can.h"

/* 电机ID定义 */
#define MOTOR_ID_CHASSIS_1      0x201
#define MOTOR_ID_CHASSIS_2      0x202
#define MOTOR_ID_CHASSIS_3      0x203
#define MOTOR_ID_CHASSIS_4      0x204
#define MOTOR_ID_GIMBAL_YAW     0x205
#define MOTOR_ID_GIMBAL_PITCH   0x206

/* 电机控制指令ID */
#define MOTOR_CMD_CHASSIS       0x200  /* 底盘电机控制指令 */
#define MOTOR_CMD_GIMBAL        0x1FF  /* 云台电机控制指令 */

/* 电机数据结构体 */
typedef struct {
    uint16_t angle;         /* 转子机械角度 */
    int16_t speed;          /* 转子转速 */
    int16_t current;        /* 实际转矩电流 */
    uint8_t temperature;    /* 电机温度 */
} motor_data_t;

/* 协议层函数声明 */
void protocol_can_init(void);
void protocol_can_rcv_data(can_rx_message_t *rx_message);
void protocol_rcv_data(uint32_t can_id, uint8_t *data, uint8_t len);
void protocol_interface_send_data(uint32_t can_id, int16_t *data, uint8_t len);

/* 电机数据获取接口 */
motor_data_t* get_chassis_motor_data(uint8_t motor_index);
motor_data_t* get_gimbal_motor_data(uint8_t motor_index);

#ifdef __cplusplus
}
#endif

#endif /* __CAN_PROTOCOL_H__ */
