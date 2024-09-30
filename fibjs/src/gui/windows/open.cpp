/*
 * WebView.cpp
 *
 *  Created on: Sep 22, 2024
 *      Author: lion
 */

#ifdef _WIN32

#include <uv/include/uv.h>
#include <windows.h>
#include <wrl.h>
#include "loader/WebView2.h"

#include "object.h"
#include "ifs/gui.h"
#include "ifs/encoding.h"
#include "utf8.h"
#include "WebView.h"
#include "EventInfo.h"

namespace fibjs {

extern ICoreWebView2Environment* g_env;
extern const wchar_t* szWndClassMain;

result_t WebView::open()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    m_window = CreateWindowExW(0, szWndClassMain, L"",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, hInstance, nullptr);

    g_env->CreateCoreWebView2Controller((HWND)m_window,
        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
            [this](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
                if (FAILED(result)) {
                    printf("Failed to create WebView2 controller. Error: 0x%08X\n", result);
                    return result;
                }

                RECT bounds;
                GetClientRect((HWND)m_window, &bounds);
                controller->put_Bounds(bounds);

                SetWindowLongPtr((HWND)m_window, 0, (LONG_PTR)controller);
                controller->AddRef();

                exlib::string url;
                Variant v;

                if (m_opt->get("url", v) == 0)
                    url = v.string();
                else if (m_opt->get("file", v) == 0) {
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

                webView->add_WebMessageReceived(
                    Microsoft::WRL::Callback<ICoreWebView2WebMessageReceivedEventHandler>(
                        [this](ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
                            LPWSTR message = nullptr;
                            HRESULT hr = args->TryGetWebMessageAsString(&message);
                            if (SUCCEEDED(hr)) {
                                exlib::string receivedMessage = utf16to8String((const char16_t*)message);
                                CoTaskMemFree(message);

                                fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(this, "message");
                                ei->add("data", receivedMessage.c_str());
                                _emit("message", ei);
                            } else {
                                hr = args->get_WebMessageAsJson(&message);
                                if (SUCCEEDED(hr)) {
                                    bool is_close = !qstrcmp(message, LR"({"type":"close"})");
                                    CoTaskMemFree(message);

                                    if (is_close)
                                        internal_close();
                                }
                            }
                            return S_OK;
                        })
                        .Get(),
                    nullptr);

                webView->add_DocumentTitleChanged(
                    Microsoft::WRL::Callback<ICoreWebView2DocumentTitleChangedEventHandler>(
                        [this](ICoreWebView2* sender, IUnknown* args) -> HRESULT {
                            LPWSTR title = nullptr;
                            HRESULT hr = sender->get_DocumentTitle(&title);
                            if (SUCCEEDED(hr)) {
                                SetWindowTextW((HWND)m_window, title);
                                CoTaskMemFree(title);
                            }
                            return S_OK;
                        })
                        .Get(),
                    nullptr);

                webView->add_NavigationCompleted(
                    Microsoft::WRL::Callback<ICoreWebView2NavigationCompletedEventHandler>(
                        [this](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
                            const wchar_t* script = L"window.postMessage = function(message) { window.chrome.webview.postMessage(message); };"
                                                    "window.close = function() { window.chrome.webview.postMessage({type:'close'}); };";
                            sender->ExecuteScript(script, nullptr);

                            m_webview = sender;
                            m_ready->set();
                            _emit("open");

                            return S_OK;
                        })
                        .Get(),
                    nullptr);

                return S_OK;
            })
            .Get());

    config();

    return 0;
}

}

#endif