/*
 * gui.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#ifdef OS_DESKTOP

#include "object.h"
#include "ifs/gui.h"
#include "ifs/fs.h"

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
result_t WebView::setup(v8::Local<v8::Object> opt)
{
    result_t hr = OpenOptions::load(opt, m_options);
    if (hr < 0)
        return hr;

    if (m_options->icon.has_value()) {
        Variant var;
        hr = fs_base::ac_readFile(m_options->icon.value(), "", var);
        if (hr)
            return hr;

        m_icon = (Buffer*)var.object();
        if (!m_icon)
            return Runtime::setError("Window icon is empty");
    }

    if (m_options->onopen.has_value())
        set_onopen(m_options->onopen.value());
    if (m_options->onclose.has_value())
        set_onclose(m_options->onclose.value());
    if (m_options->onmove.has_value())
        set_onmove(m_options->onmove.value());
    if (m_options->onresize.has_value())
        set_onresize(m_options->onresize.value());
    if (m_options->onfocus.has_value())
        set_onfocus(m_options->onfocus.value());
    if (m_options->onblur.has_value())
        set_onblur(m_options->onblur.value());
    if (m_options->onmessage.has_value())
        set_onmessage(m_options->onmessage.value());

    return 0;
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
