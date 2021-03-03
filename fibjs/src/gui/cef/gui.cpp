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

class GuiApp : public CefApp,
               public CefBrowserProcessHandler,
               public CefPrintHandler {
public:
    // CefApp
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
        OVERRIDE
    {
        return this;
    }

    virtual CefRefPtr<CefPrintHandler> GetPrintHandler()
        OVERRIDE
    {
        return this;
    }

public:
    // CefBrowserProcessHandler
    virtual void OnBeforeCommandLineProcessing(const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line) OVERRIDE
    {
#ifdef Darwin
        command_line->AppendSwitch("use-mock-keychain");
#endif
    }

    void OnContextInitialized() OVERRIDE
    {
        m_gui_ready.set();
    }

public:
    // CefPrintHandler
    virtual void OnPrintStart(CefRefPtr<CefBrowser> browser) OVERRIDE
    {
    }

    virtual void OnPrintSettings(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefPrintSettings> settings, bool get_defaults)
        OVERRIDE
    {
    }

    virtual bool OnPrintDialog(CefRefPtr<CefBrowser> browser,
        bool has_selection, CefRefPtr<CefPrintDialogCallback> callback)
        OVERRIDE
    {
        return false;
    }

    virtual bool OnPrintJob(CefRefPtr<CefBrowser> browser, const CefString& document_name,
        const CefString& pdf_file_path, CefRefPtr<CefPrintJobCallback> callback)
        OVERRIDE
    {
        return false;
    }

    virtual void OnPrintReset(CefRefPtr<CefBrowser> browser) OVERRIDE
    {
    }

    virtual CefSize GetPdfPaperSize(int device_units_per_inch)
        OVERRIDE
    {
        return CefSize((int32_t)(8.27 * device_units_per_inch),
            (int32_t)(11.75 * device_units_per_inch));
    }

    void load_cef()
    {
        exlib::string str_cef;
        exlib::string str_path;
        exlib::string str_exe;

        process_base::get_execPath(str_exe);
        os_dirname(str_exe, str_path);

        str_path.append(1, PATH_SLASH);
        str_path.append(s_cef_sdk);
        os_normalize(str_path, str_cef);

        if (!g_cefprocess)
            m_gui.wait();

        fs_base::cc_exists(str_cef, m_has_cef);
        if (!m_has_cef) {
            m_gui_ready.set();
            run_os_gui();
        }

        if (!cef_load_library(str_cef.c_str()))
            _exit(-1);
    }

public:
    CefSettings m_settings;
    obj_ptr<NObject> m_opt;

    bool m_has_cef = false;

    exlib::Event m_gui;
    exlib::Event m_gui_ready;
    exlib::Event m_gui_done;

private:
    IMPLEMENT_REFCOUNTING(GuiApp);
};

static CefRefPtr<GuiApp> g_app;

void MacRunMessageLoop(const CefMainArgs& args, const CefSettings& settings, CefRefPtr<CefApp> application);

void run_gui(int argc, char* argv[])
{
    exlib::OSThread th;
    th.bindCurrent();

    Runtime rt(NULL);

#ifdef WIN32
    CefMainArgs main_args(NULL);
#else
    CefMainArgs main_args(argc, argv);
#endif
    g_app = new GuiApp;
    g_app->load_cef();

    if (g_cefprocess)
        _exit(CefExecuteProcess(main_args, g_app.get(), NULL));
    else {
        if (g_app->m_opt) {
            Variant v;

            if (g_app->m_opt->get("cache_path", v) == 0)
                CefString(&g_app->m_settings.cache_path) = v.string().c_str();
        }

#ifdef Darwin
        MacRunMessageLoop(main_args, g_app->m_settings, g_app.get());
#else
        CefInitialize(main_args, g_app->m_settings, g_app.get(), nullptr);
        CefRunMessageLoop();
#endif

        CefShutdown();
        g_app->m_gui_done.set();

        th.suspend();
    }
}

static result_t async_flush(int32_t w)
{
    CefQuitMessageLoop();
    return 0;
}

void gui_flush()
{
    if (g_app && g_app->m_has_cef) {
        asyncCall(async_flush, 0, CALL_E_GUICALL);
        g_app->m_gui_done.wait();
    }
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

    if (!g_app->m_has_cef)
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

result_t gui_base::open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    g_app->m_gui.set();
    g_app->m_gui_ready.wait();

    if (!g_app->m_has_cef)
        return os_gui_open(url, opt, retVal);

    obj_ptr<NObject> o = new NObject();
    o->add(opt);

    obj_ptr<CefWebView> w = new CefWebView(url, o);
    w->wrap();

    asyncCall(async_open, w, CALL_E_GUICALL);
    retVal = w;

    return 0;
}

result_t gui_base::config(v8::Local<v8::Object> opt)
{
    g_app->m_opt = new NObject();
    g_app->m_opt->add(opt);

    return 0;
}
}
