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
    result_t open()
    {
        printf("[WebView::open] before \n");
        m_bSilent = false;
        m_maximize = false;

        if (m_opt) {
        }

        struct webview webview = {};
        webview.title = m_title.c_str();
        webview.url = m_url.c_str();
        webview.width = m_WinW;
        webview.height = m_WinH;
        webview.resizable = m_bResizable;
        webview.debug = m_bDebug;
        webview.clsWebView = this;

        m_webview = &webview;

        objc_nsAppInit(m_webview);
        webview_init(m_webview);

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

    static void send_event_to_sharedApplicatoin_and_check_should_exit(id event)
    {
        if (event) {
            objc_msgSend(objc_msgSend((id)objc_getClass("NSApplication"),
                             sel_registerName("sharedApplication")),
                sel_registerName("sendEvent:"), event);
        }
    }

    static result_t should_exit(struct webview* w)
    {
        return w->priv.should_exit;
    }

    // pure C API about webview
    static int webview_loop(struct webview* w, int blocking)
    {
        id event = WebView::webview_get_event_from_mainloop(blocking);

        WebView::send_event_to_sharedApplicatoin_and_check_should_exit(event);

        return WebView::should_exit(w);
    }

    void objc_nsAppInit(struct webview* w);

    id prepareWKScriptMessageHandler();

    id prepareWKDownloadDelegate()
    {
        /***
            _WKDownloadDelegate is an undocumented/private protocol with methods called
            from WKNavigationDelegate
            References:
            https://github.com/WebKit/webkit/blob/master/Source/WebKit/UIProcess/API/Cocoa/_WKDownload.h
            https://github.com/WebKit/webkit/blob/master/Source/WebKit/UIProcess/API/Cocoa/_WKDownloadDelegate.h
            https://github.com/WebKit/webkit/blob/master/Tools/TestWebKitAPI/Tests/WebKitCocoa/Download.mm
        ***/

        Class __WKDownloadDelegate = objc_allocateClassPair(
            objc_getClass("NSObject"), "__WKDownloadDelegate", 0);
        class_addMethod(
            __WKDownloadDelegate,
            sel_registerName("_download:decideDestinationWithSuggestedFilename:"
                             "completionHandler:"),
            (IMP)run_save_panel, "v@:@@?");
        class_addMethod(__WKDownloadDelegate,
            sel_registerName("_download:didFailWithError:"),
            (IMP)download_failed, "v@:@@");
        objc_registerClassPair(__WKDownloadDelegate);
        return objc_msgSend((id)__WKDownloadDelegate, sel_registerName("new"));
    }

    id prepareWKPreferences(struct webview* w)
    {

        Class __WKPreferences
            = objc_allocateClassPair(objc_getClass("WKPreferences"),
                "__WKPreferences", 0);
        objc_property_attribute_t type = { "T", "c" };
        objc_property_attribute_t ownership = { "N", "" };
        objc_property_attribute_t attrs[] = { type, ownership };
        class_replaceProperty(__WKPreferences, "developerExtrasEnabled", attrs, 2);
        objc_registerClassPair(__WKPreferences);
        id webviewid_wkPref = objc_msgSend((id)__WKPreferences, sel_registerName("new"));
        objc_msgSend(webviewid_wkPref, sel_registerName("setValue:forKey:"),
            objc_msgSend((id)objc_getClass("NSNumber"),
                sel_registerName("numberWithBool:"), !!w->debug),
            objc_msgSend((id)objc_getClass("NSString"),
                sel_registerName("stringWithUTF8String:"),
                "developerExtrasEnabled"));

        return webviewid_wkPref;
    }

    id getWKUserController(struct webview* w);

    id prepareWKWebViewConfig(struct webview* w);

    void initWindowRect(struct webview* w)
    {
        this->webview_window_rect = CGRectMake(0, 0, w->width, w->height);
    }

    void initWindow(struct webview* w);

    void setupWindowDelegation(struct webview* w)
    {
        Class __NSWindowDelegate = objc_allocateClassPair(objc_getClass("NSObject"), "__NSWindowDelegate", 0);
        /**
         * @see https://developer.apple.com/documentation/appkit/nswindowdelegate
         */
        class_addProtocol(__NSWindowDelegate, objc_getProtocol("NSWindowDelegate"));

        class_replaceMethod(__NSWindowDelegate, sel_registerName("windowWillClose:"),
            (IMP)WebView::webview_windowWillClose, "v@:@");
        class_replaceMethod(__NSWindowDelegate, sel_registerName("windowDidMove:"),
            (IMP)WebView::webview_windowDidMove, "v@:@");
        class_replaceMethod(__NSWindowDelegate, sel_registerName("windowShouldClose:"),
            (IMP)WebView::webview_windowShouldClose, "v@:@");

        objc_registerClassPair(__NSWindowDelegate);

        w->priv.windowDelegate = objc_msgSend((id)__NSWindowDelegate, sel_registerName("new"));
        objc_setAssociatedObject(w->priv.windowDelegate, "webview", (id)(w), OBJC_ASSOCIATION_ASSIGN);

        objc_msgSend(w->priv.window, sel_registerName("setDelegate:"), w->priv.windowDelegate);
    }

    void setupWindowTitle(struct webview* w)
    {
        // title
        id nsTitle = objc_msgSend((id)objc_getClass("NSString"),
            sel_registerName("stringWithUTF8String:"), w->title);
        objc_msgSend(w->priv.window, sel_registerName("setTitle:"), nsTitle);
    }

    id prepareWKWebViewUIDelegation()
    {
        Class __WKUIDelegate = objc_allocateClassPair(objc_getClass("NSObject"), "__WKUIDelegate", 0);
        class_addProtocol(__WKUIDelegate, objc_getProtocol("WKUIDelegate"));
        class_addMethod(__WKUIDelegate,
            sel_registerName("webView:runOpenPanelWithParameters:"
                             "initiatedByFrame:completionHandler:"),
            (IMP)run_open_panel, "v@:@@@?");
        class_addMethod(__WKUIDelegate,
            sel_registerName("webView:runJavaScriptAlertPanelWithMessage:"
                             "initiatedByFrame:completionHandler:"),
            (IMP)run_alert_panel, "v@:@@@?");
        class_addMethod(
            __WKUIDelegate,
            sel_registerName("webView:runJavaScriptConfirmPanelWithMessage:"
                             "initiatedByFrame:completionHandler:"),
            (IMP)run_confirmation_panel, "v@:@@@?");
        objc_registerClassPair(__WKUIDelegate);
        return objc_msgSend((id)__WKUIDelegate, sel_registerName("new"));
    }

    id prepareWkWebViewNavDelegation()
    {
        Class __WKNavigationDelegate = objc_allocateClassPair(
            objc_getClass("NSObject"), "__WKNavigationDelegate", 0);
        class_addProtocol(__WKNavigationDelegate,
            objc_getProtocol("WKNavigationDelegate"));
        class_addMethod(
            __WKNavigationDelegate,
            sel_registerName(
                "webView:decidePolicyForNavigationResponse:decisionHandler:"),
            (IMP)make_nav_policy_decision, "v@:@@?");
        objc_registerClassPair(__WKNavigationDelegate);
        return objc_msgSend((id)__WKNavigationDelegate, sel_registerName("new"));
    }

    id getWKWebView(struct webview* w)
    {
        id webview = objc_msgSend((id)objc_getClass("WKWebView"), sel_registerName("alloc"));

        objc_msgSend(webview,
            sel_registerName("initWithFrame:configuration:"), this->webview_window_rect, prepareWKWebViewConfig(w));
        objc_msgSend(webview, sel_registerName("setUIDelegate:"), prepareWKWebViewUIDelegation());
        objc_msgSend(webview, sel_registerName("setNavigationDelegate:"), prepareWkWebViewNavDelegation());

        return webview;
    }

    void navigateWKWebView(struct webview* w)
    {
        id nsURL = objc_msgSend((id)objc_getClass("NSURL"),
            sel_registerName("URLWithString:"),
            get_nsstring(webview_check_url(w->url)));

        objc_msgSend(w->priv.webview, sel_registerName("loadRequest:"),
            objc_msgSend((id)objc_getClass("NSURLRequest"),
                sel_registerName("requestWithURL:"), nsURL));
    }

    void setWKWebViewStyle(id webview)
    {
        objc_msgSend(webview, sel_registerName("setAutoresizesSubviews:"), 1);
        objc_msgSend(webview, sel_registerName("setAutoresizingMask:"),
            (NSViewWidthSizable | NSViewHeightSizable));
    }

    void linkWindowWithWebview(struct webview* w)
    {
        objc_msgSend(objc_msgSend(w->priv.window, sel_registerName("contentView")),
            sel_registerName("addSubview:"), w->priv.webview);
    }

    void putWindowToTopOrder(struct webview* w)
    {
        objc_msgSend(w->priv.window, sel_registerName("orderFrontRegardless"));
    }

    void activeApp()
    {
        objc_msgSend(objc_msgSend((id)objc_getClass("NSApplication"),
                         sel_registerName("sharedApplication")),
            sel_registerName("setActivationPolicy:"),
            NSApplicationActivationPolicyRegular);

        objc_msgSend(objc_msgSend((id)objc_getClass("NSApplication"),
                         sel_registerName("sharedApplication")),
            sel_registerName("finishLaunching"));

        objc_msgSend(objc_msgSend((id)objc_getClass("NSApplication"),
                         sel_registerName("sharedApplication")),
            sel_registerName("activateIgnoringOtherApps:"), 1);
    }

    void linkAppWithWebView(struct webview* w)
    {
        // id app = objc_msgSend((id)objc_getClass("NSApplication"),
        //     sel_registerName("sharedApplication"));
        // objc_setAssociatedObject(app, "webview", (id)(w), OBJC_ASSOCIATION_ASSIGN);
    }

    int webview_init(struct webview* w)
    {
        initWindowRect(w);

        initWindow(w);
        setupWindowDelegation(w);
        setupWindowTitle(w);

        // make it center
        objc_msgSend(w->priv.window, sel_registerName("center"));

        w->priv.webview = getWKWebView(w);
        navigateWKWebView(w);

        setWKWebViewStyle(w->priv.webview);
        linkWindowWithWebview(w);

        putWindowToTopOrder(w);

        linkAppWithWebView(w);
        activeApp();

        SetupAppMenubar();

        w->priv.should_exit = 0;
        return 0;
    }

    static void webview_terminate(struct webview* w)
    {
        w->priv.should_exit = 1;
    }

    // useless, it means end up sharedApplication.
    void webview_exit()
    {
        id app = objc_msgSend((id)objc_getClass("NSApplication"),
            sel_registerName("sharedApplication"));

        objc_msgSend(app, sel_registerName("terminate:"), app);
    }

public:
    // static registration methods, ONLY run it in GUI Thread
    static void RegNSApplicationDelegations();

    static void SetupAppMenubar()
    {
        id menubar = objc_msgSend((id)objc_getClass("NSMenu"), sel_registerName("alloc"));
        objc_msgSend(menubar, sel_registerName("initWithTitle:"), get_nsstring(""));
        objc_msgSend(menubar, sel_registerName("autorelease"));

        // id appName = objc_msgSend(objc_msgSend((id)objc_getClass("NSProcessInfo"),
        //                               sel_registerName("processInfo")),
        //     sel_registerName("processName"));
        id appName = get_nsstring("喵喵喵");

        id appMenuItem = objc_msgSend((id)objc_getClass("NSMenuItem"), sel_registerName("alloc"));
        objc_msgSend(appMenuItem,
            sel_registerName("initWithTitle:action:keyEquivalent:"), appName,
            NULL, get_nsstring(""));

        id appMenu = objc_msgSend((id)objc_getClass("NSMenu"), sel_registerName("alloc"));
        objc_msgSend(appMenu, sel_registerName("initWithTitle:"), appName);
        objc_msgSend(appMenu, sel_registerName("autorelease"));

        objc_msgSend(appMenuItem, sel_registerName("setSubmenu:"), appMenu);
        objc_msgSend(menubar, sel_registerName("addItem:"), appMenuItem);

        id title = objc_msgSend(get_nsstring("Hide "),
            sel_registerName("stringByAppendingString:"), appName);
        id item = create_menu_item(title, "hide:", "h");
        objc_msgSend(appMenu, sel_registerName("addItem:"), item);

        item = create_menu_item(get_nsstring("Hide Others"),
            "hideOtherApplications:", "h");
        objc_msgSend(item, sel_registerName("setKeyEquivalentModifierMask:"),
            (NSEventModifierFlagOption | NSEventModifierFlagCommand));
        objc_msgSend(appMenu, sel_registerName("addItem:"), item);

        item = create_menu_item(get_nsstring("Show All"), "unhideAllApplications:", "");
        objc_msgSend(appMenu, sel_registerName("addItem:"), item);

        objc_msgSend(appMenu, sel_registerName("addItem:"),
            objc_msgSend((id)objc_getClass("NSMenuItem"),
                sel_registerName("separatorItem")));

        title = objc_msgSend(get_nsstring("Quit "),
            sel_registerName("stringByAppendingString:"), appName);
        item = create_menu_item(title, "terminate:", "q");
        objc_msgSend(appMenu, sel_registerName("addItem:"), item);

        objc_msgSend(objc_msgSend((id)objc_getClass("NSApplication"),
                         sel_registerName("sharedApplication")),
            sel_registerName("setMainMenu:"), menubar);
    }

public:
    static struct webview* getCurrentWebViewStruct()
    {
        if (!s_activeWinObjcId)
            return NULL;

        // printf("s_activeWinObjcId is not NULL \n");

        struct webview* w = (struct webview*)objc_getAssociatedObject(s_activeWinObjcId, "webview");

        return w;
    }
    static WebView* getCurrentWebViewInstance()
    {

        struct webview* w = getCurrentWebViewStruct();
        if (w == NULL)
            return NULL;

        WebView* wv = getClsWebView(w);

        return wv;
    }

public:
    // objc handlers of delegations
    static bool webview_windowShouldClose(id window)
    {
        printf("[webview_windowShouldClose] 看看 winDelegate 生效没 \n");

        // id alert = objc_msgSend((id)objc_getClass("NSAlert"), sel_registerName("new"));
        // objc_msgSend(alert, sel_registerName("setAlertStyle:"), NSAlertStyleWarning);
        // objc_msgSend(alert, sel_registerName("setMessageText:"), get_nsstring("确定退出吗?"));
        // objc_msgSend(alert, sel_registerName("addButtonWithTitle:"), get_nsstring("退出"));
        // objc_msgSend(alert, sel_registerName("addButtonWithTitle:"), get_nsstring("取消"));

        // unsigned long result = (unsigned long)objc_msgSend(alert, sel_registerName("runModal"));
        // objc_msgSend(alert, sel_registerName("release"));

        // if (result != NSAlertFirstButtonReturn) {
        //     return NO;
        // }
        return YES;
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

        // obj_ptr<EventInfo> ei = new EventInfo(wv, "move");
        // wv->_emit("move", ei);

        // wv->_emit("move");
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
        const char* msg = (const char*)objc_msgSend(objc_msgSend(message, sel_registerName("body")), sel_registerName("UTF8String"));
        // normalize to one function
        printf("[webview_external_postMessage] view view msg %s \n", w->title);

        // wv->_emit("message", msg);
    }

    static void webview_windowWillClose(id self, SEL cmd, id willCloseNotification)
    {
        printf("[webview_windowWillClose] before \n");
        // struct webview* w = (struct webview*)objc_getAssociatedObject(self, "webview");

        // if (w != NULL)
        //     WebView::on_webview_say_close(w);

        // asyncCall(WebView::on_webview_say_close, w, CALL_E_GUICALL);
        printf("[webview_windowWillClose] after \n");
    }

    static int on_webview_say_close(struct webview* w)
    {
        WebView* wv = getClsWebView(w);

        if (wv) {
            // wv->postClose();
            wv->_emit("close");

            wv->webview_terminate(w);
            // TODO: use new fiber?
            wv->_emit("closed");

            wv->holder()->Unref();
            wv->clear();
            wv->Release();
        }
        return 0;
    }

    static void onExternalClosed(struct webview* w, const char* arg)
    {
        printf("[onExternalClosed], %s \n", arg);
        WebView* wv = (WebView*)w->clsWebView;
        wv->_emit("closed", arg);
    }

public:
    struct webview* m_webview;

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
    CGRect webview_window_rect;
    // id webviewid_wkwebviewuiDel;
    // id webivewid_wkwebviewnavDel;

    AsyncEvent* m_ac;
};
} /* namespace fibjs */

#endif /* WEBVIEW_APPLE_H_ */
#endif /* __APPLE__ */