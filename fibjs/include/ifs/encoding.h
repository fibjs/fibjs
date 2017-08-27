/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _encoding_base_H_
#define _encoding_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class base32_base;
class base64_base;
class base64vlq_base;
class hex_base;
class iconv_base;
class json_base;
class bson_base;

class encoding_base : public object_base {
    DECLARE_CLASS(encoding_base);

public:
    // encoding_base
    static result_t jsstr(exlib::string str, bool json, exlib::string& retVal);
    static result_t encodeURI(exlib::string url, exlib::string& retVal);
    static result_t encodeURIComponent(exlib::string url, exlib::string& retVal);
    static result_t decodeURI(exlib::string url, exlib::string& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_jsstr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_encodeURI(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_encodeURIComponent(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_decodeURI(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "base32.h"
#include "base64.h"
#include "base64vlq.h"
#include "hex.h"
#include "iconv.h"
#include "json.h"
#include "bson.h"

namespace fibjs {
inline ClassInfo& encoding_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "jsstr", s_jsstr, true },
        { "encodeURI", s_encodeURI, true },
        { "encodeURIComponent", s_encodeURIComponent, true },
        { "decodeURI", s_decodeURI, true }
    };

    static ClassData::ClassObject s_object[] = {
        { "base32", base32_base::class_info },
        { "base64", base64_base::class_info },
        { "base64vlq", base64vlq_base::class_info },
        { "hex", hex_base::class_info },
        { "iconv", iconv_base::class_info },
        { "json", json_base::class_info },
        { "bson", bson_base::class_info }
    };

    static ClassData s_cd = {
        "encoding", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void encoding_base::s_jsstr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(bool, 1, false);

    hr = jsstr(v0, v1, vr);

    METHOD_RETURN();
}

inline void encoding_base::s_encodeURI(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = encodeURI(v0, vr);

    METHOD_RETURN();
}

inline void encoding_base::s_encodeURIComponent(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = encodeURIComponent(v0, vr);

    METHOD_RETURN();
}

inline void encoding_base::s_decodeURI(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = decodeURI(v0, vr);

    METHOD_RETURN();
}
}

#endif
