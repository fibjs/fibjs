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

        objc_nsAppInit(&webview);
        webview_init(&webview);

        while (WebView::webview_loop(&webview, 0) == 0)
            ;
        // webview_exit(/* &webview */);

        Unref();
        printf("[openFromAsyncCall] after\n");

        return CALL_E_NOSYNC;
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

private:
    // pure C API about webview
    int webview_loop(struct webview* w, int blocking)
    {
        id until = (blocking ? objc_msgSend((id)objc_getClass("NSDate"),
                                   sel_registerName("distantFuture"))
                             : objc_msgSend((id)objc_getClass("NSDate"),
                                   sel_registerName("distantPast")));

        id event = objc_msgSend(
            objc_msgSend((id)objc_getClass("NSApplication"),
                sel_registerName("sharedApplication")),
            sel_registerName("nextEventMatchingMask:untilDate:inMode:dequeue:"),
            ULONG_MAX, until,
            objc_msgSend((id)objc_getClass("NSString"),
                sel_registerName("stringWithUTF8String:"),
                "kCFRunLoopDefaultMode"),
            true);

        if (event) {
            objc_msgSend(objc_msgSend((id)objc_getClass("NSApplication"),
                             sel_registerName("sharedApplication")),
                sel_registerName("sendEvent:"), event);
        }

        return w->priv.should_exit;
    }

    void objc_nsAppInit(struct webview* w)
    {
        w->priv.pool = objc_msgSend((id)objc_getClass("NSAutoreleasePool"),
            sel_registerName("new"));
        objc_msgSend((id)objc_getClass("NSApplication"),
            sel_registerName("sharedApplication"));
    }

    id prepareWKScriptMessageHandler(struct webview* w)
    {
        Class __WKScriptMessageHandler = objc_allocateClassPair(
            objc_getClass("NSObject"), "__WKScriptMessageHandler", 0);
        assert(w->objc_msgHandlers.webview_external_postMessage != NULL);
        class_addMethod(
            __WKScriptMessageHandler,
            sel_registerName("userContentController:didReceiveScriptMessage:"),
            (IMP)w->objc_msgHandlers.webview_external_postMessage, "v@:@@");
        objc_registerClassPair(__WKScriptMessageHandler);

        return objc_msgSend((id)__WKScriptMessageHandler, sel_registerName("new"));
    }

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

    id getWKUserController(struct webview* w)
    {
        id webviewid_wkUserController = objc_msgSend((id)objc_getClass("WKUserContentController"),
            sel_registerName("new"));
        objc_setAssociatedObject(webviewid_wkUserController, "webview", (id)(w),
            OBJC_ASSOCIATION_ASSIGN);
        objc_msgSend(
            webviewid_wkUserController, sel_registerName("addScriptMessageHandler:name:"),
            prepareWKScriptMessageHandler(w),
            objc_msgSend((id)objc_getClass("NSString"),
                sel_registerName("stringWithUTF8String:"), "invoke"));

        /***
         In order to maintain compatibility with the other 'webviews' we need to
            override window.external.invoke to call
            webkit.messageHandlers.invoke.postMessage
        ***/
        id windowExternalOverrideScript = objc_msgSend(
            (id)objc_getClass("WKUserScript"), sel_registerName("alloc"));
        objc_msgSend(
            windowExternalOverrideScript,
            sel_registerName("initWithSource:injectionTime:forMainFrameOnly:"),
            get_nsstring("window.external = this;"
                         "postMessage = function(arg){ webkit.messageHandlers.invoke.postMessage(arg); };"
                         "invoke = postMessage.bind(this);"),
            WKUserScriptInjectionTimeAtDocumentStart, 0);

        objc_msgSend(webviewid_wkUserController, sel_registerName("addUserScript:"),
            windowExternalOverrideScript);

        return webviewid_wkUserController;
    }

    id prepareWKWebViewConfig(struct webview* w)
    {
        id webviewid_wkwebviewconfig = objc_msgSend((id)objc_getClass("WKWebViewConfiguration"),
            sel_registerName("new"));
        id processPool = objc_msgSend(webviewid_wkwebviewconfig, sel_registerName("processPool"));
        objc_msgSend(processPool, sel_registerName("_setDownloadDelegate:"), prepareWKDownloadDelegate());
        objc_msgSend(webviewid_wkwebviewconfig, sel_registerName("setProcessPool:"), processPool);
        objc_msgSend(webviewid_wkwebviewconfig, sel_registerName("setUserContentController:"), getWKUserController(w));
        objc_msgSend(webviewid_wkwebviewconfig, sel_registerName("setPreferences:"), prepareWKPreferences(w));

        return webviewid_wkwebviewconfig;
    }

    void initWindowRect(struct webview* w)
    {
        this->webview_window_rect = CGRectMake(0, 0, w->width, w->height);
    }

    void initWindow(struct webview* w)
    {
        unsigned int style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
        if (w->resizable) {
            style = style | NSWindowStyleMaskResizable;
        }

        w->priv.window = objc_msgSend((id)objc_getClass("NSWindow"), sel_registerName("alloc"));
        objc_msgSend(w->priv.window,
            sel_registerName("initWithContentRect:styleMask:backing:defer:"),
            this->webview_window_rect, style, NSBackingStoreBuffered, 0);

        objc_msgSend(w->priv.window, sel_registerName("autorelease"));
    }

    void setupWindowDelegation(struct webview* w)
    {
        Class __NSWindowDelegate = objc_allocateClassPair(objc_getClass("NSObject"),
            "__NSWindowDelegate", 0);
        /**
         * @see https://developer.apple.com/documentation/appkit/nswindowdelegate
         */
        class_addProtocol(__NSWindowDelegate, objc_getProtocol("NSWindowDelegate"));

        if (w->objc_msgHandlers.webview_windowWillClose != NULL)
            class_replaceMethod(__NSWindowDelegate, sel_registerName("windowWillClose:"),
                (IMP)w->objc_msgHandlers.webview_windowWillClose, "v@:@");

        if (w->objc_msgHandlers.webview_windowDidMove != NULL)
            class_replaceMethod(__NSWindowDelegate, sel_registerName("windowDidMove:"),
                (IMP)w->objc_msgHandlers.webview_windowDidMove, "v@:@");
        class_replaceMethod(__NSWindowDelegate, sel_registerName("windowShouldClose:"),
            (IMP)WebView::webview_windowShouldClose, "v@:@");

        objc_registerClassPair(__NSWindowDelegate);

        w->priv.windowDelegate = objc_msgSend((id)__NSWindowDelegate, sel_registerName("new"));
        objc_setAssociatedObject(w->priv.windowDelegate, "webview", (id)(w),
            OBJC_ASSOCIATION_ASSIGN);

        objc_msgSend(w->priv.window, sel_registerName("setDelegate:"),
            w->priv.windowDelegate);
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
        activeApp();

        SetupAppMenubar();

        w->priv.should_exit = 0;
        return 0;
    }

    void webview_terminate(struct webview* w)
    {
        w->priv.should_exit = 1;
    }

    // useless
    void webview_exit()
    {
        id app = objc_msgSend((id)objc_getClass("NSApplication"),
            sel_registerName("sharedApplication"));

        objc_msgSend(app, sel_registerName("terminate:"), app);
    }

public:
    // static registration methods, ONLY run it in GUI Thread
    static void RegNSApplicationDelegations()
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
    // objc Delegation
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

        wv->webview_terminate(w);
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