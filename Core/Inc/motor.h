#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdint.h>

typedef enum {
    MOTOR_1,
    MOTOR_2
} Motor_ID;

void Motor_Init(void);
void Motor_SetPWM(Motor_ID motor, int16_t pwm);
void Motor_Stop(Motor_ID motor);

#endif
