/*
 * gui_handler.h
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_

#include "include/cef_client.h"
#include "CefWebView.h"

#include <list>

namespace fibjs {

class GuiHandler : public CefClient,
                   public CefDisplayHandler,
                   public CefLifeSpanHandler,
                   public CefLoadHandler {
public:
    explicit GuiHandler();
    ~GuiHandler();

public:
    static GuiHandler* GetInstance();

    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE
    {
        return this;
    }

    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE
    {
        return this;
    }

    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE
    {
        return this;
    }

public:
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, cef_log_severity_t level,
        const CefString& message, const CefString& source, int line);

public:
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) OVERRIDE;

    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        int httpStatusCode) OVERRIDE;

public:
    typedef std::list<obj_ptr<CefWebView>> BrowserList;
    BrowserList browser_list_;

private:
    BrowserList::iterator fromBrowser(CefRefPtr<CefBrowser> browser);
    void PlatformTitleChange(CefRefPtr<CefBrowser> browser,
        const CefString& title);

    IMPLEMENT_REFCOUNTING(GuiHandler);
};
}

#endif
