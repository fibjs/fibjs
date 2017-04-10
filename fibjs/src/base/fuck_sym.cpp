#include <exlib/include/osconfig.h>

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef Linux

extern "C" {

#if defined(amd64)
__asm__(".symver memcpy,memcpy@GLIBC_2.2.5");
void* __wrap_memcpy(void* dest, const void* src, size_t n)
{
    return memcpy(dest, src, n);
}
#endif

void __longjmp_chk()
{
    puts("unexpected __longjmp_chk.");
    exit(-1);
}

size_t __fread_chk(void*, size_t, size_t, size_t, FILE*)
{
    puts("unexpected __fread_chk.");
    exit(-1);
    return 0;
}

int32_t __isoc99_sscanf(const char* s, const char* format, ...)
{
    va_list arg;
    int32_t done;

    va_start(arg, format);
    done = vsscanf(s, format, arg);
    va_end(arg);

    return done;
}
}

#endif
