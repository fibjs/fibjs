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
        // initialize one fibjs runtime
        Runtime rt(NULL);

        printf("gui_thread->Run 1\n");
        WebView::RegNSApplicationDelegations();
        WebView::SetupAppMenubar();

        /**
         * 目前是: 
         * - 另一方面, 关闭窗口后, 从 webview_windowWillClose 回调中执行了 WebView::clear, 之后, 这个 Loop 又开始运行了(如果此时系统中还有 isolate 在运行的话)
         */
        while (true) {
            AsyncEvent* p = s_uiPool.getHead();

            if (p) {
                // printf("[gui_thread::Run] in loop, it's p \n");
                p->invoke();
                // p->js_invoke();
            }
            // 从 sharedApplication 的事件循环中中取得事件
            id event = WebView::webview_get_event_from_mainloop(0);
            WebView::send_event_to_sharedApplicatoin_and_check_should_exit(event);

            struct webview* w = WebView::getCurrentWebViewStruct();

            if (w) {
                // printf("[gui_thread::Run] in loop, webview struct not NULL \n");

                if (!WebView::should_exit(w))
                    continue;

                // if (WebView::webview_loop(w, 0) == 0)
                //     continue;

                // if (WebView::should_exit(w) == 0)
                //     continue;
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

    obj_ptr<WebView> wv = new WebView(url, o);
    wv->wrap();

    asyncCall(WebView::async_open, wv, CALL_E_GUICALL);
    // printf("--- [here] gui_base::open 4 --- \n");
    retVal = wv;

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

    m_bDebug = false;

    m_ac = NULL;
    // _onmessage = NULL;
    // _onclose = NULL;

    m_visible = true;

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

    if (s_activeWinObjcId) {
        s_activeWinObjcId = NULL;
        printf("[WebView::clear]s_activeWinObjcId set as NULL \n");
    }

    // if (s_activeWinObjcId == this->priv_windowDelegate)
    //     s_activeWinObjcId = NULL;
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

static int async_webview_eval(const char* jsscript)
{
    struct webview* w = WebView::getCurrentWebViewStruct();
    if (w != NULL) {
        printf("[async_webview_eval] would eval \n");
        webview_eval(w, jsscript);
    }

    return 0;
}

result_t WebView::postMessage(exlib::string msg)
{
    printf("[WebView::postMessage] view view argument %s \n", msg.c_str());

    struct webview* w = WebView::getCurrentWebViewStruct();
    if (w != NULL) {
        exlib::string jsstr = "if (this.onmessage) { this.onmessage(";
        // TODO: maybe escape here?
        jsstr.append("\"");
        jsstr.append(msg.c_str());
        jsstr.append("\"");
        jsstr.append("); }");

        printf("[WebView::postMessage] simple WebView::postMessage %s \n", jsstr.c_str());

        asyncCall(async_webview_eval, jsstr.c_str(), CALL_E_GUICALL);
    } else {
        printf("[WebView::postMessage] no s_activeWinObjcId, no webview struct \n");
    }

    return 0;
}

result_t WebView::postMessage(exlib::string msg, AsyncEvent* ac)
{
    if (ac->isSync()) {
        printf("[WebView::postMessage] I would go into CALL_E_GUICALL\n");
        return CHECK_ERROR(CALL_E_GUICALL);
    }

    return postMessage(msg);
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