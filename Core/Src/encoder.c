#include "encoder.h"
#include "tim.h"
#include "stm32f1xx_hal_tim.h"

static int32_t encoder_total_count[2] = {0, 0};
static float current_rpm[2] = {0.0f, 0.0f};
static uint32_t last_time[2] = {0, 0};
static int32_t last_count[2] = {0, 0};

void Encoder_Init(void)
{
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
    // 使用宏替代函数，避免链接错误
    __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
    __HAL_TIM_ENABLE_IT(&htim4, TIM_IT_UPDATE);
}

void Encoder_UpdateIRQHandler(Encoder_ID enc)
{
    TIM_HandleTypeDef *htim;
    int32_t *total;
    if (enc == ENCODER_1) {
        htim = &htim3;
        total = &encoder_total_count[0];
    } else {
        htim = &htim4;
        total = &encoder_total_count[1];
    }

    if (__HAL_TIM_GET_FLAG(htim, TIM_FLAG_UPDATE) != RESET) {
        __HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_UPDATE);
        // 直接读取方向位
        if ((__HAL_TIM_GET_COUNTER(htim) & TIM_CR1_DIR) == TIM_CR1_DIR) {
            *total -= 65536;
        } else {
            *total += 65536;
        }
    }
}

int32_t Encoder_GetCount(Encoder_ID enc)
{
    TIM_HandleTypeDef *htim;
    int32_t total;
    if (enc == ENCODER_1) {
        htim = &htim3;
        total = encoder_total_count[0];
    } else {
        htim = &htim4;
        total = encoder_total_count[1];
    }
    uint32_t count = __HAL_TIM_GET_COUNTER(htim);
    return total + (int32_t)count;
}

void Encoder_ResetCount(Encoder_ID enc)
{
    if (enc == ENCODER_1) {
        encoder_total_count[0] = 0;
        __HAL_TIM_SET_COUNTER(&htim3, 0);
    } else {
        encoder_total_count[1] = 0;
        __HAL_TIM_SET_COUNTER(&htim4, 0);
    }
}

void Encoder_UpdateSpeed(Encoder_ID enc)
{
    uint32_t now = HAL_GetTick();
    int32_t count = Encoder_GetCount(enc);
    int32_t delta_count = count - last_count[enc];
    uint32_t delta_time = now - last_time[enc];
    if (delta_time > 0) {
        float rps = (float)delta_count / (ENCODER_PPR * (delta_time / 1000.0f));
        current_rpm[enc] = rps * 60.0f;
    }
    last_count[enc] = count;
    last_time[enc] = now;
}

float Encoder_GetRPM(Encoder_ID enc)
{
    return current_rpm[enc];
}
