/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _DatagramPacket_base_H_
#define _DatagramPacket_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;

class DatagramPacket_base : public object_base
{
    DECLARE_CLASS(DatagramPacket_base);

public:
    // DatagramPacket_base
    virtual result_t get_data(obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t get_address(exlib::string& retVal) = 0;
    virtual result_t get_port(int32_t& retVal) = 0;

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
    static void s_get_address(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_port(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

#include "Buffer.h"

namespace fibjs
{
    inline ClassInfo& DatagramPacket_base::class_info()
    {
        static ClassData::ClassProperty s_property[] = 
        {
            {"data", s_get_data, block_set, false},
            {"address", s_get_address, block_set, false},
            {"port", s_get_port, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "DatagramPacket", s__new, NULL, 
            0, NULL, 0, NULL, 3, s_property, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void DatagramPacket_base::s_get_data(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<Buffer_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(DatagramPacket_base);

        hr = pInst->get_data(vr);

        METHOD_RETURN();
    }

    inline void DatagramPacket_base::s_get_address(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(DatagramPacket_base);

        hr = pInst->get_address(vr);

        METHOD_RETURN();
    }

    inline void DatagramPacket_base::s_get_port(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(DatagramPacket_base);

        hr = pInst->get_port(vr);

        METHOD_RETURN();
    }

}

#endif

