/*
 * gui_app.h
 *
 *  Created on: Mar 1, 2021
 *      Author: lion
 */

#ifndef GUI_APP_H
#define GUI_APP_H

#include "object.h"
#include "options.h"
#include "include/cef_app.h"

namespace fibjs {
class GuiApp : public CefApp,
               public CefBrowserProcessHandler,
               public CefPrintHandler {
public:
    GuiApp(int argc, char* argv[])
#ifdef WIN32
        : m_args(NULL)
#else
        : m_args(argc, argv)
#endif
    {
        if (g_cefprocess) {
            for (int32_t i = 0; i < argc; i++) {
                if (!qstrcmp(argv[i], "--cef_path=", 11))
                    m_cef_path = argv[i] + 11;
            }
        }
    }

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
    virtual void OnContextInitialized() OVERRIDE
    {
        m_gui_ready.set();
    }

    virtual void OnBeforeCommandLineProcessing(const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line) OVERRIDE
    {
#ifdef Darwin
        command_line->AppendSwitch("use-mock-keychain");
#endif
    }

    virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
        OVERRIDE
    {
        command_line->AppendSwitchWithValue("cef_path", m_cef_path.c_str());
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

    void load_cef();

public:
    CefMainArgs m_args;
    CefSettings m_settings;
    obj_ptr<NObject> m_opt;

    exlib::string m_cef_path;
    bool m_has_cef = false;

    exlib::Event m_gui;
    exlib::Event m_gui_ready;
    exlib::Event m_gui_done;

private:
    IMPLEMENT_REFCOUNTING(GuiApp);
};
}

#endif