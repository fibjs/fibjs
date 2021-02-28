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

static exlib::Event s_gui;
static exlib::Event s_gui_ready;

class GuiApp : public CefApp,
               public CefBrowserProcessHandler,
               public CefPrintHandler {
public:
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

    void OnContextInitialized() OVERRIDE
    {
        s_gui_ready.set();
    }

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
        return CefSize(8.27 * device_units_per_inch, 11.75 * device_units_per_inch);
    }

private:
    IMPLEMENT_REFCOUNTING(GuiApp);
};

static bool s_has_cef;

const char* cef_path()
{
#ifdef Linux
    const char* s_cef_sdk = "./libcef.so";
#elif defined(Windows)
    const char* s_cef_sdk = "./libcef.dll";
#else
    const char* s_cef_sdk = "../Frameworks/Chromium Embedded Framework.framework/Chromium Embedded Framework";
#endif

    static exlib::string str_cef;

    if (str_cef.empty()) {
        exlib::string str_exe;
        exlib::string str_path;

        process_base::get_execPath(str_exe);

#ifdef WIN32
        _dirname_win32(str_exe, str_path);
        str_path.append(1, '/');
        str_path.append(s_cef_sdk);
        _normalize_win32(str_path, str_cef);
#else
        _dirname(str_exe, str_path);
        str_path.append(1, '/');
        str_path.append(s_cef_sdk);
        _normalize(str_path, str_cef);
#endif
    }

    fs_base::cc_exists(str_cef, s_has_cef);
    if (!s_has_cef) {
        s_gui.wait();
        s_gui_ready.set();

        run_os_gui();
    }

    return str_cef.c_str();
}

static CefSettings settings;

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
    CefRefPtr<GuiApp> app(new GuiApp);

    if (g_cefprocess) {
        if (!cef_load_library(cef_path()))
            _exit(-1);

        _exit(CefExecuteProcess(main_args, app.get(), NULL));
    } else {
        s_gui.wait();
        if (!cef_load_library(cef_path()))
            _exit(-1);

#ifdef Darwin
        MacRunMessageLoop(main_args, settings, app.get());
#else
        CefInitialize(main_args, settings, app.get(), nullptr);
        CefRunMessageLoop();
#endif
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

    if (!s_has_cef)
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
    s_gui.set();
    s_gui_ready.wait();

    if (!s_has_cef)
        return os_gui_open(url, opt, retVal);

    obj_ptr<NObject> o = new NObject();
    o->add(opt);

    obj_ptr<CefWebView> w = new CefWebView(url, o);
    w->wrap();

    asyncCall(async_open, w, CALL_E_GUICALL);
    retVal = w;

    return 0;
}
}
