#ifndef __ENCODER_H
#define __ENCODER_H

#include <stdint.h>

#define ENCODER_PPR 400  // 编码器每转脉冲数（需根据实际修改）

typedef enum {
    ENCODER_1,
    ENCODER_2
} Encoder_ID;

void Encoder_Init(void);
void Encoder_UpdateIRQHandler(Encoder_ID enc);
int32_t Encoder_GetCount(Encoder_ID enc);
void Encoder_ResetCount(Encoder_ID enc);
void Encoder_UpdateSpeed(Encoder_ID enc);
float Encoder_GetRPM(Encoder_ID enc);

#endif

