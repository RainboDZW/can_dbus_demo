/**
 * @file robot_control.h
 * @brief 机器人控制头文件 - 实现拨杆控制机器人运动
 * @author Your Name
 * @date 2025-01-05
 */

#ifndef __ROBOT_CONTROL_H__
#define __ROBOT_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "motor_device.h"

/* 运动控制参数定义 */
#define ROBOT_FORWARD_SPEED     500     /* 向前运动速度 */
#define ROBOT_BACKWARD_SPEED    -500    /* 向后运动速度 */
#define ROBOT_LEFT_SPEED        500     /* 向左运动速度 */
#define ROBOT_RIGHT_SPEED       -500    /* 向右运动速度 */
#define ROBOT_ROTATE_SPEED      300     /* 旋转速度 */

/* 机器人运动状态枚举 */
typedef enum {
    ROBOT_STOP = 0,         /* 停止 */
    ROBOT_FORWARD,          /* 向前 */
    ROBOT_BACKWARD,         /* 向后 */
    ROBOT_LEFT,             /* 向左 */
    ROBOT_RIGHT,            /* 向右 */
    ROBOT_ROTATE_LEFT,      /* 左转 */
    ROBOT_ROTATE_RIGHT      /* 右转 */
} robot_movement_t;

/* 机器人控制结构体 */
typedef struct {
    robot_movement_t movement;      /* 当前运动状态 */
    chassis_device_t *chassis;     /* 底盘设备 */
    uint8_t control_enabled;       /* 控制使能标志 */
} robot_control_t;

/* 函数声明 */
void robot_control_init(void);
void robot_control_task(void);
void robot_movement_control(robot_movement_t movement);
void robot_stop(void);

/* 拨杆控制映射函数 */
robot_movement_t get_movement_from_switches(uint8_t sw1, uint8_t sw2);

#ifdef __cplusplus
}
#endif

#endif // __ROBOT_CONTROL_H__
