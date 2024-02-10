namespace fibjs {

void init_sym()
{
}
}

#if defined(NDEBUG) && defined(linux)

#include "glibc_config.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <elf.h>
#include <errno.h>
#include <sys/syscall.h>
#include <linux/random.h>

extern "C" {

// GLIBC_2.14
#ifdef GLIB_C_MEMCPY
__asm__(".symver _memcpy,memcpy@GLIBC_" GLIB_C_MEMCPY);
void* _memcpy(void* dest, const void* src, size_t n);
void* memcpy(void* dest, const void* src, size_t n)
{
    return _memcpy(dest, src, n);
}
#endif

// GLIBC_2.17
#ifdef GLIB_C_TIME
__asm__(".symver _clock_gettime,clock_gettime@GLIBC_" GLIB_C_TIME);
int _clock_gettime(clockid_t clk_id, struct timespec* tp);
int clock_gettime(clockid_t clk_id, struct timespec* tp)
{
    return _clock_gettime(clk_id, tp);
}

__asm__(".symver _clock_getres,clock_getres@GLIBC_" GLIB_C_TIME);
int _clock_getres(clockid_t clk_id, struct timespec* tp);
int clock_getres(clockid_t clk_id, struct timespec* tp)
{
    return _clock_getres(clk_id, tp);
}
#endif

// GLIBC_2.17
char* secure_getenv(const char* name) throw()
{
    return getenv(name);
}

// GLIBC_2.25
static ssize_t __getrandom(void* buffer, size_t length, unsigned int flags)
{
    return syscall(SYS_getrandom, buffer, length, flags);
}

int getentropy(void* buf, size_t length)
{
    char* buffer = (char*)buf;
    if (length > 256)
        return EIO;

    char* end = buffer + length;
    while (buffer < end) {
        ssize_t bytes = __getrandom(buffer, end - buffer, 0);
        if (bytes < 0) {
            if (errno == EINTR)
                continue;
            else
                return -1;
        }
        if (bytes == 0)
            return EIO;
        buffer += bytes;
    }
    return 0;
}

// GLIBC_2.28
#ifdef GLIB_C_FCNTL
__asm__(".symver _fcntl,fcntl@GLIBC_" GLIB_C_FCNTL);
int _fcntl(int fd, int cmd, ...);
int fcntl64(int fd, int cmd, ...)
{
    int result;
    va_list va;

    va_start(va, cmd);
    result = _fcntl(fd, cmd, va_arg(va, void*));
    va_end(va);

    return result;
}

int fcntl(int fd, int cmd, ...)
{
    int result;
    va_list va;

    va_start(va, cmd);
    result = _fcntl(fd, cmd, va_arg(va, void*));
    va_end(va);

    return result;
}
#endif

// GLIBC_2.29
#ifdef GLIB_C_MATH
__asm__(".symver _exp,exp@GLIBC_" GLIB_C_MATH);
double _exp(double x);
double exp(double x)
{
    return _exp(x);
}

__asm__(".symver _expf,expf@GLIBC_" GLIB_C_MATH);
float _expf(float x);
float expf(float x)
{
    return _expf(x);
}

__asm__(".symver _log,log@GLIBC_" GLIB_C_MATH);
double _log(double x);
double log(double x)
{
    return _log(x);
}

__asm__(".symver _logf,logf@GLIBC_" GLIB_C_MATH);
float _logf(float x);
float logf(float x)
{
    return _logf(x);
}

__asm__(".symver _pow,pow@GLIBC_" GLIB_C_MATH);
double _pow(double x, double y);
double pow(double x, double y)
{
    return _pow(x, y);
}

__asm__(".symver _powf,powf@GLIBC_" GLIB_C_MATH);
float _powf(float x, float y);
float powf(float x, float y)
{
    return _powf(x, y);
}
#endif

#ifdef GLIB_C_MATH2
__asm__(".symver _log2,log2@GLIBC_" GLIB_C_MATH);
double _log2(double x);
double log2(double x)
{
    return _log2(x);
}
#endif
}

#endif
