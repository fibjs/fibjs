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
class Menu_base;

class MenuItem_base : public EventEmitter_base {
    DECLARE_CLASS(MenuItem_base);

public:
    // MenuItem_base
    virtual result_t get_id(exlib::string& retVal) = 0;
    virtual result_t set_id(exlib::string newVal) = 0;
    virtual result_t get_type(exlib::string& retVal) = 0;
    virtual result_t get_icon(exlib::string& retVal) = 0;
    virtual result_t set_icon(exlib::string newVal) = 0;
    virtual result_t get_label(exlib::string& retVal) = 0;
    virtual result_t set_label(exlib::string newVal) = 0;
    virtual result_t get_tooltip(exlib::string& retVal) = 0;
    virtual result_t set_tooltip(exlib::string newVal) = 0;
    virtual result_t get_enabled(bool& retVal) = 0;
    virtual result_t set_enabled(bool newVal) = 0;
    virtual result_t get_checked(bool& retVal) = 0;
    virtual result_t set_checked(bool newVal) = 0;
    virtual result_t get_submenu(obj_ptr<Menu_base>& retVal) = 0;
    virtual result_t get_onclick(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onclick(v8::Local<v8::Function> newVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_id(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_id(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_icon(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_icon(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_label(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_label(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_tooltip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_tooltip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_enabled(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_enabled(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_checked(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_checked(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_submenu(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onclick(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onclick(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Menu.h"

namespace fibjs {
inline ClassInfo& MenuItem_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "id", s_get_id, s_set_id, false },
        { "type", s_get_type, block_set, false },
        { "icon", s_get_icon, s_set_icon, false },
        { "label", s_get_label, s_set_label, false },
        { "tooltip", s_get_tooltip, s_set_tooltip, false },
        { "enabled", s_get_enabled, s_set_enabled, false },
        { "checked", s_get_checked, s_set_checked, false },
        { "submenu", s_get_submenu, block_set, false },
        { "onclick", s_get_onclick, s_set_onclick, false }
    };

    static ClassData s_cd = {
        "MenuItem", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void MenuItem_base::s_get_id(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_id(vr);

    METHOD_RETURN();
}

inline void MenuItem_base::s_set_id(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_id(v0);

    METHOD_VOID();
}

inline void MenuItem_base::s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void MenuItem_base::s_get_icon(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_icon(vr);

    METHOD_RETURN();
}

inline void MenuItem_base::s_set_icon(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_icon(v0);

    METHOD_VOID();
}

inline void MenuItem_base::s_get_label(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_label(vr);

    METHOD_RETURN();
}

inline void MenuItem_base::s_set_label(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_label(v0);

    METHOD_VOID();
}

inline void MenuItem_base::s_get_tooltip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_tooltip(vr);

    METHOD_RETURN();
}

inline void MenuItem_base::s_set_tooltip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_tooltip(v0);

    METHOD_VOID();
}

inline void MenuItem_base::s_get_enabled(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_enabled(vr);

    METHOD_RETURN();
}

inline void MenuItem_base::s_set_enabled(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = pInst->set_enabled(v0);

    METHOD_VOID();
}

inline void MenuItem_base::s_get_checked(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_checked(vr);

    METHOD_RETURN();
}

inline void MenuItem_base::s_set_checked(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = pInst->set_checked(v0);

    METHOD_VOID();
}

inline void MenuItem_base::s_get_submenu(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Menu_base> vr;

    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_submenu(vr);

    METHOD_RETURN();
}

inline void MenuItem_base::s_get_onclick(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onclick(vr);

    METHOD_RETURN();
}

inline void MenuItem_base::s_set_onclick(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(MenuItem_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onclick(v0);

    METHOD_VOID();
}
}
