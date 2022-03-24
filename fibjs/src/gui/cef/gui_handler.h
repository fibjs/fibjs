/*
 * gui_handler.h
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#pragma once

#include "include/cef_client.h"
#include "CefWebView.h"

#include <list>

namespace fibjs {

class GuiHandler : public CefClient,
                   public CefContextMenuHandler,
                   public CefDisplayHandler,
                   public CefDownloadHandler,
                   public CefLifeSpanHandler,
                   public CefLoadHandler,
                   public CefRenderHandler,
                   public CefPrintHandler {
public:
    virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override
    {
        return this;
    }

    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override
    {
        return this;
    }

    virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() override
    {
        return this;
    }

    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override
    {
        return this;
    }

    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override
    {
        return this;
    }

    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override
    {
        return this;
    }

    virtual CefRefPtr<CefPrintHandler> GetPrintHandler() override
    {
        return this;
    }

public:
    // CefContextMenuHandler
    virtual bool RunContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model,
        CefRefPtr<CefRunContextMenuCallback> callback) override;

public:
    // CefLifeSpanHandler
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        const CefString& target_url, const CefString& target_frame_name, WindowOpenDisposition target_disposition,
        bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client,
        CefBrowserSettings& settings, CefRefPtr<CefDictionaryValue>& extra_info, bool* no_javascript_access) override;
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

public:
    // CefDisplayHandler
    virtual void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        const CefString& url) override;
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;
    virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, cef_log_severity_t level,
        const CefString& message, const CefString& source, int line) override;

public:
    // CefDownloadHandler
    virtual void OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item,
        const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback) override;
    virtual void OnDownloadUpdated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item,
        CefRefPtr<CefDownloadItemCallback> callback) override;

public:
    // CefLoadHandler
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) override;
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        int httpStatusCode) override;

public:
    // CefRenderHandler
    virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
    virtual void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type,
        const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width,
        int height) override;

public:
    // CefPrintHandler
    virtual void OnPrintStart(CefRefPtr<CefBrowser> browser) override
    {
    }

    virtual void OnPrintSettings(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefPrintSettings> settings, bool get_defaults)
        override
    {
    }

    virtual bool OnPrintDialog(CefRefPtr<CefBrowser> browser,
        bool has_selection, CefRefPtr<CefPrintDialogCallback> callback)
        override
    {
        return false;
    }

    virtual bool OnPrintJob(CefRefPtr<CefBrowser> browser, const CefString& document_name,
        const CefString& pdf_file_path, CefRefPtr<CefPrintJobCallback> callback)
        override
    {
        return false;
    }

    virtual void OnPrintReset(CefRefPtr<CefBrowser> browser) override
    {
    }

    virtual CefSize GetPdfPaperSize(CefRefPtr<CefBrowser> browser,
        int device_units_per_inch) override
    {
        return CefSize((int32_t)(8.27 * device_units_per_inch),
            (int32_t)(11.75 * device_units_per_inch));
    }

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
