//
// Created by 28331 on 25-2-24.
//

#ifndef KALMAN_H
#define KALMAN_H

typedef struct {
    float x;     // 上一次预测值
    float p;     // 上一次预测值的方差
    float k;     // 卡尔曼增益
    float q_min; // 测量噪声的方差， 越大越相信测量值
    float q_max; // 测量噪声的方差， 越大越相信测量值
    float r;     // 过程噪声的方差， 越大越相信上次的预测值
} KalmanFilter;

void Kalman_Init(KalmanFilter* kal, float q_min, float q_max, float r) ;

float Kalman_update(KalmanFilter* kal, float measure);

#endif //KALMAN_H
