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
#include "ifs/Stream.h"

namespace fibjs {

class Stream_base;

class TTYOutputStream_base : public Stream_base {
    DECLARE_CLASS(TTYOutputStream_base);

public:
    // TTYOutputStream_base
    virtual result_t get_isTTY(bool& retVal) = 0;
    virtual result_t get_columns(int32_t& retVal) = 0;
    virtual result_t get_rows(int32_t& retVal) = 0;
    virtual result_t clearLine(int32_t dir) = 0;
    virtual result_t clearScreenDown() = 0;
    virtual result_t cursorTo(int32_t x, int32_t y, AsyncEvent* ac) = 0;
    virtual result_t moveCursor(int32_t dx, int32_t dy, AsyncEvent* ac) = 0;
    virtual result_t getWindowSize(obj_ptr<NArray>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_isTTY(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_columns(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_rows(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clearLine(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clearScreenDown(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_cursorTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_moveCursor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getWindowSize(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER2(TTYOutputStream_base, cursorTo, int32_t, int32_t);
    ASYNC_MEMBER2(TTYOutputStream_base, moveCursor, int32_t, int32_t);
};
}

namespace fibjs {
inline ClassInfo& TTYOutputStream_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "clearLine", s_clearLine, false, ClassData::ASYNC_SYNC },
        { "clearScreenDown", s_clearScreenDown, false, ClassData::ASYNC_SYNC },
        { "cursorTo", s_cursorTo, false, ClassData::ASYNC_ASYNC },
        { "moveCursor", s_moveCursor, false, ClassData::ASYNC_ASYNC },
        { "getWindowSize", s_getWindowSize, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "isTTY", s_get_isTTY, block_set, false },
        { "columns", s_get_columns, block_set, false },
        { "rows", s_get_rows, block_set, false }
    };

    static ClassData s_cd = {
        "TTYOutputStream", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &Stream_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void TTYOutputStream_base::s_get_isTTY(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(TTYOutputStream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_isTTY(vr);

    METHOD_RETURN();
}

inline void TTYOutputStream_base::s_get_columns(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(TTYOutputStream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_columns(vr);

    METHOD_RETURN();
}

inline void TTYOutputStream_base::s_get_rows(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(TTYOutputStream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_rows(vr);

    METHOD_RETURN();
}

inline void TTYOutputStream_base::s_clearLine(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(TTYOutputStream_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->clearLine(v0);

    METHOD_VOID();
}

inline void TTYOutputStream_base::s_clearScreenDown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(TTYOutputStream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->clearScreenDown();

    METHOD_VOID();
}

inline void TTYOutputStream_base::s_cursorTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(TTYOutputStream_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, -1);

    if (!cb.IsEmpty())
        hr = pInst->acb_cursorTo(v0, v1, cb, args);
    else
        hr = pInst->ac_cursorTo(v0, v1);

    METHOD_VOID();
}

inline void TTYOutputStream_base::s_moveCursor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(TTYOutputStream_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);

    if (!cb.IsEmpty())
        hr = pInst->acb_moveCursor(v0, v1, cb, args);
    else
        hr = pInst->ac_moveCursor(v0, v1);

    METHOD_VOID();
}

inline void TTYOutputStream_base::s_getWindowSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(TTYOutputStream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getWindowSize(vr);

    METHOD_RETURN();
}
}
