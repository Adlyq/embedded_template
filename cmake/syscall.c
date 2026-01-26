// ReSharper disable CppParameterMayBeConstPtrOrRef
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "SEGGER_RTT.h"

#define MAX_STACK_SIZE 0x2000

// 检查是否使用了 FDEV_SETUP_STREAM (Picolibc 特有宏)
#if defined(FDEV_SETUP_STREAM) && defined(SEGGER_RTT) && defined(STDIO_PRINT)

static int picolibc_rtt_put(const char c, FILE* file) {
    (void)file;
    SEGGER_RTT_PutCharSkip(0, c);
    return c;
}

static int picolibc_rtt_get(FILE* file) {
    (void)file;
    return SEGGER_RTT_WaitKey();
}

static FILE __stdio = FDEV_SETUP_STREAM(picolibc_rtt_put, picolibc_rtt_get, NULL, _FDEV_SETUP_RW); // NOLINT(*-non-copyable-objects, *-reserved-identifier)

// 4. 覆盖标准流指针
FILE* const stdout = &__stdio;
FILE* const stderr = &__stdio;
FILE* const stdin  = &__stdio;

#endif

// ----------------------------------------------------------------------------
// Newlib Stdout 重定向 (适用于标准 GCC + Newlib/Nano)
// ----------------------------------------------------------------------------
// 如果不是 Picolibc，通常是 Newlib，需要实现 _write 等函数
#ifndef FDEV_SETUP_STREAM

#include <sys/stat.h>

__attribute__((weak)) int _write(int file, char* ptr, int len) {
    (void)file;
    return SEGGER_RTT_Write(0, ptr, len);
}

__attribute__((weak)) int _close(int file) {
    (void)file;
    return -1;
}

__attribute__((weak)) int _fstat(int file, struct stat* st) {
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

__attribute__((weak)) int _isatty(int file) {
    (void)file;
    return 1;
}

__attribute__((weak)) int _lseek(int file, int ptr, int dir) {
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

__attribute__((weak)) int _read(int file, char* ptr, int len) {
    (void)file;
    (void)ptr;
    (void)len;
    return 0;
}

#endif

void _init(void) {
}
