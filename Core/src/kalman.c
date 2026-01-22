//
// Created by 28331 on 25-2-24.
//

#include "kalman.h"

#include <math.h>

#define LARGE_CHANGE_THRESHOLD 50.0f

void Kalman_Init(KalmanFilter* kal, const float q_min, const float q_max, const float r) {
    kal->x = 1000.0f;
    kal->p = 0.7f;
    kal->q_min = q_min;
    kal->q_max = q_max;
    kal->r = r;
}

float Kalman_update(KalmanFilter* kal, const float measure) {
    float q;
    if (fabsf(measure - kal->x) > LARGE_CHANGE_THRESHOLD) {
        q = kal->q_max;
    } else {
        q = kal->q_min;
    }

    kal->k = kal->p / (kal->p + kal->r);
    kal->x = kal->x + kal->k * ((float)measure - kal->x);
    kal->p = (1 - kal->k) * kal->p + q;

    return kal->x;
}
