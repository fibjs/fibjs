/*
 * gui_handler.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#include "object.h"
#include "ifs/console.h"

#include <sstream>
#include <string>

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/cef_parser.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "gui_handler.h"

namespace fibjs {

GuiHandler* g_instance = nullptr;

inline std::string GetDataURI(const std::string& data, const std::string& mime_type)
{
    return "data:" + mime_type + ";base64," + CefURIEncode(CefBase64Encode(data.data(), data.size()), false).ToString();
}

GuiHandler::GuiHandler()
{
    g_instance = this;
}

GuiHandler::~GuiHandler()
{
    g_instance = nullptr;
}

GuiHandler* GuiHandler::GetInstance()
{
    return g_instance;
}

void GuiHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
    const CefString& title)
{
    CefRefPtr<CefBrowserView> browser_view = CefBrowserView::GetForBrowser(browser);
    if (browser_view) {
        CefRefPtr<CefWindow> window = browser_view->GetWindow();
        if (window)
            window->SetTitle(title);
    }else
        PlatformTitleChange(browser, title);
}

void GuiHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
}

GuiHandler::BrowserList::iterator GuiHandler::fromBrowser(CefRefPtr<CefBrowser> browser)
{
    BrowserList::iterator bit = browser_list_.begin();
    for (; bit != browser_list_.end(); ++bit) {
        CefRefPtr<CefBrowser> _browser = (*bit)->m_browser;
        if (_browser && _browser->IsSame(browser))
            break;
    }

    return bit;
}

void GuiHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    BrowserList::iterator bit = fromBrowser(browser);
    if (bit != browser_list_.end()) {
        (*bit)->_emit("closed");
        (*bit)->holder()->Unref();
        browser_list_.erase(bit);
    }
}

bool GuiHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
    cef_log_severity_t level, const CefString& message,
    const CefString& source, int line)
{
    static int32_t console_level[] = {
        console_base::_NOTSET,
        console_base::_DEBUG,
        console_base::_INFO,
        console_base::_WARN,
        console_base::_ERROR,
        console_base::_FATAL
    };

    outLog(console_level[level], message.ToString());
    return true;
}

void GuiHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
    ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
    if (errorCode == ERR_ABORTED)
        return;

    std::stringstream ss;
    ss << "<html><body bgcolor=\"white\">"
          "<h2>Failed to load URL "
       << std::string(failedUrl) << " with error " << std::string(errorText)
       << " (" << errorCode << ").</h2></body></html>";

    frame->LoadURL(GetDataURI(ss.str(), "text/html"));
}

void GuiHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    if (frame->IsMain()) {
        BrowserList::iterator bit = fromBrowser(browser);
        if (bit != browser_list_.end())
            (*bit)->_emit("load");
    }
}
}