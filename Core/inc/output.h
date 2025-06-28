//
// Created by adlyq on 25-3-25.
//

#ifndef OUTPUT_H
#define OUTPUT_H
#include <stdbool.h>

// #define OUTPUT_DEST_SUPPORT
// #define OUTPUT_DEST_KEEP

void outputInit(void);

#ifdef OUTPUT_DEST_SUPPORT
void outputLDSet(bool);
#endif

void outputSet(bool);

bool outputGet(void);

void onShortCircuit();

void onNotShortCircuit();

bool isShortCircuit();

void outputFlash(void);

#endif //OUTPUT_H
