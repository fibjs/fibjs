/*
 * WebView.cpp
 *
 *  Created on: Sep 22, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#if defined(Windows) && defined(OS_DESKTOP)

#include "object.h"
#include "ifs/gui.h"
#include "ifs/encoding.h"
#include "utf8.h"
#include "WebView.h"
#include "EventInfo.h"
#include "loader/WebView2.h"

namespace fibjs {

extern ICoreWebView2Environment* g_env;
extern const wchar_t* szWndClassMain;

class WebMessageReceivedHandler : public ICoreWebView2WebMessageReceivedEventHandler {
public:
    WebMessageReceivedHandler(WebView* webview)
        : m_webview(webview)
        , m_refCount(1)
    {
    }

    HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) override
    {
        LPWSTR message = nullptr;
        HRESULT hr = args->TryGetWebMessageAsString(&message);
        if (SUCCEEDED(hr)) {
            exlib::string receivedMessage = utf16to8String((const char16_t*)message);
            CoTaskMemFree(message);

            fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(m_webview, "message");
            ei->add("data", receivedMessage.c_str());
            m_webview->_emit("message", ei);
        } else {
            hr = args->get_WebMessageAsJson(&message);
            if (SUCCEEDED(hr)) {
                bool is_close = !qstrcmp(message, LR"({"type":"close"})");
                CoTaskMemFree(message);

                if (is_close)
                    m_webview->internal_close();
            }
        }

        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0) {
            delete this;
        }
        return refCount;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override
    {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(ICoreWebView2WebMessageReceivedEventHandler)) {
            *ppv = static_cast<ICoreWebView2WebMessageReceivedEventHandler*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

private:
    obj_ptr<WebView> m_webview;
    ULONG m_refCount;
};

class NavigationCompletedHandler : public ICoreWebView2NavigationCompletedEventHandler {
public:
    NavigationCompletedHandler(WebView* webview)
        : m_webview(webview)
        , m_refCount(1)
    {
    }

    HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) override
    {
        const wchar_t* script = L"window.postMessage = function(message) { window.chrome.webview.postMessage(message); };"
                                "window.close = function() { window.chrome.webview.postMessage({type:'close'}); };";
        sender->ExecuteScript(script, nullptr);

        m_webview->m_webview = sender;
        m_webview->m_ready->set();
        m_webview->_emit("open");

        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0) {
            delete this;
        }
        return refCount;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override
    {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(ICoreWebView2NavigationCompletedEventHandler)) {
            *ppv = static_cast<ICoreWebView2NavigationCompletedEventHandler*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

private:
    obj_ptr<WebView> m_webview;
    ULONG m_refCount;
};

class ControllerCompletedHandler : public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler {
public:
    ControllerCompletedHandler(WebView* webview)
        : m_webview(webview)
        , m_refCount(1)
    {
    }

    HRESULT STDMETHODCALLTYPE Invoke(HRESULT result, ICoreWebView2Controller* controller) override
    {
        if (FAILED(result)) {
            printf("Failed to create WebView2 controller. Error: 0x%08X\n", result);
            exit(-1);
        }

        RECT bounds;
        GetClientRect((HWND)m_webview->m_window, &bounds);
        controller->put_Bounds(bounds);

        SetWindowLongPtr((HWND)m_webview->m_window, 0, (LONG_PTR)controller);
        controller->AddRef();

        exlib::string url;
        Variant v;

        if (m_webview->m_opt->get("url", v) == 0)
            url = v.string();
        else if (m_webview->m_opt->get("file", v) == 0) {
            obj_ptr<UrlObject_base> u;
            result_t hr = url_base::pathToFileURL(v.string(), u);
            if (hr < 0)
                return hr;

            u->get_href(url);
        } else
            url = "about:blank";

        ICoreWebView2* webView = nullptr;
        controller->get_CoreWebView2(&webView);

        exlib::wstring wurl = utf8to16String(url);
        webView->Navigate((LPCWSTR)wurl.c_str());

        WebMessageReceivedHandler* messageHandler = new WebMessageReceivedHandler(m_webview);
        webView->add_WebMessageReceived(messageHandler, nullptr);

        NavigationCompletedHandler* navigationHandler = new NavigationCompletedHandler(m_webview);
        webView->add_NavigationCompleted(navigationHandler, nullptr);

        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0) {
            delete this;
        }
        return refCount;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override
    {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(ICoreWebView2CreateCoreWebView2ControllerCompletedHandler)) {
            *ppv = static_cast<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

private:
    obj_ptr<WebView> m_webview;
    ULONG m_refCount;
};

result_t WebView::open()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    m_window = CreateWindowExW(0, szWndClassMain, L"",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, hInstance, nullptr);

    g_env->CreateCoreWebView2Controller((HWND)m_window, new ControllerCompletedHandler(this));

    config();

    return 0;
}

result_t WebView::loadURL(exlib::string url, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    ICoreWebView2* webview = (ICoreWebView2*)m_webview;
    exlib::wstring wurl = utf8to16String(url);
    ((ICoreWebView2*)m_webview)->Navigate((LPCWSTR)wurl.c_str());

    return 0;
}

result_t WebView::getUrl(exlib::string& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    LPWSTR url = nullptr;
    ((ICoreWebView2*)m_webview)->get_Source(&url);
    retVal = utf16to8String((const char16_t*)url);
    CoTaskMemFree(url);

    return 0;
}

result_t WebView::setHtml(exlib::string html, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    exlib::wstring whtml = utf8to16String(html);
    ((ICoreWebView2*)m_webview)->NavigateToString((LPCWSTR)whtml.c_str());

    return 0;
}

result_t WebView::reload(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    ((ICoreWebView2*)m_webview)->Reload();

    return 0;
}

result_t WebView::goBack(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    ((ICoreWebView2*)m_webview)->GoBack();

    return 0;
}

result_t WebView::goForward(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    ((ICoreWebView2*)m_webview)->GoForward();

    return 0;
}

result_t WebView::eval(exlib::string code, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    exlib::wstring wcode = utf8to16String(code);
    ((ICoreWebView2*)m_webview)->ExecuteScript((LPCWSTR)wcode.c_str(), nullptr);

    return 0;
}

result_t WebView::close(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    internal_close();

    return 0;
}

result_t WebView::postMessage(exlib::string msg, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    encoding_base::jsstr(msg, false, msg);
    exlib::string code = "window.dispatchEvent(new MessageEvent('message', { data: '" + msg + "' }));";

    exlib::wstring wcode = utf8to16String(code);
    ((ICoreWebView2*)m_webview)->ExecuteScript((LPCWSTR)wcode.c_str(), nullptr);

    return 0;
}

}

#endif