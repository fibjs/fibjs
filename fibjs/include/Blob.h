#pragma once

#include "ifs/Blob.h"
#include "ifs/File.h"
#include "ifs/Buffer.h"

namespace fibjs {

class Buffer_base;

class BlobImpl {
public:
    result_t initialize(v8::Local<v8::Array> blobParts, v8::Local<v8::Object> options);

public:
    // Blob_base
    result_t get_type(exlib::string& retVal);
    result_t get_size(int32_t& retVal);
    result_t clone(BlobImpl& retVal);
    result_t slice(int32_t start, int32_t end, exlib::string contentType, BlobImpl& retVal);
    result_t slice(int32_t start, int32_t end, exlib::string contentType, obj_ptr<Blob_base>& retVal);
    result_t text(exlib::string& retVal, AsyncEvent* ac);
    result_t arrayBuffer(v8::Local<v8::ArrayBuffer>& retVal, AsyncEvent* ac);

public:
    void init(Buffer_base* buffer, const exlib::string& type)
    {
        m_buffer = buffer;
        m_type = type;
    }

    // Get underlying buffer for direct access (avoid memory copy)
    Buffer_base* getBuffer() const
    {
        return m_buffer;
    }

private:
    obj_ptr<Buffer_base> m_buffer;
    exlib::string m_type;
};

class Blob : public Blob_base {
private:
    obj_ptr<Buffer_base> m_buffer;
    exlib::string m_type;

public:
    // Blob_base
    virtual result_t get_type(exlib::string& retVal)
    {
        return m_impl.get_type(retVal);
    }

    virtual result_t get_size(int32_t& retVal)
    {
        return m_impl.get_size(retVal);
    }

    virtual result_t slice(int32_t start, int32_t end, exlib::string contentType, obj_ptr<Blob_base>& retVal)
    {
        return m_impl.slice(start, end, contentType, retVal);
    }

    virtual result_t text(exlib::string& retVal, AsyncEvent* ac)
    {
        return m_impl.text(retVal, ac);
    }

    virtual result_t arrayBuffer(v8::Local<v8::ArrayBuffer>& retVal, AsyncEvent* ac)
    {
        return m_impl.arrayBuffer(retVal, ac);
    }

public:
    BlobImpl m_impl;
};

class File : public File_base {
public:
    // Blob_base
    virtual result_t get_type(exlib::string& retVal)
    {
        return m_impl.get_type(retVal);
    }

    virtual result_t get_size(int32_t& retVal)
    {
        return m_impl.get_size(retVal);
    }

    virtual result_t slice(int32_t start, int32_t end, exlib::string contentType, obj_ptr<Blob_base>& retVal)
    {
        return m_impl.slice(start, end, contentType, retVal);
    }

    virtual result_t text(exlib::string& retVal, AsyncEvent* ac)
    {
        return m_impl.text(retVal, ac);
    }

    virtual result_t arrayBuffer(v8::Local<v8::ArrayBuffer>& retVal, AsyncEvent* ac)
    {
        return m_impl.arrayBuffer(retVal, ac);
    }

public:
    // File_base
    virtual result_t get_name(exlib::string& retVal)
    {
        retVal = m_name;
        return 0;
    }

    virtual result_t get_lastModified(double& retVal)
    {
        retVal = m_lastModified;
        return 0;
    }

public:
    BlobImpl m_impl;
    exlib::string m_name;
    double m_lastModified = 0;
};

}
