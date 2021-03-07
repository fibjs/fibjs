/*
 * gui.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#include "object.h"
#include "ifs/gui.h"
#include "ifs/process.h"
#include "ifs/fs.h"
#include "path.h"
#include "options.h"
#include "include/cef_app.h"
#include "include/cef_task.h"
#include "include/base/cef_bind.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_library_loader.h"
#include "CefWebView.h"
#include "../os_gui.h"
#include "gui_app.h"
#include "Url.h"

namespace fibjs {

DECLARE_MODULE(gui);

#ifdef WIN32
#define os_dirname _dirname_win32
#define os_normalize _normalize_win32
#else
#define os_dirname _dirname
#define os_normalize _normalize
#endif

#if defined(Darwin)
const char* s_cef_sdk = "../Frameworks/Chromium Embedded Framework.framework/Chromium Embedded Framework";
#elif defined(Windows)
const char* s_cef_sdk = "libcef.dll";
#else
const char* s_cef_sdk = "libcef.so";
#endif

void GuiApp::load_cef()
{
    exlib::string str_path;

    if (!g_cefprocess)
        m_gui.wait();

    if (m_cef_path.empty()) {
        exlib::string str_exe;

        process_base::get_execPath(str_exe);
        os_dirname(str_exe, m_cef_path);
    }

    str_path = m_cef_path;

    str_path.append(1, PATH_SLASH);
    str_path.append(s_cef_sdk);

    exlib::string str_cef;
    os_normalize(str_path, str_cef);

    fs_base::cc_exists(str_cef, m_has_cef);
    if (!m_has_cef) {
        m_gui_ready.set();
        run_os_gui();
    }

#ifdef WIN32
    exlib::string str_chrome;

    os_dirname(str_cef, str_chrome);
    str_chrome.append("\\chrome_elf.dll");
    LoadLibraryW(UTF8_W(str_chrome));
#endif

    if (!cef_load_library(str_cef.c_str()))
        _exit(-1);
}

CefRefPtr<GuiApp> g_app;

void MacRunMessageLoop(const CefMainArgs& args, const CefSettings& settings, CefRefPtr<CefApp> application);

void GuiApp::run_gui(int argc, char* argv[])
{
    exlib::OSThread th;
    th.bindCurrent();

    Runtime rt(NULL);

    load_cef();

    if (g_cefprocess)
        _exit(CefExecuteProcess(m_args, this, NULL));

    if (m_opt) {
        Variant v;

        if (m_opt->get("debug", v) == 0)
            m_bDebug = v.boolVal();

        if (m_opt->get("popup", v) == 0)
            m_bPopup = v.boolVal();

        if (m_opt->get("menu", v) == 0)
            m_bMenu = v.boolVal();

        if (!m_bHeadless && m_opt->get("headless", v) == 0)
            m_bHeadless = v.boolVal();

        if (m_opt->get("cache_path", v) == 0)
            CefString(&m_settings.cache_path) = v.string().c_str();

#ifndef Darwin
        if (m_opt->get("locales_path", v) == 0)
            CefString(&m_settings.locales_dir_path) = v.string().c_str();
        else {
            exlib::string str_locales;

            os_normalize(m_cef_path + "/locales", str_locales);
            CefString(&m_settings.locales_dir_path) = str_locales.c_str();
        }
#endif
    }

#ifdef Darwin
    MacRunMessageLoop(m_args, m_settings, this);
#else
    CefInitialize(m_args, m_settings, this, nullptr);
    CefRunMessageLoop();
#endif

    for (int32_t i = 0; i < 10; i++) {
        CefDoMessageLoopWork();
        exlib::OSThread::sleep(10);
    }

    CefShutdown();
    m_gui_done.set();

    th.suspend();
}

void run_gui(int argc, char* argv[])
{
    g_app = new GuiApp(argc, argv);
    g_app.get()->run_gui(argc, argv);
}

static result_t async_flush(int32_t w)
{
    CefQuitMessageLoop();
    return 0;
}

void GuiApp::gui_flush()
{
    if (m_has_cef) {
        asyncCall(async_flush, 0, CALL_E_GUICALL);
        m_gui_done.wait();
    }
}

void gui_flush()
{
    if (g_app.get())
        g_app.get()->gui_flush();
}

void putGuiPool(AsyncEvent* ac)
{
    class GuiTask : public CefTask {
    public:
        GuiTask(AsyncEvent* ac)
            : m_ac(ac)
        {
        }

        virtual void Execute()
        {
            m_ac->invoke();
        }

    private:
        IMPLEMENT_REFCOUNTING(GuiTask);
        AsyncEvent* m_ac;
    };

    if (!g_app.get()->m_has_cef)
        os_putGuiPool(ac);
    else
        CefPostTask(TID_UI, new GuiTask(ac));
}

result_t gui_base::setVersion(int32_t ver)
{
    return os_gui_setVersion(ver);
}

static result_t async_open(obj_ptr<CefWebView> w)
{
    w->open();
    return 0;
}

result_t GuiApp::open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    m_gui.set();
    m_gui_ready.wait();

    if (!m_has_cef)
        return os_gui_open(url, opt, retVal);

    obj_ptr<NObject> o = new NObject();
    o->add(opt);

    obj_ptr<CefWebView> w = new CefWebView(url, o);
    w->wrap();

    asyncCall(async_open, w, CALL_E_GUICALL);
    retVal = w;

    return 0;
}

result_t gui_base::open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    return g_app.get()->open(url, opt, retVal);
}

result_t GuiApp::config(v8::Local<v8::Object> opt)
{
    Isolate* isolate = Isolate::current();

    m_opt = new NObject();
    m_opt->add(opt);

    Variant v;
    result_t hr;

#ifndef Darwin
    if (m_opt->get("cef_path", v) == 0)
        m_cef_path = v.string();
#endif

    v8::Local<v8::Object> o;
    hr = GetConfigValue(isolate->m_isolate, opt, "backend", o, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;

        JSArray ks = o->GetPropertyNames();
        int32_t len = ks->Length();
        int32_t i;

        for (i = 0; i < len; i++) {
            std::map<exlib::string, CefRefPtr<GuiSchemeHandlerFactory>>::iterator it;
            JSValue k = ks->Get(i);
            exlib::string ks(ToCString(v8::String::Utf8Value(isolate->m_isolate, k)));
            Url u;
            exlib::string scheme;

            u.parse(ks);
            u.m_protocol.tolower();
            u.m_hostname.tolower();

            scheme = u.m_protocol.substr(0, u.m_protocol.length() - 1);
            it = m_schemes.find(scheme);
            if (it == m_schemes.end()) {
                std::pair<exlib::string, CefRefPtr<GuiSchemeHandlerFactory>> _pair(scheme, new GuiSchemeHandlerFactory());
                it = m_schemes.insert(_pair).first;
            }

            obj_ptr<Handler_base> hdr;
            hr = GetConfigValue(isolate->m_isolate, o, ks.c_str(), hdr);
            if (hr < 0)
                return hr;

            m_hdrs.push_back(new ValueHolder(hdr->wrap()));
            it->second.get()->m_domains.insert(std::pair<exlib::string, obj_ptr<Handler_base>>(u.m_hostname, hdr));
        }
    }

    return 0;
}

result_t gui_base::config(v8::Local<v8::Object> opt)
{
    return g_app.get()->config(opt);
}

}
