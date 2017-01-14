/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _XmlCharacterData_base_H_
#define _XmlCharacterData_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "XmlNode.h"

namespace fibjs
{

class XmlNode_base;

class XmlCharacterData_base : public XmlNode_base
{
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

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_data(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_data(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_substringData(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_appendData(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_insertData(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_deleteData(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_replaceData(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}


namespace fibjs
{
    inline ClassInfo& XmlCharacterData_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"substringData", s_substringData, false},
            {"appendData", s_appendData, false},
            {"insertData", s_insertData, false},
            {"deleteData", s_deleteData, false},
            {"replaceData", s_replaceData, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"data", s_get_data, s_set_data, false},
            {"length", s_get_length, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "XmlCharacterData", s__new, NULL, 
            5, s_method, 0, NULL, 2, s_property, NULL, NULL,
            &XmlNode_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void XmlCharacterData_base::s_get_data(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        METHOD_INSTANCE(XmlCharacterData_base);
        PROPERTY_ENTER();

        hr = pInst->get_data(vr);

        METHOD_RETURN();
    }

    inline void XmlCharacterData_base::s_set_data(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(XmlCharacterData_base);
        PROPERTY_ENTER();

        PROPERTY_VAL(exlib::string);
        hr = pInst->set_data(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void XmlCharacterData_base::s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        METHOD_INSTANCE(XmlCharacterData_base);
        PROPERTY_ENTER();

        hr = pInst->get_length(vr);

        METHOD_RETURN();
    }

    inline void XmlCharacterData_base::s_substringData(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        exlib::string vr;

        METHOD_INSTANCE(XmlCharacterData_base);
        METHOD_ENTER(2, 2);

        ARG(int32_t, 0);
        ARG(int32_t, 1);

        hr = pInst->substringData(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void XmlCharacterData_base::s_appendData(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(XmlCharacterData_base);
        METHOD_ENTER(1, 1);

        ARG(exlib::string, 0);

        hr = pInst->appendData(v0);

        METHOD_VOID();
    }

    inline void XmlCharacterData_base::s_insertData(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(XmlCharacterData_base);
        METHOD_ENTER(2, 2);

        ARG(int32_t, 0);
        ARG(exlib::string, 1);

        hr = pInst->insertData(v0, v1);

        METHOD_VOID();
    }

    inline void XmlCharacterData_base::s_deleteData(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(XmlCharacterData_base);
        METHOD_ENTER(2, 2);

        ARG(int32_t, 0);
        ARG(int32_t, 1);

        hr = pInst->deleteData(v0, v1);

        METHOD_VOID();
    }

    inline void XmlCharacterData_base::s_replaceData(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(XmlCharacterData_base);
        METHOD_ENTER(3, 3);

        ARG(int32_t, 0);
        ARG(int32_t, 1);
        ARG(exlib::string, 2);

        hr = pInst->replaceData(v0, v1, v2);

        METHOD_VOID();
    }

}

#endif

