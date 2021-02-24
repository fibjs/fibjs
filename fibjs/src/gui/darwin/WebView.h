/**
 * @author Richard
 * @email richardo2016@gmail.com
 * @create date 2018-04-23 03:25:07
 * @modify date 2018-04-23 03:25:42
 * @desc WebView Object for Mac OSX
 */
#if defined(__APPLE__) && !defined(FIBJS_DISABLE_GUI)

#ifndef WEBVIEW_APPLE_H_
#define WEBVIEW_APPLE_H_

#include "ifs/WebView.h"
#include "EventInfo.h"
#include <Cocoa/Cocoa.h>
#include "ns-api.h"

namespace fibjs {

class WebView : public WebView_base {
    FIBER_FREE();

public:
    WebView(exlib::string url, NObject* opt);
    ~WebView();

    EVENT_SUPPORT();

public:
    // WebView_base
    virtual result_t loadUrl(exlib::string url, AsyncEvent* ac);
    virtual result_t setHtml(exlib::string html, AsyncEvent* ac);
    virtual result_t reload(AsyncEvent* ac);
    virtual result_t goBack(AsyncEvent* ac);
    virtual result_t goForward(AsyncEvent* ac);
    virtual result_t print(int32_t mode, AsyncEvent* ac);
    virtual result_t printToPDF(exlib::string file, AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac);
    virtual result_t get_visible(bool& retVal);
    virtual result_t set_visible(bool newVal);

public:
    EVENT_FUNC(open);
    EVENT_FUNC(load);
    EVENT_FUNC(move);
    EVENT_FUNC(resize);
    EVENT_FUNC(resizestart);
    EVENT_FUNC(resizeend);
    EVENT_FUNC(closed);
    EVENT_FUNC(message);

public:
    // async call handler & real executation.
    result_t open()
    {
        initializeWebView();

        Ref();

        return 0;
    }

    void forceCloseWindow();

private:
    void clear();
    result_t postMessage(exlib::string msg);

public:
    typedef void (^JsEvaluateResultHdlr)(id result, NSError* _Nullable error);
    void evaluateWebviewJS(const char* js, JsEvaluateResultHdlr hdlr = NULL);

    void toggleFullScreen(int fullscreen);

    void setWindowColor(WebView* w, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    static int helperEncodeJS(const char* s, char* esc, size_t n);

    static NSMenuItem* createMenuItem(id title, const char* action, const char* key);

    void initNSWindow();

    void navigateWKWebView(exlib::string url);

    void putWindowToTopOrder();

    static WebView* getWebViewFromNSWindow(NSWindow* win)
    {
        return (WebView*)objc_getAssociatedObject(win, "webview");
    }

    static WebView* getWebViewFromWKUserContentController(WKUserContentController* userCtrl)
    {
        return (WebView*)objc_getAssociatedObject(userCtrl, "webview");
    }

    static WebView* getWebViewFromWKWebView(WKWebView* wkwv)
    {
        return (WebView*)objc_getAssociatedObject(wkwv, "webview");
    }

private:
    int initializeWebView();

    void initWKWebView();

    id createWKUserContentController();

    id createWKWebViewConfig();

    void assignToToNSWindow(NSWindow* win)
    {
        objc_setAssociatedObject(win, "webview", (id)this, OBJC_ASSOCIATION_ASSIGN);
    }

    void assignToWKUserContentController(WKUserContentController* userCtrl)
    {
        objc_setAssociatedObject(userCtrl, "webview", (id)this, OBJC_ASSOCIATION_ASSIGN);
    }

    void assignToWKWebView(WKWebView* wkwv)
    {
        objc_setAssociatedObject(wkwv, "webview", (id)this, OBJC_ASSOCIATION_ASSIGN);
    }

    void startWKUI();

    void centralizeWindow();

    void initNSWindowRect()
    {
        m_nsWindowFrame = CGRectMake(0, 0, m_WinW, m_WinH);
    }

public:
    void setupAppMenubar();

public:
    void onNSWindowWillClose()
    {
        _emit("closed");

        m_nsWindow = nil;
        m_wkWebView = nil;

        holder()->Unref();

        Unref();
    }

public:
    bool onNSWindowShouldClose();

    void onWKWebViewPostMessage(WKScriptMessage* message);
    void onWKWebViewInwardMessage(WKScriptMessage* message);
    void onWKWebViewExternalLogMessage(WKScriptMessage* message);

public:
    NSWindow* m_nsWindow;
    WKWebView* m_wkWebView;
    NSUInteger m_nsWinStyle;
    NSUInteger m_wkViewStyle;

public:
    static result_t asyncToggleVisible(WebView* wv);

public:
    bool isInternalScriptLoaded() { return m_bIScriptLoaded; }

private:
    bool m_bIScriptLoaded;

protected:
    exlib::string m_title;
    exlib::string m_url;

    int32_t m_WinW;
    int32_t m_WinH;
    bool m_visible;
    bool m_bDebug;

    obj_ptr<NObject> m_opt;

    bool m_fullscreenable;

    bool m_maximize;

    exlib::string m_initScriptDocAfter;

    CGRect m_nsWindowFrame;
    bool m_iUseContentViewController;

    AsyncEvent* m_ac;
};
} /* namespace fibjs */

#endif /* WEBVIEW_APPLE_H_ */
#endif /* __APPLE__ */