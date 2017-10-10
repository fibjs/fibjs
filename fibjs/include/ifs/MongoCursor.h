/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _MongoCursor_base_H_
#define _MongoCursor_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class MongoCursor_base : public object_base {
    DECLARE_CLASS(MongoCursor_base);

public:
    // MongoCursor_base
    virtual result_t skip(int32_t num, obj_ptr<MongoCursor_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t limit(int32_t size, obj_ptr<MongoCursor_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t sort(v8::Local<v8::Object> opts, obj_ptr<MongoCursor_base>& retVal) = 0;
    virtual result_t hasNext(bool& retVal) = 0;
    virtual result_t next(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t count(bool applySkipLimit, int32_t& retVal) = 0;
    virtual result_t size(int32_t& retVal) = 0;
    virtual result_t forEach(v8::Local<v8::Function> func) = 0;
    virtual result_t map(v8::Local<v8::Function> func, v8::Local<v8::Array>& retVal) = 0;
    virtual result_t toArray(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t hint(v8::Local<v8::Object> opts, obj_ptr<MongoCursor_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_skip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_limit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sort(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_hasNext(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_next(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_count(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_size(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_forEach(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_map(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toArray(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_hint(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE2(MongoCursor_base, skip, int32_t, obj_ptr<MongoCursor_base>);
    ASYNC_MEMBERVALUE2(MongoCursor_base, limit, int32_t, obj_ptr<MongoCursor_base>);
};
}

namespace fibjs {
inline ClassInfo& MongoCursor_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "skip", s_skip, false },
        { "skipSync", s_skip, false },
        { "limit", s_limit, false },
        { "limitSync", s_limit, false },
        { "sort", s_sort, false },
        { "hasNext", s_hasNext, false },
        { "next", s_next, false },
        { "count", s_count, false },
        { "size", s_size, false },
        { "forEach", s_forEach, false },
        { "map", s_map, false },
        { "toArray", s_toArray, false },
        { "hint", s_hint, false }
    };

    static ClassData s_cd = {
        "MongoCursor", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void MongoCursor_base::s_skip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MongoCursor_base> vr;

    METHOD_INSTANCE(MongoCursor_base);
    METHOD_ENTER("MongoCursor.skip");

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_skip(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_skip(v0, vr);

    METHOD_RETURN();
}

inline void MongoCursor_base::s_limit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MongoCursor_base> vr;

    METHOD_INSTANCE(MongoCursor_base);
    METHOD_ENTER("MongoCursor.limit");

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_limit(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_limit(v0, vr);

    METHOD_RETURN();
}

inline void MongoCursor_base::s_sort(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MongoCursor_base> vr;

    METHOD_INSTANCE(MongoCursor_base);
    METHOD_ENTER("MongoCursor.sort");

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->sort(v0, vr);

    METHOD_RETURN();
}

inline void MongoCursor_base::s_hasNext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(MongoCursor_base);
    METHOD_ENTER("MongoCursor.hasNext");

    METHOD_OVER(0, 0);

    hr = pInst->hasNext(vr);

    METHOD_RETURN();
}

inline void MongoCursor_base::s_next(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(MongoCursor_base);
    METHOD_ENTER("MongoCursor.next");

    METHOD_OVER(0, 0);

    hr = pInst->next(vr);

    METHOD_RETURN();
}

inline void MongoCursor_base::s_count(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(MongoCursor_base);
    METHOD_ENTER("MongoCursor.count");

    METHOD_OVER(1, 0);

    OPT_ARG(bool, 0, false);

    hr = pInst->count(v0, vr);

    METHOD_RETURN();
}

inline void MongoCursor_base::s_size(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(MongoCursor_base);
    METHOD_ENTER("MongoCursor.size");

    METHOD_OVER(0, 0);

    hr = pInst->size(vr);

    METHOD_RETURN();
}

inline void MongoCursor_base::s_forEach(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MongoCursor_base);
    METHOD_ENTER("MongoCursor.forEach");

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->forEach(v0);

    METHOD_VOID();
}

inline void MongoCursor_base::s_map(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(MongoCursor_base);
    METHOD_ENTER("MongoCursor.map");

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->map(v0, vr);

    METHOD_RETURN();
}

inline void MongoCursor_base::s_toArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(MongoCursor_base);
    METHOD_ENTER("MongoCursor.toArray");

    METHOD_OVER(0, 0);

    hr = pInst->toArray(vr);

    METHOD_RETURN();
}

inline void MongoCursor_base::s_hint(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MongoCursor_base> vr;

    METHOD_INSTANCE(MongoCursor_base);
    METHOD_ENTER("MongoCursor.hint");

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->hint(v0, vr);

    METHOD_RETURN();
}
}

#endif
