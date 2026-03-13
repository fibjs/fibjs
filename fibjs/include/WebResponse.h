/*
 * WebResponse.h
 *
 *  Web Fetch API standard Response implementation.
 */

#pragma once

#include "ifs/WebResponse.h"
#include "HttpResponse.h"
#include "Headers.h"
#include "Buffer.h"
#include "Blob.h"
#include "MemoryStream.h"

namespace fibjs {

class WebResponse : public WebResponse_base {
public:
    WebResponse()
        : m_status(200)
        , m_statusText("OK")
        , m_ok(true)
        , m_redirected(false)
        , m_type("basic")
        , m_bodyUsed(false)
        , m_isError(false)
    {
        m_headers = new Headers();
    }

public:
    // WebResponse_base
    virtual result_t get_status(int32_t& retVal);
    virtual result_t get_statusText(exlib::string& retVal);
    virtual result_t get_ok(bool& retVal);
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t get_url(exlib::string& retVal);
    virtual result_t get_redirected(bool& retVal);
    virtual result_t get_bodyUsed(bool& retVal);
    virtual result_t get_headers(obj_ptr<Headers_base>& retVal);
    virtual result_t get_body(obj_ptr<Stream_base>& retVal);
    virtual result_t text(exlib::string& retVal, AsyncEvent* ac);
    virtual result_t json(Variant& retVal, AsyncEvent* ac);
    virtual result_t arrayBuffer(std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac);
    virtual result_t blob(obj_ptr<Blob_base>& retVal, AsyncEvent* ac);
    virtual result_t bytes(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t clone(obj_ptr<WebResponse_base>& retVal);
    static result_t json(v8::Local<v8::Value> data, v8::Local<v8::Object> options, obj_ptr<WebResponse_base>& retVal);
    static result_t redirect(exlib::string url, int32_t status, obj_ptr<WebResponse_base>& retVal);
    static result_t error(obj_ptr<WebResponse_base>& retVal);

public:
    // Initialize from an HttpResponse (used by http.fetch())
    result_t initFromHttpResponse(HttpResponse_base* resp, const exlib::string& url, bool redirected);

    // Initialize from a body value + options (used by constructor)
    result_t initFromBody(Isolate* isolate, v8::Local<v8::Value> body, v8::Local<v8::Object> options);

private:
    // Read body into buffer and mark bodyUsed; returns error if already used
    result_t consumeBody(obj_ptr<Buffer_base>& retVal);

    // Get content-type from headers
    exlib::string getContentType();

public:
    int32_t m_status;
    exlib::string m_statusText;
    bool m_ok;
    exlib::string m_url;
    bool m_redirected;
    exlib::string m_type;
    bool m_bodyUsed;
    bool m_isError;
    obj_ptr<Headers> m_headers;
    obj_ptr<SeekableStream_base> m_body; // MemoryStream holding body bytes
};

} /* namespace fibjs */
