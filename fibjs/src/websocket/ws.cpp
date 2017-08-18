/*
 * WebSocketMessage.cpp
 *
 *  Created on: Sep 11, 2015
 *      Author: lion
 */

#include "object.h"
#include "ifs/io.h"
#include "ifs/ws.h"
#include "ifs/http.h"
#include "Map.h"
#include <mbedtls/mbedtls/sha1.h>
#include "encoding.h"
#include <stdlib.h>

namespace fibjs {

DECLARE_MODULE(ws);

result_t ws_base::connect(exlib::string url, exlib::string origin,
    obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    class asyncConnect : public AsyncState {
    public:
        asyncConnect(exlib::string url, exlib::string origin,
            obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_url(url)
            , m_origin(origin)
            , m_retVal(retVal)
        {
            set(handshake);
        }

        static int32_t handshake(AsyncState* pState, int32_t n)
        {
            asyncConnect* pThis = (asyncConnect*)pState;

            if (!qstrcmp(pThis->m_url.c_str(), "wss://", 6))
                pThis->m_url = "https://" + pThis->m_url.substr(6);
            else if (!qstrcmp(pThis->m_url.c_str(), "ws://", 5))
                pThis->m_url = "http://" + pThis->m_url.substr(5);
            else
                return CHECK_ERROR(Runtime::setError("websocket: unknown protocol"));

            pThis->m_headers = new Map();

            pThis->m_headers->put("Upgrade", "websocket");
            pThis->m_headers->put("Connection", "Upgrade");
            pThis->m_headers->put("Sec-WebSocket-Version", "13");

            if (!pThis->m_origin.empty())
                pThis->m_headers->put("Origin", pThis->m_origin);

            char keys[16];
            int32_t i;

            for (i = 0; i < (int32_t)sizeof(keys); i++)
                keys[i] = (char)rand();

            exlib::string key;
            baseEncode(
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
                6, (const char*)&keys, sizeof(keys), key);

            pThis->m_headers->put("Sec-WebSocket-Key", key);

            key.append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

            unsigned char output[20];
            mbedtls_sha1((const unsigned char*)key.c_str(), key.length(), output);

            baseEncode(
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
                6, (const char*)output, 20, pThis->m_accept);

            pThis->set(response);
            return http_base::request("GET", pThis->m_url,
                NULL, pThis->m_headers, pThis->m_httprep, pThis);
        }

        static int32_t response(AsyncState* pState, int32_t n)
        {
            asyncConnect* pThis = (asyncConnect*)pState;

            Variant v;
            result_t hr;
            int32_t status;

            pThis->m_httprep->get_status(status);
            if (status != 101)
                return CHECK_ERROR(Runtime::setError("websocket: server error."));

            hr = pThis->m_httprep->firstHeader("Upgrade", v);
            if (hr < 0)
                return hr;

            if (hr == CALL_RETURN_UNDEFINED)
                return CHECK_ERROR(Runtime::setError("websocket: missing Upgrade header."));

            if (qstricmp(v.string().c_str(), "websocket"))
                return CHECK_ERROR(Runtime::setError("websocket: invalid Upgrade header."));

            bool bUpgrade;
            pThis->m_httprep->get_upgrade(bUpgrade);
            if (!bUpgrade)
                return CHECK_ERROR(Runtime::setError("websocket: invalid connection header."));

            hr = pThis->m_httprep->firstHeader("Sec-WebSocket-Accept", v);
            if (hr < 0)
                return hr;

            if (hr == CALL_RETURN_UNDEFINED)
                return CHECK_ERROR(Runtime::setError("websocket: missing Sec-WebSocket-Accept header."));

            if (v.string() != pThis->m_accept)
                return CHECK_ERROR(Runtime::setError("websocket: invalid Sec-WebSocket-Accept header."));

            pThis->m_httprep->get_stream(pThis->m_retVal);

            return pThis->done(0);
        }

    private:
        exlib::string m_url;
        exlib::string m_origin;
        obj_ptr<Stream_base>& m_retVal;
        obj_ptr<HttpResponse_base> m_httprep;
        obj_ptr<Map> m_headers;
        exlib::string m_accept;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncConnect(url, origin, retVal, ac))->post(0);
}
}
