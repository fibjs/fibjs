/*
 * gui_app.h
 *
 *  Created on: Mar 1, 2021
 *      Author: lion
 */

#pragma once

#include "object.h"
#include "options.h"
#include "include/cef_app.h"
#include "res_handler.h"

namespace fibjs {
class GuiApp : public CefApp,
               public CefBrowserProcessHandler {
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
                if (!qstrcmp(argv[i], "--cef_path=", 11)) {
                    m_cef_path = argv[i] + 11;
                    break;
                }
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

public:
    // CefBrowserProcessHandler
    virtual void OnContextInitialized() OVERRIDE
    {
        for (auto const& it : m_schemes)
            it.second->RegisterScheme(it.first);

        m_gui_ready.set();
    }

    virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) OVERRIDE
    {
        for (auto const& it : m_schemes)
            if (it.first != "http" && it.first != "https")
                registrar->AddCustomScheme(it.first.c_str(),
                    CEF_SCHEME_OPTION_FETCH_ENABLED | CEF_SCHEME_OPTION_CORS_ENABLED);
    }

    void config_opt(CefRefPtr<CefCommandLine> command_line, const char* opt, bool& value)
    {
        bool cmd_opt = command_line->HasSwitch(opt);

        if (cmd_opt)
            value = cmd_opt;
        else if (value)
            command_line->AppendSwitch(opt);
    }

    virtual void OnBeforeCommandLineProcessing(const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line) OVERRIDE
    {
        config_opt(command_line, "headless", m_bHeadless);

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
    void load_cef();
    void run_gui(int argc, char* argv[]);
    void gui_flush();
    result_t open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal);
    result_t config(v8::Local<v8::Object> opt);
    exlib::string get_path(const char* p);

public:
    obj_ptr<NObject> m_opt;
    exlib::string m_cef_path;

    CefMainArgs m_args;
    CefSettings m_settings;

    exlib::string m_proxy_mode;
    exlib::string m_proxy_server;

    bool m_has_cef = false;

    bool m_bDebug = true;
    bool m_bPopup = true;
    bool m_bMenu = true;
    bool m_bHeadless = false;

    exlib::Event m_gui;
    exlib::Event m_gui_ready;
    exlib::Event m_gui_done;

public:
    std::map<exlib::string, CefRefPtr<GuiSchemeHandlerFactory>> m_schemes;
    std::vector<obj_ptr<ValueHolder>> m_hdrs;

private:
    IMPLEMENT_REFCOUNTING(GuiApp);
};
}
