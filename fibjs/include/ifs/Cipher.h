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
#include <boost/preprocessor.hpp>

namespace fibjs {

class Buffer_base;
class Options_Cipher_setAAD_options;

class Cipher_base : public object_base {
    DECLARE_CLASS(Cipher_base);

public:
    using Union_setAuthTag_buffer = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_setAAD_buffer = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_update_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;

public:
    // Cipher_base
    virtual result_t setAuthTag(Union_setAuthTag_buffer buffer, exlib::string encoding, obj_ptr<Cipher_base>& retVal) = 0;
    virtual result_t getAuthTag(obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t setAAD(Union_setAAD_buffer buffer, Options_Cipher_setAAD_options* options, obj_ptr<Cipher_base>& retVal) = 0;
    virtual result_t setAutoPadding(bool autoPadding, obj_ptr<Cipher_base>& retVal) = 0;
    virtual result_t update(Union_update_data data, exlib::string inputEncoding, exlib::string outputEncoding, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t final(exlib::string outputEncoding, v8::Local<v8::Value>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<Cipher_base>& retVal)
    { return result_t::type_mismatch(); }

public:
    static void s_setAuthTag(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getAuthTag(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setAAD(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setAutoPadding(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_update(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_final(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
class Options_Cipher_setAAD_options : public obj_base {
public:
    LOAD_OPTIONS(Options_Cipher_setAAD_options, (encoding)(plaintextLength));

public:
    std::optional<exlib::string> encoding = "utf8";
    std::optional<int32_t> plaintextLength = -1;
};

inline ClassInfo& Cipher_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "setAuthTag", s_setAuthTag, false, ClassData::ASYNC_SYNC },
        { "getAuthTag", s_getAuthTag, false, ClassData::ASYNC_SYNC },
        { "setAAD", s_setAAD, false, ClassData::ASYNC_SYNC },
        { "setAutoPadding", s_setAutoPadding, false, ClassData::ASYNC_SYNC },
        { "update", s_update, false, ClassData::ASYNC_SYNC },
        { "final", s_final, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "Cipher", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Cipher_base::s_setAuthTag(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Cipher_base> vr;

    METHOD_INSTANCE(Cipher_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "buffer");

    ARG(Union_setAuthTag_buffer, 0);
    OPT_ARG_NAMED(exlib::string, 1, "utf8", "encoding");

    hr = pInst->setAuthTag(v0, v1, vr);

    METHOD_RETURN();
}

inline void Cipher_base::s_getAuthTag(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(Cipher_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getAuthTag(vr);

    METHOD_RETURN();
}

inline void Cipher_base::s_setAAD(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Cipher_base> vr;

    METHOD_INSTANCE(Cipher_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "buffer");

    ARG(Union_setAAD_buffer, 0);
    OPT_ARG_NAMED(obj_ptr<Options_Cipher_setAAD_options>, 1, new Options_Cipher_setAAD_options(), "options");

    hr = pInst->setAAD(v0, (Options_Cipher_setAAD_options*)v1, vr);

    METHOD_RETURN();
}

inline void Cipher_base::s_setAutoPadding(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Cipher_base> vr;

    METHOD_INSTANCE(Cipher_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG_NAMED(bool, 0, true, "autoPadding");

    hr = pInst->setAutoPadding(v0, vr);

    METHOD_RETURN();
}

inline void Cipher_base::s_update(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(Cipher_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 1, "data");

    ARG(Union_update_data, 0);
    OPT_ARG_NAMED(exlib::string, 1, "utf8", "inputEncoding");
    OPT_ARG_NAMED(exlib::string, 2, "buffer", "outputEncoding");

    hr = pInst->update(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void Cipher_base::s_final(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(Cipher_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG_NAMED(exlib::string, 0, "buffer", "outputEncoding");

    hr = pInst->final(v0, vr);

    METHOD_RETURN();
}
}
