#ifndef _WIN32

#include "config.h"

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

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
}

#endif