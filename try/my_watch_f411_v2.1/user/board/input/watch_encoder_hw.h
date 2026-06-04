#ifndef WATCH_ENCODER_HW_H
#define WATCH_ENCODER_HW_H

#include <stdint.h>

void watch_encoder_hw_init(void);
int16_t watch_encoder_hw_get_delta(void);
void watch_encoder_hw_reset(void);

#endif /* WATCH_ENCODER_HW_H */
