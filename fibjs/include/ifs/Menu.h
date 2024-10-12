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
#include "ifs/EventEmitter.h"

namespace fibjs {

class EventEmitter_base;
class MenuItem_base;

class Menu_base : public EventEmitter_base {
    DECLARE_CLASS(Menu_base);

public:
    // Menu_base
    virtual result_t append(v8::Local<v8::Object> item) = 0;
    virtual result_t insert(int32_t pos, v8::Local<v8::Object> item) = 0;
    virtual result_t remove(int32_t pos) = 0;
    virtual result_t get_length(int32_t& retVal) = 0;
    virtual result_t getMenuItemById(exlib::string id, obj_ptr<MenuItem_base>& retVal) = 0;
    virtual result_t _indexed_getter(uint32_t index, obj_ptr<MenuItem_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_append(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_insert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_length(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getMenuItemById(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Intercepted i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

#include "ifs/MenuItem.h"

namespace fibjs {
inline ClassInfo& Menu_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "append", s_append, false, ClassData::ASYNC_SYNC },
        { "insert", s_insert, false, ClassData::ASYNC_SYNC },
        { "remove", s_remove, false, ClassData::ASYNC_SYNC },
        { "getMenuItemById", s_getMenuItemById, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "length", s_get_length, block_set, false }
    };

    static ClassData::ClassIndexed s_indexed = {
        i_IndexedGetter, i_IndexedSetter
    };

    static ClassData s_cd = {
        "Menu", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, &s_indexed, NULL,
        &EventEmitter_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Menu_base::s_append(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Menu_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->append(v0);

    METHOD_VOID();
}

inline void Menu_base::s_insert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Menu_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(v8::Local<v8::Object>, 1);

    hr = pInst->insert(v0, v1);

    METHOD_VOID();
}

inline void Menu_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Menu_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->remove(v0);

    METHOD_VOID();
}

inline void Menu_base::s_get_length(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Menu_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_length(vr);

    METHOD_RETURN();
}

inline void Menu_base::s_getMenuItemById(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<MenuItem_base> vr;

    METHOD_INSTANCE(Menu_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getMenuItemById(v0, vr);

    METHOD_RETURN();
}

inline v8::Intercepted Menu_base::i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<MenuItem_base> vr;

    NAMED_METHOD_INSTANCE(Menu_base);
    PROPERTY_ENTER();

    hr = pInst->_indexed_getter(index, vr);
    if (hr == CALL_RETURN_NULL)
        return v8::Intercepted::kNo;

    NAMED_METHOD_RETURN();
}
}
