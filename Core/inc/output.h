//
// Created by adlyq on 25-3-25.
//

#ifndef OUTPUT_H
#define OUTPUT_H
#include <stdbool.h>

// #define OUTPUT_DEST_L

void outputInit(void);

void outputStart(void);

void outputStop(void);

bool  outputGet(void);

void onShortCircuit();

void onNotShortCircuit();

bool isShortCircuit();

void outputFlash(void);

#endif //OUTPUT_H
