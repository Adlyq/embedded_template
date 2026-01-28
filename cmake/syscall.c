// ReSharper disable CppParameterMayBeConstPtrOrRef
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "SEGGER_RTT.h"

#define MAX_STACK_SIZE 0x2000

// 检查是否使用了 FDEV_SETUP_STREAM (Picolibc 特有宏)
#ifdef FDEV_SETUP_STREAM

static int picolibc_rtt_put(const char c, FILE* file) {
    (void)file;

#ifdef SEGGER_RTT
    SEGGER_RTT_PutCharSkip(0, c);
#endif

    return c;
}

static int picolibc_rtt_get(FILE* file) {
    (void)file;

#ifdef SEGGER_RTT
    return SEGGER_RTT_WaitKey();
#else
    return -1;
#endif
}

static FILE __stdio = FDEV_SETUP_STREAM(picolibc_rtt_put, picolibc_rtt_get, NULL, _FDEV_SETUP_RW); // NOLINT(*-non-copyable-objects, *-reserved-identifier)

// 4. 覆盖标准流指针
FILE* const stdout = &__stdio;
FILE* const stderr = &__stdio;
FILE* const stdin  = &__stdio;


void _init(void) {
}

#else

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
