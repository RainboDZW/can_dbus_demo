/**
 * @file app_tasks.h
 * @brief 应用层任务头文件示例
 * @author Your Name
 * @date 2025-01-04
 */

#ifndef __APP_TASKS_H__
#define __APP_TASKS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "motor_device.h"

/* 任务函数声明 */
void chassis_task_init(void);
void chassis_task(void);
void gimbal_task_init(void);
void gimbal_task(void);

/* PID控制结构体 */
typedef struct {
    float kp;
    float ki;
    float kd;
    float error;
    float last_error;
    float integral;
    float output;
    float max_output;
} pid_controller_t;

/* PID控制函数 */
float pid_calculate(pid_controller_t *pid, float setpoint, float measured_value);
void pid_init(pid_controller_t *pid, float kp, float ki, float kd, float max_output);

#ifdef __cplusplus
}
#endif

#endif /* __APP_TASKS_H__ */
