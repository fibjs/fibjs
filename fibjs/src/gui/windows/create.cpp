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
#include <shlwapi.h>
#include "loader/WebView2.h"

#include "object.h"
#include "ifs/gui.h"
#include "ifs/fs.h"
#include "ifs/encoding.h"
#include "ifs/mime.h"
#include "utf8.h"
#include "WebView.h"
#include "EventInfo.h"
#include "Buffer.h"

namespace fibjs {

extern ICoreWebView2Environment* g_env;
extern const wchar_t* szWndClassMain;

exlib::string fs_url_to_path(const exlib::string& url)
{
    obj_ptr<UrlObject_base> u;

    result_t hr = url_base::parse(url, false, false, u);
    if (hr < 0)
        return "";

    u->set_protocol("file:");

    exlib::string path;
    url_base::fileURLToPath(u, path);

    return path;
}

result_t WebView::createWebView()
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

                ICoreWebView2* webView = nullptr;
                controller->get_CoreWebView2(&webView);

                controller->MoveFocus(COREWEBVIEW2_MOVE_FOCUS_REASON_PROGRAMMATIC);

                if (!m_options->devtools.value()) {
                    ICoreWebView2Settings* settings = nullptr;
                    webView->get_Settings(&settings);
                    settings->put_AreDevToolsEnabled(FALSE);
                    settings->Release();
                }

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
                                    if (!qstrcmp(message, LR"({"type":"close"})"))
                                        internal_close();
                                    else if (!qstrcmp(message, LR"({"type":"minimize"})"))
                                        internal_minimize();
                                    else if (!qstrcmp(message, LR"({"type":"maximize"})"))
                                        internal_maximize();
                                    else if (!qstrcmp(message, LR"({"type":"drag"})")) {
                                        ReleaseCapture();
                                        PostMessage((HWND)m_window, WM_NCLBUTTONDOWN, HTCAPTION, 0);
                                    }

                                    CoTaskMemFree(message);
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

                webView->add_NavigationStarting(
                    Microsoft::WRL::Callback<ICoreWebView2NavigationStartingEventHandler>(
                        [this](ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT {
                            m_isLoading = true;

                            obj_ptr<EventInfo> ei = new EventInfo(this, "loading");

                            LPWSTR uri = nullptr;
                            args->get_Uri(&uri);
                            ei->add("url", utf16to8String((const char16_t*)uri));
                            CoTaskMemFree(uri);

                            _emit("loading", ei);
                            return S_OK;
                        })
                        .Get(),
                    nullptr);

                webView->add_ContentLoading(
                    Microsoft::WRL::Callback<ICoreWebView2ContentLoadingEventHandler>(
                        [this](ICoreWebView2* sender, IUnknown* args) -> HRESULT {
                            const wchar_t* script = L"window.postMessage = function(message) { window.chrome.webview.postMessage(message); };"
                                                    "window.close = function() { window.chrome.webview.postMessage({type:'close'}); };"
                                                    "window.minimize = function() { window.chrome.webview.postMessage({type:'minimize'}); };"
                                                    "window.maximize = function() { window.chrome.webview.postMessage({type:'maximize'}); };"
                                                    "window.drag = function() { window.chrome.webview.postMessage({type:'drag'}); };";
                            sender->ExecuteScript(script, nullptr);

                            m_webview = sender;
                            m_ready->set();

                            return S_OK;
                        })
                        .Get(),
                    nullptr);

                webView->add_NavigationCompleted(
                    Microsoft::WRL::Callback<ICoreWebView2NavigationCompletedEventHandler>(
                        [this](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
                            m_isLoading = false;
                            obj_ptr<EventInfo> ei = new EventInfo(this, "load");

                            LPWSTR uri = nullptr;
                            sender->get_Source(&uri);
                            ei->add("url", utf16to8String((const char16_t*)uri));
                            CoTaskMemFree(uri);

                            _emit("load", ei);
                            return S_OK;
                        })
                        .Get(),
                    nullptr);

                webView->add_WebResourceRequested(
                    Microsoft::WRL::Callback<ICoreWebView2WebResourceRequestedEventHandler>(
                        [](ICoreWebView2* sender, ICoreWebView2WebResourceRequestedEventArgs* args) -> HRESULT {
                            Microsoft::WRL::ComPtr<ICoreWebView2WebResourceRequest> request;
                            args->get_Request(&request);

                            LPWSTR uri = nullptr;
                            request->get_Uri(&uri);
                            exlib::string url = utf16to8String((const char16_t*)uri);
                            CoTaskMemFree(uri);

                            ICoreWebView2Deferral* deferral;
                            args->GetDeferral(&deferral);

                            args->AddRef();
                            async([url, args, deferral]() {
                                exlib::string fname = fs_url_to_path(url);
                                Variant var;
                                result_t hr = fs_base::cc_readFile(fname, "", var, Isolate::main());

                                if (hr >= 0) {
                                    async([fname, args, deferral, var]() {
                                        Buffer* _buf = (Buffer*)var.object();

                                        Microsoft::WRL::ComPtr<IStream> responseStream;
                                        responseStream = _buf ? SHCreateMemStream(_buf->data(), _buf->length())
                                                              : SHCreateMemStream((const BYTE*)"", 0);

                                        exlib::string mtype;
                                        mime_base::getType(fname, mtype);

                                        mtype = "Content-Type: " + mtype;
                                        exlib::wstring wmtype = utf8to16String(mtype);

                                        Microsoft::WRL::ComPtr<ICoreWebView2WebResourceResponse> response;
                                        g_env->CreateWebResourceResponse(
                                            responseStream.Get(), 200, L"OK", (LPCWSTR)wmtype.c_str(), &response);

                                        args->put_Response(response.Get());
                                        deferral->Complete();

                                        args->Release();
                                        deferral->Release();
                                    },
                                        CALL_E_GUICALL);
                                } else {
                                    async([fname, args, deferral]() {
                                        exlib::string page = "<html><body><h1>Error</h1><p>File not found at path:<br>" + fname + "</p></body></html>";
                                        Microsoft::WRL::ComPtr<IStream> responseStream;
                                        responseStream = SHCreateMemStream((const BYTE*)page.c_str(), page.length());

                                        Microsoft::WRL::ComPtr<ICoreWebView2WebResourceResponse> response;
                                        g_env->CreateWebResourceResponse(
                                            responseStream.Get(), 404, L"Not Found", L"Content-Type: text/html", &response);

                                        args->put_Response(response.Get());
                                        deferral->Complete();

                                        args->Release();
                                        deferral->Release();
                                    },
                                        CALL_E_GUICALL);
                                }
                            });

                            return S_OK;
                        })
                        .Get(),
                    nullptr);

                webView->AddWebResourceRequestedFilter(L"fs:*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);

                exlib::string url;
                if (m_options->url.has_value())
                    url = m_options->url.value();
                else if (m_options->file.has_value()) {
                    obj_ptr<UrlObject_base> u;
                    result_t hr = url_base::pathToFileURL(m_options->file.value(), u);
                    if (hr < 0)
                        return hr;

                    u->set_protocol("fs:");
                    u->set_slashes(true);

                    u->get_href(url);
                } else
                    url = "about:blank";

                exlib::wstring wurl = utf8to16String(url);
                webView->Navigate((LPCWSTR)wurl.c_str());

                return S_OK;
            })
            .Get());

    config();

    return 0;
}
}

#endif