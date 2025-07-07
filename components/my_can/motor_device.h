/**
 * @file motor_device.h
 * @brief 电机设备层头文件
 * @author Your Name
 * @date 2025-01-04
 */

#ifndef __MOTOR_DEVICE_H__
#define __MOTOR_DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "can_protocol.h"

/* 电机类型定义 */
typedef enum {
    MOTOR_TYPE_CHASSIS = 0,
    MOTOR_TYPE_GIMBAL = 1
} motor_type_t;

/* 电机控制结构体 */
typedef struct {
    int16_t target_current;     /* 目标电流 */
    int16_t target_speed;       /* 目标速度 */
    uint16_t target_angle;      /* 目标角度 */
    motor_data_t feedback;      /* 反馈数据 */
    uint8_t online;             /* 在线状态 */
} motor_device_t;

/* 底盘控制结构体 */
typedef struct {
    motor_device_t motor[4];    /* 4个底盘电机 */
    int16_t vx;                 /* X轴速度 */
    int16_t vy;                 /* Y轴速度 */
    int16_t vw;                 /* 角速度 */
} chassis_device_t;

/* 云台控制结构体 */
typedef struct {
    motor_device_t yaw_motor;   /* YAW轴电机 */
    motor_device_t pitch_motor; /* PITCH轴电机 */
    float yaw_angle;            /* YAW角度 */
    float pitch_angle;          /* PITCH角度 */
} gimbal_device_t;

/* 设备层函数声明 */
void motor_device_init(void);
void motor_device_update(void);

/* 底盘设备接口 */
chassis_device_t* get_chassis_device(void);
void chassis_set_speed(int16_t vx, int16_t vy, int16_t vw);
void chassis_send_current(void);

/* 云台设备接口 */
gimbal_device_t* get_gimbal_device(void);
void gimbal_set_angle(float yaw, float pitch);
void gimbal_send_current(void);

#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_DEVICE_H__ */
