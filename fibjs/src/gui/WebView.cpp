/*
 * WebView.cpp
 *
 *  Created on: Nov 2, 2024
 *      Author: lion
 */

#ifdef OS_DESKTOP

#include "object.h"
#include "ifs/gui.h"
#include "gui.h"

#include "WebView.h"
#include "Fiber.h"

namespace fibjs {

result_t WebView::loadFile(exlib::string file, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    obj_ptr<UrlObject_base> u;
    hr = url_base::pathToFileURL(file, v8::Local<v8::Object>(), u);
    if (hr < 0)
        return hr;

    exlib::string url;
    u->get_href(url);

    url = "fs:" + url.substr(5);

    return loadUrl(url, ac);
}

result_t WebView::waitFor(exlib::string url, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    if (internal_isReady() && (url.empty() || url == internal_getUrl()))
        return 0;

    m_waitFor.push_back({ url, ac });

    return CALL_E_PENDDING;
}

void WebView::postWaitFor(exlib::string url)
{
    for (auto it = m_waitFor.begin(); it != m_waitFor.end();) {
        if (it->first.empty() || it->first == url) {
            it->second->post(0);
            it = m_waitFor.erase(it);
        } else {
            ++it;
        }
    }
}

void WebView::app_rpc(exlib::string json)
{
    holder()->sync([this, json]() {
        Isolate* isolate = holder();
        v8::HandleScope handle_scope(isolate->m_isolate);
        JSFiber::EnterJsScope s;
        v8::Local<v8::Context> context = isolate->context();
        v8::Local<v8::Object> result = v8::Object::New(isolate->m_isolate);
        result_t hr;

        v8::Local<v8::Value> v = v8::JSON::Parse(context, isolate->NewString(json)).FromMaybe(v8::Local<v8::Value>());
        if (v.IsEmpty() || !v->IsObject())
            return 0;
        v8::Local<v8::Object> req = v.As<v8::Object>();

        v8::Local<v8::String> keyId = isolate->NewString("id");
        v = req->Get(context, keyId).FromMaybe(v8::Local<v8::Value>());
        if (v.IsEmpty() || !v->IsString())
            return 0;
        result->Set(context, keyId, v).IsJust();

        do {
            v = GetPrivate("app");
            if (v.IsEmpty() || !v->IsObject()) {
                result->Set(context, isolate->NewString("error"), isolate->NewString("app is required")).IsJust();
                break;
            }

            v8::Local<v8::Object> app = v.As<v8::Object>();

            exlib::string method;
            hr = GetConfigValue(req, "method", method, true);
            if (hr < 0) {
                result->Set(context, isolate->NewString("error"), isolate->NewString("method is required")).IsJust();
                break;
            }

            size_t start = 0;
            size_t pos = method.find('.');
            while (start < method.length()) {
                exlib::string name = pos == exlib::string::npos ? method.substr(start) : method.substr(start, pos - start);
                v = app->Get(context, isolate->NewString(name)).FromMaybe(v8::Local<v8::Value>());
                if (v.IsEmpty() || !v->IsObject()) {
                    app = v8::Local<v8::Object>();
                    break;
                }
                app = v.As<v8::Object>();

                start = pos == exlib::string::npos ? pos : pos + 1;
                pos = method.find('.', start);
            }

            if (app.IsEmpty() || app->IsUndefined()) {
                result->Set(context, isolate->NewString("error"), isolate->NewString("method '" + method + "' is not found")).IsJust();
                break;
            }

            if (!app->IsFunction()) {
                result->Set(context, isolate->NewString("error"), isolate->NewString("method '" + method + "' is not a function")).IsJust();
                break;
            }

            JSFunction func = app.As<v8::Function>();

            std::vector<v8::Local<v8::Value>> params;
            hr = GetConfigValue(req, "params", params);
            if (hr < 0) {
                result->Set(context, isolate->NewString("error"), isolate->NewString("params is required")).IsJust();
                break;
            }

            TryCatch try_catch;
            v8::Local<v8::Value> result1 = func.Call(app, params.size(), params.data());
            if (try_catch.HasCaught()) {
                result->Set(context, isolate->NewString("error"), isolate->NewString(isolate->toString(try_catch.Exception()))).IsJust();
                break;
            }
            result->Set(context, isolate->NewString("result"), result1).IsJust();
        } while (false);

        v8::Local<v8::String> result_json = v8::JSON::Stringify(context, result).FromMaybe(v8::Local<v8::String>());
        if (result_json.IsEmpty()) {
            result->Set(context, isolate->NewString("error"), isolate->NewString("unsupported result type")).IsJust();
            result->Delete(context, isolate->NewString("result")).IsJust();

            result_json = v8::JSON::Stringify(context, result).FromMaybe(v8::Local<v8::String>());
            if (result_json.IsEmpty())
                return 0;
        }

        v8::String::Utf8Value utf8(isolate->m_isolate, result_json);
        exlib::string code = "window.app(" + exlib::string(*utf8, utf8.length()) + ")";
        async([this, code]() {
            internal_eval(code);
        },
            CALL_E_GUICALL);
        return 0;
    });
}

result_t WebView::setup(v8::Local<v8::Object> opt)
{
    Isolate* isolate = Isolate::current(opt);
    result_t hr = OpenOptions::load(opt, m_options);
    if (hr < 0)
        return hr;

    obj_ptr<TitlebarOptions> titlebar;
    auto& titlebar_opt = m_options->titlebar.value();
    if (std::holds_alternative<exlib::string>(titlebar_opt)) {
        titlebar = new TitlebarOptions();
        titlebar->style = std::get<exlib::string>(titlebar_opt);
        m_options->titlebar = titlebar;
    } else
        titlebar = std::get<obj_ptr<TitlebarOptions>>(titlebar_opt);

    exlib::string& titlebar_style = titlebar->style.value();
    if (titlebar_style != "show" && titlebar_style != "hide" && titlebar_style != "transparent")
        return Runtime::setError("WebView: titlebar style must be 'show', 'hide' or 'transparent', but got '" + titlebar_style + "'");

    exlib::string& titlebar_height = titlebar->height.value();
    if (titlebar_height != "normal" && titlebar_height != "tall")
        return Runtime::setError("WebView: titlebar height must be 'normal', 'tall', but got '" + titlebar_height + "'");

    if (m_options->icon.has_value()) {
        Variant var;
        hr = fs_base::ac_readFile(m_options->icon.value(), "", var);
        if (hr)
            return hr;

        m_icon = (Buffer*)var.object();
        if (!m_icon)
            return Runtime::setError("Window icon is empty");
    }

    if (m_options->menu.has_value())
        m_menu = new ValueHolder(m_options->menu.value()->wrap());

    if (m_options->app.has_value())
        SetPrivate("app", m_options->app.value());

    if (m_options->onloading.has_value())
        setListener("loading", m_options->onloading.value());
    if (m_options->onload.has_value())
        setListener("load", m_options->onload.value());
    if (m_options->onclose.has_value())
        setListener("close", m_options->onclose.value());
    if (m_options->onmove.has_value())
        setListener("move", m_options->onmove.value());
    if (m_options->onresize.has_value())
        setListener("resize", m_options->onresize.value());
    if (m_options->onfocus.has_value())
        setListener("focus", m_options->onfocus.value());
    if (m_options->onblur.has_value())
        setListener("blur", m_options->onblur.value());
    if (m_options->onmessage.has_value())
        setListener("message", m_options->onmessage.value());

    return 0;
}

result_t WebView::check_status(AsyncEvent* ac)
{
    if (ac->isSync()) {
        m_ready->ac_wait();
        return CHECK_ERROR(CALL_E_GUICALL);
    }

    if (!m_window)
        return Runtime::setError("WebView: webview is closed");

    return 0;
}

result_t WebView::async_open()
{
    isolate_ref();
    m_self = new ValueHolder(wrap());

    async([this]() {
        createWebView();
    },
        CALL_E_GUICALL);

    return 0;
}

void WebView::release()
{
    if (m_webview) {
        m_webview = nullptr;
        m_window = nullptr;

        _emit("close");

        m_self.Release();
        isolate_unref();
    }
}

result_t WebView::open(exlib::string url, v8::Local<v8::Object> opt)
{
    result_t hr = setup(opt);
    if (hr < 0)
        return hr;

    m_options->url = url;

    return async_open();
}

result_t WebView::open(v8::Local<v8::Object> opt)
{
    result_t hr = setup(opt);
    if (hr < 0)
        return hr;

    return async_open();
}

result_t WebView::openFile(exlib::string file, v8::Local<v8::Object> opt)
{
    result_t hr = setup(opt);
    if (hr < 0)
        return hr;

    m_options->file = file;
    m_options->url.reset();

    return async_open();
}

result_t WebView::getMenu(obj_ptr<Menu_base>& retVal)
{
    if (m_options->menu.has_value()) {
        retVal = m_options->menu.value();
        return 0;
    }

    return CALL_RETURN_NULL;
}

result_t WebView::ref(obj_ptr<WebView_base>& retVal)
{
    isolate_ref();
    retVal = this;
    return 0;
}

result_t WebView::unref(obj_ptr<WebView_base>& retVal)
{
    isolate_unref();
    retVal = this;
    return 0;
}

result_t gui_base::open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    obj_ptr<WebView> webview = new WebView();
    retVal = webview;
    return webview->open(url, opt);
}

result_t gui_base::openFile(exlib::string file, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    obj_ptr<WebView> webview = new WebView();
    retVal = webview;
    return webview->openFile(file, opt);
}

result_t gui_base::open(v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    obj_ptr<WebView> webview = new WebView();
    retVal = webview;
    return webview->open(opt);
}
}

#endif