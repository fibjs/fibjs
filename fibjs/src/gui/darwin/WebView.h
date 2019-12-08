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
    result_t openFromAsyncCall();
    static result_t async_open(obj_ptr<fibjs::WebView> w)
    {
        printf("[WebView::async_open]\n");
        w->openFromAsyncCall();
        return 0;
    }

private:
    void clear();

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
        wv->_emit("close");

        webview_terminate(w);
        // TODO: use new fiber?
        wv->_emit("closed");
    }

    static void onExternalClosed(struct webview* w, const char* arg)
    {
        printf("[onExternalClosed], %s \n", arg);
        WebView* wv = (WebView*)w->clsWebView;
        wv->_emit("closed", arg);
    }
    // typedef void (*external_invoke_cb_t)(WebView* w, const char* arg);
    // id nso;
    // // Cocoa infos
    // // struct fibjs::webview winfo;
    // /* priv about: start */;
    // id priv_pool;
    // id priv_window;
    // id priv_webview;
    // id priv_windowDelegate;
    // int priv_should_exit;
    // /* priv about: end */

    // /* webview about :start */
    // const char* webview__url;
    // const char* webview__title;
    // int webview__width;
    // int webview__height;
    // int webview__resizable;
    // int webview__debug;
    // WebView::external_invoke_cb_t webview__external_invoke_cb;
    // void* webview__userdata;
    // /* webview about :end */

    // id userController;
    // id scriptMessageHandler;
    // id downloadDelegate;
    // id wkPref;
    // id nsTitle;
    // id nsURL;
    // id navDel;
    // id uiDel;
    // // @WKWebViewConfiguration
    // id wkconfig;
    // id processPool;

    // CGRect rect;

    // WebView::external_invoke_cb_t _onmessage;
    // WebView::external_invoke_cb_t _onclose;

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