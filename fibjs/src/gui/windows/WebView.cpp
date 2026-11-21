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

#include <memory>
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

result_t WebView::loadUrl(exlib::string url)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    exlib::wstring wurl = utf8to16String(url);

    begin_load();
    ((ICoreWebView2*)m_webview)->Navigate((LPCWSTR)wurl.c_str());

    return finish_load();
}

result_t WebView::getUrl(exlib::string& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = internal_getUrl();

    return result_t();
}

result_t WebView::setHtml(exlib::string html)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    exlib::wstring whtml = utf8to16String(html);

    begin_load();
    ((ICoreWebView2*)m_webview)->NavigateToString((LPCWSTR)whtml.c_str());

    return finish_load();
}

result_t WebView::getHtml(exlib::string& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    // The WebView2 callback runs on this thread while the fiber is parked, so
    // the stack references stay valid.
    exlib::Event ev;
    exlib::string* err = new exlib::string();

    ICoreWebView2* webView = (ICoreWebView2*)m_webview;
    webView->ExecuteScript(L"document.documentElement.outerHTML.toString()",
        Microsoft::WRL::Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
            [&retVal, &ev, err](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
                // A page with no document yet (about:blank, a navigation still
                // in flight, a window being torn down) makes the script return
                // JSON null. get<std::string>() on that throws nlohmann's
                // type_error, and an exception escaping a COM callback is an
                // unhandled C++ exception: the process takes a minidump. The
                // other platforms leave retVal untouched when the script fails,
                // so do the same here instead of reporting an empty document.
                if (SUCCEEDED(errorCode) && resultObjectAsJson != nullptr) {
                    try {
                        std::string resultStr = utf16to8String((const char16_t*)resultObjectAsJson);
                        nlohmann::json jsonResult = nlohmann::json::parse(resultStr, nullptr, false);

                        if (jsonResult.is_string())
                            retVal = jsonResult.get<std::string>();
                    } catch (const std::exception&) {
                    }
                } else {
                    _com_error e(errorCode);
                    *err = utf16to8String((const char16_t*)e.ErrorMessage());
                }

                ev.set();
                return S_OK;
            })
            .Get());

    ev.wait();

    exlib::string message = *err;
    delete err;

    if (!message.empty())
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_GUI_ERROR").with_message(message);

    return result_t();
}

result_t WebView::reload()
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    begin_load();
    ((ICoreWebView2*)m_webview)->Reload();

    return finish_load();
}

result_t WebView::goBack(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    ICoreWebView2* webView = (ICoreWebView2*)m_webview;

    BOOL can = FALSE;
    webView->get_CanGoBack(&can);

    retVal = can == TRUE;
    if (!retVal)
        return result_t();

    begin_load();
    webView->GoBack();

    return finish_load();
}

result_t WebView::goForward(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    ICoreWebView2* webView = (ICoreWebView2*)m_webview;

    BOOL can = FALSE;
    webView->get_CanGoForward(&can);

    retVal = can == TRUE;
    if (!retVal)
        return result_t();

    begin_load();
    webView->GoForward();

    return finish_load();
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

result_t WebView::eval(exlib::string code, Variant& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    encoding_base::jsstr(code, false, code);
    code = "try{({result:eval(\"" + code + "\")})}catch(e){({error:e.message});}";
    exlib::wstring wcode = utf8to16String(code);

    exlib::Event ev;
    exlib::string* err = new exlib::string();

    ICoreWebView2* webView = (ICoreWebView2*)m_webview;
    webView->ExecuteScript((LPCWSTR)wcode.c_str(),
        Microsoft::WRL::Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
            [&retVal, &ev, err](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
                if (FAILED(errorCode)) {
                    _com_error e(errorCode);
                    *err = utf16to8String((const char16_t*)e.ErrorMessage());
                    ev.set();
                    return errorCode;
                }

                if (resultObjectAsJson == nullptr) {
                    *err = "The result is null";
                    ev.set();
                    return E_FAIL;
                }

                try {
                    std::string resultStr = utf16to8String((const char16_t*)resultObjectAsJson);
                    nlohmann::json jsonResult = nlohmann::json::parse(resultStr);

                    if (jsonResult.contains("error")) {
                        *err = jsonResult["error"].get<std::string>();
                    } else if (jsonResult.contains("result")) {
                        json2Variant(jsonResult["result"], retVal);
                    }
                } catch (const std::exception& e) {
                    *err = e.what();
                }

                ev.set();
                return S_OK;
            })
            .Get());

    ev.wait();

    exlib::string message = *err;
    delete err;

    if (!message.empty())
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_GUI_ERROR").with_message(message);

    return result_t();
}

result_t WebView::setTitle(exlib::string title)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    exlib::wstring wtitle = utf8to16String(title);
    SetWindowTextW((HWND)m_window, (LPCWSTR)wtitle.c_str());

    return result_t();
}

result_t WebView::getTitle(exlib::string& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    wchar_t wtitle[256];
    int cnt = GetWindowTextW((HWND)m_window, wtitle, sizeof(wtitle) / sizeof(wtitle[0]));
    retVal = utf16to8String((const char16_t*)wtitle, cnt);

    return result_t();
}

result_t WebView::isVisible(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = IsWindowVisible((HWND)m_window);

    return result_t();
}

result_t WebView::show()
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    if (!IsWindowVisible((HWND)m_window)) {
        ShowWindow((HWND)m_window, SW_SHOW);

        ICoreWebView2Controller* controller = (ICoreWebView2Controller*)GetWindowLongPtr((HWND)m_window, 0);
        controller->put_IsVisible(TRUE);
    }

    return result_t();
}

result_t WebView::hide()
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    if (IsWindowVisible((HWND)m_window))
        ShowWindow((HWND)m_window, SW_HIDE);

    return result_t();
}

extern int dpix, dpiy;

result_t WebView::setSize(int32_t width, int32_t height)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    SetWindowPos((HWND)m_window, nullptr, 0, 0, width * dpix / 96, height * dpiy / 96, SWP_NOZORDER | SWP_NOMOVE);

    return result_t();
}

result_t WebView::getSize(std::vector<double>& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    RECT rect;
    GetWindowRect((HWND)m_window, &rect);

    retVal.push_back((int32_t)(rect.right - rect.left) * 96 / dpix);
    retVal.push_back((int32_t)(rect.bottom - rect.top) * 96 / dpiy);

    return result_t();
}

result_t WebView::setPosition(int32_t left, int32_t top)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    SetWindowPos((HWND)m_window, nullptr, left * dpix / 96, top * dpiy / 96, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

    return result_t();
}

result_t WebView::getPosition(std::vector<double>& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    RECT rect;
    GetWindowRect((HWND)m_window, &rect);

    retVal.push_back((int32_t)rect.left * 96 / dpix);
    retVal.push_back((int32_t)rect.top * 96 / dpiy);

    return result_t();
}

result_t WebView::isActived(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = (GetForegroundWindow() == (HWND)m_window);

    return result_t();
}

result_t WebView::active()
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    SetForegroundWindow((HWND)m_window);

    return result_t();
}

result_t WebView::get_focused(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = (GetForegroundWindow() == (HWND)m_window);
    return result_t();
}

static void apply_fullscreen(HWND hwnd, bool on, void*& placement_holder)
{
    WINDOWPLACEMENT* placement = (WINDOWPLACEMENT*)placement_holder;

    if (on) {
        if ((GetWindowLongPtr(hwnd, GWL_STYLE) & WS_POPUP) == WS_POPUP)
            return;

        if (!placement) {
            placement = new WINDOWPLACEMENT();
            placement_holder = placement;
        }

        placement->length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(hwnd, placement);

        MONITORINFO mi = { sizeof(mi) };
        if (GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi)) {
            SetWindowLongPtr(hwnd, GWL_STYLE, GetWindowLongPtr(hwnd, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(hwnd, HWND_TOP,
                mi.rcMonitor.left, mi.rcMonitor.top,
                mi.rcMonitor.right - mi.rcMonitor.left,
                mi.rcMonitor.bottom - mi.rcMonitor.top,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } else {
        if ((GetWindowLongPtr(hwnd, GWL_STYLE) & WS_POPUP) != WS_POPUP)
            return;

        SetWindowLongPtr(hwnd, GWL_STYLE, GetWindowLongPtr(hwnd, GWL_STYLE) | WS_OVERLAPPEDWINDOW);

        if (placement) {
            SetWindowPlacement(hwnd, placement);
            delete placement;
            placement_holder = nullptr;
        }

        SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

static exlib::string window_state(HWND hwnd, bool fullscreen)
{
    if (fullscreen)
        return "fullscreen";

    if (IsIconic(hwnd))
        return "minimized";

    if (IsZoomed(hwnd))
        return "maximized";

    return "normal";
}

result_t WebView::get_state(exlib::string& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = window_state((HWND)m_window, m_fullscreen);
    return result_t();
}

result_t WebView::set_state(exlib::string newVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    HWND hwnd = (HWND)m_window;

    if (newVal == "normal") {
        if (m_fullscreen)
            apply_fullscreen(hwnd, false, m_placement);
        else if (IsIconic(hwnd) || IsZoomed(hwnd))
            ShowWindow(hwnd, SW_RESTORE);

        m_fullscreen = false;
    } else if (newVal == "minimized") {
        if (m_fullscreen)
            apply_fullscreen(hwnd, false, m_placement);

        m_fullscreen = false;
        ShowWindow(hwnd, SW_MINIMIZE);
    } else if (newVal == "maximized") {
        if (m_fullscreen)
            apply_fullscreen(hwnd, false, m_placement);

        m_fullscreen = false;
        ShowWindow(hwnd, SW_MAXIMIZE);
    } else if (newVal == "fullscreen") {
        apply_fullscreen(hwnd, true, m_placement);
        m_fullscreen = true;
    } else
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("WebView: state must be 'normal', 'minimized', 'maximized' or 'fullscreen', but got '" + newVal + "'");

    return result_t();
}

result_t WebView::getBounds(obj_ptr<GetBoundsType>& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    RECT rect;
    GetWindowRect((HWND)m_window, &rect);

    obj_ptr<GetBoundsType> bounds = new GetBoundsType();
    bounds->x = (int32_t)rect.left * 96 / dpix;
    bounds->y = (int32_t)rect.top * 96 / dpiy;
    bounds->width = (int32_t)(rect.right - rect.left) * 96 / dpix;
    bounds->height = (int32_t)(rect.bottom - rect.top) * 96 / dpiy;

    retVal = bounds;
    return result_t();
}

result_t WebView::setBounds(WindowBounds* bounds)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    HWND hwnd = (HWND)m_window;
    RECT rect;
    GetWindowRect(hwnd, &rect);

    int32_t x = (int32_t)rect.left * 96 / dpix;
    int32_t y = (int32_t)rect.top * 96 / dpiy;
    int32_t width = (int32_t)(rect.right - rect.left) * 96 / dpix;
    int32_t height = (int32_t)(rect.bottom - rect.top) * 96 / dpiy;

    // SWP_NOMOVE / SWP_NOSIZE apply to the whole call, not to one axis: a
    // rectangle that carries only x still has to move, so decide once from the
    // fields the caller supplied and keep the rest at its current value.
    bool move = bounds->x.has_value() || bounds->y.has_value();
    bool resize = bounds->width.has_value() || bounds->height.has_value();

    UINT flags = SWP_NOZORDER;

    if (!move)
        flags |= SWP_NOMOVE;
    if (!resize)
        flags |= SWP_NOSIZE;

    if (bounds->x.has_value())
        x = (int32_t)bounds->x.value();
    if (bounds->y.has_value())
        y = (int32_t)bounds->y.value();
    if (bounds->width.has_value())
        width = (int32_t)bounds->width.value();
    if (bounds->height.has_value())
        height = (int32_t)bounds->height.value();

    SetWindowPos(hwnd, nullptr, x * dpix / 96, y * dpiy / 96, width * dpix / 96, height * dpiy / 96, flags);

    return result_t();
}

result_t WebView::focus(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    HWND hwnd = (HWND)m_window;
    SetForegroundWindow(hwnd);

    retVal = (GetForegroundWindow() == hwnd);
    return result_t();
}

result_t WebView::takeScreenshot(bool fullPage, obj_ptr<Buffer_base>& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    exlib::Event ev;
    exlib::string* err = new exlib::string();

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
            [&retVal, &ev, err](HRESULT errorCode, LPCWSTR resultJson) -> HRESULT {
                if (FAILED(errorCode) || resultJson == nullptr) {
                    *err = "Failed to capture screenshot";
                    ev.set();
                    return errorCode;
                }

                exlib::string result = utf16to8String((const char16_t*)resultJson);
                auto base64Start = result.find("\"data\":\"") + 8;
                auto base64End = result.find("\"", base64Start);
                exlib::string base64Data = result.substr(base64Start, base64End - base64Start);

                base64_base::decode(base64Data, retVal);

                ev.set();
                return S_OK;
            })
            .Get());

    ev.wait();

    exlib::string message = *err;
    delete err;

    if (!message.empty())
        return result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code("ERR_NOT_SUPPORTED").with_message(message);

    return result_t();
}

result_t WebView::postMessage(exlib::string msg)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    encoding_base::jsstr(msg, false, msg);
    exlib::string code = "window.dispatchEvent(new MessageEvent('message', { data: '" + msg + "' }));";

    exlib::wstring wcode = utf8to16String(code);
    ((ICoreWebView2*)m_webview)->ExecuteScript((LPCWSTR)wcode.c_str(), nullptr);

    return result_t();
}

}

#endif