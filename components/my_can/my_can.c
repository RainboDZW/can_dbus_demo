/**
 * @file my_can.c
 * @brief CAN系统统一接口实现
 * @author RainboDZW
 * @date 2025-07-07
 */

#include "my_can.h"

/* 函数指针实现，避免未使用警告 */
can_init_status_t (*my_can_init_func)(void) = bsp_can_init;
can_tx_status_t (*my_can_transmit_func)(uint32_t, uint8_t*, uint8_t) = can1_std_transmit;
void (*my_protocol_init_func)(void) = protocol_can_init;
void (*my_motor_init_func)(void) = motor_device_init;
void (*my_chassis_task_func)(void) = chassis_task;
void (*my_gimbal_task_func)(void) = gimbal_task;

/**
 * @brief CAN系统初始化（一键初始化所有功能）
 */
void my_can_system_init(void)
{
    /* 初始化底盘任务 */
    chassis_task_init();

    /* 初始化云台任务 */
    gimbal_task_init();
}

/**
 * @brief CAN系统主循环（一键运行所有任务）
 */
void my_can_system_loop(void)
{
    /* 运行底盘任务 */
    chassis_task();

    /* 运行云台任务 */
    gimbal_task();
}
