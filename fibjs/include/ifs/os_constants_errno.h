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

class os_constants_errno_base : public object_base {
    DECLARE_CLASS(os_constants_errno_base);

public:
    enum {
        C_E2BIG = 7,
        C_EACCES = 13,
        C_EADDRINUSE = 98,
        C_EADDRNOTAVAIL = 99,
        C_EAFNOSUPPORT = 97,
        C_EAGAIN = 11,
        C_EALREADY = 114,
        C_EBADF = 9,
        C_EBADMSG = 74,
        C_EBUSY = 16,
        C_ECANCELED = 125,
        C_ECHILD = 10,
        C_ECONNABORTED = 103,
        C_ECONNREFUSED = 111,
        C_ECONNRESET = 104,
        C_EDEADLK = 35,
        C_EDESTADDRREQ = 89,
        C_EDOM = 33,
        C_EDQUOT = 122,
        C_EEXIST = 17,
        C_EFAULT = 14,
        C_EFBIG = 27,
        C_EHOSTUNREACH = 113,
        C_EIDRM = 43,
        C_EILSEQ = 84,
        C_EINPROGRESS = 115,
        C_EINTR = 4,
        C_EINVAL = 22,
        C_EIO = 5,
        C_EISCONN = 106,
        C_EISDIR = 21,
        C_ELOOP = 40,
        C_EMFILE = 24,
        C_EMLINK = 31,
        C_EMSGSIZE = 90,
        C_EMULTIHOP = 72,
        C_ENAMETOOLONG = 36,
        C_ENETDOWN = 100,
        C_ENETRESET = 102,
        C_ENETUNREACH = 101,
        C_ENFILE = 23,
        C_ENOBUFS = 105,
        C_ENODATA = 61,
        C_ENODEV = 19,
        C_ENOENT = 2,
        C_ENOEXEC = 8,
        C_ENOLCK = 37,
        C_ENOLINK = 67,
        C_ENOMEM = 12,
        C_ENOMSG = 42,
        C_ENOPROTOOPT = 92,
        C_ENOSPC = 28,
        C_ENOSR = 63,
        C_ENOSTR = 60,
        C_ENOSYS = 38,
        C_ENOTCONN = 107,
        C_ENOTDIR = 20,
        C_ENOTEMPTY = 39,
        C_ENOTSOCK = 88,
        C_ENOTSUP = 95,
        C_ENOTTY = 25,
        C_ENXIO = 6,
        C_EOPNOTSUPP = 95,
        C_EOVERFLOW = 75,
        C_EPERM = 1,
        C_EPIPE = 32,
        C_EPROTO = 71,
        C_EPROTONOSUPPORT = 93,
        C_EPROTOTYPE = 91,
        C_ERANGE = 34,
        C_EROFS = 30,
        C_ESPIPE = 29,
        C_ESRCH = 3,
        C_ESTALE = 116,
        C_ETIME = 62,
        C_ETIMEDOUT = 110,
        C_ETXTBSY = 26,
        C_EWOULDBLOCK = 11,
        C_EXDEV = 18
    };

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<os_constants_errno_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }
};
}

namespace fibjs {
inline ClassInfo& os_constants_errno_base::class_info()
{
    static ClassData::ClassConst s_const[] = {
        { "E2BIG", ClassData::CONST_Integer, { .intValue = C_E2BIG } },
        { "EACCES", ClassData::CONST_Integer, { .intValue = C_EACCES } },
        { "EADDRINUSE", ClassData::CONST_Integer, { .intValue = C_EADDRINUSE } },
        { "EADDRNOTAVAIL", ClassData::CONST_Integer, { .intValue = C_EADDRNOTAVAIL } },
        { "EAFNOSUPPORT", ClassData::CONST_Integer, { .intValue = C_EAFNOSUPPORT } },
        { "EAGAIN", ClassData::CONST_Integer, { .intValue = C_EAGAIN } },
        { "EALREADY", ClassData::CONST_Integer, { .intValue = C_EALREADY } },
        { "EBADF", ClassData::CONST_Integer, { .intValue = C_EBADF } },
        { "EBADMSG", ClassData::CONST_Integer, { .intValue = C_EBADMSG } },
        { "EBUSY", ClassData::CONST_Integer, { .intValue = C_EBUSY } },
        { "ECANCELED", ClassData::CONST_Integer, { .intValue = C_ECANCELED } },
        { "ECHILD", ClassData::CONST_Integer, { .intValue = C_ECHILD } },
        { "ECONNABORTED", ClassData::CONST_Integer, { .intValue = C_ECONNABORTED } },
        { "ECONNREFUSED", ClassData::CONST_Integer, { .intValue = C_ECONNREFUSED } },
        { "ECONNRESET", ClassData::CONST_Integer, { .intValue = C_ECONNRESET } },
        { "EDEADLK", ClassData::CONST_Integer, { .intValue = C_EDEADLK } },
        { "EDESTADDRREQ", ClassData::CONST_Integer, { .intValue = C_EDESTADDRREQ } },
        { "EDOM", ClassData::CONST_Integer, { .intValue = C_EDOM } },
        { "EDQUOT", ClassData::CONST_Integer, { .intValue = C_EDQUOT } },
        { "EEXIST", ClassData::CONST_Integer, { .intValue = C_EEXIST } },
        { "EFAULT", ClassData::CONST_Integer, { .intValue = C_EFAULT } },
        { "EFBIG", ClassData::CONST_Integer, { .intValue = C_EFBIG } },
        { "EHOSTUNREACH", ClassData::CONST_Integer, { .intValue = C_EHOSTUNREACH } },
        { "EIDRM", ClassData::CONST_Integer, { .intValue = C_EIDRM } },
        { "EILSEQ", ClassData::CONST_Integer, { .intValue = C_EILSEQ } },
        { "EINPROGRESS", ClassData::CONST_Integer, { .intValue = C_EINPROGRESS } },
        { "EINTR", ClassData::CONST_Integer, { .intValue = C_EINTR } },
        { "EINVAL", ClassData::CONST_Integer, { .intValue = C_EINVAL } },
        { "EIO", ClassData::CONST_Integer, { .intValue = C_EIO } },
        { "EISCONN", ClassData::CONST_Integer, { .intValue = C_EISCONN } },
        { "EISDIR", ClassData::CONST_Integer, { .intValue = C_EISDIR } },
        { "ELOOP", ClassData::CONST_Integer, { .intValue = C_ELOOP } },
        { "EMFILE", ClassData::CONST_Integer, { .intValue = C_EMFILE } },
        { "EMLINK", ClassData::CONST_Integer, { .intValue = C_EMLINK } },
        { "EMSGSIZE", ClassData::CONST_Integer, { .intValue = C_EMSGSIZE } },
        { "EMULTIHOP", ClassData::CONST_Integer, { .intValue = C_EMULTIHOP } },
        { "ENAMETOOLONG", ClassData::CONST_Integer, { .intValue = C_ENAMETOOLONG } },
        { "ENETDOWN", ClassData::CONST_Integer, { .intValue = C_ENETDOWN } },
        { "ENETRESET", ClassData::CONST_Integer, { .intValue = C_ENETRESET } },
        { "ENETUNREACH", ClassData::CONST_Integer, { .intValue = C_ENETUNREACH } },
        { "ENFILE", ClassData::CONST_Integer, { .intValue = C_ENFILE } },
        { "ENOBUFS", ClassData::CONST_Integer, { .intValue = C_ENOBUFS } },
        { "ENODATA", ClassData::CONST_Integer, { .intValue = C_ENODATA } },
        { "ENODEV", ClassData::CONST_Integer, { .intValue = C_ENODEV } },
        { "ENOENT", ClassData::CONST_Integer, { .intValue = C_ENOENT } },
        { "ENOEXEC", ClassData::CONST_Integer, { .intValue = C_ENOEXEC } },
        { "ENOLCK", ClassData::CONST_Integer, { .intValue = C_ENOLCK } },
        { "ENOLINK", ClassData::CONST_Integer, { .intValue = C_ENOLINK } },
        { "ENOMEM", ClassData::CONST_Integer, { .intValue = C_ENOMEM } },
        { "ENOMSG", ClassData::CONST_Integer, { .intValue = C_ENOMSG } },
        { "ENOPROTOOPT", ClassData::CONST_Integer, { .intValue = C_ENOPROTOOPT } },
        { "ENOSPC", ClassData::CONST_Integer, { .intValue = C_ENOSPC } },
        { "ENOSR", ClassData::CONST_Integer, { .intValue = C_ENOSR } },
        { "ENOSTR", ClassData::CONST_Integer, { .intValue = C_ENOSTR } },
        { "ENOSYS", ClassData::CONST_Integer, { .intValue = C_ENOSYS } },
        { "ENOTCONN", ClassData::CONST_Integer, { .intValue = C_ENOTCONN } },
        { "ENOTDIR", ClassData::CONST_Integer, { .intValue = C_ENOTDIR } },
        { "ENOTEMPTY", ClassData::CONST_Integer, { .intValue = C_ENOTEMPTY } },
        { "ENOTSOCK", ClassData::CONST_Integer, { .intValue = C_ENOTSOCK } },
        { "ENOTSUP", ClassData::CONST_Integer, { .intValue = C_ENOTSUP } },
        { "ENOTTY", ClassData::CONST_Integer, { .intValue = C_ENOTTY } },
        { "ENXIO", ClassData::CONST_Integer, { .intValue = C_ENXIO } },
        { "EOPNOTSUPP", ClassData::CONST_Integer, { .intValue = C_EOPNOTSUPP } },
        { "EOVERFLOW", ClassData::CONST_Integer, { .intValue = C_EOVERFLOW } },
        { "EPERM", ClassData::CONST_Integer, { .intValue = C_EPERM } },
        { "EPIPE", ClassData::CONST_Integer, { .intValue = C_EPIPE } },
        { "EPROTO", ClassData::CONST_Integer, { .intValue = C_EPROTO } },
        { "EPROTONOSUPPORT", ClassData::CONST_Integer, { .intValue = C_EPROTONOSUPPORT } },
        { "EPROTOTYPE", ClassData::CONST_Integer, { .intValue = C_EPROTOTYPE } },
        { "ERANGE", ClassData::CONST_Integer, { .intValue = C_ERANGE } },
        { "EROFS", ClassData::CONST_Integer, { .intValue = C_EROFS } },
        { "ESPIPE", ClassData::CONST_Integer, { .intValue = C_ESPIPE } },
        { "ESRCH", ClassData::CONST_Integer, { .intValue = C_ESRCH } },
        { "ESTALE", ClassData::CONST_Integer, { .intValue = C_ESTALE } },
        { "ETIME", ClassData::CONST_Integer, { .intValue = C_ETIME } },
        { "ETIMEDOUT", ClassData::CONST_Integer, { .intValue = C_ETIMEDOUT } },
        { "ETXTBSY", ClassData::CONST_Integer, { .intValue = C_ETXTBSY } },
        { "EWOULDBLOCK", ClassData::CONST_Integer, { .intValue = C_EWOULDBLOCK } },
        { "EXDEV", ClassData::CONST_Integer, { .intValue = C_EXDEV } }
    };

    static ClassData s_cd = {
        "os_constants_errno", true, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}
