/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "my_can.h"
#include "app_tasks.h"
#include "dbus.h"
#include "bsp_uart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CAN1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  /* 初始化系统组件 */
  my_can_system_init();
  chassis_task_init();
  gimbal_task_init();

  /* 初始化DBUS双缓冲区串口系统 */
  HAL_Delay(100);  // 等待系统稳定
  DBUS_Init();

  /* 定义运动控制变量 */
  int16_t vx = 0, vy = 0, vw = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* 监控遥控器连接状态 */
    Remote_Message_Moniter(&remote_ctrl);

    /* 静态变量用于状态变化检测 */
    static bool last_rc_status = true;  // 上次遥控器状态

    /* 检查遥控器连接状态变化 */
    if (last_rc_status != remote_ctrl.rc_lost)
    {
        last_rc_status = remote_ctrl.rc_lost;
    }

    /* 检查遥控器是否在线 */
    if (!remote_ctrl.rc_lost)
    {
        /* 左拨杆控制前后左右移动
         * remote_ctrl.rc.ch[1]: ���拨杆Y轴（前后）- 向上为正（前进）
         * remote_ctrl.rc.ch[0]: 左拨杆X轴（左右）- 向右为正（右移）
         * remote_ctrl.rc.ch[2]: 右拨杆X轴（旋转）- 向右为正（顺时针）
         * 遥控器数据范围: -660 到 +660
         */

        /* 前后控制（Y轴）- 向上推为前进 */
        vx = remote_ctrl.rc.ch[1] * 500 / 660;

        /* 左右控制（X轴）- 向右推为右移 */
        vy = remote_ctrl.rc.ch[0] * 500 / 660;

        /* 旋转控制（右拨杆X轴）- 可选功能 */
        if (switch_is_up(remote_ctrl.rc.s[0]))  // 左开关在上位时启用旋转
        {
            vw = remote_ctrl.rc.ch[2] * 300 / 660;
        }
        else
        {
            vw = 0;  // 其他情况下禁用旋转
        }


        /* 限制最大速度 */
        if (vx > 500) vx = 500;
        if (vx < -500) vx = -500;
        if (vy > 500) vy = 500;
        if (vy < -500) vy = -500;
        if (vw > 300) vw = 300;
        if (vw < -300) vw = -300;

    }
    else
    {
        /* 遥控器离线，停止运动 */
        vx = vy = vw = 0;
    }

    /* 设置底盘速度 */
    chassis_set_speed(vx, vy, vw);

    /* 发送底盘电机CAN命令 */
    chassis_send_current();

    /* 延时，避免CPU占用过高 */
    HAL_Delay(10);  /* 10ms延时，100Hz控制频率 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
 //   Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
