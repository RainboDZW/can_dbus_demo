/**
 * @file robot_control.c
 * @brief 机器人控制逻辑实现
 * @author RainboDZW
 * @date 2025-07-07
 */

#include "robot_control.h"

/* 全局变量 */
static robot_control_t robot_ctrl;

/**
 * @brief 机器人控制初始化
 */
void robot_control_init(void)
{
    /* 初始化控制结构体 */
    robot_ctrl.movement = ROBOT_STOP;
    robot_ctrl.chassis = get_chassis_device();    /* 获取底盘设备 */
    robot_ctrl.control_enabled = 0;               /* 默认关闭控制 */

    /* 初始化相关设备 */
    motor_device_init();
}

/**
 * @brief 机器人控制主任务 - 在主循环中调用
 */
void robot_control_task(void)
{
    /* 更新电机数据 */
    motor_device_update();

    /* 发送底盘电机控制命令 */
    if (robot_ctrl.control_enabled) {
        chassis_send_current();
    }
}

/**
 * @brief 机器人运动控制
 * @param movement: 运动模式
 */
void robot_movement_control(robot_movement_t movement)
{
    int16_t vx = 0, vy = 0, vw = 0;

    robot_ctrl.movement = movement;

    switch (movement) {
        case ROBOT_FORWARD:
            /* 向前运动 */
            vx = ROBOT_FORWARD_SPEED;
            vy = 0;
            vw = 0;
            break;

        case ROBOT_BACKWARD:
            /* 向后运动 */
            vx = ROBOT_BACKWARD_SPEED;
            vy = 0;
            vw = 0;
            break;

        case ROBOT_LEFT:
            /* 向左运动 */
            vx = 0;
            vy = ROBOT_LEFT_SPEED;
            vw = 0;
            break;

        case ROBOT_RIGHT:
            /* 向右运动 */
            vx = 0;
            vy = ROBOT_RIGHT_SPEED;
            vw = 0;
            break;

        case ROBOT_ROTATE_LEFT:
            /* 左转 */
            vx = 0;
            vy = 0;
            vw = ROBOT_ROTATE_SPEED;
            break;

        case ROBOT_ROTATE_RIGHT:
            /* 右转 */
            vx = 0;
            vy = 0;
            vw = -ROBOT_ROTATE_SPEED;
            break;

        case ROBOT_STOP:
        default:
            /* 停止运动 */
            vx = 0;
            vy = 0;
            vw = 0;
            break;
    }

    /* 设置底盘速度 */
    chassis_set_speed(vx, vy, vw);
}

/**
 * @brief 机器人紧急停止
 */
void robot_stop(void)
{
    robot_ctrl.control_enabled = 0;
    robot_movement_control(ROBOT_STOP);
}
