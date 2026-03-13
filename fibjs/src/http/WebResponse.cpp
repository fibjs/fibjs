/*
 * WebResponse.cpp
 *
 *  Web Fetch API standard Response implementation.
 */

#include "object.h"
#include "WebResponse.h"
#include "HttpResponse.h"
#include "HttpMessage.h"
#include "Buffer.h"
#include "Blob.h"
#include "MemoryStream.h"
#include "ifs/json.h"

namespace fibjs {

// ─── Constructor (new Response(body?, options?)) ──────────────────────────────

result_t WebResponse_base::_new(v8::Local<v8::Value> body, v8::Local<v8::Object> options,
    obj_ptr<WebResponse_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<WebResponse> resp = new WebResponse();
    result_t hr = resp->initFromBody(Isolate::current(options), body, options);
    if (hr < 0)
        return hr;
    retVal = resp;
    return 0;
}

result_t WebResponse_base::_new(v8::Local<v8::Object> options,
    obj_ptr<WebResponse_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current(options);
    obj_ptr<WebResponse> resp = new WebResponse();
    v8::Local<v8::Value> nullBody = v8::Null(isolate->m_isolate);
    result_t hr = resp->initFromBody(isolate, nullBody, options);
    if (hr < 0)
        return hr;
    retVal = resp;
    return 0;
}

// ─── Internal initializers ────────────────────────────────────────────────────

result_t WebResponse::initFromHttpResponse(HttpResponse_base* resp,
    const exlib::string& url, bool redirected)
{
    m_url = url;
    m_redirected = redirected;
    m_type = "basic";

    // status / statusText
    resp->get_status(m_status);
    resp->get_statusMessage(m_statusText);
    m_ok = (m_status >= 200 && m_status <= 299);

    // Copy headers – incoming response headers are already lowercased by the HTTP parser.
    obj_ptr<Headers_base> srcHdrs;
    resp->get_headers(srcHdrs);
    Headers* src = (Headers*)srcHdrs.get();
    m_headers->m_map.insert(m_headers->m_map.end(), src->m_map.begin(), src->m_map.end());

    // body – MemoryStream is already filled after HttpClient::request()
    obj_ptr<SeekableStream_base> bodyStm;
    resp->get_body(bodyStm);
    m_body = bodyStm; // may be null for empty / HEAD responses

    m_bodyUsed = false;
    return 0;
}

result_t WebResponse::initFromBody(Isolate* isolate, v8::Local<v8::Value> body,
    v8::Local<v8::Object> options)
{
    // Parse options using ResponseOptions from HttpResponse
    obj_ptr<HttpResponse::ResponseOptions> opts;
    result_t hr = HttpResponse::ResponseOptions::load(options, opts);
    if (hr < 0)
        return hr;

    if (opts->status.has_value()) {
        m_status = opts->status.value();
        m_ok = (m_status >= 200 && m_status <= 299);
    }

    if (opts->statusText.has_value())
        m_statusText = opts->statusText.value();

    if (opts->headers.has_value()) {
        auto& headersVar = opts->headers.value();
        if (std::holds_alternative<v8::Local<v8::Object>>(headersVar)) {
            hr = m_headers->set(std::get<v8::Local<v8::Object>>(headersVar));
            if (hr < 0)
                return hr;
        } else {
            Headers* src = (Headers*)std::get<obj_ptr<Headers_base>>(headersVar).get();
            m_headers->m_map.insert(m_headers->m_map.end(), src->m_map.begin(), src->m_map.end());
        }
    }

    // Convert body → SeekableStream
    obj_ptr<SeekableStream_base> stm;
    hr = body_to_stream(isolate, body, stm, m_headers.get());
    if (hr < 0)
        return hr;

    m_body = stm;
    m_bodyUsed = false;
    return 0;
}

// ─── Body read helper ─────────────────────────────────────────────────────────

result_t WebResponse::consumeBody(obj_ptr<Buffer_base>& retVal)
{
    if (m_bodyUsed)
        return CHECK_ERROR(Runtime::setError("WebResponse: body already used"));

    m_bodyUsed = true;

    if (!m_body) {
        retVal = new Buffer("", 0);
        return 0;
    }

    m_body->rewind();
    result_t hr = m_body->cc_readAll(retVal);
    if (hr == CALL_RETURN_NULL)
        retVal = new Buffer("", 0);
    else if (hr < 0)
        return hr;

    return 0;
}

exlib::string WebResponse::getContentType()
{
    Variant varCt;
    exlib::string ct;
    if (m_headers->first("content-type", varCt) == 0)
        varCt.toString(ct);
    return ct;
}

// ─── Property accessors ───────────────────────────────────────────────────────

result_t WebResponse::get_status(int32_t& retVal)
{
    retVal = m_status;
    return 0;
}

result_t WebResponse::get_statusText(exlib::string& retVal)
{
    retVal = m_statusText;
    return 0;
}

result_t WebResponse::get_ok(bool& retVal)
{
    retVal = m_ok;
    return 0;
}

result_t WebResponse::get_type(exlib::string& retVal)
{
    retVal = m_type;
    return 0;
}

result_t WebResponse::get_url(exlib::string& retVal)
{
    retVal = m_url;
    return 0;
}

result_t WebResponse::get_redirected(bool& retVal)
{
    retVal = m_redirected;
    return 0;
}

result_t WebResponse::get_bodyUsed(bool& retVal)
{
    retVal = m_bodyUsed;
    return 0;
}

result_t WebResponse::get_headers(obj_ptr<Headers_base>& retVal)
{
    retVal = m_headers;
    return 0;
}

result_t WebResponse::get_body(obj_ptr<Stream_base>& retVal)
{
    if (!m_body || m_bodyUsed)
        return CALL_RETURN_NULL;

    int64_t sz = 0;
    m_body->size(sz);
    if (sz == 0)
        return CALL_RETURN_NULL;

    retVal = m_body;
    return 0;
}

// ─── Async body consumer methods ─────────────────────────────────────────────

result_t WebResponse::text(exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> data;
    result_t hr = consumeBody(data);
    if (hr < 0)
        return hr;

    return data->toString(retVal);
}

result_t WebResponse::json(Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> data;
    result_t hr = consumeBody(data);
    if (hr < 0)
        return hr;

    exlib::string str;
    data->toString(str);

    // Store as VT_JSON so V8 decoding happens back in the JS thread
    retVal.setJSON(str);
    return 0;
}

result_t WebResponse::arrayBuffer(std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> data;
    result_t hr = consumeBody(data);
    if (hr < 0)
        return hr;

    Buffer* buf = (Buffer*)data.get();
    int32_t bufSize = buf->length();
    const uint8_t* bufData = buf->data();

    std::shared_ptr<v8::BackingStore> store = NewBackingStore(bufSize);
    if (bufSize > 0 && store->Data() && bufData)
        memcpy(store->Data(), bufData, bufSize);

    retVal = std::move(store);
    return 0;
}

result_t WebResponse::blob(obj_ptr<Blob_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> data;
    result_t hr = consumeBody(data);
    if (hr < 0)
        return hr;

    exlib::string contentType = getContentType();
    // Strip parameters (e.g. charset) from content-type
    size_t pos = contentType.find(';');
    if (pos != exlib::string::npos)
        contentType = contentType.substr(0, pos);
    exlib::string trimmed;
    for (size_t i = 0; i < contentType.size(); i++)
        if (contentType[i] != ' ')
            trimmed += contentType[i];
    contentType = trimmed;

    retVal = new Blob(data, contentType);
    return 0;
}

result_t WebResponse::bytes(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return consumeBody(retVal);
}

// ─── clone() ─────────────────────────────────────────────────────────────────

result_t WebResponse::clone(obj_ptr<WebResponse_base>& retVal)
{
    if (m_bodyUsed)
        return CHECK_ERROR(Runtime::setError("WebResponse: cannot clone a used response"));

    obj_ptr<WebResponse> copy = new WebResponse();
    copy->m_status = m_status;
    copy->m_statusText = m_statusText;
    copy->m_ok = m_ok;
    copy->m_url = m_url;
    copy->m_redirected = m_redirected;
    copy->m_type = m_type;
    copy->m_bodyUsed = false;
    copy->m_isError = m_isError;

    // Clone headers
    obj_ptr<Headers> hdrs = new Headers();
    hdrs->m_map = m_headers->m_map;
    copy->m_headers = hdrs;

    // Clone body: read current body into a new MemoryStream
    if (m_body) {
        obj_ptr<Buffer_base> data;
        m_body->rewind();
        result_t hr = m_body->ac_readAll(data);
        if (hr != CALL_RETURN_NULL && hr < 0)
            return hr;

        if (hr != CALL_RETURN_NULL && data) {
            obj_ptr<MemoryStream> newBody = new MemoryStream();
            bool ok;
            newBody->ac_write(data, ok);
            newBody->rewind();
            copy->m_body = newBody;
        }
    }

    retVal = copy;
    return 0;
}

// ─── Static factory methods ───────────────────────────────────────────────────

result_t WebResponse_base::json(v8::Local<v8::Value> data, v8::Local<v8::Object> options,
    obj_ptr<WebResponse_base>& retVal)
{
    Isolate* isolate = Isolate::current(options);

    // Serialize data to JSON string
    exlib::string jsonStr;
    result_t hr = json_base::encode(data, jsonStr);
    if (hr < 0)
        return hr;

    obj_ptr<WebResponse> resp = new WebResponse();

    // Parse options
    obj_ptr<HttpResponse::ResponseOptions> opts;
    hr = HttpResponse::ResponseOptions::load(options, opts);
    if (hr < 0)
        return hr;

    if (opts->status.has_value()) {
        resp->m_status = opts->status.value();
        resp->m_ok = (resp->m_status >= 200 && resp->m_status <= 299);
    }
    if (opts->statusText.has_value())
        resp->m_statusText = opts->statusText.value();

    if (opts->headers.has_value()) {
        auto& headersVar = opts->headers.value();
        if (std::holds_alternative<v8::Local<v8::Object>>(headersVar)) {
            hr = resp->m_headers->set(std::get<v8::Local<v8::Object>>(headersVar));
            if (hr < 0)
                return hr;
        } else {
            Headers* src = (Headers*)std::get<obj_ptr<Headers_base>>(headersVar).get();
            resp->m_headers->m_map.insert(resp->m_headers->m_map.end(), src->m_map.begin(), src->m_map.end());
        }
    }

    // Set Content-Type if not already set
    Variant varCt;
    if (resp->m_headers->first("content-type", varCt) == CALL_RETURN_NULL)
        resp->m_headers->set(exlib::string("content-type"), Variant(exlib::string("application/json")));

    // Create body MemoryStream with JSON bytes
    obj_ptr<MemoryStream> stm = new MemoryStream();
    obj_ptr<Buffer_base> buf = new Buffer(jsonStr.c_str(), jsonStr.length());
    bool ok;
    stm->ac_write(buf, ok);
    stm->rewind();
    resp->m_body = stm;

    retVal = resp;
    return 0;
}

result_t WebResponse_base::redirect(exlib::string url, int32_t status,
    obj_ptr<WebResponse_base>& retVal)
{
    // Validate redirect status codes
    if (status != 301 && status != 302 && status != 303 && status != 307 && status != 308)
        return CHECK_ERROR(Runtime::setError("WebResponse.redirect: invalid redirect status"));

    obj_ptr<WebResponse> resp = new WebResponse();
    resp->m_status = status;
    resp->m_statusText = "";
    resp->m_ok = false;
    resp->m_type = "basic";

    resp->m_headers->set(exlib::string("location"), Variant(url));

    retVal = resp;
    return 0;
}

result_t WebResponse_base::error(obj_ptr<WebResponse_base>& retVal)
{
    obj_ptr<WebResponse> resp = new WebResponse();
    resp->m_status = 0;
    resp->m_statusText = "";
    resp->m_ok = false;
    resp->m_type = "error";
    resp->m_isError = true;
    resp->m_body = nullptr;

    retVal = resp;
    return 0;
}

} /* namespace fibjs */
