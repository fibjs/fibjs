#pragma once

#include "ifs/Blob.h"

namespace fibjs {

class Buffer_base;

class Blob : public Blob_base {
private:
    obj_ptr<Buffer_base> m_buffer;
    exlib::string m_type;

public:
    Blob() : m_type("") {}

    result_t initialize(v8::Local<v8::Array> blobParts, v8::Local<v8::Object> options, obj_ptr<Blob_base>& retVal);

    // Blob_base implementation
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t get_size(int32_t& retVal);
    virtual result_t slice(int32_t start, int32_t end, exlib::string contentType, obj_ptr<Blob_base>& retVal);
    virtual result_t text(exlib::string& retVal, AsyncEvent* ac);
    virtual result_t arrayBuffer(v8::Local<v8::ArrayBuffer>& retVal, AsyncEvent* ac);
};

}
