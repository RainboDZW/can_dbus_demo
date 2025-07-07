/**
 * @file app_tasks.c
 * @brief 应用任务实现
 * @author RainboDZW
 * @date 2025-07-07
 */

#include "app_tasks.h"

/* PID控制器实例 */
static pid_controller_t chassis_speed_pid[4];
static pid_controller_t gimbal_angle_pid[2];

/* 任务控制变量 */
static uint32_t chassis_last_time = 0;
static uint32_t gimbal_last_time = 0;

/**
 * @brief PID控制器初始化
 */
void pid_init(pid_controller_t *pid, float kp, float ki, float kd, float max_output)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->error = 0;
    pid->last_error = 0;
    pid->integral = 0;
    pid->output = 0;
    pid->max_output = max_output;
}

/**
 * @brief PID控制计算
 */
float pid_calculate(pid_controller_t *pid, float setpoint, float measured_value)
{
    pid->error = setpoint - measured_value;
    pid->integral += pid->error;

    /* 积分限幅 */
    if (pid->integral > pid->max_output) pid->integral = pid->max_output;
    if (pid->integral < -pid->max_output) pid->integral = -pid->max_output;

    pid->output = pid->kp * pid->error +
                  pid->ki * pid->integral +
                  pid->kd * (pid->error - pid->last_error);

    /* 输出限幅 */
    if (pid->output > pid->max_output) pid->output = pid->max_output;
    if (pid->output < -pid->max_output) pid->output = -pid->max_output;

    pid->last_error = pid->error;

    return pid->output;
}

/**
 * @brief 底盘任务初始化
 */
void chassis_task_init(void)
{
    /* 初始化底盘电机PID控制器 */
    for (int i = 0; i < 4; i++) {
        pid_init(&chassis_speed_pid[i], 3.0f, 0.1f, 0.0f, 1000.0f);
    }

    /* 初始化电机设备 */
    motor_device_init();
}

/**
 * @brief 底盘任务主循环（应用层使用）
 */
void chassis_task(void)
{
    chassis_device_t *chassis = get_chassis_device();
    uint32_t current_time = HAL_GetTick();

    /* 1ms执行一次 */
    if (current_time - chassis_last_time >= 1) {
        chassis_last_time = current_time;

        /* 更新电机数据（从协议层获取传感器数据） */
        motor_device_update();

        /* PID速度控制 */
        for (int i = 0; i < 4; i++) {
            if (chassis->motor[i].online) {
                float current_speed = chassis->motor[i].feedback.speed;
                float target_speed = chassis->motor[i].target_speed;

                /* 计算PID输出作为电流控制值 */
                chassis->motor[i].target_current = (int16_t)pid_calculate(
                    &chassis_speed_pid[i], target_speed, current_speed);
            }
        }

        /* 发送控制电流到电机 */
        chassis_send_current();
    }
}

/**
 * @brief 云台任务初始化
 */
void gimbal_task_init(void)
{
    /* 初始化云台PID控制器 */
    pid_init(&gimbal_angle_pid[0], 10.0f, 0.1f, 0.5f, 15000.0f);  // YAW
    pid_init(&gimbal_angle_pid[1], 8.0f, 0.1f, 0.3f, 12000.0f);   // PITCH
}

/**
 * @brief 云台任务主循环（应用层使用）
 */
void gimbal_task(void)
{
    gimbal_device_t *gimbal = get_gimbal_device();
    uint32_t current_time = HAL_GetTick();

    /* 1ms执行一次 */
    if (current_time - gimbal_last_time >= 1) {
        gimbal_last_time = current_time;

        /* 更新电机数据（从协议层获取传感器数据） */
        motor_device_update();

        /* YAW轴角度控制 */
        if (gimbal->yaw_motor.online) {
            float current_angle = gimbal->yaw_motor.feedback.angle;
            float target_angle = gimbal->yaw_motor.target_angle;

            gimbal->yaw_motor.target_current = (int16_t)pid_calculate(
                &gimbal_angle_pid[0], target_angle, current_angle);
        }

        /* PITCH轴角度控制 */
        if (gimbal->pitch_motor.online) {
            float current_angle = gimbal->pitch_motor.feedback.angle;
            float target_angle = gimbal->pitch_motor.target_angle;

            gimbal->pitch_motor.target_current = (int16_t)pid_calculate(
                &gimbal_angle_pid[1], target_angle, current_angle);
        }

        /* 发送控制电流到电机 */
        gimbal_send_current();
    }
}

/* 示例：在主函数或其他地方的使用方法 */
void example_usage(void)
{
    /* 初始化任务 */
    chassis_task_init();
    gimbal_task_init();

    /* 在主循环中调用任务函数 */
    while (1) {
        chassis_task();
        gimbal_task();

        /* 示例：设置底盘速度 */
        chassis_set_speed(1000, 0, 500);  // 前进 + 右转

        /* 示例：设置云台角度 */
        gimbal_set_angle(45.0f, 10.0f);   // YAW 45度, PITCH 10度

        HAL_Delay(1);
    }
}
