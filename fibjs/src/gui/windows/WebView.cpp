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

result_t WebView::loadURL(exlib::string url, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

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
    ((ICoreWebView2*)m_webview)->ExecuteScript((LPCWSTR)wcode.c_str(), Microsoft::WRL::Callback<ICoreWebView2ExecuteScriptCompletedHandler>([&retVal, ac](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
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
    }).Get());

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