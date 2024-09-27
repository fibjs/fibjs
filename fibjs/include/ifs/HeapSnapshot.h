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

class HeapGraphNode_base;

class HeapSnapshot_base : public object_base {
    DECLARE_CLASS(HeapSnapshot_base);

public:
    // HeapSnapshot_base
    virtual result_t diff(HeapSnapshot_base* before, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t getNodeById(int32_t id, obj_ptr<HeapGraphNode_base>& retVal) = 0;
    virtual result_t save(exlib::string fname, AsyncEvent* ac) = 0;
    virtual result_t get_time(date_t& retVal) = 0;
    virtual result_t get_root(obj_ptr<HeapGraphNode_base>& retVal) = 0;
    virtual result_t get_nodes(obj_ptr<NArray>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_diff(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getNodeById(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_save(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_time(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_root(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_nodes(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(HeapSnapshot_base, save, exlib::string);
};
}

#include "ifs/HeapGraphNode.h"

namespace fibjs {
inline ClassInfo& HeapSnapshot_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "diff", s_diff, false, ClassData::ASYNC_SYNC },
        { "getNodeById", s_getNodeById, false, ClassData::ASYNC_SYNC },
        { "save", s_save, false, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "time", s_get_time, block_set, false },
        { "root", s_get_root, block_set, false },
        { "nodes", s_get_nodes, block_set, false }
    };

    static ClassData s_cd = {
        "HeapSnapshot", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void HeapSnapshot_base::s_diff(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(HeapSnapshot_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<HeapSnapshot_base>, 0);

    hr = pInst->diff(v0, vr);

    METHOD_RETURN();
}

inline void HeapSnapshot_base::s_getNodeById(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HeapGraphNode_base> vr;

    METHOD_INSTANCE(HeapSnapshot_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->getNodeById(v0, vr);

    METHOD_RETURN();
}

inline void HeapSnapshot_base::s_save(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(HeapSnapshot_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_save(v0, cb, args);
    else
        hr = pInst->ac_save(v0);

    METHOD_VOID();
}

inline void HeapSnapshot_base::s_get_time(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_INSTANCE(HeapSnapshot_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_time(vr);

    METHOD_RETURN();
}

inline void HeapSnapshot_base::s_get_root(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HeapGraphNode_base> vr;

    METHOD_INSTANCE(HeapSnapshot_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_root(vr);

    METHOD_RETURN();
}

inline void HeapSnapshot_base::s_get_nodes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(HeapSnapshot_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_nodes(vr);

    METHOD_RETURN();
}
}
