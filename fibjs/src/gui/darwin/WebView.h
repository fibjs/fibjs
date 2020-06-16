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
#include <Cocoa/Cocoa.h>
#include "darwin.h" 

enum webview_dialog_type {
    WEBVIEW_DIALOG_TYPE_OPEN = 0,
    WEBVIEW_DIALOG_TYPE_SAVE = 1,
    WEBVIEW_DIALOG_TYPE_ALERT = 2
};

namespace fibjs {

static exlib::LockedList<AsyncEvent> s_uiPool;
class gui_thread : public exlib::OSThread {
public:
    virtual void Run();

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
public:
    exlib::OSSemaphore m_sem;
};

static gui_thread* s_thGUI;

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
    EVENT_FUNC(resizestart);
    EVENT_FUNC(resizeend);
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
        m_bSilent = false;
        m_maximize = false;
        m_visible = true;
        m_nsWinStyle |= NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
        m_nsWinStyle ^= NSWindowStyleMaskBorderless;

        m_bDebug = false;

        if (m_opt) {
            Variant v;

            // if (m_opt->get("border", v) == 0 && !v.boolVal())
            //     m_nsWinStyle |= NSWindowStyleMaskBorderless;
            
            if (m_opt->get("title", v) == 0)
                m_title = v.string();

            if (m_opt->get("width", v) == 0)
                m_WinW = v.intVal();

            if (m_opt->get("height", v) == 0)
                m_WinH = v.intVal();

            if (!(m_opt->get("resizable", v) == 0 && !v.boolVal()))
                m_nsWinStyle |= NSWindowStyleMaskResizable;

            if (m_opt->get("maximize", v) == 0)
                m_maximize = v.boolVal();

            if (m_opt->get("visible", v) == 0)
                m_visible = v.boolVal();

            if (m_opt->get("debug", v) == 0)
                m_bSilent = v.boolVal();
        }

        initialize();

        AddRef();

        return 0;
    }

    void forceCloseWindow();
    
    static result_t openWebViewInGUIThread(obj_ptr<fibjs::WebView> wv)
    {
        wv->open();
        return 0;
    }

private:
    void clear();
    result_t postMessage(exlib::string msg);

public:
    typedef void (^JsEvaluateResultHdlr)(id result, NSError * _Nullable error);
    void evaluateWebviewJS(const char* js, JsEvaluateResultHdlr hdlr = NULL);

    static int injectCSS(WebView* w, const char* css);

    void toggleFullScreen(int fullscreen);

    static void webview_set_color(WebView* w, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    static void webview_dialog(WebView* w, enum webview_dialog_type dlgtype, int flags, const char* title, const char* arg, char* result, size_t resultsz);

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

    static int helperEncodeJS(const char* s, char* esc, size_t n);

    static id create_menu_item(id title, const char* action, const char* key);

    void initNSWindow();

    void navigateWKWebView();

    void putWindowToTopOrder();

    static WebView* getWebViewFromNSWindow(NSWindow* win) {
        return (WebView* )objc_getAssociatedObject(win, "webview");
    }

    static WebView* getWebViewFromWKUserContentController(WKUserContentController* userCtrl) {
        return (WebView* )objc_getAssociatedObject(userCtrl, "webview");
    }

private:
    int initialize();

    void initWKWebView();

    id createWKUserContentController();

    id createWKWebViewConfig();

    void assignToToNSWindow(NSWindow* win) {
        objc_setAssociatedObject(win, "webview", (id)this, OBJC_ASSOCIATION_ASSIGN);
    }

    void assignToWKUserContentController(WKUserContentController* userCtrl) {
        objc_setAssociatedObject(userCtrl, "webview", (id)this, OBJC_ASSOCIATION_ASSIGN);
    }

    void startWKUI();

    void toggleNSWindowVisible(BOOL nextVisible);

    void centralizeWindow();

    void initNSWindowRect()
    {
        m_webview_window_rect = CGRectMake(0, 0, m_WinW, m_WinH);
    }

public:
    static void setupAppMenubar();

public:
    void onNSWindowWillClose()
    {
        _emit("closed");

        holder()->Unref();

        Release();
    }

public:
    bool onNSWindowShouldClose(bool initshouldClose);

    void onWKWebViewPostMessage(WKScriptMessage* message);
    void onWKWebViewInwardMessage(WKScriptMessage* message);
    void onWKWebViewExternalLogMessage(WKScriptMessage* message);

public:
    NSWindow* m_nsWindow;
    WKWebView* m_wkWebView;
    NSUInteger m_nsWinStyle;
    // NSAutoreleasePool* m_nsPool;

protected:
    exlib::string m_title;
    exlib::string m_url;

    int32_t m_WinW;
    int32_t m_WinH;
    bool m_bDebug;

    obj_ptr<NObject> m_opt;

    bool m_visible;
    bool m_fullscreen;

    bool m_maximize;
    bool m_bSilent;
    
    CGRect m_webview_window_rect;

    AsyncEvent* m_ac;
};
} /* namespace fibjs */

#endif /* WEBVIEW_APPLE_H_ */
#endif /* __APPLE__ */