/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */

//#include <_ansi.h>
// ReSharper disable CppParameterMayBeConstPtrOrRef
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
//#include <reent.h>
#include <unistd.h>
#include <sys/wait.h>

#include "cmsis_gcc.h"
#include "SEGGER_RTT.h"

//#undef errno
//extern int errno;

#define MAX_STACK_SIZE 0x2000

#ifndef FreeRTOS
char* stack_ptr asm("sp");
#endif

unsigned int __atomic_fetch_add_4(volatile void* ptr, unsigned int val, int memmodel) {
    (void)memmodel;
    const unsigned int tmp = *(volatile unsigned int*)ptr;
    __disable_irq();
    *(volatile unsigned int*)ptr = tmp + val;
    __enable_irq();
    return tmp;
}

caddr_t _sbrk(const int incr) {
    extern char  end asm("end");
    static char* heap_end;

    if (heap_end == 0) {
        heap_end = &end;
    }

    char* prev_heap_end = heap_end;

#ifdef FreeRTOS
    char* min_stack_ptr;
    /* Use the NVIC offset register to locate the main stack pointer. */
    min_stack_ptr = (char*)(*(unsigned int*)*(unsigned int*)0xE000ED08);
    /* Locate the STACK bottom address */
    min_stack_ptr -= MAX_STACK_SIZE;

    if (heap_end + incr > min_stack_ptr)
#else
    if (heap_end + incr > stack_ptr)
#endif
    {
        //		write(1, "Heap and stack collision\n", 25);
        //		abort();
        errno = ENOMEM;
        return (caddr_t)-1;
    }

    heap_end += incr;

    return (caddr_t)prev_heap_end;
}

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
