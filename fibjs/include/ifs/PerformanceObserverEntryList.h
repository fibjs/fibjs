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

class PerformanceObserverEntryList_base : public object_base {
    DECLARE_CLASS(PerformanceObserverEntryList_base);

public:
    // PerformanceObserverEntryList_base
    virtual result_t getEntries(obj_ptr<NArray>& retVal) = 0;
    virtual result_t getEntriesByName(exlib::string name, exlib::string entryType, obj_ptr<NArray>& retVal) = 0;
    virtual result_t getEntriesByType(exlib::string entryType, obj_ptr<NArray>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<PerformanceObserverEntryList_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_getEntries(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getEntriesByName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getEntriesByType(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& PerformanceObserverEntryList_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "getEntries", s_getEntries, false, ClassData::ASYNC_SYNC },
        { "getEntriesByName", s_getEntriesByName, false, ClassData::ASYNC_SYNC },
        { "getEntriesByType", s_getEntriesByType, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "PerformanceObserverEntryList", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void PerformanceObserverEntryList_base::s_getEntries(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(PerformanceObserverEntryList_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getEntries(vr);

    METHOD_RETURN();
}

inline void PerformanceObserverEntryList_base::s_getEntriesByName(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(PerformanceObserverEntryList_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = pInst->getEntriesByName(v0, v1, vr);

    METHOD_RETURN();
}

inline void PerformanceObserverEntryList_base::s_getEntriesByType(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(PerformanceObserverEntryList_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getEntriesByType(v0, vr);

    METHOD_RETURN();
}
}
