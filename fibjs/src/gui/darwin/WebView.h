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
#include "lib.h"

namespace fibjs {

class WebView;

WebView* getClsWebView(struct webview* w)
{
    return (WebView*)w->clsWebView;
}

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
    result_t openFromAsyncCall()
    {
        printf("[openFromAsyncCall] before \n");
        m_bSilent = false;
        m_maximize = false;

        if (m_opt) {
        }

        AddRef();

        struct webview webview = {};
        webview.title = m_title.c_str();
        webview.url = m_url.c_str();
        webview.width = m_WinW;
        webview.height = m_WinH;
        webview.resizable = m_bResizable;
        webview.debug = m_bDebug;
        webview.clsWebView = this;

        {
            webview.objc_msgHandlers = {};
            webview.objc_msgHandlers.webview_external_postMessage = WebView::webview_external_postMessage;
            webview.objc_msgHandlers.webview_windowDidMove = WebView::webview_windowDidMove;
            webview.objc_msgHandlers.webview_windowWillClose = WebView::webview_windowWillClose;
        }

        webview_init(&webview);

        while (webview_loop(&webview, 0) == 0)
            ;
        // webview_exit(/* &webview */);

        Unref();
        printf("[openFromAsyncCall] after");

        return CALL_E_NOASYNC;
    }
    static result_t async_open(obj_ptr<fibjs::WebView> w)
    {
        printf("[WebView::async_open]\n");
        w->openFromAsyncCall();
        return 0;
    }

private:
    void clear();
    // result_t WebView::postClose();

public:
    // static std::string url_encode(const std::string& value)
    // {
    //     const char hex[] = "0123456789ABCDEF";
    //     std::string escaped;
    //     for (char c : value) {
    //         if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == '=') {
    //             escaped = escaped + c;
    //         } else {
    //             escaped = escaped + '%' + hex[(c >> 4) & 0xf] + hex[c & 0xf];
    //         }
    //     }
    //     return escaped;
    // }

public:
    static void webview_applicationDidFinishLaunching(id applicationDidFinishLaunching)
    {
        printf("[webview_applicationDidFinishLaunching] 看看 appDelegate 生效没\n");
    }
    static void webview_applicationWillTerminate(id applicationWillTerminate)
    {
        printf("[webview_applicationWillTerminate] 看看 appDelegate 生效没 \n");
        return;
    }
    static int webview_applicationShouldTerminate(id applicationShouldTerminate)
    {
        printf("[webview_applicationShouldTerminate] 看看 appDelegate 生效没 \n");
        // NSTerminateNow = 1
        // NSTerminateLater = 2
        return 1;
    }
    static bool webview_applicationShouldTerminateAfterLastWindowClosed(id app)
    {
        printf("[webview_applicationShouldTerminateAfterLastWindowClosed] 看看 appDelegate 生效没 \n");
        return false;
    }

    // only run it in GUI Thread
    static void RegStopTerminateAfterLastWindowClosed()
    {
        Class __NSApplicationDelegate = objc_allocateClassPair(objc_getClass("NSObject"),
            "__NSApplicationDelegate", 0);
        /**
         * @see https://developer.apple.com/documentation/appkit/nsapplicationdelegate
         */
        // class_addProtocol(__NSApplicationDelegate, objc_getProtocol("NSApplicationDelegate"));
        class_addMethod(__NSApplicationDelegate, sel_registerName("applicationWillTerminate:"),
            (IMP)WebView::webview_applicationWillTerminate, "v@:@");
        class_addMethod(__NSApplicationDelegate, sel_registerName("applicationDidFinishLaunching:"),
            (IMP)WebView::webview_applicationDidFinishLaunching, "v@:@");
        class_addMethod(__NSApplicationDelegate, sel_registerName("applicationShouldTerminate:"),
            (IMP)WebView::webview_applicationShouldTerminate, "v@:@");
        class_addMethod(__NSApplicationDelegate, sel_registerName("applicationShouldTerminateAfterLastWindowClosed:"),
            (IMP)WebView::webview_applicationShouldTerminateAfterLastWindowClosed, "v@:@");

        objc_registerClassPair(__NSApplicationDelegate);

        id appDelegate = objc_msgSend((id)__NSApplicationDelegate, sel_registerName("new"));
        objc_msgSend(objc_msgSend((id)objc_getClass("NSApplication"),
                         sel_registerName("sharedApplication")),
            sel_registerName("setDelegate:"), appDelegate);
        // objc_msgSend((id)objc_getClass("NSApp"),
        //     sel_registerName("setDelegate:"), appDelegate);
    }
    // interact with webview_** api
    static void onExternalLoad(struct webview* w, const char* arg)
    {
        printf("[onExternalLoad], %s \n", arg);
        WebView* wv = (WebView*)w->clsWebView;
        wv->_emit("load", arg);
    }
    static void onExternalInvoke(struct webview* w, const char* arg)
    {
        printf("[onExternalInvoke], %s \n", arg);
        WebView* wv = (WebView*)w->clsWebView;
        wv->_emit("invoke", arg);
    }

    static void webview_windowDidMove(id self, SEL cmd, id didMoveNotification)
    {
        struct webview* w = (struct webview*)objc_getAssociatedObject(self, "webview");
        if (w == NULL)
            return;

        WebView* wv = getClsWebView(w);
        if (wv == NULL)
            return;

        // TODO: use information in didMoveNotification
        printf("[onWindowDidMove]\n");
        wv->_emit("move");
    }

    static void webview_external_postMessage(id self, SEL cmd, id userContentController, id message)
    {
        printf("[webview_external_postMessage] \n");
        struct webview* w = (struct webview*)objc_getAssociatedObject(userContentController, "webview");
        if (w == NULL)
            return;

        WebView* wv = getClsWebView(w);
        if (wv == NULL)
            return;
        const char* arg = (const char*)objc_msgSend(objc_msgSend(message, sel_registerName("body")), sel_registerName("UTF8String"));
        // normalize to one function
        wv->_emit("message", arg);
    }

    static void webview_windowWillClose(id self, SEL cmd, id willCloseNotification)
    {
        printf("[webview_windowWillClose] \n");
        struct webview* w = (struct webview*)objc_getAssociatedObject(self, "webview");

        WebView* wv = getClsWebView(w);

        // wv->postClose();
        wv->_emit("close");

        // webview_terminate(w);
        // TODO: use new fiber?
        wv->_emit("closed");

        wv->holder()->Unref();
        wv->clear();
        wv->Release();
    }

    static void onExternalClosed(struct webview* w, const char* arg)
    {
        printf("[onExternalClosed], %s \n", arg);
        WebView* wv = (WebView*)w->clsWebView;
        wv->_emit("closed", arg);
    }

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

    AsyncEvent* m_ac;
};
} /* namespace fibjs */

#endif /* WEBVIEW_APPLE_H_ */
#endif /* __APPLE__ */