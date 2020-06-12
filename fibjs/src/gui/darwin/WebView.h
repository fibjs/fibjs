/**
 * @author Richard
 * @email richardo2016@gmail.com
 * @create date 2018-04-23 03:25:07
 * @modify date 2018-04-23 03:25:42
 * @desc WebView Object for Mac OSX
 */
#ifdef __APPLE__

#ifndef WEBVIEW_APPLE_H_
#define WEBVIEW_APPLE_H_

#include "ifs/WebView.h"
#include "EventInfo.h"
#include "lib.h"
#include <Cocoa/Cocoa.h>

#include "objc.inl.mm"

namespace fibjs {

const char* WEBVIEW_MSG_HANDLER_NAME = "invoke";

static exlib::LockedList<AsyncEvent> s_uiPool;
static pthread_t s_thread;
class gui_thread;
static gui_thread* s_thGUI;

/**
 * would be called when asyncCall(xx, xx, CALL_E_GUICALL)
 */
void putGuiPool(AsyncEvent* ac)
{
    // printf("putGuiPool\n");
    s_uiPool.putTail(ac);
}

static id s_activeWinObjcId = NULL;

class WebView;

// WebView* getClsWebView(struct webview* w)
// {
//     return (WebView*)w->clsWebView;
// }

class WebView : public WebView_base {
    FIBER_FREE();

public:
    WebView(exlib::string url, NObject* opt);
    ~WebView();

    EVENT_SUPPORT();

public:
    // WebView_base
    virtual result_t setHtml(exlib::string html, AsyncEvent* ac);
    virtual result_t print(int32_t mode, AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac);
    virtual result_t get_visible(bool& retVal);
    virtual result_t set_visible(bool newVal);

public:
    EVENT_FUNC(load);
    EVENT_FUNC(move);
    EVENT_FUNC(resize);
    EVENT_FUNC(closed);
    EVENT_FUNC(message);

private:
    void GoBack();
    void GoForward();
    void Refresh();
    void Navigate(exlib::string szUrl);

public:
    result_t AddRef(void);
    result_t Release(void);

public:
    // async call handler & real executation.
    result_t open()
    {
        printf("[WebView::open] before \n");
        m_bSilent = false;
        m_maximize = false;

        if (m_opt) {
        }

        struct webview webview = {};

        m_webview = &webview;

        objc_nsAppInit();
        webview_init();

        AddRef();

        // result_t hr = 0;
        // while ((hr = WebView::webview_loop(m_webview, 0)) == 0)
        //     ;

        printf("[WebView::open] after\n");

        return 0;
    }
    static result_t async_open(obj_ptr<fibjs::WebView> w)
    {
        // printf("[WebView::async_open] before \n");
        w->open();
        // printf("[WebView::async_open] after\n");
        return 0;
    }

private:
    void clear();
    result_t postMessage(exlib::string msg);
    // result_t WebView::postClose();

public:
    static id webview_get_event_from_mainloop(int blocking = 0);

    static void send_event_to_sharedApplicatoin_and_check_should_exit(id event);

    // static result_t should_exit(struct webview* w)
    // {
    //     return w->priv.should_exit;
    // }

    static int webview_eval(struct webview* w, const char* js);

    static int webview_inject_css(struct webview* w, const char* css);

    static void webview_set_fullscreen(struct webview* w, int fullscreen);

    static void webview_set_color(struct webview* w, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    static void webview_dialog(struct webview* w, enum webview_dialog_type dlgtype, int flags, const char* title, const char* arg, char* result, size_t resultsz);

    static void webview_print_log(const char* s) { printf("%s\n", s); }

    static void webview_debug(const char* format, ...)
    {
        char buf[4096];
        va_list ap;
        va_start(ap, format);
        vsnprintf(buf, sizeof(buf), format, ap);
        webview_print_log(buf);
        va_end(ap);
    }

    static int webview_js_encode(const char* s, char* esc, size_t n);

    static id create_menu_item(id title, const char* action, const char* key);

    void objc_nsAppInit();

    id prepareWKPreferences();

    id getWKUserController();

    id prepareWKWebViewConfig();

    void initWindowRect()
    {
        m_webview_window_rect = CGRectMake(0, 0, m_WinW, m_WinH);
    }

    void initWindow();

    void setupWindowDelegation();

    void setupWindowTitle();

    id getWKWebView();

    void navigateWKWebView();

    void setWKWebViewStyle();

    void linkWindowWithWebview();

    void putWindowToTopOrder();

    void activeApp();

    void linkAppWithWebView()
    {
        // id app = objc_msgSend((id)objc_getClass("NSApplication"),
        //     sel_registerName("sharedApplication"));
        // objc_setAssociatedObject(app, "webview", (id)(w), OBJC_ASSOCIATION_ASSIGN);
    }

    int webview_init()
    {
        initWindowRect();

        initWindow();
        setupWindowDelegation();
        setupWindowTitle();

        // make it center
        objc_msgSend(m_nsWindow, sel_registerName("center"));

        m_wkWebView = getWKWebView();
        navigateWKWebView();

        setWKWebViewStyle();
        linkWindowWithWebview();

        putWindowToTopOrder();

        linkAppWithWebView();
        activeApp();

        SetupAppMenubar();

        // w->priv.should_exit = 0;
        return 0;
    }

    // static void webview_terminate(struct webview* w)
    // {
    //     w->priv.should_exit = 1;
    // }

    // useless, it means end up sharedApplication.
    void webview_exit();

public:
    // static registration methods, ONLY run it in GUI Thread
    static void RegNSApplicationDelegations();

    static void SetupAppMenubar();

public:
    static struct webview* getCurrentWebViewStruct_deprecated()
    {
        if (!s_activeWinObjcId)
            return NULL;

        return (struct webview*)objc_getAssociatedObject(s_activeWinObjcId, "webview");
    }

public:
    // static int on_webview_say_close(struct webview* w)
    // {
    //     WebView* wv = getClsWebView(w);

    //     if (wv) {
    //         // wv->postClose();
    //         wv->_emit("close");

    //         wv->webview_terminate(w);
    //         // TODO: use new fiber?
    //         wv->_emit("closed");

    //         wv->holder()->Unref();
    //         wv->clear();
    //         wv->Release();
    //     }
    //     return 0;
    // }

    static void onExternalClosed(struct webview* w, const char* arg)
    {
        printf("[onExternalClosed], %s \n", arg);
        // WebView* wv = (WebView*)w->clsWebView;
        // wv->_emit("closed", arg);
    }

public:
    struct webview* m_webview;

    NSWindow* m_nsWindow;
    WKWebView* m_wkWebView;
    NSAutoreleasePool* m_nsPool;

protected:
    exlib::string m_title;
    exlib::string m_url;

    int32_t m_WinW;
    int32_t m_WinH;
    int32_t m_bResizable;
    bool m_bDebug;

    obj_ptr<NObject> m_opt;

    bool m_visible;
    bool m_maximize;
    bool m_bSilent;

    // id webviewid_scriptMessageHandler;
    // id webviewid_downloadDelegate;
    // id webviewid_wkPref;
    // id webviewid_wkUserController;
    // id webviewid_wkwebviewconfig;
    CGRect m_webview_window_rect;
    // id webviewid_wkwebviewuiDel;
    // id webivewid_wkwebviewnavDel;

    AsyncEvent* m_ac;
};
} /* namespace fibjs */

#endif /* WEBVIEW_APPLE_H_ */
#endif /* __APPLE__ */