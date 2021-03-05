/*
 * res_handler.cpp
 *
 *  Created on: Mar 5, 2021
 *      Author: lion
 */

#include "res_handler.h"
#include "ifs/process.h"
#include "ifs/mq.h"
#include "Url.h"
#include "include/cef_task.h"

namespace fibjs {

extern exlib::LockedList<Isolate> s_isolates;

bool GuiResourceHandler::ProcessRequest(CefRefPtr<CefRequest> request,
    CefRefPtr<CefCallback> callback)
{
    class asyncInvoke : public AsyncState,
                        public CefTask {
    public:
        asyncInvoke(Handler_base* hdr, HttpRequest_base* req, CefRefPtr<CefCallback> callback)
            : AsyncState(NULL)
            , m_handler(hdr)
            , m_req(req)
            , m_callback(callback)
        {
            m_req->get_response(m_rep);
            next(invoke);
        }

    public:
        ON_STATE(asyncInvoke, invoke)
        {
            return mq_base::invoke(m_handler, m_req, next(ok));
        }

        ON_STATE(asyncInvoke, ok)
        {
            CefPostTask(TID_IO, this);
            return CALL_E_PENDDING;
        }

        virtual void Execute()
        {
            m_callback->Continue();
        }

        virtual int32_t error(int32_t v)
        {
            if (at(invoke)) {
                m_rep->set_statusCode(500);
                return 0;
            }
            return v;
        }

        virtual Isolate* isolate()
        {
            return s_isolates.head();
        }

    private:
        obj_ptr<Handler_base> m_handler;
        obj_ptr<HttpRequest_base> m_req;
        obj_ptr<HttpResponse_base> m_rep;
        CefRefPtr<CefCallback> m_callback;

    private:
        IMPLEMENT_REFCOUNTING(asyncInvoke);
    };

    std::string url = request->GetURL();
    exlib::string p;

    Url u;
    u.parse(url);

    m_req = new HttpRequest();
    m_req->set_method(request->GetMethod().ToString().c_str());
    m_req->set_value(u.m_pathname);
    m_req->set_address(u.m_pathname);
    m_req->set_queryString(u.m_query);

    CefRequest::HeaderMap headers;
    request->GetHeaderMap(headers);

    CefRequest::HeaderMap::iterator it;

    it = headers.begin();
    while (it != headers.end()) {
        m_req->addHeader(it->first.ToString().c_str(), it->second.ToString().c_str());
        it++;
    }

    // TODO: post body

    (new asyncInvoke(m_handler, m_req, callback))->apost(0);

    return true; //
}

void GuiResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
    int64& response_length, CefString& redirectUrl)
{
    obj_ptr<HttpResponse_base> rep;
    exlib::string hdr;

    m_req->get_response(rep);

    if (rep->firstHeader("Content-Type", hdr) != CALL_RETURN_NULL)
        response->SetMimeType(hdr.c_str());
    else
        response->SetMimeType("text/html");

    int32_t code;
    rep->get_statusCode(code);
    response->SetStatus(code);

    obj_ptr<NObject> headers;
    rep->allHeader("", headers);

    std::map<exlib::string, int32_t>::iterator it = headers->m_keys.begin();
    while (it != headers->m_keys.end()) {
        NObject::Value& v = headers->m_values[it->second];
        obj_ptr<NArray> list = NArray::getInstance(v.m_val.object());

        if (list) {
            int32_t sz;

            list->get_length(sz);
            for (int32_t i = 0; i < sz; i++)
                response->SetHeaderByName(it->first.c_str(), list->m_array[i].string().c_str(), false);
        } else
            response->SetHeaderByName(it->first.c_str(), v.m_val.string().c_str(), false);

        it++;
    }

    response->SetHeaderByName("Access-Control-Allow-Origin", "*", false);

    obj_ptr<SeekableStream_base> body;
    rep->get_body(body);

    int64_t sz;
    body->size(sz);
    body->rewind();

    m_done = false;
    size_readed = 0;

    response_length = sz;
}

void GuiResourceHandler::Cancel()
{
    // Cancel the response...
}

bool GuiResourceHandler::ReadResponse(void* data_out, int bytes_to_read,
    int& bytes_read, CefRefPtr<CefCallback> callback)
{
    class asyncSend : public AsyncState,
                      public CefTask {
    public:
        asyncSend(HttpRequest_base* req, exlib::string& _data, bool& done,
            int bytes_to_read, CefRefPtr<CefCallback> callback)
            : AsyncState(NULL)
            , m_data(_data)
            , m_done(done)
            , m_bytes_to_read(bytes_to_read)
            , m_callback(callback)
        {
            obj_ptr<HttpResponse_base> rep;
            req->get_response(rep);
            obj_ptr<SeekableStream_base> body;
            rep->get_body(body);
            m_body = body;

            next(read);
        }

    public:
        ON_STATE(asyncSend, read)
        {
            return m_body->read(m_bytes_to_read, m_buf, next(ok));
        }

        ON_STATE(asyncSend, ok)
        {
            if (m_buf)
                m_buf->toString(m_data);
            else
                m_done = true;

            CefPostTask(TID_IO, this);
            return CALL_E_PENDDING;
        }

        virtual void Execute()
        {
            m_callback->Continue();
        }

        virtual int32_t error(int32_t v)
        {
            return 0;
        }

    private:
        obj_ptr<Stream_base> m_body;
        exlib::string& m_data;
        bool& m_done;
        int m_bytes_to_read;
        CefRefPtr<CefCallback> m_callback;

        obj_ptr<Buffer_base> m_buf;

    private:
        IMPLEMENT_REFCOUNTING(asyncSend);
    };

    if (m_done)
        return false;

    if (size_readed < data_.length()) {
        bytes_read = (int)data_.length() - size_readed;
        if (bytes_read > bytes_to_read)
            bytes_read = bytes_to_read;

        memcpy(data_out, data_.c_str() + size_readed, bytes_read);
        size_readed += bytes_read;

        return true;
    }

    size_readed = 0;
    data_.clear();

    (new asyncSend(m_req, data_, m_done, bytes_to_read, callback))->apost(0);
    return true;
}

CefRefPtr<CefResourceHandler> GuiSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame, const CefString& scheme_name,
    CefRefPtr<CefRequest> request)
{
    std::string url = request->GetURL();

    Url u;
    u.parse(url);

    std::map<exlib::string, obj_ptr<Handler_base>>::iterator it;
    it = m_domains.find(u.m_hostname);
    if (it == m_domains.end())
        return NULL;

    return new GuiResourceHandler(it->second);
}

void GuiSchemeHandlerFactory::RegisterScheme(exlib::string schame)
{
    if (schame != "http" && schame != "https") {
        CefRegisterSchemeHandlerFactory(schame.c_str(), "", this);
    } else {
        std::map<exlib::string, obj_ptr<Handler_base>>::iterator it = m_domains.begin();

        while (it != m_domains.end()) {
            CefRegisterSchemeHandlerFactory(schame.c_str(), it->first.c_str(), this);
            it++;
        }
    }
}

}