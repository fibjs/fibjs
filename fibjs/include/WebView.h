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
#include "Buffer.h"
#include "Menu.h"
#include <boost/preprocessor.hpp>

namespace fibjs {

class WebView : public WebView_base {
public:
    class OpenOptions : public obj_base {
    public:
        LOAD_OPTIONS(OpenOptions, (url)(file)(icon)(left)(top)(width)(height)(visible)(hideOnClose)(minWidth)(minHeight)(maxWidth)(maxHeight)(frame)(titlebar)(resizable)(fullscreen)(maximize)(menu)(app)(devtools)(onloading)(onload)(onclose)(onmove)(onresize)(onfocus)(onblur)(onmessage));

    public:
        std::optional<exlib::string> url;
        std::optional<exlib::string> file;
        std::optional<exlib::string> icon;

        std::optional<int32_t> left;
        std::optional<int32_t> top;
        std::optional<int32_t> width;
        std::optional<int32_t> height;

        std::optional<bool> visible = true;
        std::optional<bool> hideOnClose = false;

        std::optional<int32_t> minWidth = 0;
        std::optional<int32_t> minHeight = 0;
        std::optional<int32_t> maxWidth;
        std::optional<int32_t> maxHeight;

        std::optional<bool> frame = true;
        std::optional<exlib::string> titlebar = "show";
        std::optional<bool> resizable = true;
        std::optional<bool> fullscreen = false;
        std::optional<bool> maximize = false;

        std::optional<obj_ptr<Menu>> menu;
        std::optional<v8::Local<v8::Object>> app;

        std::optional<bool> devtools = false;

        std::optional<v8::Local<v8::Function>> onloading;
        std::optional<v8::Local<v8::Function>> onload;
        std::optional<v8::Local<v8::Function>> onclose;
        std::optional<v8::Local<v8::Function>> onmove;
        std::optional<v8::Local<v8::Function>> onresize;
        std::optional<v8::Local<v8::Function>> onfocus;
        std::optional<v8::Local<v8::Function>> onblur;
        std::optional<v8::Local<v8::Function>> onmessage;
    };

public:
    WebView()
    {
        m_ready = new Event();
    }

    EVENT_SUPPORT();

public:
    // WebView_base
    virtual result_t loadUrl(exlib::string url, AsyncEvent* ac);
    virtual result_t loadFile(exlib::string file, AsyncEvent* ac);
    virtual result_t getUrl(exlib::string& retVal, AsyncEvent* ac);
    virtual result_t setHtml(exlib::string html, AsyncEvent* ac);
    virtual result_t getHtml(exlib::string& retVal, AsyncEvent* ac);
    virtual result_t isReady(bool& retVal, AsyncEvent* ac);
    virtual result_t waitFor(exlib::string url, AsyncEvent* ac);
    virtual result_t reload(AsyncEvent* ac);
    virtual result_t goBack(AsyncEvent* ac);
    virtual result_t goForward(AsyncEvent* ac);
    virtual result_t eval(exlib::string code, Variant& retVal, AsyncEvent* ac);
    virtual result_t setTitle(exlib::string title, AsyncEvent* ac);
    virtual result_t getTitle(exlib::string& retVal, AsyncEvent* ac);
    virtual result_t isVisible(bool& retVal, AsyncEvent* ac);
    virtual result_t show(AsyncEvent* ac);
    virtual result_t hide(AsyncEvent* ac);
    virtual result_t setSize(int32_t width, int32_t height, AsyncEvent* ac);
    virtual result_t getSize(obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t setPosition(int32_t left, int32_t top, AsyncEvent* ac);
    virtual result_t getPosition(obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t isActived(bool& retVal, AsyncEvent* ac);
    virtual result_t active(AsyncEvent* ac);
    virtual result_t getMenu(obj_ptr<Menu_base>& retVal);
    virtual result_t takeScreenshot(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac);

public:
    void postWaitFor(exlib::string url);

public:
    EVENT_FUNC(loading);
    EVENT_FUNC(load);
    EVENT_FUNC(move);
    EVENT_FUNC(resize);
    EVENT_FUNC(focus);
    EVENT_FUNC(blur);
    EVENT_FUNC(close);
    EVENT_FUNC(message);

public:
    result_t createWebView();
    void config();

public:
    result_t setup(v8::Local<v8::Object> opt);
    result_t open(exlib::string url, v8::Local<v8::Object> opt);
    result_t open(v8::Local<v8::Object> opt);
    result_t openFile(exlib::string file, v8::Local<v8::Object> opt);
    result_t async_open();

    void app_rpc(exlib::string json);

public:
    void internal_close();
    void internal_minimize();
    void internal_maximize();
    void internal_eval(exlib::string code);
    bool internal_isReady();
    exlib::string internal_getUrl();

    result_t check_status(AsyncEvent* ac);
    void release();

public:
    obj_ptr<OpenOptions> m_options;

    obj_ptr<ValueHolder> m_self;
    obj_ptr<ValueHolder> m_menu;

    obj_ptr<Buffer> m_icon;

    void* m_window = nullptr;
    void* m_webview = nullptr;

    obj_ptr<Event_base> m_ready;

    std::vector<std::pair<exlib::string, AsyncEvent*>> m_waitFor;

#ifdef _WIN32
    bool m_isLoading = false;
#endif

public:
    int32_t m_x = 0;
    int32_t m_y = 0;
    int32_t m_width = 0;
    int32_t m_height = 0;
};

}
