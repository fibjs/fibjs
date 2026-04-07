/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class os_constants_signals_base : public object_base {
    DECLARE_CLASS(os_constants_signals_base);

public:
    enum {
        C_SIGHUP = 1,
        C_SIGINT = 2,
        C_SIGQUIT = 3,
        C_SIGILL = 4,
        C_SIGTRAP = 5,
        C_SIGABRT = 6,
        C_SIGIOT = 6,
        C_SIGBUS = 7,
        C_SIGFPE = 8,
        C_SIGKILL = 9,
        C_SIGUSR1 = 10,
        C_SIGSEGV = 11,
        C_SIGUSR2 = 12,
        C_SIGPIPE = 13,
        C_SIGALRM = 14,
        C_SIGTERM = 15,
        C_SIGSTKFLT = 16,
        C_SIGCHLD = 17,
        C_SIGCONT = 18,
        C_SIGSTOP = 19,
        C_SIGTSTP = 20,
        C_SIGTTIN = 21,
        C_SIGTTOU = 22,
        C_SIGURG = 23,
        C_SIGXCPU = 24,
        C_SIGXFSZ = 25,
        C_SIGVTALRM = 26,
        C_SIGPROF = 27,
        C_SIGWINCH = 28,
        C_SIGIO = 29,
        C_SIGPOLL = 29,
        C_SIGPWR = 30,
        C_SIGSYS = 31
    };

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<os_constants_signals_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }
};
}

namespace fibjs {
inline ClassInfo& os_constants_signals_base::class_info()
{
    static ClassData::ClassConst s_const[] = {
        { "SIGHUP", ClassData::CONST_Integer, { .intValue = C_SIGHUP } },
        { "SIGINT", ClassData::CONST_Integer, { .intValue = C_SIGINT } },
        { "SIGQUIT", ClassData::CONST_Integer, { .intValue = C_SIGQUIT } },
        { "SIGILL", ClassData::CONST_Integer, { .intValue = C_SIGILL } },
        { "SIGTRAP", ClassData::CONST_Integer, { .intValue = C_SIGTRAP } },
        { "SIGABRT", ClassData::CONST_Integer, { .intValue = C_SIGABRT } },
        { "SIGIOT", ClassData::CONST_Integer, { .intValue = C_SIGIOT } },
        { "SIGBUS", ClassData::CONST_Integer, { .intValue = C_SIGBUS } },
        { "SIGFPE", ClassData::CONST_Integer, { .intValue = C_SIGFPE } },
        { "SIGKILL", ClassData::CONST_Integer, { .intValue = C_SIGKILL } },
        { "SIGUSR1", ClassData::CONST_Integer, { .intValue = C_SIGUSR1 } },
        { "SIGSEGV", ClassData::CONST_Integer, { .intValue = C_SIGSEGV } },
        { "SIGUSR2", ClassData::CONST_Integer, { .intValue = C_SIGUSR2 } },
        { "SIGPIPE", ClassData::CONST_Integer, { .intValue = C_SIGPIPE } },
        { "SIGALRM", ClassData::CONST_Integer, { .intValue = C_SIGALRM } },
        { "SIGTERM", ClassData::CONST_Integer, { .intValue = C_SIGTERM } },
        { "SIGSTKFLT", ClassData::CONST_Integer, { .intValue = C_SIGSTKFLT } },
        { "SIGCHLD", ClassData::CONST_Integer, { .intValue = C_SIGCHLD } },
        { "SIGCONT", ClassData::CONST_Integer, { .intValue = C_SIGCONT } },
        { "SIGSTOP", ClassData::CONST_Integer, { .intValue = C_SIGSTOP } },
        { "SIGTSTP", ClassData::CONST_Integer, { .intValue = C_SIGTSTP } },
        { "SIGTTIN", ClassData::CONST_Integer, { .intValue = C_SIGTTIN } },
        { "SIGTTOU", ClassData::CONST_Integer, { .intValue = C_SIGTTOU } },
        { "SIGURG", ClassData::CONST_Integer, { .intValue = C_SIGURG } },
        { "SIGXCPU", ClassData::CONST_Integer, { .intValue = C_SIGXCPU } },
        { "SIGXFSZ", ClassData::CONST_Integer, { .intValue = C_SIGXFSZ } },
        { "SIGVTALRM", ClassData::CONST_Integer, { .intValue = C_SIGVTALRM } },
        { "SIGPROF", ClassData::CONST_Integer, { .intValue = C_SIGPROF } },
        { "SIGWINCH", ClassData::CONST_Integer, { .intValue = C_SIGWINCH } },
        { "SIGIO", ClassData::CONST_Integer, { .intValue = C_SIGIO } },
        { "SIGPOLL", ClassData::CONST_Integer, { .intValue = C_SIGPOLL } },
        { "SIGPWR", ClassData::CONST_Integer, { .intValue = C_SIGPWR } },
        { "SIGSYS", ClassData::CONST_Integer, { .intValue = C_SIGSYS } }
    };

    static ClassData s_cd = {
        "os_constants_signals", true, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}
