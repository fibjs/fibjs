#ifndef _WIN32

#include "config.h"

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>

namespace fibjs {
void init_sym()
{
}
}

#ifdef NDEBUG

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

// GLIBC_2.28
#ifdef GLIB_C_FCNTL
__asm__(".symver _fcntl,fcntl@GLIBC_" GLIB_C_FCNTL);
int _fcntl(int fd, int cmd, ...);
int fcntl64(int fd, int cmd, ...)
{
    int result;
    va_list va;
    va_start(va, cmd);

    switch (cmd) {
    //
    // File descriptor flags
    //
    case F_GETFD:
        goto takes_void;
    case F_SETFD:
        goto takes_int;

    // File status flags
    //
    case F_GETFL:
        goto takes_void;
    case F_SETFL:
        goto takes_int;

    // File byte range locking, not held across fork() or clone()
    //
    case F_SETLK:
        goto takes_flock_ptr_INCOMPATIBLE;
    case F_SETLKW:
        goto takes_flock_ptr_INCOMPATIBLE;
    case F_GETLK:
        goto takes_flock_ptr_INCOMPATIBLE;

    // File byte range locking, held across fork()/clone() -- Not POSIX
    //
    case F_OFD_SETLK:
        goto takes_flock_ptr_INCOMPATIBLE;
    case F_OFD_SETLKW:
        goto takes_flock_ptr_INCOMPATIBLE;
    case F_OFD_GETLK:
        goto takes_flock_ptr_INCOMPATIBLE;

    // Managing I/O availability signals
    //
    case F_GETOWN:
        goto takes_void;
    case F_SETOWN:
        goto takes_int;
    case F_GETOWN_EX:
        goto takes_f_owner_ex_ptr;
    case F_SETOWN_EX:
        goto takes_f_owner_ex_ptr;
    case F_GETSIG:
        goto takes_void;
    case F_SETSIG:
        goto takes_int;

    // Notified when process tries to open or truncate file (Linux 2.4+)
    //
    case F_SETLEASE:
        goto takes_int;
    case F_GETLEASE:
        goto takes_void;

    // File and directory change notification
    //
    case F_NOTIFY:
        goto takes_int;

    // Changing pipe capacity (Linux 2.6.35+)
    //
    case F_SETPIPE_SZ:
        goto takes_int;
    case F_GETPIPE_SZ:
        goto takes_void;

    // File sealing (Linux 3.17+)
    //
    case F_ADD_SEALS:
        goto takes_int;
    case F_GET_SEALS:
        goto takes_void;

    // File read/write hints (Linux 4.13+)
    //
    case F_GET_RW_HINT:
        goto takes_uint64_t_ptr;
    case F_SET_RW_HINT:
        goto takes_uint64_t_ptr;
    case F_GET_FILE_RW_HINT:
        goto takes_uint64_t_ptr;
    case F_SET_FILE_RW_HINT:
        goto takes_uint64_t_ptr;

    default:
        fprintf(stderr, "fcntl64 workaround got unknown F_XXX constant");
    }

takes_void:
    va_end(va);
    return _fcntl(fd, cmd);

takes_int:
    result = _fcntl(fd, cmd, va_arg(va, int));
    va_end(va);
    return result;

takes_flock_ptr_INCOMPATIBLE:
    fprintf(stderr, "_fcntl hack can't use glibc flock directly");
    exit(1);

takes_f_owner_ex_ptr:
    result = _fcntl(fd, cmd, va_arg(va, struct f_owner_ex*));
    va_end(va);
    return result;

takes_uint64_t_ptr:
    result = _fcntl(fd, cmd, va_arg(va, uint64_t*));
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

__asm__(".symver _log,log@GLIBC_" GLIB_C_MATH);
double _log(double x);
double log(double x)
{
    return _log(x);
}

__asm__(".symver _pow,pow@GLIBC_" GLIB_C_MATH);
double _pow(double x, double y);
double pow(double x, double y)
{
    return _pow(x, y);
}
#endif
}

#endif

#endif
