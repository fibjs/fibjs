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
#include "ifs/XmlNode.h"

namespace fibjs {

class XmlNode_base;

class XmlCharacterData_base : public XmlNode_base {
    DECLARE_CLASS(XmlCharacterData_base);

public:
    // XmlCharacterData_base
    virtual result_t get_data(exlib::string& retVal) = 0;
    virtual result_t set_data(exlib::string newVal) = 0;
    virtual result_t get_length(int32_t& retVal) = 0;
    virtual result_t substringData(int32_t offset, int32_t count, exlib::string& retVal) = 0;
    virtual result_t appendData(exlib::string arg) = 0;
    virtual result_t insertData(int32_t offset, exlib::string arg) = 0;
    virtual result_t deleteData(int32_t offset, int32_t count) = 0;
    virtual result_t replaceData(int32_t offset, int32_t count, exlib::string arg) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_data(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_data(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_length(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_substringData(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_appendData(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_insertData(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_deleteData(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_replaceData(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& XmlCharacterData_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "substringData", s_substringData, false, ClassData::ASYNC_SYNC },
        { "appendData", s_appendData, false, ClassData::ASYNC_SYNC },
        { "insertData", s_insertData, false, ClassData::ASYNC_SYNC },
        { "deleteData", s_deleteData, false, ClassData::ASYNC_SYNC },
        { "replaceData", s_replaceData, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "data", s_get_data, s_set_data, false },
        { "length", s_get_length, block_set, false }
    };

    static ClassData s_cd = {
        "XmlCharacterData", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &XmlNode_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void XmlCharacterData_base::s_get_data(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlCharacterData_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_data(vr);

    METHOD_RETURN();
}

inline void XmlCharacterData_base::s_set_data(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlCharacterData_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_data(v0);

    METHOD_VOID();
}

inline void XmlCharacterData_base::s_get_length(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(XmlCharacterData_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_length(vr);

    METHOD_RETURN();
}

inline void XmlCharacterData_base::s_substringData(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlCharacterData_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);

    hr = pInst->substringData(v0, v1, vr);

    METHOD_RETURN();
}

inline void XmlCharacterData_base::s_appendData(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlCharacterData_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->appendData(v0);

    METHOD_VOID();
}

inline void XmlCharacterData_base::s_insertData(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlCharacterData_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);

    hr = pInst->insertData(v0, v1);

    METHOD_VOID();
}

inline void XmlCharacterData_base::s_deleteData(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlCharacterData_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);

    hr = pInst->deleteData(v0, v1);

    METHOD_VOID();
}

inline void XmlCharacterData_base::s_replaceData(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlCharacterData_base);
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(int32_t, 0);
    ARG(int32_t, 1);
    ARG(exlib::string, 2);

    hr = pInst->replaceData(v0, v1, v2);

    METHOD_VOID();
}
}
