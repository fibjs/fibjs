/*
 * CI crash reporter.
 *
 * bionic's debuggerd handler only prints the signal and the fault address, and
 * the core dumps produced inside a container do not contain the faulting
 * thread, so a SIGSEGV in CI leaves nothing to symbolize.  This small preload
 * library runs on the faulting thread and prints
 *
 *   - the signal, si_code, fault address, tid/pid,
 *   - the full register set,
 *   - the bytes at RIP (so the faulting instruction is verifiable),
 *   - the 0x120 bytes at r14 and r15 (a window on the object each of them
 *     points to: r14 is the state machine in the AsyncEnd frames, r15 the slot
 *     it dereferenced),
 *   - the top of the stack, the process mappings (so the load bias is known),
 *   - the frame pointer chain and a scan of the thread stack for code pointers
 *     (candidate return addresses),
 *
 * and then restores SIG_DFL and re-raises the signal so the process keeps its
 * original exit status (a child killed with SIGSEGV is still killed by SIGSEGV).
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
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <ucontext.h>
#include <unistd.h>

#if !defined(__x86_64__)
/* Only wired up for android/x64; compiling elsewhere yields a no-op library. */
__attribute__((constructor)) static void crash_report_init(void) { }
#else

/* One entry per mapping; enough for a full V8 process plus the harness. */
#define MAX_RANGES 4096
#define MAX_SCAN (256 * 1024)
#define OBJ_WINDOW 0x120

static volatile sig_atomic_t g_busy;

static uintptr_t g_exec_start[MAX_RANGES];
static uintptr_t g_exec_end[MAX_RANGES];
static int g_exec_count;

static uintptr_t g_rd_start[MAX_RANGES];
static uintptr_t g_rd_end[MAX_RANGES];
static int g_rd_count;

static uintptr_t g_stack_end;

/* Retry on EINTR/EAGAIN: stderr is a pipe in CI and a short or empty write used
 * to truncate the dump in the middle of the maps (the interesting part). */
static void put(const char *s, size_t n) {
    int spins = 0;
    while (n > 0) {
        ssize_t r = write(2, s, n);
        if (r > 0) {
            s += r;
            n -= (size_t)r;
            spins = 0;
        } else if (r < 0 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)) {
            if (++spins > (1 << 20))
                return;
        } else {
            return;
        }
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

/* "name=" followed by an address and a newline. */
static void putkv(const char *name, uintptr_t v) {
    puts_(name);
    puthex(v);
}

/* "tag-BEGIN <addr>" then hex bytes, 16 per line, then "tag-END". */
static void putbytes(const char *tag, uintptr_t addr, size_t len) {
    static const char digits[] = "0123456789abcdef";
    char line[3 * 16 + 1];

    puts_(tag);
    puts_("-BEGIN ");
    puthex(addr);
    for (size_t i = 0; i < len; i += 16) {
        size_t n = len - i > 16 ? 16 : len - i;
        for (size_t j = 0; j < n; j++) {
            unsigned char c = *(volatile unsigned char *)(addr + i + j);
            line[j * 3] = digits[c >> 4];
            line[j * 3 + 1] = digits[c & 0xf];
            line[j * 3 + 2] = ' ';
        }
        line[n * 3 - 1] = '\n';
        put(line, n * 3);
    }
    puts_(tag);
    puts_("-END\n");
}

/* Parse one "start-end perms offset dev inode path" line: remember executable
 * and readable ranges, and the end of the mapping holding the faulting stack
 * pointer. */
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
    if (line[j] == 'r' && g_rd_count < MAX_RANGES) {
        g_rd_start[g_rd_count] = start;
        g_rd_end[g_rd_count] = end;
        g_rd_count++;
    }
    if (line[j + 2] == 'x' && g_exec_count < MAX_RANGES) {
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

static int range_readable(uintptr_t a, size_t n) {
    for (int i = 0; i < g_rd_count; i++)
        if (a >= g_rd_start[i] && n <= g_rd_end[i] - a)
            return 1;
    return 0;
}

static void scan_maps(uintptr_t sp) {
    int fd = (int)syscall(SYS_openat, AT_FDCWD, "/proc/self/maps", O_RDONLY, 0);
    if (fd < 0)
        return;

    char line[512];
    int len = 0;
    for (;;) {
        char c;
        long n = syscall(SYS_read, fd, &c, 1);
        if (n <= 0)
            break;
        if (c == '\n') {
            line[len] = 0;
            parse_maps_line(line, sp);
            len = 0;
        } else if (len < (int)sizeof(line) - 1) {
            line[len++] = c;
        }
    }
    syscall(SYS_close, fd);
}

static void dump_maps(void) {
    char buf[64 * 1024];
    int fd = (int)syscall(SYS_openat, AT_FDCWD, "/proc/self/maps", O_RDONLY, 0);
    if (fd < 0)
        return;

    puts_("MAPS-BEGIN\n");
    for (;;) {
        long n = syscall(SYS_read, fd, buf, sizeof(buf));
        if (n <= 0)
            break;
        put(buf, (size_t)n);
    }
    syscall(SYS_close, fd);
    puts_("MAPS-END\n");
}

static void dump_object(const char *tag, uintptr_t addr) {
    if (!addr || !range_readable(addr, OBJ_WINDOW)) {
        puts_(tag);
        puts_("-UNREADABLE\n");
        return;
    }
    putkv(tag, addr);
    putbytes(tag, addr, OBJ_WINDOW);
}

static void handler(int sig, siginfo_t *si, void *uc) {
    ucontext_t *u = (ucontext_t *)uc;
    greg_t *g = u->uc_mcontext.gregs;

    if (g_busy)
        _exit(128 + sig);
    g_busy = 1;

    uintptr_t rip = (uintptr_t)g[REG_RIP];
    uintptr_t rsp = (uintptr_t)g[REG_RSP];
    uintptr_t rbp = (uintptr_t)g[REG_RBP];
    uintptr_t r14 = (uintptr_t)g[REG_R14];
    uintptr_t r15 = (uintptr_t)g[REG_R15];

    g_exec_count = 0;
    g_rd_count = 0;
    g_stack_end = 0;

    puts_("CRASH sig=");
    putdec(sig);
    puts_("CRASH code=");
    putdec(si->si_code);
    puts_("CRASH addr=");
    puthex((uintptr_t)si->si_addr);
    puts_("CRASH tid=");
    putdec(syscall(SYS_gettid));
    puts_("CRASH pid=");
    putdec(syscall(SYS_getpid));

    putkv("CRASH rax=", (uintptr_t)g[REG_RAX]);
    putkv("CRASH rbx=", (uintptr_t)g[REG_RBX]);
    putkv("CRASH rcx=", (uintptr_t)g[REG_RCX]);
    putkv("CRASH rdx=", (uintptr_t)g[REG_RDX]);
    putkv("CRASH rsi=", (uintptr_t)g[REG_RSI]);
    putkv("CRASH rdi=", (uintptr_t)g[REG_RDI]);
    putkv("CRASH rbp=", rbp);
    putkv("CRASH rsp=", rsp);
    putkv("CRASH r08=", (uintptr_t)g[REG_R8]);
    putkv("CRASH r09=", (uintptr_t)g[REG_R9]);
    putkv("CRASH r10=", (uintptr_t)g[REG_R10]);
    putkv("CRASH r11=", (uintptr_t)g[REG_R11]);
    putkv("CRASH r12=", (uintptr_t)g[REG_R12]);
    putkv("CRASH r13=", (uintptr_t)g[REG_R13]);
    putkv("CRASH r14=", r14);
    putkv("CRASH r15=", r15);
    putkv("CRASH rip=", rip);

    /* First pass: address space layout. */
    scan_maps(rsp);

    /* The faulting instruction, byte for byte. */
    if (range_readable(rip, 16))
        putbytes("INSN", rip, 16);
    else
        puts_("INSN-UNREADABLE\n");

    /* The object each of r14/r15 points at. */
    dump_object("R14", r14);
    dump_object("R15", r15);

    /* The top of the stack, including the return address of the faulting
     * frame. */
    if (range_readable(rsp, 0x80))
        putbytes("STACKTOP", rsp, 0x80);

    dump_maps();

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
