#ifndef _WIN32

#include "config.h"

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

namespace fibjs {
void init_sym()
{
}
}

extern "C" {

#ifdef HAVE_GLIB_C_225_H
__asm__(".symver memcpy,memcpy@GLIBC_2.2.5");
void* __wrap_memcpy(void* dest, const void* src, size_t n)
{
    return memcpy(dest, src, n);
}
#endif

// GLIBC_2.7
size_t __fread_chk(void*, size_t, size_t, size_t, FILE*)
{
    puts("unexpected __fread_chk.");
    exit(-1);
    return 0;
}

// GLIBC_2.11
void __longjmp_chk()
{
    puts("unexpected __longjmp_chk.");
    exit(-1);
}

// GLIBC_2.11
int __isoc99_sscanf(const char* s, const char* format, ...)
{
    va_list arg;
    int done;

    va_start(arg, format);
    done = vsscanf(s, format, arg);
    va_end(arg);

    return done;
}

// GLIBC_2.28
#undef fcntl

#ifdef HAVE_GLIB_C_FCNTL_2_H
__asm__(".symver fcntl,fcntl@GLIBC_2.0");
#endif

#ifdef HAVE_GLIB_C_FCNTL_225_H
__asm__(".symver fcntl,fcntl@GLIBC_2.2.5");
#endif

#ifdef HAVE_GLIB_C_FCNTL_24_H
__asm__(".symver fcntl,fcntl@GLIBC_2.4");
#endif

#ifdef HAVE_GLIB_C_FCNTL_217_H
__asm__(".symver fcntl,fcntl@GLIBC_2.17");
#endif

int fcntl(int fd, int cmd, void* lock);
int fcntl64(int fd, int cmd, void* lock)
{
    return fcntl(fd, cmd, lock);
}
}

#endif