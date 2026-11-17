/*
 * CI crash reporter.
 *
 * bionic's debuggerd handler only prints the signal and the fault address, and
 * the core dumps produced inside a container do not contain the faulting
 * thread, so a SIGSEGV in CI leaves nothing to symbolize.  This small preload
 * library runs on the faulting thread, prints the signal, tid, RIP/RSP/RBP,
 * the process mappings, the frame pointer chain and a scan of the thread stack
 * for return addresses, and then restores SIG_DFL and re-raises the signal so
 * the process keeps its original exit status (a child killed with SIGSEGV is
 * still killed by SIGSEGV).
 *
 * It is built by .github/workflows/test.sh inside the test container with the
 * image's own $CC and preloaded into the fibjs process only.  The constructor
 * removes LD_PRELOAD from the environment: the suite spawns glibc programs
 * (sh, node) that cannot load a bionic library and would otherwise fail.
 *
 * Keep this file free of anything but the x86_64 Android target: it is a
 * debugging aid for the android/x64 job, not part of the product.
 */
#define _GNU_SOURCE
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ucontext.h>
#include <fcntl.h>
#include <sys/syscall.h>

#if !defined(__x86_64__)
/* Only wired up for android/x64; compiling elsewhere yields a no-op library. */
__attribute__((constructor)) static void crash_report_init(void) { }
#else

#define MAX_EXEC_RANGES 2048
#define MAX_SCAN (256 * 1024)

static volatile sig_atomic_t g_busy;

static uintptr_t g_exec_start[MAX_EXEC_RANGES];
static uintptr_t g_exec_end[MAX_EXEC_RANGES];
static int g_exec_count;
static uintptr_t g_stack_end;

static void put(const char *s, size_t n) {
    while (n > 0) {
        ssize_t r = write(2, s, n);
        if (r <= 0)
            return;
        s += r;
        n -= (size_t)r;
    }
}

static void puts_(const char *s) { put(s, strlen(s)); }

static void puthex(uintptr_t v) {
    static const char digits[] = "0123456789abcdef";
    char b[19];
    b[0] = '0';
    b[1] = 'x';
    for (int i = 0; i < 16; i++)
        b[2 + i] = digits[(v >> ((15 - i) * 4)) & 0xf];
    b[18] = '\n';
    put(b, sizeof(b));
}

static void putdec(long v) {
    char b[24];
    int i = (int)sizeof(b);
    int neg = v < 0;
    unsigned long u = neg ? (unsigned long)(-v) : (unsigned long)v;

    b[--i] = '\n';
    if (u == 0)
        b[--i] = '0';
    while (u) {
        b[--i] = (char)('0' + u % 10);
        u /= 10;
    }
    if (neg)
        b[--i] = '-';
    put(b + i, sizeof(b) - (size_t)i);
}

/* Parse one "start-end perms ..." line: remember executable ranges and the end
 * of the mapping that contains the faulting stack pointer. */
static void parse_maps_line(const char *line, uintptr_t sp) {
    uintptr_t start = 0, end = 0;
    int i = 0, j = 0;

    while (line[i] && line[i] != '-') {
        char c = line[i++];
        int d;
        if (c >= '0' && c <= '9')
            d = c - '0';
        else if (c >= 'a' && c <= 'f')
            d = c - 'a' + 10;
        else
            break;
        start = (start << 4) | (uintptr_t)d;
    }
    if (line[i] != '-')
        return;
    i++;
    while (line[i]) {
        char c = line[i++];
        int d;
        if (c >= '0' && c <= '9')
            d = c - '0';
        else if (c >= 'a' && c <= 'f')
            d = c - 'a' + 10;
        else
            break;
        end = (end << 4) | (uintptr_t)d;
    }
    if (!end)
        return;
    if (start <= sp && sp < end)
        g_stack_end = end;

    while (line[j] && line[j] != ' ')
        j++;
    if (!line[j])
        return;
    j++;
    if (line[j + 2] == 'x' && g_exec_count < MAX_EXEC_RANGES) {
        g_exec_start[g_exec_count] = start;
        g_exec_end[g_exec_count] = end;
        g_exec_count++;
    }
}

static int in_exec(uintptr_t v) {
    for (int i = 0; i < g_exec_count; i++)
        if (v >= g_exec_start[i] && v < g_exec_end[i])
            return 1;
    return 0;
}

static void handler(int sig, siginfo_t *si, void *uc) {
    ucontext_t *u = (ucontext_t *)uc;
    char buf[64 * 1024];
    int fd;
    long n;

    if (g_busy)
        _exit(128 + sig);
    g_busy = 1;

    uintptr_t rip = (uintptr_t)u->uc_mcontext.gregs[REG_RIP];
    uintptr_t rsp = (uintptr_t)u->uc_mcontext.gregs[REG_RSP];
    uintptr_t rbp = (uintptr_t)u->uc_mcontext.gregs[REG_RBP];

    g_exec_count = 0;
    g_stack_end = 0;

    puts_("CRASH sig=");
    putdec(sig);
    puts_("CRASH addr=");
    puthex((uintptr_t)si->si_addr);
    puts_("CRASH tid=");
    putdec(syscall(SYS_gettid));
    puts_("CRASH pid=");
    putdec(syscall(SYS_getpid));
    puts_("CRASH rip=");
    puthex(rip);
    puts_("CRASH rsp=");
    puthex(rsp);
    puts_("CRASH rbp=");
    puthex(rbp);

    /* First pass: executable ranges and the end of the faulting stack. */
    fd = syscall(SYS_openat, AT_FDCWD, "/proc/self/maps", O_RDONLY, 0);
    if (fd >= 0) {
        char line[512];
        int len = 0;
        for (;;) {
            char c;
            n = syscall(SYS_read, fd, &c, 1);
            if (n <= 0)
                break;
            if (c == '\n') {
                line[len] = 0;
                parse_maps_line(line, rsp);
                len = 0;
            } else if (len < (int)sizeof(line) - 1) {
                line[len++] = c;
            }
        }
        syscall(SYS_close, fd);
    }

    /* Dump the maps so the addresses can be symbolized offline. */
    fd = syscall(SYS_openat, AT_FDCWD, "/proc/self/maps", O_RDONLY, 0);
    if (fd >= 0) {
        puts_("MAPS-BEGIN\n");
        for (;;) {
            n = syscall(SYS_read, fd, buf, sizeof(buf));
            if (n <= 0)
                break;
            put(buf, (size_t)n);
        }
        syscall(SYS_close, fd);
        puts_("MAPS-END\n");
    }

    /* Frame pointer chain (fibjs keeps the frame pointer on release builds). */
    puts_("FPCHAIN-BEGIN\n");
    {
        uintptr_t fp = rbp;
        for (int i = 0; i < 64; i++) {
            if (fp < rsp || fp + 16 > g_stack_end || (fp & 7))
                break;
            uintptr_t next = *(volatile uintptr_t *)fp;
            uintptr_t ret = *(volatile uintptr_t *)(fp + 8);
            puthex(ret);
            fp = next;
        }
    }
    puts_("FPCHAIN-END\n");

    /* Stack scan: words that point into an executable mapping are candidate
     * return addresses; symbolization filters the rest. */
    puts_("STACK-BEGIN\n");
    {
        uintptr_t end = g_stack_end ? g_stack_end : rsp + MAX_SCAN;
        if (end > rsp + MAX_SCAN)
            end = rsp + MAX_SCAN;
        for (uintptr_t p = rsp & ~(uintptr_t)7; p + 8 <= end; p += 8) {
            uintptr_t v = *(volatile uintptr_t *)p;
            if (in_exec(v))
                puthex(v);
        }
    }
    puts_("STACK-END\n");

    signal(sig, SIG_DFL);
    syscall(SYS_tgkill, syscall(SYS_getpid), syscall(SYS_gettid), sig);
    _exit(128 + sig);
}

__attribute__((constructor)) static void crash_report_init(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, 0);
    sigaction(SIGBUS, &sa, 0);
    sigaction(SIGILL, &sa, 0);
    sigaction(SIGABRT, &sa, 0);

    unsetenv("LD_PRELOAD");
}
#endif
