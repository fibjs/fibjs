/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpUploadData_base_H_
#define _HttpUploadData_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class SeekableStream_base;

class HttpUploadData_base : public object_base {
    DECLARE_CLASS(HttpUploadData_base);

public:
    // HttpUploadData_base
    virtual result_t get_fileName(exlib::string& retVal) = 0;
    virtual result_t get_contentType(exlib::string& retVal) = 0;
    virtual result_t get_contentTransferEncoding(exlib::string& retVal) = 0;
    virtual result_t get_body(obj_ptr<SeekableStream_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_fileName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_contentType(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_contentTransferEncoding(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_body(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

#include "SeekableStream.h"

namespace fibjs {
inline ClassInfo& HttpUploadData_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "fileName", s_get_fileName, block_set, false },
        { "contentType", s_get_contentType, block_set, false },
        { "contentTransferEncoding", s_get_contentTransferEncoding, block_set, false },
        { "body", s_get_body, block_set, false }
    };

    static ClassData s_cd = {
        "HttpUploadData", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void HttpUploadData_base::s_get_fileName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpUploadData_base);
    PROPERTY_ENTER("HttpUploadData.fileName");

    hr = pInst->get_fileName(vr);

    METHOD_RETURN();
}

inline void HttpUploadData_base::s_get_contentType(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpUploadData_base);
    PROPERTY_ENTER("HttpUploadData.contentType");

    hr = pInst->get_contentType(vr);

    METHOD_RETURN();
}

inline void HttpUploadData_base::s_get_contentTransferEncoding(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpUploadData_base);
    PROPERTY_ENTER("HttpUploadData.contentTransferEncoding");

    hr = pInst->get_contentTransferEncoding(vr);

    METHOD_RETURN();
}

inline void HttpUploadData_base::s_get_body(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<SeekableStream_base> vr;

    METHOD_INSTANCE(HttpUploadData_base);
    PROPERTY_ENTER("HttpUploadData.body");

    hr = pInst->get_body(vr);

    METHOD_RETURN();
}
}

#endif
