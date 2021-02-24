/*
 * CefWebView.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#include "CefWebView.h"
#include "gui_handler.h"
#include "include/cef_parser.h"
#include "include/views/cef_window.h"
#include "include/views/cef_browser_view.h"

namespace fibjs {

class GuiWindowDelegate : public CefWindowDelegate {
public:
    explicit GuiWindowDelegate(CefRefPtr<CefBrowserView> browser_view, CefWebView* webview = NULL)
        : browser_view_(browser_view)
        , m_webview(webview)
    {
    }

    void OnWindowCreated(CefRefPtr<CefWindow> window) OVERRIDE
    {
        window->AddChildView(browser_view_);
        window->Show();

        browser_view_->RequestFocus();

        if (m_webview) {
            m_webview->m_browser = browser_view_->GetBrowser();
            m_webview->_emit("open");
        }
    }

    void OnWindowDestroyed(CefRefPtr<CefWindow> window) OVERRIDE
    {
        browser_view_ = nullptr;
        m_webview.Release();
    }

    bool CanClose(CefRefPtr<CefWindow> window) OVERRIDE
    {
        CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
        if (browser)
            return browser->GetHost()->TryCloseBrowser();
        return true;
    }

    CefSize GetPreferredSize(CefRefPtr<CefView> view) OVERRIDE
    {
        return CefSize(800, 600);
    }

private:
    CefRefPtr<CefBrowserView> browser_view_;
    obj_ptr<CefWebView> m_webview;

    IMPLEMENT_REFCOUNTING(GuiWindowDelegate);
};

class GuiBrowserViewDelegate : public CefBrowserViewDelegate {
public:
    GuiBrowserViewDelegate() {}

    bool OnPopupBrowserViewCreated(CefRefPtr<CefBrowserView> browser_view,
        CefRefPtr<CefBrowserView> popup_browser_view, bool is_devtools) OVERRIDE
    {
        CefWindow::CreateTopLevelWindow(new GuiWindowDelegate(popup_browser_view));
        return true;
    }

private:
    IMPLEMENT_REFCOUNTING(GuiBrowserViewDelegate);
};

CefWebView::CefWebView(exlib::string url, NObject* opt)
    : m_url(url)
    , m_opt(opt)
{
    holder()->Ref();

    m_url = url;
    m_opt = opt;
}

CefWebView::~CefWebView()
{
}

void CefWebView::open()
{
    static CefRefPtr<GuiHandler> handler;
    if (handler == NULL)
        handler = new GuiHandler();

    handler->browser_list_.push_back(this);

#ifdef Darwin
    CefWindowInfo window_info;
    m_browser = CefBrowserHost::CreateBrowserSync(window_info, handler, m_url.c_str(), browser_settings,
        nullptr, nullptr);
    _emit("open");
#else
    static CefRefPtr<GuiBrowserViewDelegate> view_delegate;
    if (view_delegate == NULL)
        view_delegate = new GuiBrowserViewDelegate();

    CefRefPtr<CefBrowserView> browser_view = CefBrowserView::CreateBrowserView(handler, m_url.c_str(), browser_settings,
        nullptr, nullptr, view_delegate);
    CefWindow::CreateTopLevelWindow(new GuiWindowDelegate(browser_view, this));
#endif
}

result_t CefWebView::loadUrl(exlib::string url, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (m_browser)
        m_browser->GetMainFrame()->LoadURL(url.c_str());

    return 0;
}

inline std::string GetDataURI(const exlib::string& data, const std::string& mime_type)
{
    return "data:" + mime_type + ";base64," + CefURIEncode(CefBase64Encode(data.c_str(), data.length()), false).ToString();
}

result_t CefWebView::setHtml(exlib::string html, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (m_browser)
        m_browser->GetMainFrame()->LoadURL(GetDataURI(html, "text/html"));

    return 0;
}

result_t CefWebView::reload(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (m_browser)
        m_browser->Reload();

    return 0;
}

result_t CefWebView::goBack(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (m_browser)
        m_browser->GoBack();

    return 0;
}

result_t CefWebView::goForward(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (m_browser)
        m_browser->GoForward();

    return 0;
}

result_t CefWebView::print(int32_t mode, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (m_browser)
        m_browser->GetHost()->Print();

    return 0;
}

result_t CefWebView::close(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (m_browser)
        m_browser->GetHost()->TryCloseBrowser();

    return 0;
}

result_t CefWebView::postMessage(exlib::string msg, AsyncEvent* ac)
{
    return 0;
}

result_t CefWebView::get_visible(bool& retVal)
{
    return 0;
}

result_t CefWebView::set_visible(bool newVal)
{
    return 0;
}
}
