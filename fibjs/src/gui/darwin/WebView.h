/**
 * @author Richard
 * @email richardo2016@gmail.com
 * @create date 2018-04-23 03:25:07
 * @modify date 2018-04-23 03:25:42
 * @desc WebView Object for Mac OSX
 */
#if defined(__APPLE__) && !defined(FIBJS_DISABLE_GUI)

#pragma once

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
    virtual result_t getUrl(exlib::string& retVal, AsyncEvent* ac);
    virtual result_t setHtml(exlib::string html, AsyncEvent* ac);
    virtual result_t reload(AsyncEvent* ac);
    virtual result_t goBack(AsyncEvent* ac);
    virtual result_t goForward(AsyncEvent* ac);
    virtual result_t print(int32_t mode, AsyncEvent* ac);
    virtual result_t printToPDF(exlib::string file, AsyncEvent* ac);
    virtual result_t executeJavaScript(exlib::string code, AsyncEvent* ac);
    virtual result_t executeDevToolsMethod(exlib::string method, v8::Local<v8::Object> params, Variant& retVal, AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac);
    virtual result_t get_dev(v8::Local<v8::Value>& retVal);

public:
    EVENT_FUNC(open);
    EVENT_FUNC(load);
    EVENT_FUNC(address);
    EVENT_FUNC(title);
    EVENT_FUNC(move);
    EVENT_FUNC(resize);
    EVENT_FUNC(closed);
    EVENT_FUNC(message);

public:
    // async call handler & real executation.
    result_t open()
    {
        initializeWebView();
        return 0;
    }

    void forceCloseWindow();

public:
    typedef void (^JsEvaluateResultHdlr)(id result, NSError* _Nullable error);
    void evaluateWebviewJS(const char* js, JsEvaluateResultHdlr hdlr = NULL);

    void initNSWindow();

    void navigateWKWebView(exlib::string url);

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

    void assignToWKUserContentController(WKUserContentController* userCtrl)
    {
        objc_setAssociatedObject(userCtrl, "webview", (id)this, OBJC_ASSOCIATION_ASSIGN);
    }

    void assignToWKWebView(WKWebView* wkwv)
    {
        objc_setAssociatedObject(wkwv, "webview", (id)this, OBJC_ASSOCIATION_ASSIGN);
    }

    void startWKUI();

public:
    bool onNSWindowShouldClose();

    void onWKWebViewPostMessage(WKScriptMessage* message);
    void onWKWebViewInwardMessage(WKScriptMessage* message);
    void onWKWebViewExternalLogMessage(WKScriptMessage* message);

public:
    NSWindow* m_nsWindow;
    WKWebView* m_wkWebView;

public:
    bool isInternalScriptLoaded() { return m_bIScriptLoaded; }

private:
    bool m_bIScriptLoaded;

protected:
    obj_ptr<NObject> m_opt;
    exlib::string m_url;

    bool m_bDebug;

    exlib::string m_initScriptDocAfter;
};
} /* namespace fibjs */

#endif /* __APPLE__ */