/*
 * HttpRequest.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#include "object.h"
#include "HttpRequest.h"
#include "parse.h"
#include "HttpCollection.h"
#include "FormData.h"
#include "URLSearchParams.h"
#include "Headers.h"
#include "HttpMessage.h"
#include "MemoryStream.h"
#include "Buffer.h"
#include "ifs/json.h"
#include "ifs/msgpack.h"
#include "ifs/TLSSocket.h"

namespace fibjs {

result_t HttpRequest_base::_new(obj_ptr<HttpRequest_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new HttpRequest();
    return 0;
}

result_t HttpRequest::Options::from_opts(exlib::string default_method, v8::Local<v8::Object> opts,
    bool urlEncoded_default, bool strict)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    result_t hr;

    method = default_method;
    GetConfigValue(opts, "method", method, true);

    JSValue v = opts->Get(context, isolate->NewString("body", 4));
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    // Validate: GET and HEAD must not have a body (Fetch API strict mode)
    bool has_body = !v->IsUndefined() && !v->IsNull();
    if (strict && has_body) {
        exlib::string m_upper = method;
        for (char& c : m_upper)
            c = toupper((unsigned char)c);
        if (m_upper == "GET" || m_upper == "HEAD")
            return CHECK_ERROR(Runtime::setError(kTypeError, "Request: GET/HEAD requests cannot have a body"));
    }

    hr = GetConfigValue(opts, "headers", headers);
    if (hr == CALL_E_PARAMNOTOPTIONAL)
        headers = new Headers();
    else if (hr < 0)
        return hr;

    if (has_body) {
        hr = body_to_stream(isolate, v, body, headers.get(), urlEncoded_default);
        if (hr < 0 && hr != CALL_RETURN_NULL)
            return hr;
    } else if (!(v = opts->Get(context, isolate->NewString("json", 4)))->IsUndefined()) {
        body = new MemoryStream();
        exlib::string s;
        hr = json_base::encode(v, s);
        if (hr < 0)
            return hr;
        obj_ptr<Buffer_base> buf = new Buffer(s.c_str(), s.length());
        bool wr;
        body->cc_write(buf, wr);
        Variant ct;
        if (headers->first("Content-Type", ct) == CALL_RETURN_NULL)
            headers->set("Content-Type", "application/json");
    } else if (!(v = opts->Get(context, isolate->NewString("pack", 4)))->IsUndefined()) {
        body = new MemoryStream();
        obj_ptr<Buffer_base> buf;
        hr = msgpack_base::encode(v, buf);
        if (hr < 0)
            return hr;
        bool wr;
        body->cc_write(buf, wr);
        Variant ct;
        if (headers->first("Content-Type", ct) == CALL_RETURN_NULL)
            headers->set("Content-Type", "application/msgpack");
    }

    bool ka;
    hr = GetConfigValue(opts, "keepAlive", ka);
    if (hr == 0) {
        keepAlive = ka;
        has_keepAlive = true;
    } else if (hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    GetConfigValue(opts, "redirect", redirect, true);

    // Parse signal option; ignore type mismatch or missing key
    {
        result_t sig_hr = GetConfigValue(opts, "signal", signal);
        if (sig_hr < 0 && sig_hr != CALL_E_PARAMNOTOPTIONAL)
            signal = nullptr; // wrong type - treat as no signal
    }

    {
        bool s;
        result_t s_hr = GetConfigValue(opts, "streaming", s);
        if (s_hr == 0)
            streaming = s;
    }

    return 0;
}

result_t HttpRequest::Options::resolve_url(exlib::string url, v8::Local<v8::Object> opts)
{
    obj_ptr<Url> base = new Url();
    result_t hr = base->parse(url);
    if (hr < 0)
        return hr;

    obj_ptr<Url> override = new Url();
    override->format(opts);

    obj_ptr<UrlObject_base> resolved;
    hr = base->resolve(override->href(), resolved);
    if (hr < 0)
        return hr;

    u = resolved.As<Url>();
    return 0;
}

result_t HttpRequest::Options::apply_from_request(HttpRequest_base* req)
{
    // Parse URL from request's address
    exlib::string url;
    req->get_url(url);
    u = new Url();
    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;

    // Body fallback: use request's body if opts didn't provide one
    if (!body) {
        obj_ptr<Stream_base> req_body_stream;
        if (req->get_body(req_body_stream) == 0 && req_body_stream) {
            body = SeekableStream_base::getInstance(req_body_stream);
            if (!body)
                return CHECK_ERROR(Runtime::setError("HttpRequest: request body must be seekable."));
        }
    }

    // Headers: request's headers as base, opts headers override
    obj_ptr<Headers_base> req_hdrs;
    req->get_headers(req_hdrs);
    if (req_hdrs) {
        Headers* req_h = static_cast<Headers*>(req_hdrs.get());
        obj_ptr<Headers> merged = new Headers();
        merged->m_map.insert(merged->m_map.end(), req_h->m_map.begin(), req_h->m_map.end());
        if (headers) {
            Headers* opt_h = static_cast<Headers*>(headers.get());
            for (auto& kv : opt_h->m_map)
                merged->set(kv.first, kv.second.string());
        }
        headers = merged;
    }

    return 0;
}

void HttpRequest::set_options(const Options& o)
{
    set_method(o.method);
    if (o.u)
        set_address(o.u->href());
    if (o.headers)
        appendHeader(o.headers.get());
    if (o.body)
        set_body(o.body);
    if (o.has_keepAlive)
        set_keepAlive(o.keepAlive);
}

result_t HttpRequest_base::_new(exlib::string url, v8::Local<v8::Object> options,
    obj_ptr<HttpRequest_base>& retVal, v8::Local<v8::Object> This)
{
    HttpRequest::Options o;
    result_t hr = o.from_opts("GET", options, false, true);
    if (hr < 0)
        return hr;

    obj_ptr<HttpRequest> req = new HttpRequest();
    req->set_address(url);
    req->set_options(o);

    retVal = req;
    return 0;
}

result_t HttpRequest_base::_new(HttpRequest_base* request, v8::Local<v8::Object> options,
    obj_ptr<HttpRequest_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<Message_base> cloned;
    result_t hr = request->clone(cloned);
    if (hr < 0)
        return hr;

    obj_ptr<HttpRequest> req = static_cast<HttpRequest*>(cloned.get());

    exlib::string cur_method;
    req->get_method(cur_method);

    HttpRequest::Options o;
    hr = o.from_opts(cur_method, options, false, true);
    if (hr < 0)
        return hr;

    req->set_options(o);

    retVal = req;
    return 0;
}

result_t HttpRequest::get_protocol(exlib::string& retVal)
{
    return m_message->get_protocol(retVal);
}

result_t HttpRequest::set_protocol(exlib::string newVal)
{
    return m_message->set_protocol(newVal);
}

result_t HttpRequest::get_headers(obj_ptr<Headers_base>& retVal)
{
    return m_message->get_headers(retVal);
}

result_t HttpRequest::get_body(obj_ptr<Stream_base>& retVal)
{
    return m_message->get_body(retVal);
}

result_t HttpRequest::set_body(Stream_base* newVal)
{
    return m_message->set_body(newVal);
}

result_t HttpRequest::get_bodyUsed(bool& retVal)
{
    return m_message->get_bodyUsed(retVal);
}

result_t HttpRequest::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    return m_message->read(bytes, retVal, ac);
}

result_t HttpRequest::readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    return m_message->readAll(retVal, ac);
}

result_t HttpRequest::write(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
{
    return m_message->write(data, retVal, ac);
}

result_t HttpRequest::text(exlib::string data, exlib::string& retVal, AsyncEvent* ac)
{
    return m_message->text(data, retVal, ac);
}

result_t HttpRequest::text(exlib::string& retVal, AsyncEvent* ac)
{
    return m_message->text(retVal, ac);
}

result_t HttpRequest::arrayBuffer(std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac)
{
    return m_message->arrayBuffer(retVal, ac);
}

result_t HttpRequest::json(v8::Local<v8::Value> data, Variant& retVal, AsyncEvent* ac)
{
    return m_message->json(data, retVal, ac);
}

result_t HttpRequest::json(Variant& retVal, AsyncEvent* ac)
{
    return m_message->json(retVal, ac);
}

result_t HttpRequest::pack(v8::Local<v8::Value> data, Variant& retVal, AsyncEvent* ac)
{
    return m_message->pack(data, retVal, ac);
}

result_t HttpRequest::pack(Variant& retVal, AsyncEvent* ac)
{
    return m_message->pack(retVal, ac);
}

result_t HttpRequest::blob(exlib::string type, obj_ptr<Blob_base>& retVal, AsyncEvent* ac)
{
    return m_message->blob(type, retVal, ac);
}

result_t HttpRequest::bytes(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    return m_message->bytes(retVal, ac);
}

result_t HttpRequest::get_length(int64_t& retVal)
{
    return m_message->get_length(retVal);
}

result_t HttpRequest::get_keepAlive(bool& retVal)
{
    return m_message->get_keepAlive(retVal);
}

result_t HttpRequest::set_keepAlive(bool newVal)
{
    return m_message->set_keepAlive(newVal);
}

result_t HttpRequest::get_upgrade(bool& retVal)
{
    return m_message->get_upgrade(retVal);
}

result_t HttpRequest::set_upgrade(bool newVal)
{
    return m_message->set_upgrade(newVal);
}

result_t HttpRequest::get_maxHeadersCount(int32_t& retVal)
{
    return m_message->get_maxHeadersCount(retVal);
}

result_t HttpRequest::set_maxHeadersCount(int32_t newVal)
{
    return m_message->set_maxHeadersCount(newVal);
}

result_t HttpRequest::get_maxHeaderSize(int32_t& retVal)
{
    return m_message->get_maxHeaderSize(retVal);
}

result_t HttpRequest::set_maxHeaderSize(int32_t newVal)
{
    return m_message->set_maxHeaderSize(newVal);
}

result_t HttpRequest::get_maxChunkSize(int32_t& retVal)
{
    return m_message->get_maxChunkSize(retVal);
}

result_t HttpRequest::set_maxChunkSize(int32_t newVal)
{
    return m_message->set_maxChunkSize(newVal);
}

result_t HttpRequest::get_maxBodySize(int32_t& retVal)
{
    return m_message->get_maxBodySize(retVal);
}

result_t HttpRequest::set_maxBodySize(int32_t newVal)
{
    return m_message->set_maxBodySize(newVal);
}

result_t HttpRequest::get_socket(obj_ptr<Stream_base>& retVal)
{
    return m_message->get_socket(retVal);
}

result_t HttpRequest::hasHeader(exlib::string name, bool& retVal)
{
    return m_message->hasHeader(name, retVal);
}

result_t HttpRequest::firstHeader(exlib::string name, exlib::string& retVal)
{
    return m_message->firstHeader(name, retVal);
}

result_t HttpRequest::allHeader(exlib::string name, obj_ptr<NObject>& retVal)
{
    return m_message->allHeader(name, retVal);
}

result_t HttpRequest::appendHeader(v8::Local<v8::Object> map)
{
    return m_message->appendHeader(map);
}

result_t HttpRequest::appendHeader(Headers_base* headers)
{
    Headers* hdrs = static_cast<Headers*>(headers);
    for (int32_t i = 0; i < (int32_t)hdrs->m_map.size(); i++) {
        auto& it = hdrs->m_map[i];
        _appendHeader(it.first, it.second.string());
    }
    return 0;
}

result_t HttpRequest::appendHeader(exlib::string name, exlib::string value)
{
    return m_message->appendHeader(name, value);
}

result_t HttpRequest::appendHeader(exlib::string name, v8::Local<v8::Array> values)
{
    return m_message->appendHeader(name, values);
}

result_t HttpRequest::setHeader(v8::Local<v8::Object> map)
{
    return m_message->setHeader(map);
}

result_t HttpRequest::setHeader(Headers_base* headers)
{
    return m_message->setHeader(headers);
}

result_t HttpRequest::setHeader(exlib::string name, exlib::string value)
{
    return m_message->setHeader(name, value);
}

result_t HttpRequest::setHeader(exlib::string name, v8::Local<v8::Array> values)
{
    return m_message->setHeader(name, values);
}

result_t HttpRequest::removeHeader(exlib::string name)
{
    return m_message->removeHeader(name);
}

result_t HttpRequest::getHeader(exlib::string name, v8::Local<v8::Value>& retVal)
{
    return m_message->getHeader(name, retVal);
}

result_t HttpRequest::getHeaders(obj_ptr<NObject>& retVal)
{
    return m_message->getHeaders(retVal);
}

result_t HttpRequest::get_headersSent(bool& retVal)
{
    return m_message->get_headersSent(retVal);
}

result_t HttpRequest::onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func)
{
    return m_message->onEventChange(type, ev, func);
}

result_t HttpRequest::get_sent(bool& retVal)
{
    return m_message->get_sent(retVal);
}

result_t HttpRequest::get_value(exlib::string& retVal)
{
    return m_message->get_value(retVal);
}

result_t HttpRequest::set_value(exlib::string newVal)
{
    return m_message->set_value(newVal);
}

result_t HttpRequest::get_params(obj_ptr<NArray>& retVal)
{
    return m_message->get_params(retVal);
}

result_t HttpRequest::get_type(int32_t& retVal)
{
    return m_message->get_type(retVal);
}

result_t HttpRequest::set_type(int32_t newVal)
{
    return m_message->set_type(newVal);
}

result_t HttpRequest::get_lastError(exlib::string& retVal)
{
    return m_message->get_lastError(retVal);
}

result_t HttpRequest::set_lastError(exlib::string newVal)
{
    return m_message->set_lastError(newVal);
}

result_t HttpRequest::end(int32_t& retVal, AsyncEvent* ac)
{
    return m_message->end(retVal, ac);
}

result_t HttpRequest::end(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
{
    return m_message->end(data, retVal, ac);
}

result_t HttpRequest::end(Buffer_base* data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
{
    return m_message->end(data, encoding, retVal, ac);
}

result_t HttpRequest::end(exlib::string data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
{
    return m_message->end(data, encoding, retVal, ac);
}

result_t HttpRequest::isEnded(bool& retVal)
{
    if (m_response) {
        m_response->isEnded(retVal);
        if (retVal)
            return 0;
    }

    return m_message->isEnded(retVal);
}

result_t HttpRequest::clear()
{
    if (m_response)
        m_response->clear();

    m_message->clear();

    m_method.assign("GET", 3);
    m_address.assign("/", 1);
    m_queryString.clear();

    if (m_cookies)
        m_cookies.Release();

    m_query.Release();
    m_form.Release();

    return 0;
}

result_t HttpRequest::sendTo(Stream_base* stm, v8::Local<v8::Object> options, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string strCommand = m_method;
    exlib::string strProtocol;

    strCommand.append(1, ' ');
    strCommand.append(m_address);
    if (!m_queryString.empty()) {
        strCommand.append(1, '?');
        strCommand.append(m_queryString);
    }

    get_protocol(strProtocol);
    strCommand.append(1, ' ');
    strCommand.append(strProtocol);

    return m_message->send(stm, strCommand, ac);
}

result_t HttpRequest::readFrom(Stream_base* stm, v8::Local<v8::Object> options, AsyncEvent* ac)
{
    class asyncReadFrom : public AsyncState {
    public:
        asyncReadFrom(HttpRequest* pThis, BufferedStream_base* stm,
            AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
        {
            next(begin);
        }

        ON_STATE(asyncReadFrom, begin)
        {
            return m_stm->readLine(m_pThis->m_message->m_maxHeaderSize, m_strLine, next(command));
        }

        ON_STATE(asyncReadFrom, command)
        {
            if (n == CALL_RETURN_NULL)
                return CHECK_ERROR(CALL_E_CLOSED);

            _parser p(m_strLine);
            result_t hr;

            if (!p.getWord(m_pThis->m_method))
                return CHECK_ERROR(Runtime::setError("HttpRequest: bad method."));

            p.skipSpace();

            exlib::string& addr = m_pThis->m_address;

            if (!p.getWord(addr, '?'))
                return CHECK_ERROR(Runtime::setError("HttpRequest: bad address."));

            if (!qstricmp(addr.c_str(), "http://", 7)) {
                const char* p = qstrchr(addr.c_str() + 7, '/');
                if (p)
                    m_pThis->m_message->set_value(p);
            } else
                m_pThis->m_message->set_value(addr);

            if (p.want('?'))
                p.getWord(m_pThis->m_queryString);

            p.skipSpace();

            if (p.end())
                return CHECK_ERROR(Runtime::setError("HttpRequest: bad protocol version."));

            hr = m_pThis->set_protocol(p.now());
            if (hr < 0)
                return hr;

            return m_pThis->m_message->readFrom(m_stm, next());
        }

    public:
        obj_ptr<HttpRequest> m_pThis;
        obj_ptr<BufferedStream_base> m_stm;
        exlib::string m_strLine;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<BufferedStream_base> _stm = BufferedStream_base::getInstance(stm);
    if (!_stm)
        return CHECK_ERROR(Runtime::setError("HttpRequest: only accept BufferedStream object."));

    return (new asyncReadFrom(this, _stm, ac))->post(0);
}

result_t HttpRequest::get_method(exlib::string& retVal)
{
    retVal = m_method;
    return 0;
}

result_t HttpRequest::get_stream(obj_ptr<Stream_base>& retVal)
{
    return m_message->get_stream(retVal);
}

result_t HttpRequest::set_method(exlib::string newVal)
{
    m_method = newVal;
    return 0;
}

result_t HttpRequest::get_address(exlib::string& retVal)
{
    retVal = m_address;
    return 0;
}

result_t HttpRequest::set_address(exlib::string newVal)
{
    m_address = newVal;
    return 0;
}

result_t HttpRequest::get_url(exlib::string& retVal)
{
    retVal = m_address;

    if (!m_queryString.empty()) {
        retVal.append(1, '?');
        retVal.append(m_queryString);
    }

    return 0;
}

result_t HttpRequest::set_url(exlib::string newVal)
{
    size_t pos = newVal.find('?');
    if (pos != exlib::string::npos) {
        m_address = newVal.substr(0, pos);
        m_queryString = newVal.substr(pos + 1);
    } else {
        m_address = newVal;
        m_queryString.clear();
    }

    return 0;
}

result_t HttpRequest::get_href(exlib::string& retVal)
{
    exlib::string protocol;
    if (firstHeader("X-Forwarded-Proto", protocol) == CALL_RETURN_NULL || protocol.empty()) {
        obj_ptr<Stream_base> socket;
        if (m_message->get_socket(socket) == 0 && socket) {
            if (TLSSocket_base::getInstance(socket))
                protocol = "https";
            else
                protocol = "http";
        } else {
            protocol = "http";
        }
    }

    exlib::string host;
    if (firstHeader("Host", host) == CALL_RETURN_NULL || host.empty())
        host = "localhost";

    retVal = protocol;
    retVal.append("://");
    retVal.append(host);
    retVal.append(m_address);

    if (!m_queryString.empty()) {
        retVal.append(1, '?');
        retVal.append(m_queryString);
    }

    return 0;
}

result_t HttpRequest::get_queryString(exlib::string& retVal)
{
    retVal = m_queryString;
    return 0;
}

result_t HttpRequest::set_queryString(exlib::string newVal)
{
    m_queryString = newVal;
    return 0;
}

result_t HttpRequest::get_response(obj_ptr<HttpResponse_base>& retVal)
{
    if (!m_response)
        m_response = new HttpResponse();

    retVal = m_response;
    return 0;
}

result_t HttpRequest::get_cookies(obj_ptr<HttpCollection_base>& retVal)
{
    if (!m_cookies) {
        exlib::string strCookie;
        obj_ptr<HttpCollection> c = new HttpCollection();

        firstHeader("cookie", strCookie);

        c->parseCookie(strCookie);
        m_cookies = c;
    }

    retVal = m_cookies;
    return 0;
}

result_t HttpRequest::get_form(obj_ptr<FormData_base>& retVal)
{
    if (m_form == NULL) {
        int64_t len = 0;

        get_length(len);
        if (len == 0)
            m_form = new FormData();
        else {
            exlib::string strType;
            bool bUpload = false;

            if (firstHeader("Content-Type", strType) == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("HttpRequest: Content-Type is missing."));

            if (!qstricmp(strType.c_str(), "multipart/form-data;", 20))
                bUpload = true;
            else if (qstricmp(strType.c_str(),
                         "application/x-www-form-urlencoded", 33))
                return CHECK_ERROR(Runtime::setError("HttpRequest: unknown form format: " + strType));

            obj_ptr<Buffer_base> buf;
            obj_ptr<Stream_base> _body_stream;

            if (get_body(_body_stream) == CALL_RETURN_NULL || !_body_stream)
                return CHECK_ERROR(Runtime::setError("HttpRequest: body is empty."));
            obj_ptr<SeekableStream_base> _body = SeekableStream_base::getInstance(_body_stream);
            if (!_body)
                return CHECK_ERROR(Runtime::setError("HttpRequest: body must be seekable."));
            _body->rewind();
            result_t hr = _body->cc_readBuffer((int32_t)len, buf);
            if (hr < 0)
                return hr;

            if (bUpload) {
                obj_ptr<FormData> col = new FormData();
                col->parseMultipart(buf, strType.c_str());
                m_form = col;
            } else {
                exlib::string strForm;
                buf->toString(strForm);

                obj_ptr<FormData> c = new FormData();
                c->parse(strForm);
                m_form = c;
            }
        }
    }

    retVal = m_form;

    return 0;
}

result_t HttpRequest::get_query(obj_ptr<URLSearchParams_base>& retVal)
{
    if (m_query == NULL) {
        obj_ptr<URLSearchParams> c = new URLSearchParams();
        c->parse(m_queryString);
        m_query = c;
    }

    retVal = m_query;
    return 0;
}

result_t HttpRequest::clone(obj_ptr<Message_base>& retVal)
{
    obj_ptr<HttpRequest> req = new HttpRequest();

    // Copy HttpMessage properties
    m_message->copyTo(req->m_message);

    // Copy HttpRequest specific properties
    req->m_method = m_method;
    req->m_address = m_address;
    req->m_queryString = m_queryString;

    // Note: m_cookies, m_query, m_form are lazily parsed,
    // they will be recreated on demand from headers/body

    retVal = req;
    return 0;
}

result_t HttpRequest::resume(obj_ptr<Message_base>& retVal)
{
    obj_ptr<Stream_base> body;
    if (m_message->get_body(body) == 0 && body) {
        obj_ptr<Stream_base> r;
        body->resume(r);
    }
    retVal = this;
    return 0;
}

result_t HttpRequest::pause(obj_ptr<Message_base>& retVal)
{
    obj_ptr<Stream_base> body;
    if (m_message->get_body(body) == 0 && body) {
        obj_ptr<Stream_base> r;
        body->pause(r);
    }
    retVal = this;
    return 0;
}

result_t HttpRequest::unpipe(Stream_base* destination)
{
    return m_message->unpipe(destination);
}

} /* namespace fibjs */
