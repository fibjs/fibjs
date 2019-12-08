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

static exlib::LockedList<AsyncEvent> s_uiPool;
static pthread_t s_thread;

/**
 * would be called when asyncCall(xx, xx, CALL_E_GUICALL)
 */
void putGuiPool(AsyncEvent* ac)
{
    printf("putGuiPool\n");
    s_uiPool.putTail(ac);
}

static id s_activeWin = NULL;

class gui_thread : public exlib::OSThread {
public:
    virtual void Run()
    {
        printf("gui_thread->Run 1\n");
        // initialize one fibjs runtime
        Runtime rt(NULL);

        while (true) {
            AsyncEvent* p = s_uiPool.getHead();
            if (p)
                p->invoke();

            if (s_activeWin) {
                // get webview from s_activeWin(@windowDelegate)
            }
        }

        // just call
        // webview("Minimal webview example", "http://fibjs.org", 800, 600, 1);

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

static gui_thread* s_gui_t = NULL;

void run_gui()
{
    gui_thread* _thGUI = new gui_thread();

    _thGUI->bindCurrent();
    s_gui_t = _thGUI;
    s_thread = _thGUI->thread_;

    _thGUI->Run();
}

// useless for darwin
result_t gui_base::setVersion(int32_t ver)
{
    return 0;
}

static result_t async_open(obj_ptr<fibjs::WebView> w)
{
    printf("[here] async_open\n");
    w->openFromAsyncCall();
    return 0;
}

// In Javascript Thread
result_t gui_base::open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    printf("--- [here] gui_base::open 1 --- \n");
    obj_ptr<NObject> o = new NObject();
    o->add(opt);
    printf("--- [here] gui_base::open 2 --- \n");

    obj_ptr<WebView> w = new WebView(url, o);
    printf("--- [here] gui_base::open 2.1 --- \n");
    w->wrap();

    printf("--- [here] gui_base::open 3 --- \n");
    asyncCall(async_open, w, CALL_E_GUICALL);
    printf("--- [here] gui_base::open 4 --- \n");
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

    // this->nso = objc_msgSend((id)objc_getClass("NSObject"), sel_registerName("new"));
    // this->nso = objc_msgSend((id)objc_getClass("WKUserContentController"), sel_registerName("new"));

    this->webview__title = exlib::string("测试2").c_str();
    this->webview__url = "http://fibjs.org";
    this->webview__width = 640;
    this->webview__height = 400;
    this->webview__resizable = true;

    // winfo.parent = this;
    // winfo.priv_windowDelegate = NULL;

    m_isdebug = false;

    m_ac = NULL;
    _onmessage = NULL;
    _onclose = NULL;

    m_visible = true;

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

result_t WebView::openFromAsyncCall()
{
    printf("[here] WebView::open\n");
    m_bSilent = false;
    m_maximize = false;

    if (m_opt) {
    }

    AddRef();

    // Timer timer;
    struct webview webview = {};
    webview.title = "[WIP] Darwin WebView";
    webview.url = "http://fibjs.org";
    webview.width = 640;
    webview.height = 400;
    webview.resizable = 1;
    // webview.external_invoke_cb = timer_cb;
    // webview.userdata = &timer;

    webview_init(&webview);

    while (webview_loop(&webview, 1) == 0)
        ;
    webview_exit(&webview);

    return 0;
}

void WebView::clear()
{
    if (_onmessage) {
        // _onmessage->Release();
        _onmessage = NULL;
    }

    if (_onclose) {
        // _onclose->Release();
        _onclose = NULL;
    }

    if (m_ac) {
        m_ac->post(0);
        m_ac = NULL;
    }

    if (s_activeWin == this->priv_windowDelegate)
        s_activeWin = NULL;

    this->priv_windowDelegate = NULL;
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
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    return 0;
}

result_t WebView::postMessage(exlib::string msg, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);
    return 0;
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