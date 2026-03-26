#include "main.h"
#include "tim.h"
#include "motor.h"
#include "encoder.h"
#include "fsm.h"
#include "usart.h"
#include "pid.h"
#include <stdio.h>

ControlMode control_mode = MODE_AUTO_PWM;
PID_HandleTypeDef pid;
uint8_t pid_enabled = 0;
float target_rpm = 0.0f;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_TIM1_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();

    Encoder_Init();
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    PID_Init(&pid, 1.0f, 0.1f, 0.0f);

    uint32_t pwm_duty = 0;
    uint8_t dir = 1;
    uint32_t last_pwm_update = 0;
    uint32_t last_pid_update = 0;
    const uint32_t PID_UPDATE_PERIOD_MS = 10; // PID 周期 10ms

    while (1) {
        uint32_t now = HAL_GetTick();

        // 自动渐变模式
        if (control_mode == MODE_AUTO_PWM) {
            if (now - last_pwm_update >= 30) {
                if (dir) {
                    pwm_duty += 5;
                    if (pwm_duty >= 1000) dir = 0;
                } else {
                    pwm_duty -= 5;
                    if (pwm_duty <= 0) dir = 1;
                }
                Motor_SetPWM(MOTOR_1, pwm_duty);  // 控制电机1
                last_pwm_update = now;
            }
        }
        // PID 闭环模式
        else if (control_mode == MODE_PID_CLOSED && pid_enabled) {
            if (now - last_pid_update >= PID_UPDATE_PERIOD_MS) {
                // 更新电机1的转速
                Encoder_UpdateSpeed(ENCODER_1);
                float real_rpm = Encoder_GetRPM(ENCODER_1);
                float pid_out = PID_Update(&pid, target_rpm, real_rpm);
                int16_t pwm_val = (int16_t)pid_out;
                if (pwm_val > 1000) pwm_val = 1000;
                if (pwm_val < -1000) pwm_val = -1000;
                Motor_SetPWM(MOTOR_1, pwm_val);
                last_pid_update = now;
            }
        }

        // 每秒上报一次转速（电机1）
        static uint32_t last_rpt = 0;
        if (now - last_rpt >= 1000) {
            char buf[64];
            float rpm1 = Encoder_GetRPM(ENCODER_1);
            sprintf(buf, "RPM1: %.0f rpm\r\n", rpm1);
            UART_SendString(buf);
            last_rpt = now;
        }

        // 更新两个电机的转速（用于其它用途）
        Encoder_UpdateSpeed(ENCODER_1);
        Encoder_UpdateSpeed(ENCODER_2);

        FSM_Process();
        HAL_Delay(10);
    }
}

void SystemClock_Config(void)
{
    // 保持原有配置，此处省略（假设已存在）
    // ...
}

static void MX_GPIO_Init(void)
{
    // 实际调用 HAL 库生成的 MX_GPIO_Init（在 gpio.c 中实现）
    // 注意：需要确保 gpio.c 中已正确配置所有引脚
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) {}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    // 可添加调试输出
}
#endif
