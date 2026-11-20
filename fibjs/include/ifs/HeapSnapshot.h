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
    class DiffType_before : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("nodes"), GetReturnValue(isolate, nodes)).Check();
            retVal->Set(context, isolate->NewString("time"), GetReturnValue(isolate, time)).Check();
            retVal->Set(context, isolate->NewString("size_bytes"), GetReturnValue(isolate, size_bytes)).Check();
            retVal->Set(context, isolate->NewString("size"), GetReturnValue(isolate, size)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, nodes));
            args.push_back(GetReturnValue(isolate, time));
            args.push_back(GetReturnValue(isolate, size_bytes));
            args.push_back(GetReturnValue(isolate, size));
        }

    public:
        int32_t nodes;
        date_t time;
        int32_t size_bytes;
        exlib::string size;
    };
    class DiffType_after : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("nodes"), GetReturnValue(isolate, nodes)).Check();
            retVal->Set(context, isolate->NewString("time"), GetReturnValue(isolate, time)).Check();
            retVal->Set(context, isolate->NewString("size_bytes"), GetReturnValue(isolate, size_bytes)).Check();
            retVal->Set(context, isolate->NewString("size"), GetReturnValue(isolate, size)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, nodes));
            args.push_back(GetReturnValue(isolate, time));
            args.push_back(GetReturnValue(isolate, size_bytes));
            args.push_back(GetReturnValue(isolate, size));
        }

    public:
        int32_t nodes;
        date_t time;
        int32_t size_bytes;
        exlib::string size;
    };
    class DiffType_change : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("size_bytes"), GetReturnValue(isolate, size_bytes)).Check();
            retVal->Set(context, isolate->NewString("size"), GetReturnValue(isolate, size)).Check();
            retVal->Set(context, isolate->NewString("freed_nodes"), GetReturnValue(isolate, freed_nodes)).Check();
            retVal->Set(context, isolate->NewString("allocated_nodes"), GetReturnValue(isolate, allocated_nodes)).Check();
            retVal->Set(context, isolate->NewString("details"), GetReturnValue(isolate, details)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, size_bytes));
            args.push_back(GetReturnValue(isolate, size));
            args.push_back(GetReturnValue(isolate, freed_nodes));
            args.push_back(GetReturnValue(isolate, allocated_nodes));
            args.push_back(GetReturnValue(isolate, details));
        }

    public:
        int32_t size_bytes;
        exlib::string size;
        int32_t freed_nodes;
        int32_t allocated_nodes;
        Variant details;
    };
    class DiffType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("before"), GetReturnValue(isolate, before)).Check();
            retVal->Set(context, isolate->NewString("after"), GetReturnValue(isolate, after)).Check();
            retVal->Set(context, isolate->NewString("change"), GetReturnValue(isolate, change)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, before));
            args.push_back(GetReturnValue(isolate, after));
            args.push_back(GetReturnValue(isolate, change));
        }

    public:
        obj_ptr<DiffType_before> before;
        obj_ptr<DiffType_after> after;
        obj_ptr<DiffType_change> change;
    };

public:
    // HeapSnapshot_base
    virtual result_t diff(HeapSnapshot_base* before, obj_ptr<DiffType>& retVal) = 0;
    virtual result_t getNodeById(int32_t id, obj_ptr<HeapGraphNode_base>& retVal) = 0;
    virtual result_t save(exlib::string fname, AsyncHandle ac) = 0;
    virtual result_t get_time(date_t& retVal) = 0;
    virtual result_t get_root(obj_ptr<HeapGraphNode_base>& retVal) = 0;
    virtual result_t get_nodes(std::vector<obj_ptr<HeapGraphNode_base>>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<HeapSnapshot_base>& retVal)
    { return result_t::type_mismatch(); }

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
    obj_ptr<DiffType> vr;

    METHOD_INSTANCE(HeapSnapshot_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "before");

    ARG_NAMED(obj_ptr<HeapSnapshot_base>, 0, "before");

    hr = pInst->diff(v0.get(), vr);

    METHOD_RETURN();
}

inline void HeapSnapshot_base::s_getNodeById(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HeapGraphNode_base> vr;

    METHOD_INSTANCE(HeapSnapshot_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "id");

    ARG_NAMED(int32_t, 0, "id");

    hr = pInst->getNodeById(v0, vr);

    METHOD_RETURN();
}

inline void HeapSnapshot_base::s_save(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(HeapSnapshot_base);
    ASYNC_METHOD_ENTER("HeapSnapshot.save");

    METHOD_OVER_NAMED(1, 1, "fname");

    ARG_NAMED(exlib::string, 0, "fname");

    if (!cb.IsEmpty())
        hr = pInst->acb_save(v0, cb, args);
    else
        hr = pInst->ac_save(v0);

    ASYNC_METHOD_VOID();
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
    std::vector<obj_ptr<HeapGraphNode_base>> vr;

    METHOD_INSTANCE(HeapSnapshot_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_nodes(vr);

    METHOD_RETURN();
}
}
