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
#include <comdef.h>

#include "object.h"
#include "ifs/gui.h"
#include "ifs/encoding.h"
#include "utf8.h"
#include "WebView.h"
#include "EventInfo.h"
#include "loader/WebView2.h"

#include <nlohmann/json.hpp>

namespace fibjs {

void WebView::internal_close()
{
    HWND hWndParent = (HWND)m_window;
    SendMessage(hWndParent, WM_CLOSE, 0, 0);
}

void WebView::internal_minimize()
{
    HWND hWndParent = (HWND)m_window;
    ShowWindow(hWndParent, SW_MINIMIZE);
}

void WebView::internal_maximize()
{
    HWND hWndParent = (HWND)m_window;

    if (IsZoomed(hWndParent))
        ShowWindow(hWndParent, SW_RESTORE);
    else
        ShowWindow(hWndParent, SW_MAXIMIZE);
}

bool WebView::internal_isReady()
{
    return !m_isLoading;
}

void WebView::internal_eval(exlib::string code)
{
    exlib::wstring wcode = utf8to16String(code);
    ((ICoreWebView2*)m_webview)->ExecuteScript((LPCWSTR)wcode.c_str(), nullptr);
}

exlib::string WebView::internal_getUrl()
{
    LPWSTR url = nullptr;
    ((ICoreWebView2*)m_webview)->get_Source(&url);
    exlib::string surl = utf16to8String((const char16_t*)url);
    CoTaskMemFree(url);

    return surl;
}

result_t WebView::loadUrl(exlib::string url, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    exlib::wstring wurl = utf8to16String(url);
    ((ICoreWebView2*)m_webview)->Navigate((LPCWSTR)wurl.c_str());
    m_isLoading = true;

    return 0;
}

result_t WebView::getUrl(exlib::string& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    retVal = internal_getUrl();

    return 0;
}

result_t WebView::setHtml(exlib::string html, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    exlib::wstring whtml = utf8to16String(html);
    ((ICoreWebView2*)m_webview)->NavigateToString((LPCWSTR)whtml.c_str());
    m_isLoading = true;

    return 0;
}

result_t WebView::getHtml(exlib::string& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    ICoreWebView2* webView = (ICoreWebView2*)m_webview;
    webView->ExecuteScript(L"document.documentElement.outerHTML.toString()",
        Microsoft::WRL::Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
            [&retVal, ac](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
                if (SUCCEEDED(errorCode)) {
                    std::string resultStr = utf16to8String((const char16_t*)resultObjectAsJson);
                    nlohmann::json jsonResult = nlohmann::json::parse(resultStr);

                    retVal = jsonResult.get<std::string>();
                }
                ac->post(0);
                return S_OK;
            })
            .Get());

    return CALL_E_PENDDING;
}

result_t WebView::isReady(bool& retVal, AsyncEvent* ac)
{
    bool is_win_ready = false;
    m_ready->isSet(is_win_ready);
    if (!is_win_ready) {
        retVal = false;
        return 0;
    }

    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    retVal = !m_isLoading;

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

void json2Variant(nlohmann::json& json, Variant& retVal)
{
    if (json.is_null()) {
        retVal.setNull();
    } else if (json.is_boolean()) {
        retVal = json.get<bool>();
    } else if (json.is_number_integer()) {
        retVal = json.get<int>();
    } else if (json.is_number_unsigned()) {
        retVal = json.get<double>();
    } else if (json.is_number_float()) {
        retVal = json.get<float>();
    } else if (json.is_number()) {
        retVal = json.get<double>();
    } else if (json.is_string()) {
        retVal = json.get<std::string>();
    } else if (json.is_array()) {
        obj_ptr<NArray> array = new NArray();
        for (auto& item : json) {
            Variant v;
            json2Variant(item, v);
            array->append(v);
        }
        retVal = array;
    } else if (json.is_object()) {
        obj_ptr<NObject> obj = new NObject();
        for (auto& item : json.items()) {
            Variant v;
            json2Variant(item.value(), v);
            obj->add(item.key(), v);
        }
        retVal = obj;
    }
}

result_t WebView::eval(exlib::string code, Variant& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    encoding_base::jsstr(code, false, code);
    code = "try{({result:eval(\"" + code + "\")})}catch(e){({error:e.message});}";
    exlib::wstring wcode = utf8to16String(code);

    ICoreWebView2* webView = (ICoreWebView2*)m_webview;
    webView->ExecuteScript((LPCWSTR)wcode.c_str(),
        Microsoft::WRL::Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
            [&retVal, ac](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
                if (FAILED(errorCode)) {
                    _com_error err(errorCode);
                    ac->post(Runtime::setError(utf16to8String((const char16_t*)err.ErrorMessage())));
                    return errorCode;
                }

                if (resultObjectAsJson == nullptr) {
                    ac->post(Runtime::setError("The result is null"));
                    return E_FAIL;
                }

                try {
                    std::string resultStr = utf16to8String((const char16_t*)resultObjectAsJson);
                    nlohmann::json jsonResult = nlohmann::json::parse(resultStr);

                    if (jsonResult.contains("error")) {
                        ac->post(Runtime::setError(jsonResult["error"].get<std::string>()));
                    } else {
                        if (jsonResult.contains("result"))
                            json2Variant(jsonResult["result"], retVal);
                        ac->post(0);
                    }
                } catch (const std::exception& e) {
                    ac->post(Runtime::setError(e.what()));
                }

                return S_OK;
            })
            .Get());

    return CALL_E_PENDDING;
}

result_t WebView::setTitle(exlib::string title, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    exlib::wstring wtitle = utf8to16String(title);
    SetWindowTextW((HWND)m_window, (LPCWSTR)wtitle.c_str());

    return 0;
}

result_t WebView::getTitle(exlib::string& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    wchar_t wtitle[256];
    int cnt = GetWindowTextW((HWND)m_window, wtitle, sizeof(wtitle) / sizeof(wtitle[0]));
    retVal = utf16to8String((const char16_t*)wtitle, cnt);

    return 0;
}

result_t WebView::isVisible(bool& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    retVal = IsWindowVisible((HWND)m_window);

    return 0;
}

result_t WebView::show(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    if (!IsWindowVisible((HWND)m_window)) {
        ShowWindow((HWND)m_window, SW_SHOW);

        ICoreWebView2Controller* controller = (ICoreWebView2Controller*)GetWindowLongPtr((HWND)m_window, 0);
        controller->put_IsVisible(TRUE);
    }

    return 0;
}

result_t WebView::hide(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    if (IsWindowVisible((HWND)m_window)) {
        ShowWindow((HWND)m_window, SW_HIDE);
    }

    return 0;
}

extern int dpix, dpiy;

result_t WebView::setSize(int32_t width, int32_t height, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    SetWindowPos((HWND)m_window, nullptr, 0, 0, width * dpix / 96, height * dpiy / 96, SWP_NOZORDER | SWP_NOMOVE);

    return 0;
}

result_t WebView::getSize(obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    RECT rect;
    GetWindowRect((HWND)m_window, &rect);

    retVal = new NArray();
    retVal->append((int32_t)(rect.right - rect.left) * 96 / dpix);
    retVal->append((int32_t)(rect.bottom - rect.top) * 96 / dpiy);

    return 0;
}

result_t WebView::setPosition(int32_t left, int32_t top, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    SetWindowPos((HWND)m_window, nullptr, left * dpix / 96, top * dpiy / 96, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

    return 0;
}

result_t WebView::getPosition(obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    RECT rect;
    GetWindowRect((HWND)m_window, &rect);

    retVal = new NArray();
    retVal->append((int32_t)rect.left * 96 / dpix);
    retVal->append((int32_t)rect.top * 96 / dpiy);

    return 0;
}

result_t WebView::isActived(bool& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    HWND hwnd = (HWND)m_window;
    retVal = (GetForegroundWindow() == hwnd);

    return 0;
}

result_t WebView::active(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    HWND hwnd = (HWND)m_window;
    SetForegroundWindow(hwnd);

    return 0;
}

result_t WebView::takeScreenshot(bool fullPage, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    ICoreWebView2* webView = (ICoreWebView2*)m_webview;

    exlib::string command = fullPage ? R"({
        "format": "png",
        "fromSurface": true,
        "captureBeyondViewport": true
    })"
                                     : R"({
        "format": "png",
        "fromSurface": true,
        "captureBeyondViewport": false
    })";

    exlib::wstring wcommand = utf8to16String(command);
    webView->CallDevToolsProtocolMethod(
        L"Page.captureScreenshot", (LPWSTR)wcommand.c_str(),
        Microsoft::WRL::Callback<ICoreWebView2CallDevToolsProtocolMethodCompletedHandler>(
            [&retVal, ac](HRESULT errorCode, LPCWSTR resultJson) -> HRESULT {
                if (FAILED(errorCode)) {
                    ac->post(Runtime::setError("Failed to capture screenshot"));
                    return errorCode;
                }

                exlib::string result = utf16to8String((const char16_t*)resultJson);
                auto base64Start = result.find("\"data\":\"") + 8;
                auto base64End = result.find("\"", base64Start);
                exlib::string base64Data = result.substr(base64Start, base64End - base64Start);

                base64_base::decode(base64Data, retVal);

                ac->post(0);

                return S_OK;
            })
            .Get());

    return CALL_E_PENDDING;
}

result_t WebView::close(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    m_options->hideOnClose = false;
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