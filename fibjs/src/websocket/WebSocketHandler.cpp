/*
 * WebSocketHandler.cpp
 *
 *  Created on: Sep 11, 2015
 *      Author: lion
 */

#include "object.h"
#include "ifs/mq.h"
#include "WebSocketHandler.h"
#include "WebSocketMessage.h"
#include "WebSocket.h"
#include "ifs/HttpRequest.h"
#include "ifs/HttpResponse.h"
#include "JSHandler.h"
#include "ifs/console.h"
#include "openssl/sha.h"
#include "encoding.h"
#include <vector>

namespace fibjs {

namespace {

inline bool is_protocol_space(char ch)
{
    return ch == ' ' || ch == '\t';
}

inline bool is_protocol_separator(char ch)
{
    switch (ch) {
    case '(': case ')': case '<': case '>': case '@':
    case ',': case ';': case ':': case '\\': case '"':
    case '/': case '[': case ']': case '?': case '=':
    case '{': case '}':
        return true;
    default:
        return ch <= 0x20 || ch >= 0x7f;
    }
}

exlib::string trim_protocol(const exlib::string& value)
{
    size_t start = 0;
    size_t end = value.length();

    while (start < end && is_protocol_space(value[start]))
        start++;

    while (end > start && is_protocol_space(value[end - 1]))
        end--;

    return value.substr(start, end - start);
}

void split_protocol_header(const exlib::string& header, std::vector<exlib::string>& protocols)
{
    size_t start = 0;

    while (start <= header.length()) {
        size_t end = header.find(',', start);
        exlib::string token = trim_protocol(header.substr(start, end == exlib::string::npos ? header.length() - start : end - start));

        if (!token.empty())
            protocols.push_back(token);

        if (end == exlib::string::npos)
            break;

        start = end + 1;
    }
}

bool select_protocol(const std::vector<exlib::string>& supported, const exlib::string& requestHeader,
    exlib::string& protocol)
{
    if (supported.empty() || requestHeader.empty())
        return false;

    std::vector<exlib::string> requested;
    split_protocol_header(requestHeader, requested);

    for (auto& requestedProtocol : requested) {
        for (auto& supportedProtocol : supported) {
            if (requestedProtocol == supportedProtocol) {
                protocol = supportedProtocol;
                return true;
            }
        }
    }

    return false;
}

result_t normalize_protocols(std::vector<exlib::string>& protocols)
{
    std::vector<exlib::string> normalizedProtocols;

    for (size_t i = 0; i < protocols.size(); i++) {
        exlib::string protocol = trim_protocol(protocols[i]);

        if (protocol.empty())
            return CHECK_ERROR(Runtime::setError("websocket: protocol must not be empty."));

        for (size_t k = 0; k < protocol.length(); k++) {
            if (is_protocol_separator(protocol[k]))
                return CHECK_ERROR(Runtime::setError("websocket: invalid protocol."));
        }

        for (size_t j = 0; j < normalizedProtocols.size(); j++) {
            if (normalizedProtocols[j] == protocol)
                return CHECK_ERROR(Runtime::setError("websocket: duplicated protocol."));

        }

        normalizedProtocols.push_back(protocol);
    }

    protocols.swap(normalizedProtocols);

    return 0;
}

result_t get_protocols(v8::Local<v8::Object> opts, std::vector<exlib::string>& protocols)
{
    result_t hr;
    exlib::string protocol;
    std::vector<exlib::string> protocolList;
    Isolate* isolate = Isolate::current(opts);
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::String> protocolsKey = isolate->NewString("protocols", 9);
    bool hasProtocols = opts->Has(context, protocolsKey).FromMaybe(false);

    hr = GetConfigValue(opts, "protocol", protocol);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (hasProtocols) {
        v8::Local<v8::Value> protocolsValue;
        if (!opts->Get(context, protocolsKey).ToLocal(&protocolsValue))
            return CALL_E_JAVASCRIPT;

        hr = GetArgumentValue(isolate, protocolsValue, protocolList);
        if (hr < 0)
            return hr;
    }

    if (hasProtocols)
        protocols = protocolList;
    else if (!protocol.empty())
        protocols.push_back(protocol);

    return normalize_protocols(protocols);
}

} // namespace

result_t WebSocket_base::upgrade(v8::Local<v8::Object> opts, v8::Local<v8::Function> accept, obj_ptr<Handler_base>& retVal)
{
    bool perMessageDeflate = false;
    int32_t maxPayload = WS_DEF_SIZE;
    std::vector<exlib::string> protocols;

    GetConfigValue(opts, "perMessageDeflate", perMessageDeflate);
    GetConfigValue(opts, "maxPayload", maxPayload);

    result_t hr = get_protocols(opts, protocols);
    if (hr < 0)
        return hr;

    retVal = new WebSocketHandler(accept, perMessageDeflate, maxPayload, protocols);
    return 0;
}

result_t WebSocket_base::upgrade(v8::Local<v8::Function> accept, obj_ptr<Handler_base>& retVal)
{
    Isolate* isolate = Isolate::current(accept);
    v8::Local<v8::Object> opts = v8::Object::New(isolate->m_isolate);

    return upgrade(opts, accept, retVal);
}

WebSocketHandler::WebSocketHandler(v8::Local<v8::Function> accept, bool enableCompress, int32_t maxSize,
    const std::vector<exlib::string>& protocols)
    : m_enableCompress(enableCompress)
    , m_maxSize(maxSize)
    , m_protocols(protocols)
{
    v8::Local<v8::Object> r;
    on(holder()->NewString("accept"), accept, r);
}

result_t WebSocketHandler::isRouting(bool& retVal)
{
    retVal = false;
    return 0;
}

result_t WebSocketHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncEvent* ac)
{
    class asyncInvoke : public AsyncState {
    public:
        asyncInvoke(WebSocketHandler* pThis, HttpRequest_base* req, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_httpreq(req)
            , m_compress(false)
        {
            m_httpreq->get_response(m_httprep);
            m_httpreq->get_stream(m_stm);

            next(handshake);
        }

        ON_STATE(asyncInvoke, handshake)
        {
            exlib::string v;
            result_t hr;

            hr = m_httpreq->firstHeader("Upgrade", v);
            if (hr < 0)
                return hr;

            if (hr == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: missing Upgrade header."));

            if (qstricmp(v.c_str(), "websocket"))
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: invalid Upgrade header."));

            hr = m_httpreq->firstHeader("Sec-WebSocket-Version", v);
            if (hr < 0)
                return hr;

            if (hr == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: missing Sec-WebSocket-Version header."));

            if (qstricmp(v.c_str(), "13"))
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: invalid Sec-WebSocket-Version header."));

            bool bUpgrade;
            m_httpreq->get_upgrade(bUpgrade);
            if (!bUpgrade)
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: invalid connection header."));

            hr = m_httpreq->firstHeader("Sec-WebSocket-Key", v);
            if (hr < 0)
                return hr;

            if (hr == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: missing Sec-WebSocket-Key header."));

            exlib::string key(v);

            key.append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

            unsigned char output[20];
            SHA1((const unsigned char*)key.c_str(), key.length(), output);

            exlib::string out;

            base64Encode((const char*)output, 20, false, out);

            m_httprep->set_statusCode(101);
            m_httprep->appendHeader("Sec-WebSocket-Accept", out);
            m_httprep->appendHeader("Upgrade", "websocket");
            m_httprep->set_upgrade(true);

            exlib::string protocol;
            hr = m_httpreq->firstHeader("Sec-WebSocket-Protocol", v);
            if (hr < 0)
                return hr;

            if (hr != CALL_RETURN_NULL && select_protocol(m_pThis->m_protocols, v, protocol))
                m_httprep->appendHeader("Sec-WebSocket-Protocol", protocol);

            hr = m_httpreq->firstHeader("Sec-WebSocket-Extensions", v);
            if (hr < 0)
                return hr;

            if (hr != CALL_RETURN_NULL && m_pThis->m_enableCompress && !qstricmp(v.c_str(), "permessage-deflate", 18)) {
                m_httprep->appendHeader("Sec-WebSocket-Extensions", "permessage-deflate");
                m_compress = true;
            }

            return m_httprep->sendTo(m_stm, v8::Local<v8::Object>(), next(accept));
        }

        ON_STATE(asyncInvoke, accept)
        {
            obj_ptr<WebSocketHandler> pHandler = m_pThis;
            exlib::string protocol;
            result_t hr = m_httprep->firstHeader("Sec-WebSocket-Protocol", protocol);
            if (hr == CALL_RETURN_NULL)
                protocol.clear();
            else if (hr < 0)
                return hr;

            obj_ptr<WebSocket> sock = new WebSocket(m_stm, protocol, this, m_pThis->m_enableCompress, m_pThis->m_maxSize);
            if (m_compress)
                sock->enableCompress();

            Variant vs[2];
            vs[0] = sock;
            vs[1] = m_httpreq;
            pHandler->_emit("accept", vs, 2);

            next(CALL_RETURN_NULL);
            return CALL_E_PENDDING;
        }

    private:
        obj_ptr<WebSocketHandler> m_pThis;
        obj_ptr<HttpRequest_base> m_httpreq;
        obj_ptr<HttpResponse_base> m_httprep;
        obj_ptr<Stream_base> m_stm;
        bool m_compress;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<HttpRequest_base> req = HttpRequest_base::getInstance(v);
    if (req == NULL)
        return CHECK_ERROR(CALL_E_BADVARTYPE);

    return (new asyncInvoke(this, req, ac))->post(0);
}
}
