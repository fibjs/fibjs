/*
 * WebView.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: lion
 */

#ifdef __APPLE__

#include "object.h"
#include "ifs/gui.h"
#include "ifs/fs.h"
#include "ifs/os.h"
#include "path.h"
#include "WebView.h"
#include "EventInfo.h"
#include "utf8.h"
#include <exlib/include/thread.h>

namespace fibjs {

DECLARE_MODULE(gui);

class gui_thread : public exlib::OSThread {
public:
    // Run In GUI Thread, get AsyncEvent from s_uiPool to invoke
    virtual void Run()
    {
        printf("gui_thread->Run 1\n");
        WebView::RegNSApplicationDelegations();
        WebView::SetupAppMenubar();

        // initialize one fibjs runtime
        Runtime rt(NULL);

        while (true) {
            AsyncEvent* p = s_uiPool.getHead();
            if (p)
                p->invoke();

            struct webview* w = WebView::getCurrentWebViewStruct();

            // webview_dispatch(w,
            //     [](struct webview* w, void* arg) {
            //         printf("just here \n");
            //     },
            //     this);

            if (w) {
                printf("just here \n");
                // asyncCall(
                //     [](struct webview* w) {
                //         result_t hr = WebView::webview_loop(w, 1);
                //         if (hr > 0) {
                //             WebView::webview_terminate(w);
                //             return CALL_E_NOSYNC;
                //         }
                //         return hr;
                //     },
                //     w, CALL_E_GUICALL);

                // while ((WebView::webview_loop(w, 1)) == 0)
                //     ;

                // WebView* wv = WebView::getCurrentWebViewInstance();

                // if (wv) {
                //     continue;
                // }
            }
        }
        printf("gui_thread->Run 2\n");
    }

public:
    // OTHERS
    result_t AddRef(void)
    {
        return 1;
    }
    result_t Release(void)
    {
        return 1;
    }
};

void run_gui()
{
    gui_thread* _thGUI = new gui_thread();

    _thGUI->bindCurrent();
    s_thGUI = _thGUI;

    _thGUI->Run();
    // _thGUI->suspend();
}

// useless for darwin
result_t gui_base::setVersion(int32_t ver)
{
    return 0;
}

// In Javascript Thread
result_t gui_base::open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    // printf("--- [here] gui_base::open 1 --- \n");
    obj_ptr<NObject> o = new NObject();
    o->add(opt);

    obj_ptr<WebView> w = new WebView(url, o);
    w->wrap();

    asyncCall(WebView::async_open, w, CALL_E_GUICALL);
    // printf("--- [here] gui_base::open 4 --- \n");
    retVal = w;

    return 0;
};

// Would Call In Javascript Thread
WebView::WebView(exlib::string url, NObject* opt)
{
    printf("[WebView::WebView] before\n");
    holder()->Ref();

    m_url = url;
    m_opt = opt;

    m_title = "[WIP] Darwin WebView";
    m_WinW = 640;
    m_WinH = 400;
    m_bResizable = true;

    // winfo.parent = this;
    // winfo.priv_windowDelegate = NULL;

    m_bDebug = false;

    m_ac = NULL;
    // _onmessage = NULL;
    // _onclose = NULL;

    m_visible = true;

    // mark Application would not terminated after lasted window closed here

    // // navite to target url
    // Navigate(m_url.c_str());

    // do it when exit;
    // holder()->Unref();
    printf("[WebView::WebView] after\n");
}

WebView::~WebView()
{
    clear();
}

void WebView::clear()
{
    printf("[WebView::clear] \n");
    // if (_onmessage) {
    //     // _onmessage->Release();
    //     _onmessage = NULL;
    // }

    // if (_onclose) {
    //     // _onclose->Release();
    //     _onclose = NULL;
    // }

    if (m_ac) {
        m_ac->post(0);
        m_ac = NULL;
    }

    if (m_webview) {
        m_webview = NULL;
    }

    if (s_activeWin) {
        s_activeWin = NULL;
    }

    // if (s_activeWin == this->priv_windowDelegate)
    //     s_activeWin = NULL;
}

result_t WebView::setHtml(exlib::string html, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    return 0;
}

result_t WebView::print(int32_t mode, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    return 0;
}

result_t WebView::close(AsyncEvent* ac)
{
    isolate_unref();

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    return 0;
}

result_t WebView::postMessage(exlib::string msg)
{
    struct webview* w = WebView::getCurrentWebViewStruct();
    if (w != NULL) {
        exlib::string jsstr = "if (typeof external.onmessage === 'function') { external.onmessage(";
        // TODO: maybe escape here?
        jsstr.append("\"");
        jsstr.append(msg.c_str());
        jsstr.append("\"");
        jsstr.append("); }");

        printf("simple WebView::postMessage %s \n", jsstr.c_str());

        webview_eval(w, jsstr.c_str());
    } else {
        printf("no webview struct \n");
    }

    return 0;
}

result_t WebView::postMessage(exlib::string msg, AsyncEvent* ac)
{
    if (ac->isSync()) {
        printf("I would go int CALL_E_GUICALL");
        return CHECK_ERROR(CALL_E_GUICALL);
    }

    return postMessage(msg);

    // struct tmp {
    //     WebView* wv;
    //     exlib::string& jscode;
    // } _tmp = { this, jsstr };

    // asyncCall(
    //     [](tmp* _tmp) {
    //         WebView* wv = _tmp->wv;
    //         exlib::string jscode = _tmp->jscode;

    //         printf("here ??? %s \n", jscode.c_str());

    //         return 0;
    //     },
    //     &_tmp, CALL_E_GUICALL);

    // return 0;
}

result_t WebView::get_visible(bool& retVal)
{
    retVal = m_visible;
    return 0;
}

result_t WebView::set_visible(bool newVal)
{
    m_visible = newVal;

    return 0;
}

// ----- Control methods -----

void WebView::GoBack()
{
    // DO REAL THING
    return;
}

void WebView::GoForward()
{
    // DO REAL THING
    return;
}

void WebView::Refresh()
{
    // DO REAL THING
    return;
}

void WebView::Navigate(exlib::string szUrl)
{
    // bstr_t url(UTF8_W(szUrl));
    // variant_t flags(0x02u); //navNoHistory
    // DO REAL THING
    return;
}

result_t WebView::AddRef(void)
{
    Ref();
    return 1;
}

result_t WebView::Release(void)
{
    Unref();
    return 1;
}

extern const wchar_t* g_console_js;
}

#endif /* __APPLE__ */