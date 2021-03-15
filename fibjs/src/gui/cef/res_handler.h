/*
 * res_handler.h
 *
 *  Created on: Mar 5, 2021
 *      Author: lion
 */

#pragma once

#include "object.h"
#include "ifs/Handler.h"
#include "include/cef_resource_handler.h"
#include "include/cef_scheme.h"
#include "HttpRequest.h"

namespace fibjs {

class GuiSchemeHandlerFactory;

class GuiResourceHandler : public CefResourceHandler {
public:
    GuiResourceHandler(obj_ptr<Handler_base> hdr)
        : m_handler(hdr)
    {
    }

public:
    // CefResourceHandler
    virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
        CefRefPtr<CefCallback> callback) OVERRIDE;
    virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
        int64& response_length, CefString& redirectUrl) OVERRIDE;
    virtual void Cancel() OVERRIDE;
    virtual bool ReadResponse(void* data_out, int bytes_to_read,
        int& bytes_read, CefRefPtr<CefCallback> callback) OVERRIDE;

private:
    obj_ptr<Handler_base> m_handler;
    obj_ptr<HttpRequest> m_req;

    exlib::string data_;
    int size_readed;
    bool m_done;
    IMPLEMENT_REFCOUNTING(GuiResourceHandler);
};

class GuiSchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
    virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, const CefString& scheme_name,
        CefRefPtr<CefRequest> request) OVERRIDE;

    void RegisterScheme(exlib::string schame);

public:
    std::map<exlib::string, obj_ptr<Handler_base>> m_domains;

private:
    IMPLEMENT_REFCOUNTING(GuiSchemeHandlerFactory);
};

}
