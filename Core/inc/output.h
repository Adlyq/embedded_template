//
// Created by adlyq on 25-3-25.
//

#pragma once

#include "common_defs.h"

EXTERN_C_BEGIN
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

EXTERN_C_END