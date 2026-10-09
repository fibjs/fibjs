/*
 * HttpResponse.cpp
 *
 *  Created on: Aug 13, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/http.h"
#include "ifs/json.h"
#include "HttpResponse.h"
#include "HttpCookie.h"
#include "HttpMessage.h"
#include "Buffer.h"
#include "union_helpers.h"
#include "MemoryStream.h"
#include "Isolate.h"

namespace fibjs {

result_t HttpResponse_base::_new(obj_ptr<HttpResponse_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new HttpResponse();
    return 0;
}

result_t HttpResponse_base::_new(v8::Local<v8::Value> body, v8::Local<v8::Object> options,
    obj_ptr<HttpResponse_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current(options);
    obj_ptr<HttpResponse> resp = new HttpResponse();
    result_t hr;

    // Load options using ResponseOptions
    obj_ptr<HttpResponse::ResponseOptions> opts;
    hr = HttpResponse::ResponseOptions::load(options, opts);
    if (hr < 0)
        return hr;

    if (opts->status.has_value())
        resp->set_statusCode(opts->status.value());

    if (opts->statusText.has_value())
        resp->set_statusMessage(opts->statusText.value());

    if (opts->headers.has_value()) {
        auto& headersVar = opts->headers.value();
        if (std::holds_alternative<v8::Local<v8::Object>>(headersVar)) {
            hr = resp->setHeader(std::get<v8::Local<v8::Object>>(headersVar));
        } else {
            hr = resp->setHeader(std::get<obj_ptr<Headers_base>>(headersVar).get());
        }
        if (hr < 0)
            return hr;
    }

    // Handle body using body_to_stream helper
    obj_ptr<SeekableStream_base> stm;
    obj_ptr<Headers_base> hdrs;
    resp->get_headers(hdrs);

    hr = body_to_stream(isolate, body, stm, hdrs);
    if (hr < 0)
        return hr;

    if (stm)
        resp->set_body(stm);

    retVal = resp;
    return 0;
}

result_t HttpResponse::get_protocol(exlib::string& retVal)
{
    return m_message->get_protocol(retVal);
}

result_t HttpResponse::set_protocol(exlib::string newVal)
{
    return m_message->set_protocol(newVal);
}

result_t HttpResponse::get_headers(obj_ptr<Headers_base>& retVal)
{
    return m_message->get_headers(retVal);
}

result_t HttpResponse::get_body(obj_ptr<Stream_base>& retVal)
{
    return m_message->get_body(retVal);
}

result_t HttpResponse::set_body(Stream_base* newVal)
{
    return m_message->set_body(newVal);
}

result_t HttpResponse::get_bodyUsed(bool& retVal)
{
    return m_message->get_bodyUsed(retVal);
}

result_t HttpResponse::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncHandle ac)
{
    return m_message->read(bytes, retVal, std::move(ac));
}

result_t HttpResponse::readAll(obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    return m_message->readAll(retVal, std::move(ac));
}

result_t HttpResponse::setEncoding(exlib::string encoding, obj_ptr<Message_base>& retVal)
{
    obj_ptr<Message_base> r;
    m_message->setEncoding(encoding, r);

    // the caller keeps the response it called this on, like node's readable
    retVal = this;
    return 0;
}

result_t HttpResponse::write(Union_write_data data, int32_t& retVal, AsyncHandle ac)
{
    if (std::holds_alternative<obj_ptr<Buffer_base>>(data))
        return write(std::get<obj_ptr<Buffer_base>>(data).get(), retVal, std::move(ac));

    return write(std::get<exlib::string>(data), retVal, std::move(ac));
}

result_t HttpResponse::write(Buffer_base* data, int32_t& retVal, AsyncHandle ac)
{
    return m_message->write(data, retVal, std::move(ac));
}

result_t HttpResponse::write(exlib::string data, int32_t& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> buf;
    result_t hr = Buffer_base::from(data, "utf8", buf);
    if (hr < 0)
        return hr;

    return m_message->write(buf.get(), retVal, std::move(ac));
}

result_t HttpResponse::text(exlib::string data, exlib::string& retVal, AsyncHandle ac)
{
    return m_message->text(data, retVal, std::move(ac));
}

result_t HttpResponse::text(exlib::string& retVal, AsyncHandle ac)
{
    return m_message->text(retVal, std::move(ac));
}

result_t HttpResponse::arrayBuffer(std::shared_ptr<v8::BackingStore>& retVal, AsyncHandle ac)
{
    return m_message->arrayBuffer(retVal, std::move(ac));
}

result_t HttpResponse::formData(obj_ptr<FormData_base>& retVal, AsyncHandle ac)
{
    return m_message->formData(retVal, std::move(ac));
}

result_t HttpResponse::json(v8::Local<v8::Value> data, Variant& retVal, AsyncHandle ac)
{
    return m_message->json(data, retVal, std::move(ac));
}

result_t HttpResponse::json(v8::Local<v8::Value> data, v8::Local<v8::Object> options, Variant& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        // Sync phase (V8-safe): encode the body here (same as Message::json's
        // sync phase, which cannot be reused because it would take the shared
        // handle), then pack the options into ctx[1]. The async phase applies
        // them.
        exlib::string str;
        result_t hr = json_base::encode(data, str);
        if (hr < 0)
            return hr;

        ac.ctxv().resize(2);
        ac.ctxv()[0] = new Buffer(str.c_str(), str.length());

        obj_ptr<HttpResponse::ResponseOptions> opts;
        hr = HttpResponse::ResponseOptions::load(options, opts);
        if (hr < 0)
            return hr;

        // Normalize headers to a C++ object: the v8::Local alternative is
        // unusable in the async phase.
        if (opts->headers.has_value() && std::holds_alternative<v8::Local<v8::Object>>(opts->headers.value())) {
            obj_ptr<Headers> hdrs = new Headers();
            hdrs->append(std::get<v8::Local<v8::Object>>(opts->headers.value()));
            opts->headers = hdrs;
        }

        ac.ctxv()[1] = opts;
        return CALL_E_NOSYNC;
    }

    // Async phase (C++-only, no V8 access): apply status / statusText /
    // headers, then let m_message write the encoded body (which also sets
    // Content-Type: application/json).
    if (ac.ctxv().size() > 1) {
        obj_ptr<HttpResponse::ResponseOptions> opts = (HttpResponse::ResponseOptions*)ac.ctxv()[1].object();
        if (opts->status.has_value())
            set_statusCode(opts->status.value());
        if (opts->statusText.has_value())
            set_statusMessage(opts->statusText.value());
        // appendHeader keeps any headers set before json(); the JSON body
        // write below sets/overrides Content-Type.
        if (opts->headers.has_value() && std::holds_alternative<obj_ptr<Headers_base>>(opts->headers.value()))
            appendHeader(std::get<obj_ptr<Headers_base>>(opts->headers.value()).get());
    }

    return m_message->json(data, retVal, std::move(ac));
}

result_t HttpResponse::json(Variant& retVal, AsyncHandle ac)
{
    return m_message->json(retVal, std::move(ac));
}

result_t HttpResponse::pack(v8::Local<v8::Value> data, Variant& retVal, AsyncHandle ac)
{
    return m_message->pack(data, retVal, std::move(ac));
}

result_t HttpResponse::pack(Variant& retVal, AsyncHandle ac)
{
    return m_message->pack(retVal, std::move(ac));
}

result_t HttpResponse::blob(exlib::string type, obj_ptr<Blob_base>& retVal, AsyncHandle ac)
{
    // If no explicit type given, use the response Content-Type header
    if (type.empty())
        m_message->firstHeader("Content-Type", type);
    return m_message->blob(type, retVal, std::move(ac));
}

result_t HttpResponse::bytes(obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    return m_message->bytes(retVal, std::move(ac));
}

result_t HttpResponse::get_length(int64_t& retVal)
{
    return m_message->get_length(retVal);
}

result_t HttpResponse::get_keepAlive(bool& retVal)
{
    return m_message->get_keepAlive(retVal);
}

result_t HttpResponse::set_keepAlive(bool newVal)
{
    return m_message->set_keepAlive(newVal);
}

result_t HttpResponse::get_upgrade(bool& retVal)
{
    return m_message->get_upgrade(retVal);
}

result_t HttpResponse::set_upgrade(bool newVal)
{
    return m_message->set_upgrade(newVal);
}

result_t HttpResponse::get_maxHeadersCount(int32_t& retVal)
{
    return m_message->get_maxHeadersCount(retVal);
}

result_t HttpResponse::set_maxHeadersCount(int32_t newVal)
{
    return m_message->set_maxHeadersCount(newVal);
}

result_t HttpResponse::get_maxHeaderSize(int32_t& retVal)
{
    return m_message->get_maxHeaderSize(retVal);
}

result_t HttpResponse::set_maxHeaderSize(int32_t newVal)
{
    return m_message->set_maxHeaderSize(newVal);
}

result_t HttpResponse::get_maxChunkSize(int32_t& retVal)
{
    return m_message->get_maxChunkSize(retVal);
}

result_t HttpResponse::set_maxChunkSize(int32_t newVal)
{
    return m_message->set_maxChunkSize(newVal);
}

result_t HttpResponse::get_maxBodySize(int32_t& retVal)
{
    return m_message->get_maxBodySize(retVal);
}

result_t HttpResponse::set_maxBodySize(int32_t newVal)
{
    return m_message->set_maxBodySize(newVal);
}

result_t HttpResponse::get_socket(obj_ptr<Stream_base>& retVal)
{
    return m_message->get_socket(retVal);
}

result_t HttpResponse::hasHeader(exlib::string name, bool& retVal)
{
    return m_message->hasHeader(name, retVal);
}

result_t HttpResponse::firstHeader(exlib::string name, exlib::string& retVal)
{
    return m_message->firstHeader(name, retVal);
}

result_t HttpResponse::allHeader(exlib::string name, obj_ptr<NObject>& retVal)
{
    return m_message->allHeader(name, retVal);
}

result_t HttpResponse::appendHeader(v8::Local<v8::Object> map)
{
    return m_message->appendHeader(map);
}

result_t HttpResponse::appendHeader(Headers_base* headers)
{
    return m_message->appendHeader(headers);
}

result_t HttpResponse::appendHeader(exlib::string name, Variant value)
{
    return m_message->appendHeader(name, value);
}

result_t HttpResponse::appendHeader(exlib::string name, v8::Local<v8::Array> values)
{
    return m_message->appendHeader(name, values);
}

result_t HttpResponse::setHeader(v8::Local<v8::Object> map)
{
    return m_message->setHeader(map);
}

result_t HttpResponse::setHeader(Headers_base* headers)
{
    return m_message->setHeader(headers);
}

result_t HttpResponse::setHeader(exlib::string name, Variant value)
{
    return m_message->setHeader(name, value);
}

result_t HttpResponse::setHeader(exlib::string name, v8::Local<v8::Array> values)
{
    return m_message->setHeader(name, values);
}

result_t HttpResponse::removeHeader(exlib::string name)
{
    return m_message->removeHeader(name);
}

result_t HttpResponse::getHeader(exlib::string name, v8::Local<v8::Value>& retVal)
{
    return m_message->getHeader(name, retVal);
}

result_t HttpResponse::getHeaders(obj_ptr<NObject>& retVal)
{
    return m_message->getHeaders(retVal);
}

result_t HttpResponse::get_headersSent(bool& retVal)
{
    return m_message->get_headersSent(retVal);
}

result_t HttpResponse::get_trailers(obj_ptr<Headers_base>& retVal)
{
    return m_message->get_trailers(retVal);
}

result_t HttpResponse::addTrailers(v8::Local<v8::Object> headers)
{
    return m_message->addTrailers(headers);
}

result_t HttpResponse::get_sent(bool& retVal)
{
    return m_message->get_sent(retVal);
}

result_t HttpResponse::get_value(exlib::string& retVal)
{
    return m_message->get_value(retVal);
}

result_t HttpResponse::set_value(exlib::string newVal)
{
    return m_message->set_value(newVal);
}

result_t HttpResponse::get_params(obj_ptr<NArray>& retVal)
{
    return m_message->get_params(retVal);
}

result_t HttpResponse::get_type(int32_t& retVal)
{
    return m_message->get_type(retVal);
}

result_t HttpResponse::set_type(int32_t newVal)
{
    return m_message->set_type(newVal);
}

result_t HttpResponse::get_lastError(exlib::string& retVal)
{
    return m_message->get_lastError(retVal);
}

result_t HttpResponse::set_lastError(exlib::string newVal)
{
    return m_message->set_lastError(newVal);
}

result_t HttpResponse::end(int32_t& retVal, AsyncHandle ac)
{
    return m_message->end(retVal, std::move(ac));
}

result_t HttpResponse::end(Buffer_base* data, int32_t& retVal, AsyncHandle ac)
{
    return m_message->end(data, retVal, std::move(ac));
}

result_t HttpResponse::end(Buffer_base* data, exlib::string encoding, int32_t& retVal, AsyncHandle ac)
{
    return m_message->end(data, encoding, retVal, std::move(ac));
}

result_t HttpResponse::end(exlib::string data, exlib::string encoding, int32_t& retVal, AsyncHandle ac)
{
    return m_message->end(data, encoding, retVal, std::move(ac));
}

result_t HttpResponse::isEnded(bool& retVal)
{
    return m_message->isEnded(retVal);
}

result_t HttpResponse::clear()
{
    m_message->clear();

    m_cookies.clear();
    m_cookies_filled = false;
    m_statusCode = 200;

    return 0;
}

static const char* const status_lines[] = {
#define LEVEL_100 0
    " 100 Continue", " 101 Switching Protocols", " 102 Processing",
#define LEVEL_200 3
    " 200 OK", " 201 Created", " 202 Accepted",
    " 203 Non-Authoritative Information", " 204 No Content",
    " 205 Reset Content", " 206 Partial Content", " 207 Multi-Status",
#define LEVEL_300 11
    " 300 Multiple Choices", " 301 Moved Permanently", " 302 Object moved",
    " 303 See Other", " 304 Not Modified", " 305 Use Proxy", " 306 unused",
    " 307 Temporary Redirect",
#define LEVEL_400 19
    " 400 Bad Request", " 401 Authorization Required",
    " 402 Payment Required", " 403 Forbidden", " 404 File Not Found",
    " 405 Method Not Allowed", " 406 Not Acceptable",
    " 407 Proxy Authentication Required", " 408 Request Time-out",
    " 409 Conflict", " 410 Gone", " 411 Length Required",
    " 412 Precondition Failed", " 413 Request Entity Too Large",
    " 414 Request-URI Too Large", " 415 Unsupported Media Type",
    " 416 Requested Range Not Satisfiable", " 417 Expectation Failed",
    " 418 Host Not Found", " 419 unused", " 420 unused", " 421 unused",
    " 422 Unprocessable Entity", " 423 Locked", " 424 Failed Dependency",
#define LEVEL_500 44
    " 500 Internal Server Error", " 501 Method Not Implemented",
    " 502 Bad Gateway", " 503 Service Temporarily Unavailable",
    " 504 Gateway Time-out", " 505 HTTP Version Not Supported",
    " 506 Variant Also Negotiates", " 507 Insufficient Storage",
    " 508 unused", " 509 unused", " 510 Not Extended"
#define RESPONSE_CODES 55
};
static int32_t shortcut[6] = { LEVEL_100, LEVEL_200, LEVEL_300, LEVEL_400, LEVEL_500, RESPONSE_CODES };
static unsigned char status_lines_size[RESPONSE_CODES];

static class _init_status_line {
public:
    _init_status_line()
    {
        int32_t i;

        for (i = 0; i < RESPONSE_CODES; i++)
            status_lines_size[i] = (unsigned char)qstrlen(status_lines[i]);
    }
} s_init_status_line;

result_t http_base::get_STATUS_CODES(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    int32_t i;

    if (!isolate->STATUS_CODES.IsEmpty()) {
        retVal = isolate->STATUS_CODES.Get(isolate->m_isolate);
        return 0;
    }

    v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);
    for (i = 0; i < RESPONSE_CODES; i++)
        o->Set(context, isolate->NewString(status_lines[i] + 1, 3), isolate->NewString(status_lines[i] + 5)).IsJust();

    isolate->STATUS_CODES.Reset(isolate->m_isolate, o);
    retVal = o;

    return 0;
}

exlib::string HttpResponse::prepareHeaders()
{
    if (m_cookies_filled) {
        for (auto& cookie : m_cookies) {
            exlib::string str;

            if (cookie) {
                cookie->toString(str);
                appendHeader("Set-Cookie", str);
            }
        }

        m_cookies.clear();
        m_cookies_filled = false;
    }

    exlib::string strCommand;
    exlib::string statusMessage;

    if (m_statusMessage.empty()) {
        int32_t statusCode = m_statusCode;

        if (statusCode >= 100 && statusCode < 600) {
            int32_t n = statusCode / 100;
            if (shortcut[n - 1] + statusCode % 100 < shortcut[n]) {
                int32_t pos = shortcut[statusCode / 100 - 1] + statusCode % 100;
                statusMessage.assign(status_lines[pos], status_lines_size[pos]);
            }
        }

        if (statusMessage.empty()) {
            char buf[32];
            snprintf(buf, sizeof(buf), " %d Unknown", statusCode);
            statusMessage = buf;
        }
    } else {
        char buf[32];

        snprintf(buf, sizeof(buf), " %d ", m_statusCode);
        statusMessage = buf;
        statusMessage.append(m_statusMessage);
    }

    get_protocol(strCommand);
    strCommand.append(statusMessage);

    return strCommand;
}

result_t HttpResponse::readFrom(Stream_base* stm, AsyncHandle ac, bool headerOnly)
{
    class asyncReadFrom : public AsyncState {
    public:
        asyncReadFrom(HttpResponse* pThis, BufferedStream_base* stm,
            AsyncHandle ac, bool headerOnly)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_headerOnly(headerOnly)
        {
            init(begin);
        }

        ON_STATE(asyncReadFrom, begin)
        {
            return m_stm->readLine(m_pThis->m_message->m_maxHeaderSize, m_strLine, next(command));
        }

        ON_STATE(asyncReadFrom, command)
        {
            if (n == CALL_RETURN_NULL)
                return CHECK_ERROR(CALL_E_CLOSED);

            result_t hr;
            const char* c_str = m_strLine.c_str();
            int32_t len = (int32_t)m_strLine.length();

            if (len < 12 || c_str[8] != ' '
                || !qisdigit(c_str[9]) || !qisdigit(c_str[10]) || !qisdigit(c_str[11])
                || qisdigit(c_str[12]))
                return CHECK_ERROR(Runtime::setError("HttpResponse: bad protocol: " + m_strLine));

            m_pThis->set_statusCode((c_str[9] - '0') * 100 + (c_str[10] - '0') * 10 + (c_str[11] - '0'));
            m_pThis->set_statusMessage(c_str[12] == ' ' ? c_str + 13 : c_str + 12);

            m_strLine.resize(8);
            hr = m_pThis->set_protocol(m_strLine);
            if (hr < 0)
                return hr;

            if (m_headerOnly)
                return m_pThis->m_message->readHeader(m_stm, next());

            return m_pThis->m_message->readFrom(m_stm, next());
        }

    public:
        obj_ptr<HttpResponse> m_pThis;
        obj_ptr<BufferedStream_base> m_stm;
        bool m_headerOnly;
        exlib::string m_strLine;
    };

    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<BufferedStream_base> _stm = BufferedStream_base::getInstance(stm);
    if (!_stm)
        return CHECK_ERROR(Runtime::setError("HttpResponse: only accept BufferedStream object."));

    return (new asyncReadFrom(this, _stm, std::move(ac), headerOnly))->post(0);
}

result_t HttpResponse::readHeader(Stream_base* stm, AsyncHandle ac)
{
    return readFrom(stm, std::move(ac), true);
}

result_t HttpResponse::sendTo(Stream_base* stm, v8::Local<v8::Object> options, AsyncHandle ac)
{
    if (ac.isSync()) {
        obj_ptr<Options> _options;
        Isolate* isolate = Isolate::current(options);
        result_t hr = Options::load(options, _options);
        if (hr < 0)
            return hr;

        if (!_options->header_only.value() && _options->content_length.has_value())
            return Runtime::setError("HttpResponse: content_length option is only valid for header_only response");

        ac.ctxv().resize(1);
        ac.ctxv()[0] = _options;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    exlib::string strCommand = prepareHeaders();

    if (ac.ctxv().size() == 1) {
        Options* _options = (Options*)ac.ctxv()[0].object();
        if (_options->header_only.value())
            return m_message->sendHeader(stm, strCommand, _options->content_length.value_or(true), std::move(ac));
    }

    return m_message->send(stm, strCommand, std::move(ac));
}

result_t HttpResponse::readFrom(Stream_base* stm, v8::Local<v8::Object> options, AsyncHandle ac)
{
    if (ac.isSync()) {
        obj_ptr<Options> _options;
        Isolate* isolate = Isolate::current(options);
        result_t hr = Options::load(options, _options);
        if (hr < 0)
            return hr;

        ac.ctxv().resize(1);
        ac.ctxv()[0] = _options;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    if (ac.ctxv().size() == 1) {
        Options* _options = (Options*)ac.ctxv()[0].object();
        if (_options->header_only.value())
            return readFrom(stm, std::move(ac), true);
    }

    return readFrom(stm, std::move(ac), false);
}

result_t HttpResponse::readBody(AsyncHandle ac)
{
    return m_message->readBody(std::move(ac));
}

result_t HttpResponse::get_stream(obj_ptr<Stream_base>& retVal)
{
    return m_message->get_stream(retVal);
}

result_t HttpResponse::get_statusCode(int32_t& retVal)
{
    retVal = m_statusCode;
    return 0;
}

result_t HttpResponse::set_statusCode(int32_t newVal)
{
    m_statusCode = newVal;
    return 0;
}

result_t HttpResponse::get_statusMessage(exlib::string& retVal)
{
    retVal = m_statusMessage;
    return 0;
}

result_t HttpResponse::set_statusMessage(exlib::string newVal)
{
    m_statusMessage = newVal;
    return 0;
}

// statusText is a Web API alias for statusMessage
result_t HttpResponse::get_statusText(exlib::string& retVal)
{
    return get_statusMessage(retVal);
}

result_t HttpResponse::set_statusText(exlib::string newVal)
{
    return set_statusMessage(newVal);
}

result_t HttpResponse::get_status(int32_t& retVal)
{
    return get_statusCode(retVal);
}

result_t HttpResponse::set_status(int32_t newVal)
{
    return set_statusCode(newVal);
}

result_t HttpResponse::get_ok(bool& retVal)
{
    retVal = m_statusCode >= 200 && m_statusCode < 300;
    return 0;
}

result_t HttpResponse::writeHead(int32_t statusCode, exlib::string statusMessage, v8::Local<v8::Object> headers)
{
    set_statusCode(statusCode);
    set_statusMessage(statusMessage);
    appendHeader(headers);
    return 0;
}

result_t HttpResponse::writeHead(int32_t statusCode, v8::Local<v8::Object> headers)
{
    set_statusCode(statusCode);
    appendHeader(headers);
    return 0;
}

result_t HttpResponse::get_cookies(std::vector<obj_ptr<HttpCookie_base>>& retVal)
{
    if (!m_cookies_filled) {
        int32_t len, i;
        obj_ptr<NArray> headers;

        allHeader("Set-Cookie", headers);

        len = headers->length();

        for (i = 0; i < len; i++) {
            Variant v;
            exlib::string str;
            obj_ptr<HttpCookie> cookie;

            headers->_indexed_getter(i, v);
            str = v.string();

            cookie = new HttpCookie();
            // No URL decoding: per RFC 6265 cookie values are opaque strings,
            // and %XX in Set-Cookie (e.g. better-auth's base64 session token)
            // must be stored and sent back verbatim.
            if (cookie->parseRaw(str) >= 0)
                m_cookies.push_back(cookie);
        }

        m_cookies_filled = true;
    }

    retVal = m_cookies;
    return 0;
}

result_t HttpResponse::addCookie(Union_addCookie_cookie cookie)
{
    obj_ptr<HttpCookie_base> cookie_;
    result_t hr = ctor_object_from_union<HttpCookie_base>(cookie, cookie_);
    if (hr < 0)
        return hr;

    std::vector<obj_ptr<HttpCookie_base>> cookies;

    hr = get_cookies(cookies);
    if (hr < 0)
        return hr;

    m_cookies.push_back(cookie_);

    return 0;
}

result_t HttpResponse::redirect(exlib::string url)
{
    return redirect(302, url);
}

result_t HttpResponse::redirect(int32_t statusCode, exlib::string url)
{
    if (statusCode != 301 && statusCode != 302 && statusCode != 307)
        return CHECK_ERROR(Runtime::setError("HttpResponse: Invalid statusCode %d, expected 301, 302, or 307.", statusCode));

    m_statusCode = statusCode;
    setHeader("Location", url);
    return 0;
}

result_t HttpResponse::get_url(exlib::string& retVal)
{
    retVal = m_fetchUrl;
    return 0;
}

result_t HttpResponse::get_redirected(bool& retVal)
{
    retVal = m_redirected;
    return 0;
}

result_t HttpResponse::get_type(exlib::string& retVal)
{
    retVal = m_fetchType;
    return 0;
}

result_t HttpResponse_base::json(v8::Local<v8::Value> data, v8::Local<v8::Object> options,
    obj_ptr<HttpResponse_base>& retVal)
{
    Isolate* isolate = Isolate::current(options);
    obj_ptr<HttpResponse> resp = new HttpResponse();
    result_t hr;

    // Load options
    obj_ptr<HttpResponse::ResponseOptions> opts;
    hr = HttpResponse::ResponseOptions::load(options, opts);
    if (hr < 0)
        return hr;

    if (opts->status.has_value())
        resp->set_statusCode(opts->status.value());
    if (opts->statusText.has_value())
        resp->set_statusMessage(opts->statusText.value());
    if (opts->headers.has_value()) {
        auto& hv = opts->headers.value();
        if (std::holds_alternative<v8::Local<v8::Object>>(hv))
            hr = resp->setHeader(std::get<v8::Local<v8::Object>>(hv));
        else
            hr = resp->setHeader(std::get<obj_ptr<Headers_base>>(hv).get());
        if (hr < 0)
            return hr;
    }

    // Serialize to JSON and set Content-Type
    exlib::string jsonStr;
    hr = json_base::encode(data, jsonStr);
    if (hr < 0)
        return hr;

    resp->setHeader("Content-Type", "application/json");

    // Set body as MemoryStream
    obj_ptr<Buffer> buf = new Buffer((const uint8_t*)jsonStr.c_str(), jsonStr.length());
    obj_ptr<MemoryStream> ms = new MemoryStream();
    ms->writeBuffer(buf, nullptr);
    ms->rewind();
    resp->set_body(ms);

    retVal = resp;
    return 0;
}

result_t HttpResponse_base::redirect(exlib::string url, int32_t status,
    obj_ptr<HttpResponse_base>& retVal)
{
    obj_ptr<HttpResponse> resp = new HttpResponse();
    resp->set_statusCode(status);
    resp->setHeader("Location", url);
    retVal = resp;
    return 0;
}

result_t HttpResponse_base::error(obj_ptr<HttpResponse_base>& retVal)
{
    obj_ptr<HttpResponse> resp = new HttpResponse();
    resp->set_statusCode(0);
    resp->m_fetchType = "error";
    retVal = resp;
    return 0;
}

result_t HttpResponse::sendHeader(Stream_base* stm, bool content_length, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string strCommand = prepareHeaders();
    return m_message->sendHeader(stm, strCommand, content_length, std::move(ac));
}

result_t HttpResponse::clone(obj_ptr<Message_base>& retVal)
{
    // Streaming responses cannot be cloned (body is a live socket stream)
    if (m_message->m_bodyStream)
        return CHECK_ERROR(Runtime::setError("Response body is a streaming body and cannot be cloned."));

    obj_ptr<HttpResponse> resp = new HttpResponse();

    // Copy HttpMessage properties
    m_message->copyTo(resp->m_message);

    // Copy HttpResponse specific properties
    resp->m_statusCode = m_statusCode;
    resp->m_statusMessage = m_statusMessage;
    // Copy fetch metadata
    resp->m_fetchUrl = m_fetchUrl;
    resp->m_redirected = m_redirected;
    resp->m_fetchType = m_fetchType;

    // Clone cookies array
    if (m_cookies_filled) {
        // Cookies are HttpCookie objects; share them (shallow copy)
        resp->m_cookies = m_cookies;
        resp->m_cookies_filled = true;
    }

    retVal = resp;
    return 0;
}

result_t HttpResponse::resume(obj_ptr<Message_base>& retVal)
{
    obj_ptr<Stream_base> body;
    if (m_message->get_body(body) == 0 && body) {
        obj_ptr<Stream_base> r;
        body->resume(r);
    }
    retVal = this;
    return 0;
}

result_t HttpResponse::pause(obj_ptr<Message_base>& retVal)
{
    obj_ptr<Stream_base> body;
    if (m_message->get_body(body) == 0 && body) {
        obj_ptr<Stream_base> r;
        body->pause(r);
    }
    retVal = this;
    return 0;
}

result_t HttpResponse::unpipe(Stream_base* destination)
{
    return m_message->unpipe(destination);
}

result_t HttpResponse::pipe(v8::Local<v8::Value> destination, v8::Local<v8::Object> options, v8::Local<v8::Value>& retVal)
{
    return holder()->call_pipe(wrap(), destination, options, retVal);
}

result_t HttpResponse::onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func)
{
    return m_message->onEventChange(type, ev, func);
}

} /* namespace fibjs */
