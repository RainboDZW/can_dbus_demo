/**
 * @file my_can.h
 * @brief CAN系统统一头文件 - 包含所有CAN相关功能
 * @author Your Name
 * @date 2025-01-04
 */

#ifndef __MY_CAN_H__
#define __MY_CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 包含所有CAN系统相关头文件 */
#include "bsp_can.h"
#include "can_protocol.h"
#include "motor_device.h"
#include "app_tasks.h"

/* 系统初始化函数 */
void my_can_system_init(void);

/* 系统主循环函数 */
void my_can_system_loop(void);

/* 快速配置宏定义 */
#define CAN_SYSTEM_INIT()           my_can_system_init()
#define CAN_SYSTEM_RUN()            my_can_system_loop()

/* 快速控制接口宏定义 */
#define CHASSIS_MOVE(vx, vy, vw)    chassis_set_speed(vx, vy, vw)
#define GIMBAL_MOVE(yaw, pitch)     gimbal_set_angle(yaw, pitch)

/* 获取数据接口宏定义 */
#define GET_CHASSIS_DATA()          get_chassis_device()
#define GET_GIMBAL_DATA()           get_gimbal_device()

/* 导出关键类型定义，避免未使用警告 */
typedef can_rx_message_t my_can_rx_message_t;
typedef can_tx_message_t my_can_tx_message_t;
typedef motor_data_t my_motor_data_t;
typedef chassis_device_t my_chassis_device_t;
typedef gimbal_device_t my_gimbal_device_t;
typedef pid_controller_t my_pid_controller_t;

/* 导出关键函数声明，避免未使用警告 */
extern can_init_status_t (*my_can_init_func)(void);
extern can_tx_status_t (*my_can_transmit_func)(uint32_t, uint8_t*, uint8_t);
extern void (*my_protocol_init_func)(void);
extern void (*my_motor_init_func)(void);
extern void (*my_chassis_task_func)(void);
extern void (*my_gimbal_task_func)(void);

#ifdef __cplusplus
}
#endif

#endif /* __MY_CAN_H__ */
