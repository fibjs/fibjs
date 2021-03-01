/*
 * gui_handler.h
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#ifndef GUI_HANDLER_H
#define GUI_HANDLER_H

#include "include/cef_client.h"
#include "CefWebView.h"

#include <list>

namespace fibjs {

class GuiHandler : public CefClient,
                   public CefContextMenuHandler,
                   public CefDisplayHandler,
                   public CefLifeSpanHandler,
                   public CefLoadHandler,
                   public CefRenderHandler {
public:
    virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE
    {
        return this;
    }

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

    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE
    {
        return this;
    }

public:
    // CefContextMenuHandler
    virtual bool RunContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model,
        CefRefPtr<CefRunContextMenuCallback> callback) OVERRIDE;

public:
    // CefLifeSpanHandler
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        const CefString& target_url, const CefString& target_frame_name, WindowOpenDisposition target_disposition,
        bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client,
        CefBrowserSettings& settings, CefRefPtr<CefDictionaryValue>& extra_info, bool* no_javascript_access) OVERRIDE;
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

public:
    // CefDisplayHandler
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;
    virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, cef_log_severity_t level,
        const CefString& message, const CefString& source, int line);

public:
    // CefLoadHandler
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) OVERRIDE;
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        int httpStatusCode) OVERRIDE;

public:
    // CefRenderHandler
    virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) OVERRIDE;
    virtual void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type,
        const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width,
        int height) OVERRIDE;

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
