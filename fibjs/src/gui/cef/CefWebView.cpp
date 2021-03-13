/*
 * CefWebView.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#include "ifs/json.h"
#include "options.h"
#include "CefWebView.h"
#include "gui_app.h"
#include "gui_handler.h"
#include "include/cef_parser.h"
#include "include/views/cef_window.h"
#include "include/views/cef_browser_view.h"
#include "include/cef_request_context_handler.h"
#include "../os_gui.h"

namespace fibjs {

extern exlib::LockedList<Isolate> s_isolates;

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

        if (m_webview) {
            m_webview->m_browser = browser_view_->GetBrowser();
            m_webview->config_window();
        } else
            s_isolates.head()->Ref();

        window->Show();
        browser_view_->RequestFocus();
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
    bool OnPopupBrowserViewCreated(CefRefPtr<CefBrowserView> browser_view,
        CefRefPtr<CefBrowserView> popup_browser_view, bool is_devtools) OVERRIDE
    {
        CefWindow::CreateTopLevelWindow(new GuiWindowDelegate(popup_browser_view));
        return true;
    }

private:
    IMPLEMENT_REFCOUNTING(GuiBrowserViewDelegate);
};

extern GuiApp* g_app;
CefWebView::CefWebView(exlib::string url, NObject* opt, CefRefPtr<CefValue> proxy)
    : m_opt(opt)
    , m_url(url)
    , m_proxy(proxy)
    , m_bDebug(g_app->m_bDebug)
    , m_bPopup(g_app->m_bPopup)
    , m_bMenu(g_app->m_bMenu)
    , m_bHeadless(g_app->m_bHeadless)
    , m_width(800)
    , m_height(600)
    , m_eid(0)
{
    holder()->Ref();

    m_opt = opt;
    m_url = url;

    if (m_opt) {
        Variant v;

        if (m_opt->get("debug", v) == 0)
            m_bDebug = v.boolVal();

        if (m_opt->get("popup", v) == 0)
            m_bPopup = v.boolVal();

        if (m_opt->get("menu", v) == 0)
            m_bMenu = v.boolVal();

        if (!m_bHeadless && m_opt->get("headless", v) == 0)
            m_bHeadless = v.boolVal();

        if (m_bHeadless) {
            if (opt->get("width", v) == 0)
                m_width = v.intVal();
            if (opt->get("height", v) == 0)
                m_height = v.intVal();
        }
    }

    if (m_bHeadless)
        m_bPopup = false;
}

CefWebView::~CefWebView()
{
}

void CefWebView::open()
{
    static CefRefPtr<GuiHandler> gui_handler;

    if (gui_handler == NULL)
        gui_handler = new GuiHandler();

#ifdef Darwin
    bool use_view = false;
#else
    bool use_view = !m_bHeadless;
#endif

    gui_handler->browser_list_.push_back(this);

    CefBrowserSettings browser_settings;
    browser_settings.background_color = CefColorSetARGB(255, 255, 255, 255);

    CefRefPtr<CefRequestContext> context;
    if (m_proxy) {
        CefString error;
        CefRequestContextSettings context_settings;

        context = CefRequestContext::CreateContext(context_settings, nullptr);
        context->SetPreference("proxy", m_proxy, error);
    }

    if (use_view) {
        static CefRefPtr<GuiBrowserViewDelegate> view_delegate;
        if (view_delegate == NULL)
            view_delegate = new GuiBrowserViewDelegate();

        CefRefPtr<CefBrowserView> browser_view = CefBrowserView::CreateBrowserView(gui_handler, m_url.c_str(), browser_settings,
            nullptr, context, view_delegate);
        CefWindow::CreateTopLevelWindow(new GuiWindowDelegate(browser_view, this));
    } else {
        CefWindowInfo window_info;

        if (m_bHeadless)
            window_info.windowless_rendering_enabled = true;

        m_browser = CefBrowserHost::CreateBrowserSync(window_info, gui_handler, m_url.c_str(), browser_settings,
            nullptr, context);

        if (!m_bHeadless)
            config_window();
        else
            _emit("open");
    }
}

void CefWebView::clear()
{
    m_browser = NULL;
    m_domain.clear();
    m_holder.Release();
    m_reg = NULL;
}

#ifndef Darwin
void CefWebView::config_window()
{
    os_config_window(this, (void*)m_browser->GetHost()->GetWindowHandle(), m_opt);
}
#endif

result_t CefWebView::loadUrl(exlib::string url, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (!m_browser)
        return 0;

    m_browser->GetMainFrame()->LoadURL(url.c_str());

    return 0;
}

result_t CefWebView::getUrl(exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (!m_browser)
        return 0;

    retVal = m_browser->GetMainFrame()->GetURL().ToString();

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

result_t CefWebView::printToPDF(exlib::string file, AsyncEvent* ac)
{
    class GuiPdfPrintCallback : public CefPdfPrintCallback {
    public:
        GuiPdfPrintCallback(AsyncEvent* ac)
            : m_ac(ac)
        {
        }

    public:
        virtual void OnPdfPrintFinished(const CefString& path, bool ok)
        {
            m_ac->post(ok ? 0 : CALL_E_INTERNAL);
        }

    private:
        AsyncEvent* m_ac;

    private:
        IMPLEMENT_REFCOUNTING(GuiPdfPrintCallback);
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (!m_browser)
        return 0;

    CefPdfPrintSettings settings;
    m_browser->GetHost()->PrintToPDF(file.c_str(), settings, new GuiPdfPrintCallback(ac));

    return CALL_E_PENDDING;
}

result_t CefWebView::executeJavaScript(exlib::string code, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (!m_browser)
        return 0;

    CefRefPtr<CefFrame> frame = m_browser->GetMainFrame();
    frame->ExecuteJavaScript(code.c_str(), frame->GetURL(), 0);

    return 0;
}

void CefWebView::OnDevToolsMethodResult(CefRefPtr<CefBrowser> browser, int message_id,
    bool success, const void* result, size_t result_size)
{
    m_method_lock.lock();
    std::map<int32_t, ac_method>::iterator it_method;
    it_method = m_method.find(message_id);
    if (it_method != m_method.end()) {
        it_method->second.m_retVal.setJSON(exlib::string((const char*)result, result_size));
        it_method->second.m_ac->post(0);
        m_method.erase(it_method);
    } else {
        Variant v;

        v.setJSON(exlib::string((const char*)result, result_size));
        m_result.insert(std::pair<int32_t, Variant>(message_id, v));
    }
    m_method_lock.unlock();
}

void CefWebView::OnDevToolsEvent(CefRefPtr<CefBrowser> browser, const CefString& method,
    const void* params, size_t params_size)
{
    Variant v;
    exlib::string name = method.ToString();
    size_t pos = name.find('.');

    v.setJSON(exlib::string((const char*)params, params_size));
    if (pos != exlib::string::npos) {
        exlib::string dom_name = name.substr(0, pos);
        std::map<exlib::string, obj_ptr<EventEmitter_base>>::iterator it_dom;

        it_dom = m_domain.find(dom_name);
        if (it_dom != m_domain.end())
            it_dom->second->_emit(name.substr(pos + 1), &v, 1);
    }

    _emit(name, &v, 1);
}

result_t CefWebView::executeDevToolsMethod(exlib::string method, v8::Local<v8::Object> params,
    Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        exlib::string strParam;

        json_base::encode(params, strParam);
        ac->m_ctx.resize(1);
        ac->m_ctx[0] = strParam;

        return CHECK_ERROR(CALL_E_GUICALL);
    }

    if (!m_browser)
        return 0;

    if (!m_reg.get())
        m_reg = m_browser->GetHost()->AddDevToolsMessageObserver(this);

    exlib::string strParam = ac->m_ctx[0].string();
    CefRefPtr<CefValue> _params = CefParseJSON(strParam.c_str(), strParam.length(),
        JSON_PARSER_ALLOW_TRAILING_COMMAS);

    int32_t rid = m_browser->GetHost()->ExecuteDevToolsMethod(m_eid++, method.c_str(), _params->GetDictionary());
    if (rid == 0)
        return CALL_E_INTERNAL;

    m_method_lock.lock();
    std::map<int32_t, Variant>::iterator it_method;
    it_method = m_result.find(rid);
    if (it_method != m_result.end()) {
        retVal = it_method->second;
        m_result.erase(it_method);
        ac->post(0);
    } else
        m_method.insert(std::pair<int32_t, ac_method>(rid, ac_method(retVal, ac)));
    m_method_lock.unlock();

    return CALL_E_PENDDING;
}

#ifndef Darwin
result_t CefWebView::close(AsyncEvent* ac)
{
    if (ac && ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (!m_browser)
        return 0;

    return m_browser->GetHost()->TryCloseBrowser();
}
#endif

result_t CefWebView::postMessage(exlib::string msg, AsyncEvent* ac)
{
    return 0;
}
}
