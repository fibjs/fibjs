/*
 * process_signal.h
 *
 * Internal helpers shared by the signal handler, the process exit paths and the
 * command runner (`fibjs <script>`), so that a signal is reported the way the
 * caller expects: a real signal death (128 + signum) instead of an arbitrary
 * exit code, and a wrapper that forwards the signal to the script it started.
 */

#ifndef FIBJS_PROCESS_SIGNAL_H
#define FIBJS_PROCESS_SIGNAL_H

#include "object.h"

namespace fibjs {

// Re-raise `signum` with the default disposition so that the parent shell
// observes a signal death (WIFSIGNALED / `$? == 128 + signum`). Returns false
// when the platform cannot re-raise it: the caller falls back to
// exit(128 + signum).
bool signal_reraise(int32_t signum);

// Forget the pending signal: a JS listener took care of it.
void process_signal_clear();

// Remember a termination signal before it is sent (process.kill on ourselves):
// the kernel may deliver it to another thread, which races with the end of the
// program and used to turn `process.kill(process.pid, "SIGTERM")` into exit(0).
void process_signal_note(int32_t signum);

// Terminate with the pending signal (no-op when there is none).
void process_signal_exit_pending();

// Same as process_signal_exit_pending() but called right before the natural
// exit of the main script, so a signal that raced with the end of the program
// wins over a clean exit(0) — "program ends" and "SIGTERM" used to produce a
// 0/1 coin flip.
void process_signal_reraise_pending();

// While the command runner waits for its child process, INT/TERM are forwarded
// to the living children instead of terminating fibjs; the exit status of the
// child then decides the final exit code (npm's signal-manager model).
void process_signal_forward_children(bool on);

// Implemented in ChildProcess.cpp (that file owns the registry of running
// children): forward `signum` to every child that is still alive.
int32_t child_process_signal_alive(int32_t signum);

} // namespace fibjs

#endif // FIBJS_PROCESS_SIGNAL_H
