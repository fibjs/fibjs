/*
 * pty_spawn.c
 *
 *  Created on: May 27, 2021
 *      Author: lion
 */

#ifndef _WIN32

#include <sys/wait.h>
#if defined(__APPLE__)
#include <util.h>
#else
#include <pty.h>
#endif

#include <stdlib.h>
#include <unistd.h>

#if defined(__APPLE__) && !TARGET_OS_IPHONE
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#else
extern char** environ;
#endif

#include "uv/src/uv-common.h"

#if defined(_AIX) || defined(__APPLE__) || defined(__DragonFly__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__linux__) || defined(__OpenBSD__) || defined(__NetBSD__)
#define uv__cloexec uv__cloexec_ioctl
#define uv__nonblock uv__nonblock_ioctl
#else
#define uv__cloexec uv__cloexec_fcntl
#define uv__nonblock uv__nonblock_fcntl
#endif

#define SAVE_ERRNO(block)         \
    do {                          \
        int _saved_errno = errno; \
        do {                      \
            block;                \
        } while (0);              \
        errno = _saved_errno;     \
    } while (0)

int uv__cloexec_ioctl(int fd, int set);
int uv__cloexec_fcntl(int fd, int set);
int uv__nonblock_ioctl(int fd, int set);
int uv__nonblock_fcntl(int fd, int set);
int uv__close(int fd); /* preserves errno */
int uv__close_nocheckstdio(int fd);
int uv__make_pipe(int fds[2], int flags);

static void uv__chld(uv_signal_t* handle, int signum)
{
    uv_process_t* process;
    uv_loop_t* loop;
    int exit_status;
    int term_signal;
    int status;
    pid_t pid;
    QUEUE pending;
    QUEUE* q;
    QUEUE* h;

    assert(signum == SIGCHLD);

    QUEUE_INIT(&pending);
    loop = handle->loop;

    h = &loop->process_handles;
    q = QUEUE_HEAD(h);
    while (q != h) {
        process = QUEUE_DATA(q, uv_process_t, queue);
        q = QUEUE_NEXT(q);

        do
            pid = waitpid(process->pid, &status, WNOHANG);
        while (pid == -1 && errno == EINTR);

        if (pid == 0)
            continue;

        if (pid == -1) {
            if (errno != ECHILD)
                abort();
            continue;
        }

        process->status = status;
        QUEUE_REMOVE(&process->queue);
        QUEUE_INSERT_TAIL(&pending, &process->queue);
    }

    h = &pending;
    q = QUEUE_HEAD(h);
    while (q != h) {
        process = QUEUE_DATA(q, uv_process_t, queue);
        q = QUEUE_NEXT(q);

        QUEUE_REMOVE(&process->queue);
        QUEUE_INIT(&process->queue);
        uv__handle_stop(process);

        if (process->exit_cb == NULL)
            continue;

        exit_status = 0;
        if (WIFEXITED(process->status))
            exit_status = WEXITSTATUS(process->status);

        term_signal = 0;
        if (WIFSIGNALED(process->status))
            term_signal = WTERMSIG(process->status);

        process->exit_cb(process, exit_status, term_signal);
    }
    assert(QUEUE_EMPTY(&pending));
}

static int uv__make_socketpair(int fds[2])
{
#if defined(__FreeBSD__) || defined(__linux__)
    if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, fds))
        return UV__ERR(errno);

    return 0;
#else
    int err;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds))
        return UV__ERR(errno);

    err = uv__cloexec(fds[0], 1);
    if (err == 0)
        err = uv__cloexec(fds[1], 1);

    if (err != 0) {
        uv__close(fds[0]);
        uv__close(fds[1]);
        return UV__ERR(errno);
    }

    return 0;
#endif
}

static void uv__write_int(int fd, int val)
{
    ssize_t n;

    do
        n = write(fd, &val, sizeof(val));
    while (n == -1 && errno == EINTR);

    if (n == -1 && errno == EPIPE)
        return; /* parent process has quit */

    assert(n == sizeof(val));
}

static void uv__process_child_init(const uv_process_options_t* options,
    int stdio_count,
    int (*pipes)[2],
    int error_fd)
{
    sigset_t set;
    int close_fd;
    int use_fd;
    int err;
    int fd;
    int n;

    if (options->flags & UV_PROCESS_DETACHED)
        setsid();

    for (fd = 0; fd < stdio_count; fd++) {
        close_fd = pipes[fd][0];
        use_fd = pipes[fd][1];

        if (fd == use_fd)
            uv__cloexec_fcntl(use_fd, 0);
        else
            fd = dup2(use_fd, fd);

        if (fd == -1) {
            uv__write_int(error_fd, UV__ERR(errno));
            _exit(127);
        }

        if (fd <= 2)
            uv__nonblock_fcntl(fd, 0);

        if (close_fd >= stdio_count)
            uv__close(close_fd);
    }

    for (fd = 0; fd < stdio_count; fd++) {
        use_fd = pipes[fd][1];

        if (use_fd >= stdio_count)
            uv__close(use_fd);
    }

    if (options->cwd != NULL && chdir(options->cwd)) {
        uv__write_int(error_fd, UV__ERR(errno));
        _exit(127);
    }

    if (options->flags & (UV_PROCESS_SETUID | UV_PROCESS_SETGID)) {
        /* When dropping privileges from root, the `setgroups` call will
     * remove any extraneous groups. If we don't call this, then
     * even though our uid has dropped, we may still have groups
     * that enable us to do super-user things. This will fail if we
     * aren't root, so don't bother checking the return value, this
     * is just done as an optimistic privilege dropping function.
     */
        SAVE_ERRNO(setgroups(0, NULL));
    }

    if ((options->flags & UV_PROCESS_SETGID) && setgid(options->gid)) {
        uv__write_int(error_fd, UV__ERR(errno));
        _exit(127);
    }

    if ((options->flags & UV_PROCESS_SETUID) && setuid(options->uid)) {
        uv__write_int(error_fd, UV__ERR(errno));
        _exit(127);
    }

    if (options->env != NULL) {
        environ = options->env;
    }

    /* Reset signal disposition.  Use a hard-coded limit because NSIG
   * is not fixed on Linux: it's either 32, 34 or 64, depending on
   * whether RT signals are enabled.  We are not allowed to touch
   * RT signal handlers, glibc uses them internally.
   */
    for (n = 1; n < 32; n += 1) {
        if (n == SIGKILL || n == SIGSTOP)
            continue; /* Can't be changed. */

#if defined(__HAIKU__)
        if (n == SIGKILLTHR)
            continue; /* Can't be changed. */
#endif

        if (SIG_ERR != signal(n, SIG_DFL))
            continue;

        uv__write_int(error_fd, UV__ERR(errno));
        _exit(127);
    }

    /* Reset signal mask. */
    sigemptyset(&set);
    err = pthread_sigmask(SIG_SETMASK, &set, NULL);

    if (err != 0) {
        uv__write_int(error_fd, UV__ERR(err));
        _exit(127);
    }

    execvp(options->file, options->args);
    uv__write_int(error_fd, UV__ERR(errno));
    _exit(127);
}

int pty_spawn(uv_loop_t* loop, uv_process_t* process, const uv_process_options_t* options, int* terminalfd)
{
    int signal_pipe[2] = { -1, -1 };
    int pipes_storage[8][2];
    int(*pipes)[2];
    int stdio_count;
    ssize_t r;
    pid_t pid;
    int err;
    int exec_errorno;
    int i;
    int status;

    uv__handle_init(loop, (uv_handle_t*)process, UV_PROCESS);
    QUEUE_INIT(&process->queue);

    stdio_count = options->stdio_count;
    if (stdio_count < 3)
        stdio_count = 3;

    err = UV_ENOMEM;
    pipes = pipes_storage;

    for (i = 0; i < stdio_count; i++) {
        pipes[i][0] = -1;
        pipes[i][1] = i;
    }

    err = uv__make_pipe(signal_pipe, 0);
    if (err)
        return err;

    uv_signal_start(&loop->child_watcher, uv__chld, SIGCHLD);

    /* Acquire write lock to prevent opening new fds in worker threads */
    uv_rwlock_wrlock(&loop->cloexec_lock);
    // pid = fork();

    pid = forkpty(terminalfd, NULL, NULL, NULL);

    if (pid == -1) {
        err = UV__ERR(errno);
        uv_rwlock_wrunlock(&loop->cloexec_lock);
        uv__close(signal_pipe[0]);
        uv__close(signal_pipe[1]);
        return err;
    }

    if (pid == 0) {
        uv__process_child_init(options, stdio_count, pipes, signal_pipe[1]);
        abort();
    }

    /* Release lock in parent process */
    uv_rwlock_wrunlock(&loop->cloexec_lock);
    uv__close(signal_pipe[1]);

    process->status = 0;
    exec_errorno = 0;
    do
        r = read(signal_pipe[0], &exec_errorno, sizeof(exec_errorno));
    while (r == -1 && errno == EINTR);

    if (r == 0)
        ; /* okay, EOF */
    else if (r == sizeof(exec_errorno)) {
        do
            err = waitpid(pid, &status, 0); /* okay, read errorno */
        while (err == -1 && errno == EINTR);
        assert(err == pid);
    } else if (r == -1 && errno == EPIPE) {
        do
            err = waitpid(pid, &status, 0); /* okay, got EPIPE */
        while (err == -1 && errno == EINTR);
        assert(err == pid);
    } else
        abort();

    uv__close_nocheckstdio(signal_pipe[0]);

    /* Only activate this handle if exec() happened successfully */
    if (exec_errorno == 0) {
        QUEUE_INSERT_TAIL(&loop->process_handles, &process->queue);
        uv__handle_start(process);
    }

    process->pid = pid;
    process->exit_cb = options->exit_cb;

    return exec_errorno;
}

#endif