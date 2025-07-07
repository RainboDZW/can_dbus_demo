/**
 * @file motor_device.c
 * @brief 电机设备层实现
 * @author RainboDZW
 * @date 2025-07-07
 */

#include "motor_device.h"

/* 设备实例 */
static chassis_device_t chassis_device;
static gimbal_device_t gimbal_device;

/**
 * @brief 电机设备初始化
 */
void motor_device_init(void)
{
    /* 初始化底盘设备 */
    for (int i = 0; i < 4; i++) {
        chassis_device.motor[i].target_current = 0;
        chassis_device.motor[i].target_speed = 0;
        chassis_device.motor[i].target_angle = 0;
        chassis_device.motor[i].online = 0;
    }

    /* 初始化云台设备 */
    gimbal_device.yaw_motor.target_current = 0;
    gimbal_device.yaw_motor.target_speed = 0;
    gimbal_device.yaw_motor.online = 0;
    gimbal_device.pitch_motor.target_current = 0;
    gimbal_device.pitch_motor.target_speed = 0;
    gimbal_device.pitch_motor.online = 0;

    /* 初始化协议层 */
    protocol_can_init();
}

/**
 * @brief 更新电机设备数据
 */
void motor_device_update(void)
{
    motor_data_t *motor_data;

    /* 更新底盘电机数据 */
    for (int i = 0; i < 4; i++) {
        motor_data = get_chassis_motor_data(i);
        if (motor_data != NULL) {
            chassis_device.motor[i].feedback = *motor_data;
            chassis_device.motor[i].online = 1;
        }
    }

    /* 更新云台电机数据 */
    motor_data = get_gimbal_motor_data(0);  // YAW
    if (motor_data != NULL) {
        gimbal_device.yaw_motor.feedback = *motor_data;
        gimbal_device.yaw_motor.online = 1;
    }

    motor_data = get_gimbal_motor_data(1);  // PITCH
    if (motor_data != NULL) {
        gimbal_device.pitch_motor.feedback = *motor_data;
        gimbal_device.pitch_motor.online = 1;
    }
}

/**
 * @brief 获取底盘设备指针
 * @retval 底盘设备指针
 */
chassis_device_t* get_chassis_device(void)
{
    return &chassis_device;
}

/**
 * @brief 设置底盘速度
 * @param vx: X轴速度
 * @param vy: Y轴速度
 * @param vw: 角速度
 */
void chassis_set_speed(int16_t vx, int16_t vy, int16_t vw)
{
    chassis_device.vx = vx;
    chassis_device.vy = vy;
    chassis_device.vw = vw;

    /* 麦克纳姆轮速度分解 */
    chassis_device.motor[0].target_speed = vx - vy - vw;  // 左前
    chassis_device.motor[1].target_speed = vx + vy - vw;  // 右前
    chassis_device.motor[2].target_speed = -vx + vy - vw; // 右后
    chassis_device.motor[3].target_speed = -vx - vy - vw; // 左后

    /* 速度转换为电流（简单的比例控制） */
    for (int i = 0; i < 4; i++) {
        chassis_device.motor[i].target_current = chassis_device.motor[i].target_speed * 5; // 比例系数可调整
    }
}

/**
 * @brief 发送底盘电机控制电流
 */
void chassis_send_current(void)
{
    int16_t current_data[4];

    for (int i = 0; i < 4; i++) {
        current_data[i] = chassis_device.motor[i].target_current;
    }

    protocol_interface_send_data(MOTOR_CMD_CHASSIS, current_data, 4);
}

/**
 * @brief 获取云台设备指针
 * @retval 云台设备指针
 */
gimbal_device_t* get_gimbal_device(void)
{
    return &gimbal_device;
}

/**
 * @brief 设置云台角度
 * @param yaw: YAW角度
 * @param pitch: PITCH角度
 */
void gimbal_set_angle(float yaw, float pitch)
{
    gimbal_device.yaw_angle = yaw;
    gimbal_device.pitch_angle = pitch;

    /* 角度转换为电机目标值（这里需要根据实际情况调整） */
    gimbal_device.yaw_motor.target_angle = (uint16_t)(yaw * 8192.0f / 360.0f);
    gimbal_device.pitch_motor.target_angle = (uint16_t)(pitch * 8192.0f / 360.0f);
}

/**
 * @brief 发送云台电机控制电流
 */
void gimbal_send_current(void)
{
    int16_t current_data[4] = {0};

    current_data[0] = gimbal_device.yaw_motor.target_current;
    current_data[1] = gimbal_device.pitch_motor.target_current;

    protocol_interface_send_data(MOTOR_CMD_GIMBAL, current_data, 4);
}
