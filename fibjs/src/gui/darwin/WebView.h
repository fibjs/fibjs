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
#include "darwin_utils.h"

namespace fibjs {

class WebView : public WebView_base {
    FIBER_FREE();

public:
    WebView(exlib::string url, NObject* opt);
    ~WebView();

    result_t open();

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
    // Message Flow
    // static id GetIDsOfNames(REFIID riid, OLECHAR** rgszNames, unsigned int cNames, LCID lcid, DISPID* rgdispid);
    void Invoke(id self, SEL cmd, id contentController, id message);

public:
    const char* get_url()
    {
        return m_url.c_str();
    };

private:
    void clear();

public:
    typedef void (*external_invoke_cb_t)(WebView* w, const char* arg);
    id nso;
    // Cocoa infos
    // struct fibjs::webview winfo;
    /* priv about: start */;
    id priv_pool;
    id priv_window;
    id priv_webview;
    id priv_windowDelegate;
    int priv_should_exit;
    /* priv about: end */

    /* webview about :start */
    const char* webview__url;
    const char* webview__title;
    int webview__width;
    int webview__height;
    int webview__resizable;
    int webview__debug;
    WebView::external_invoke_cb_t webview__external_invoke_cb;
    void* webview__userdata;
    /* webview about :end */

    objc_id userController;
    objc_id scriptMessageHandler;
    objc_id downloadDelegate;
    objc_id wkPref;
    objc_id nsTitle;
    objc_id nsURL;
    objc_id navDel;
    objc_id uiDel;
    // @WKWebViewConfiguration
    objc_id wkconfig;
    objc_id processPool;

    CGRect rect;

    WebView::external_invoke_cb_t _onmessage;
    WebView::external_invoke_cb_t _onclose;

public:
    // Cocoa internal call
    // void webview_init();

    // void RegNSApplication();
    int32_t init();
    void customizeWKScriptMessage();
    void customizeWKDownloadDelegate();
    void customizeWKPreference();
    void initUserController();
    void initParentWindow();
    void initNavigation();
    void initWKWebView();
    void finishSetupApplication();
    void finishSetupMenu();

    int32_t loop();
    void exit();

    void terminate();

public:
    // handlers
    static void run_open_panel(id self, SEL cmd, id wkWebView, id parameters,
        id frame, void (^completionHandler)(id))
    {
        printf("run_open_panel \n");
        id openPanel = objc_msgSend((id)objc_getClass("NSOpenPanel"),
            sel_registerName("openPanel"));

        objc_msgSend(
            openPanel, sel_registerName("setAllowsMultipleSelection:"),
            objc_msgSend(parameters, sel_registerName("allowsMultipleSelection")));

        objc_msgSend(openPanel, sel_registerName("setCanChooseFiles:"), 1);
        objc_msgSend(
            openPanel, sel_registerName("beginWithCompletionHandler:"), ^(id result) {
                if (result == (id)NSModalResponseOK) {
                    completionHandler(objc_msgSend(openPanel, sel_registerName("URLs")));
                } else {
                    completionHandler(nil);
                }
            });
    }

    static void run_alert_panel(id self, SEL cmd, id wkWebView, id message, id frame,
        void (^completionHandler)(void))
    {
        id alert = objc_msgSend((id)objc_getClass("NSAlert"), sel_registerName("new"));
        objc_msgSend(alert, sel_registerName("setIcon:"),
            objc_msgSend((id)objc_getClass("NSImage"),
                sel_registerName("imageNamed:"),
                get_nsstring("NSCaution")));
        objc_msgSend(alert, sel_registerName("setShowsHelp:"), 0);
        objc_msgSend(alert, sel_registerName("setInformativeText:"), message);
        objc_msgSend(alert, sel_registerName("addButtonWithTitle:"),
            get_nsstring("OK"));
        objc_msgSend(alert, sel_registerName("runModal"));
        objc_msgSend(alert, sel_registerName("release"));
        completionHandler();
    }

    static void run_confirmation_panel(id self, SEL cmd, id webView, id message,
        id frame, void (^completionHandler)(bool))
    {

        id alert = objc_msgSend((id)objc_getClass("NSAlert"), sel_registerName("new"));
        objc_msgSend(alert, sel_registerName("setIcon:"),
            objc_msgSend((id)objc_getClass("NSImage"),
                sel_registerName("imageNamed:"),
                get_nsstring("NSCaution")));
        objc_msgSend(alert, sel_registerName("setShowsHelp:"), 0);
        objc_msgSend(alert, sel_registerName("setInformativeText:"), message);
        objc_msgSend(alert, sel_registerName("addButtonWithTitle:"),
            get_nsstring("OK"));
        objc_msgSend(alert, sel_registerName("addButtonWithTitle:"),
            get_nsstring("Cancel"));
        if (objc_msgSend(alert, sel_registerName("runModal")) == (id)NSAlertFirstButtonReturn) {
            completionHandler(true);
        } else {
            completionHandler(false);
        }
        objc_msgSend(alert, sel_registerName("release"));
    }

    static void webview_window_will_close(id self, SEL cmd, id notification)
    {
        WebView* w = (WebView*)objc_getAssociatedObject(self, "webview");
        assert(w != NULL);

        w->terminate();
    }

    static void webview_external_invoke(id self, SEL cmd, id contentController, id message)
    {
        WebView* w = (WebView*)objc_getAssociatedObject(contentController, "webview");
        if (w == NULL || w->webview__external_invoke_cb == NULL) {
            return;
        }

        w->webview__external_invoke_cb(w, (const char*)objc_msgSend(objc_msgSend(message, sel_registerName("body")), sel_registerName("UTF8String")));
    }

protected:
    exlib::string m_url;
    obj_ptr<NObject> m_opt;

    bool m_visible;
    bool m_maximize;
    bool m_bSilent;
    bool m_isdebug;

    AsyncEvent* m_ac;
};
} /* namespace fibjs */

#endif /* WEBVIEW_APPLE_H_ */
#endif /* __APPLE__ */