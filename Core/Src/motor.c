#include "motor.h"
#include "tim.h"
#include "gpio.h"
#include "main.h"

#define MOTOR1_IN1_PIN   MotorA_Pin
#define MOTOR1_IN1_PORT  MotorA_GPIO_Port
#define MOTOR1_IN2_PIN   Motora_Pin
#define MOTOR1_IN2_PORT  Motora_GPIO_Port

#define MOTOR2_IN1_PIN   MotorB_Pin
#define MOTOR2_IN1_PORT  MotorB_GPIO_Port
#define MOTOR2_IN2_PIN   Motorb_Pin
#define MOTOR2_IN2_PORT  Motorb_GPIO_Port

#define MOTOR1_PWM_CHANNEL  TIM_CHANNEL_1
#define MOTOR2_PWM_CHANNEL  TIM_CHANNEL_2

void Motor_Init(void)
{
    // 方向引脚已在 gpio.c 中配置，无需重复
}

void Motor_SetPWM(Motor_ID motor, int16_t pwm)
{
    TIM_HandleTypeDef *htim = &htim1;
    uint32_t channel;
    uint16_t in1_pin, in2_pin;
    GPIO_TypeDef *in1_port, *in2_port;

    if (motor == MOTOR_1) {
        channel = MOTOR1_PWM_CHANNEL;
        in1_pin = MOTOR1_IN1_PIN;
        in1_port = MOTOR1_IN1_PORT;
        in2_pin = MOTOR1_IN2_PIN;
        in2_port = MOTOR1_IN2_PORT;
    } else {
        channel = MOTOR2_PWM_CHANNEL;
        in1_pin = MOTOR2_IN1_PIN;
        in1_port = MOTOR2_IN1_PORT;
        in2_pin = MOTOR2_IN2_PIN;
        in2_port = MOTOR2_IN2_PORT;
    }

    uint32_t pwm_value = (pwm > 0) ? pwm : -pwm;
    if (pwm_value > 1000) pwm_value = 1000;

    __HAL_TIM_SET_COMPARE(htim, channel, pwm_value);

    // 方向控制
    if (pwm > 0) {
        HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_RESET);
    } else if (pwm < 0) {
        HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_RESET);
    }
}

void Motor_Stop(Motor_ID motor)
{
    Motor_SetPWM(motor, 0);
}

// 以下函数为占位，保持 FSM 兼容性
void Motor_SetMode(uint8_t mode)
{
    // 当前仅支持 PWM 模式，无需操作
}

void Motor_SetDirection(uint8_t dir)
{
    // 方向由 PWM 符号决定，无需单独设置
}
