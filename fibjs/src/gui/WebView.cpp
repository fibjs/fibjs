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
#include "DOMEvent.h"
#include "EventEmitter.h"
#include "EventInfo.h"
#include "Fiber.h"

namespace fibjs {

// The schemes that carry a document the caller expects to arrive. An engine
// that answers one of them with an empty document failed the navigation;
// inline content does not: setHtml navigates to a data: URL on Windows and the
// engine reports the finished document as about:blank, which is correct.
static bool requests_document(const exlib::string& url)
{
    return url.find("http:") == 0 || url.find("https:") == 0
        || url.find("fs:") == 0 || url.find("file:") == 0;
}

result_t WebView::loadFile(exlib::string file)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    obj_ptr<UrlObject_base> u;
    hr = url_base::pathToFileURL(file, v8::Local<v8::Object>(), u);
    if (hr.failed())
        return hr;

    exlib::string url;
    u->get_href(url);

    url = "fs:" + url.substr(5);

    return loadUrl(url);
}

result_t WebView::get_loading(bool& retVal)
{
    retVal = m_loading;
    return result_t();
}

// Cleared when a navigation starts, set when it ends: a blocking member issues
// its command between the two and then waits for the destination.
void WebView::begin_load()
{
    m_wait_gen = m_load_gen + 1;
    m_load_done = false;
    m_load->clear();
    m_loading = true;
    m_waiting = true;
}

result_t WebView::finish_load()
{
    m_load->ac_wait();

    // The outcome is consumed: a later completion (an error page the engine
    // loaded after the failure) is a new navigation again.
    m_waiting = false;

    return m_load_hr;
}

void WebView::postLoading(exlib::string url)
{
    m_load_gen++;
    m_loading_url = url;

    // A result the waiting fiber has not read yet is final: the engine may be
    // starting the error page that belongs to the navigation it just failed.
    if (!(m_waiting && m_load_done)) {
        m_load_done = false;
        m_load->clear();
    }

    m_loading = true;

    obj_ptr<EventInfo> ei = new EventInfo(this, "loading");
    ei->add("url", url);
    ei->emit();
}

void WebView::postLoad(exlib::string url, result_t hr, exlib::string message)
{
    // The tail of a navigation that started before the caller asked for this
    // one (the about:blank of a fresh window, a replaced request) must not
    // satisfy the wait.
    if (m_load_gen < m_wait_gen)
        return;

    // The same navigation already reported; a later signal (WebKitGTK finishing
    // the error page of a failed load) must not turn a failure into success.
    if (m_load_done)
        return;

    m_load_done = true;

    // A failed navigation leaves the engine on the previous document, so the
    // platform reports no usable URL for it: use the one the navigation was
    // started with.
    if (url.empty())
        url = m_loading_url;

    // WebKitGTK answers a request it refuses to start (a blocked port, for
    // example) by committing an empty document: the load finishes without an
    // error even though the requested document never arrived. The caller asked
    // for a document over a real scheme, so this is the failure it is.
    if (hr.ok() && url == "about:blank" && requests_document(m_loading_url)) {
        url = m_loading_url;
        message = "WebView: the engine did not load the requested document";
        hr = result_t::make(CALL_E_INVALID_DATA)
                 .with_type_name(errtype::kError)
                 .with_code("ERR_GUI_LOAD_FAILED")
                 .with_message(message);
    }

    m_wait_gen = 0;
    m_load_url = url;
    m_load_hr = hr;
    m_loading = false;

    if (hr.failed()) {
        obj_ptr<EventInfo> ei = new EventInfo(this, "load-failed");
        ei->add("url", url);
        ei->add("message", message);
        ei->emit();
    } else {
        obj_ptr<EventInfo> ei = new EventInfo(this, "load");
        ei->add("url", url);
        ei->emit();
    }

    m_load->set();
}

result_t WebView::waitForLoad(exlib::string url)
{
    if (m_closed)
        return result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code("ERR_GUI_CLOSED").with_message("WebView: webview is closed");

    // Asking for a document that is already the current one answers at once.
    if (!url.empty() && !m_loading && m_load_url == url)
        return m_load_hr;

    exlib::string lastUrl;
    result_t lastHr;

    m_waiting = true;

    do {
        // Wait for the next navigation: a completion already in hand belongs to
        // the document the caller asked for.
        m_wait_gen = m_load_gen + 1;
        m_load_done = false;
        m_load->clear();

        m_load->ac_wait();

        lastUrl = m_load_url;
        lastHr = m_load_hr;
    } while (!url.empty() && lastUrl != url);

    m_waiting = false;

    return lastHr;
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
            return;
        v8::Local<v8::Object> req = v.As<v8::Object>();

        v8::Local<v8::String> keyId = isolate->NewString("id");
        v = req->Get(context, keyId).FromMaybe(v8::Local<v8::Value>());
        if (v.IsEmpty() || !v->IsString())
            return;
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
            if (hr.failed()) {
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
            if (hr.failed()) {
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
                return;
        }

        v8::String::Utf8Value utf8(isolate->m_isolate, result_json);
        exlib::string code = "window.app(" + exlib::string(*utf8, utf8.length()) + ")";
        async([this, code]() {
            internal_eval(code);
        },
            result_t::guiloop());
    });
}

result_t WebView::setup(v8::Local<v8::Object> opt)
{
    Isolate* isolate = Isolate::current(opt);
    result_t hr = OpenOptions::load(opt, m_options);
    if (hr.failed())
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
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("WebView: titlebar style must be 'show', 'hide' or 'transparent', but got '" + titlebar_style + "'");

    exlib::string& titlebar_height = titlebar->height.value();
    if (titlebar_height != "normal" && titlebar_height != "tall")
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("WebView: titlebar height must be 'normal', 'tall', but got '" + titlebar_height + "'");

    if (m_options->icon.has_value()) {
        Variant var;
        hr = fs_base::ac_readFile(m_options->icon.value(), "", var);
        if (hr.failed())
            return hr;

        m_icon = (Buffer*)var.object();
        if (!m_icon)
            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("Window icon is empty");
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

    return result_t();
}

result_t WebView::ensure_open()
{
    if (!m_window)
        return result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code("ERR_GUI_CLOSED").with_message("WebView: webview is closed");

    return result_t();
}

void WebView::wait_close()
{
    if (m_closed)
        return;

    m_closed_ev->ac_wait();
}

void WebView::postClose()
{
    if (m_closed)
        return;

    m_closed = true;
    m_closed_ev->set();
}

bool WebView::emit_close_requested()
{
    Isolate* isolate = get_holder();
    if (!isolate || isolate->is_terminating())
        return false;

    // Already running JS: a second EnterJsScope would rewrite the fiber's JS
    // bookkeeping while a fiber is active. Only the posted path needs it.
    bool in_js = Isolate::safe_current() == isolate;

    std::unique_ptr<JSFiber::EnterJsScope> scope;
    if (!in_js)
        scope.reset(new JSFiber::EnterJsScope());

    v8::HandleScope handle_scope(isolate->m_isolate);

    // A cancelable DOMEvent: the listener calls preventDefault() instead of
    // writing a field, which keeps the flag on the native object where the
    // close path can read it back after the listeners returned.
    obj_ptr<DOMEvent> ev = new DOMEvent("close-requested", false, true, false);
    Variant arg = (object_base*)ev;
    v8::Local<v8::Value> args[] = { arg };

    bool handled = false;
    JSTrigger(this)._emit("close-requested", args, 1, handled);

    bool prevented = false;
    ev->get_defaultPrevented(prevented);

    return prevented;
}

bool WebView::should_cancel_close()
{
    if (m_allowClose || m_closed || !m_window)
        return false;

    Isolate* isolate = get_holder();
    if (!isolate || isolate->is_terminating())
        return false;

    // close()/destroy() ask from the JS thread: the listeners can run here and
    // the answer is available before the native window is torn down.
    if (Isolate::safe_current() == isolate)
        return emit_close_requested();

    // The platform asked (title-bar button, Alt+F4) from inside the native
    // window teardown, where a JS turn cannot start. Cancel now, run the
    // listeners on the isolate and close again when nobody prevented it.
    isolate->sync([this]() {
        if (m_allowClose || m_closed || !m_window)
            return;

        if (emit_close_requested())
            return;

        m_allowClose = true;
        async([this]() {
            if (m_allowClose && !m_closed && m_window)
                internal_close();
        },
            result_t::guiloop());
    });

    return true;
}

result_t WebView::waitForClose()
{
    wait_close();
    return result_t();
}

result_t WebView::close(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = false;

    if (should_cancel_close())
        return result_t();

    // close()/destroy() destroy the window; the platform hideOnClose policy
    // only applies to the user closing it.
    if (m_options)
        m_options->hideOnClose = false;

    m_allowClose = true;
    internal_close();
    wait_close();

    retVal = true;
    return result_t();
}

result_t WebView::destroy()
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    if (m_options)
        m_options->hideOnClose = false;

    m_allowClose = true;
    internal_close();
    wait_close();

    return result_t();
}

result_t WebView::async_open()
{
    isolate_ref();
    m_self = new ValueHolder(wrap());

    async([this]() {
        createWebView();
    },
        result_t::guiloop());

    // The window exists when open returns: the creation task runs on the host
    // loop while this fiber is parked.
    m_ready->ac_wait();

    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    // The initial document belongs to the object: when open returns, the page
    // the window was created with has finished loading. A failure is reported
    // through the load-failed event, the window itself is still usable.
    finish_load();

    return result_t();
}

void WebView::release()
{
    if (m_webview) {
        m_webview = nullptr;
        m_window = nullptr;

        postClose();
        _emit("close");

        m_self.Release();
        isolate_unref();
    }
}

result_t WebView::open(exlib::string url, v8::Local<v8::Object> opt)
{
    result_t hr = setup(opt);
    if (hr.failed())
        return hr;

    m_options->url = url;

    return async_open();
}

result_t WebView::open(v8::Local<v8::Object> opt)
{
    result_t hr = setup(opt);
    if (hr.failed())
        return hr;

    return async_open();
}

result_t WebView::openFile(exlib::string file, v8::Local<v8::Object> opt)
{
    result_t hr = setup(opt);
    if (hr.failed())
        return hr;

    m_options->file = file;
    m_options->url.reset();

    return async_open();
}

result_t WebView::getMenu(obj_ptr<Menu_base>& retVal)
{
    if (m_options->menu.has_value()) {
        retVal = m_options->menu.value();
        return result_t();
    }

    return result_t::return_null();
}

result_t WebView::ref(obj_ptr<WebView_base>& retVal)
{
    isolate_ref();
    retVal = this;
    return result_t();
}

result_t WebView::unref(obj_ptr<WebView_base>& retVal)
{
    isolate_unref();
    retVal = this;
    return result_t();
}

result_t gui_base::open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    result_t gui_hr = check_gui_available();
    if (gui_hr.failed())
        return gui_hr;

    obj_ptr<WebView> webview = new WebView();
    retVal = webview;
    return webview->open(url, opt);
}

result_t gui_base::openFile(exlib::string file, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    result_t gui_hr = check_gui_available();
    if (gui_hr.failed())
        return gui_hr;

    obj_ptr<WebView> webview = new WebView();
    retVal = webview;
    return webview->openFile(file, opt);
}

result_t gui_base::open(v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    result_t gui_hr = check_gui_available();
    if (gui_hr.failed())
        return gui_hr;

    obj_ptr<WebView> webview = new WebView();
    retVal = webview;
    return webview->open(opt);
}
}

#endif