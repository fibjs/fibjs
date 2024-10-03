/*
 * WebView.h
 *
 *  Created on: Sep 22, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/WebView.h"
#include "ifs/url.h"
#include "Event.h"
#include "boost/preprocessor.hpp"

#define LOAD_MEMBER(r, data, elem)                                                 \
    hr = GetConfigValue(isolate, opt, BOOST_PP_STRINGIZE(elem), data->elem, true); \
    if (hr < 0)                                                                    \
        return hr;

#define LOAD(Class, Members)                                                \
    static result_t load(v8::Local<v8::Object> opt, obj_ptr<Class>& retVal) \
    {                                                                       \
        Isolate* isolate = Isolate::current(opt);                           \
        obj_ptr<Class> o = new Class();                                     \
        result_t hr = 0;                                                    \
        BOOST_PP_SEQ_FOR_EACH(LOAD_MEMBER, o, Members)                      \
        retVal = o;                                                         \
        return 0;                                                           \
    }

namespace fibjs {

class WebView : public WebView_base {
    FIBER_FREE();

public:
    WebView()
    {
        m_ready = new Event();
        isolate_ref();
    }

    EVENT_SUPPORT();

public:
    // WebView_base
    virtual result_t loadURL(exlib::string url, AsyncEvent* ac);

    virtual result_t loadFile(exlib::string file, AsyncEvent* ac)
    {
        result_t hr = check_status(ac);
        if (hr < 0)
            return hr;

        obj_ptr<UrlObject_base> u;
        hr = url_base::pathToFileURL(file, u);
        if (hr < 0)
            return hr;

        u->set_protocol("fs:");
        u->set_slashes(true);

        exlib::string url;
        u->get_href(url);

        return loadURL(url, ac);
    }

    virtual result_t getUrl(exlib::string& retVal, AsyncEvent* ac);
    virtual result_t setHtml(exlib::string html, AsyncEvent* ac);
    virtual result_t reload(AsyncEvent* ac);
    virtual result_t goBack(AsyncEvent* ac);
    virtual result_t goForward(AsyncEvent* ac);
    virtual result_t eval(exlib::string code, AsyncEvent* ac);
    virtual result_t setTitle(exlib::string title, AsyncEvent* ac);
    virtual result_t getTitle(exlib::string& retVal, AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac);

public:
    class OpenOptions : public obj_base {
    public:
        LOAD(OpenOptions, (width)(height)(left)(top)(url)(file)(frame)(caption)(resizable)(fullscreen)(maximize));

    public:
        std::optional<int32_t> width;
        std::optional<int32_t> height;
        std::optional<int32_t> left;
        std::optional<int32_t> top;
        std::optional<exlib::string> url;
        std::optional<exlib::string> file;
        std::optional<bool> frame;
        std::optional<bool> caption;
        std::optional<bool> resizable;
        std::optional<bool> fullscreen;
        std::optional<bool> maximize;
    };

public:
    result_t createWebView();
    void config();

public:
    result_t open(exlib::string url, v8::Local<v8::Object> opt);
    result_t open(v8::Local<v8::Object> opt);
    result_t openFile(exlib::string file, v8::Local<v8::Object> opt);
    result_t async_open();

public:
    static void run_os_gui(exlib::Event& gui_ready);

public:
    void internal_close();

    result_t check_status(AsyncEvent* ac)
    {
        if (ac->isSync()) {
            m_ready->ac_wait();
            return CHECK_ERROR(CALL_E_GUICALL);
        }

        if (!m_window)
            return Runtime::setError("WebView: webview is closed");

        return 0;
    }

    void release()
    {
        if (m_webview) {
            m_webview = nullptr;
            m_window = nullptr;

            _emit("close");

            isolate_unref();
            Unref();
        }
    }

public:
    EVENT_FUNC(open);
    EVENT_FUNC(move);
    EVENT_FUNC(resize);
    EVENT_FUNC(close);
    EVENT_FUNC(message);

public:
    obj_ptr<OpenOptions> m_options;

    void* m_window = nullptr;
    void* m_webview = nullptr;

    obj_ptr<Event_base> m_ready;

public:
    int32_t m_x = 0;
    int32_t m_y = 0;
    int32_t m_width = 0;
    int32_t m_height = 0;
};

}
