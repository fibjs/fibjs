/*
 * gui.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#ifdef OS_DESKTOP

#include "object.h"
#include "ifs/gui.h"

#include "WebView.h"

namespace fibjs {

DECLARE_MODULE(gui);

static exlib::Event s_gui;
static exlib::Event s_gui_ready;

void run_gui(int argc, char* argv[])
{
    exlib::OSThread th;
    th.bindCurrent();

    Runtime rt(NULL);

    s_gui.wait();
    WebView::run_os_gui(s_gui_ready);
}

result_t WebView::async_open()
{
    s_gui.set();
    s_gui_ready.wait();

    wrap();

    Ref();
    async([this]() {
        createWebView();
        Unref();
    },
        CALL_E_GUICALL);

    return 0;
}

result_t WebView::open(exlib::string url, v8::Local<v8::Object> opt)
{
    result_t hr = OpenOptions::load(opt, m_options);
    if (hr < 0)
        return hr;

    m_options->url = url;

    return async_open();
}

result_t WebView::open(v8::Local<v8::Object> opt)
{
    result_t hr = OpenOptions::load(opt, m_options);
    if (hr < 0)
        return hr;

    return async_open();
}

result_t WebView::openFile(exlib::string file, v8::Local<v8::Object> opt)
{
    result_t hr = OpenOptions::load(opt, m_options);
    if (hr < 0)
        return hr;

    m_options->file = file;
    m_options->url.reset();

    return async_open();
}

static result_t check_options(v8::Local<v8::Object> opt, obj_ptr<NObject>& retVal)
{
    const char* num_keys[] = {
        "width",
        "height",
        "left",
        "top"
    };

    const char* str_keys[] = {
        "url",
        "file"
    };

    const char* bool_keys[] = {
        "frame",
        "caption",
        "resizable",
        "fullscreen",
        "maximize"
    };

    Isolate* isolate = Isolate::current(opt);
    obj_ptr<NObject> o = new NObject();
    result_t hr = 0;

    for (int i = 0; i < ARRAYSIZE(num_keys); i++) {
        int32_t v;
        hr = GetConfigValue(isolate, opt, num_keys[i], v, true);
        if (hr >= 0)
            o->add(num_keys[i], v);
        else if (hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
    }

    for (int i = 0; i < ARRAYSIZE(str_keys); i++) {
        exlib::string v;
        hr = GetConfigValue(isolate, opt, str_keys[i], v, true);
        if (hr >= 0)
            o->add(str_keys[i], v);
        else if (hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
    }

    for (int i = 0; i < ARRAYSIZE(bool_keys); i++) {
        bool v;
        hr = GetConfigValue(isolate, opt, bool_keys[i], v, true);
        if (hr >= 0)
            o->add(bool_keys[i], v);
        else if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
    }

    retVal = o;

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
