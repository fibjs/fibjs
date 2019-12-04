/*
 * WebView.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: lion
 */

#ifdef __APPLE__

#define OBJC_OLD_DISPATCH_PROTOTYPES 1

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
static void webview_external_invoke(id self, SEL cmd, id contentController, id message);

extern exlib::LockedList<Isolate> s_isolates;

void putGuiPool(AsyncEvent* ac)
{
    printf("putGuiPool\n");
    s_uiPool.putTail(ac);
    // PostThreadMessage(s_thread, WM_USER + 1000, 0, 0);
}

static id s_activeWin = NULL;

class gui_thread : public exlib::OSThread {
public:
    virtual void Run()
    {
        printf("gui_thread->Run 1\n");
        // initialize one fibjs runtime
        Runtime rt(NULL);
        printf("gui_thread->Run 2\n");

        Loop();
        Exit();
    }

private:
    // Main
    result_t Loop()
    {
        while (true) {
            AsyncEvent* p = s_uiPool.getHead();
            if (p)
                p->invoke();

            // 在有 s_activeWin 的时候, 跟 s_activeWin 发生交互, 获取信息存到 msg;
            // 通过类似 webview_dispatch 的方式, 把消息推入 NS 应用队列
            if (s_activeWin) {
                
            }
        }

        return 0;
    }
    void Exit()
    {
        id app = objc_msgSend((id)objc_getClass("NSApplication"),
            sel_registerName("sharedApplication"));
        objc_msgSend(app, sel_registerName("terminate:"), app);
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

void run_gui()
{
    gui_thread* _thGUI = new gui_thread();

    _thGUI->bindCurrent();
    s_thread = _thGUI->thread_;

    gui_base::setVersion(99999);
    printf("[here] run_gui pre\n");

    _thGUI->Run();
    printf("[here] run_gui post\n");
}

// useless for darwin
result_t gui_base::setVersion(int32_t ver)
{
    return 0;
}

static result_t async_open(obj_ptr<fibjs::WebView> w)
{
    printf("[here] async_open\n");
    w->open();
    return 0;
}

void send_open_event () {
    id until = objc_msgSend((id)objc_getClass("NSDate"),
        sel_registerName("distantFuture"));

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

static result_t RegNSApplication(obj_ptr<fibjs::WebView> wv)
{
    static bool s_init = false;

    if (!s_init) {
        s_init = true;

        wv->winfo->priv.pool = objc_msgSend((id)objc_getClass("NSAutoreleasePool"),
                                    sel_registerName("new"));
        objc_msgSend((id)objc_getClass("NSApplication"),
                    sel_registerName("sharedApplication"));

        Class __WKScriptMessageHandler = objc_allocateClassPair(
            objc_getClass("NSObject"), "__WKScriptMessageHandler", 0);
        class_addMethod(
            __WKScriptMessageHandler,
            sel_registerName("userContentController:didReceiveScriptMessage:"),
            (IMP)(webview_external_invoke), "v@:@@");
        objc_registerClassPair(__WKScriptMessageHandler);

        wv->scriptMessageHandler = objc_msgSend((id)__WKScriptMessageHandler, sel_registerName("new"));
        
        wv->initDownloadDelegate();
        // initPreference();

        printf("[WebView::WebView] mid\n");

        // 这里要做类似于初始化一个窗体管理对象的工作, 而且是一次性的
        wv->initUserController();
        wv->initParentWindow();
        wv->initNavigation();
        wv->initWKWebView();

        wv->finishSetupApplication();
        wv->finishSetupMenu();

        wv->winfo->priv.should_exit = 0;
    }

    return 0;
}

// Would Call In Javascript Thread
WebView::WebView(exlib::string url, NObject* opt)
{
    printf("[WebView::WebView] before\n");
    holder()->Ref();

    m_url = url;
    m_opt = opt;

    struct fibjs::webview webview;
    memset(&webview, 0, sizeof(webview));
    webview.title = exlib::string("测试").c_str();
    webview.url = m_url.c_str();
    webview.width = 640;
    webview.height = 400;
    webview.resizable = false;

    winfo = &webview;
    m_isdebug = false;

    m_ac = NULL;
    _onmessage = NULL;
    _onclose = NULL;
    winfo->priv.windowDelegate = NULL;

    obj_ptr<WebView> self = this;
    asyncCall(RegNSApplication, self, CALL_E_GUICALL);

    m_visible = true;
    holder()->UnRef();
    printf("[WebView::WebView] after\n");
}

WebView::~WebView()
{
    clear();
}

// Cocoa
void WebView::initDownloadDelegate()
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
    downloadDelegate = objc_msgSend((id)__WKDownloadDelegate, sel_registerName("new"));
}

void WebView::initPreference()
{
    Class __WKPreferences = objc_allocateClassPair(objc_getClass("WKPreferences"),
        "__WKPreferences", 0);
    objc_property_attribute_t type = { "T", "c" };
    objc_property_attribute_t ownership = { "N", "" };
    objc_property_attribute_t attrs[] = { type, ownership };
    class_replaceProperty(__WKPreferences, "developerExtrasEnabled", attrs, 2);
    objc_registerClassPair(__WKPreferences);
    wkPref = objc_msgSend((id)__WKPreferences, sel_registerName("new"));

    objc_msgSend(wkPref, sel_registerName("setValue:forKey:"),
        objc_msgSend((id)objc_getClass("NSNumber"),
            sel_registerName("numberWithBool:"), true /* !!m_isdebug */),
        objc_msgSend((id)objc_getClass("NSString"),
            sel_registerName("stringWithUTF8String:"),
            "developerExtrasEnabled"));
}
void WebView::initUserController()
{
    printf("[WebView::initUserController] 1\n");
    printf("[WebView::initUserController] 1 --- check winfo %s \n", winfo->title);

    id userController = objc_msgSend((id)objc_getClass("WKUserContentController"),
        sel_registerName("new"));
    objc_setAssociatedObject(userController, "webview", (id)(winfo),
        OBJC_ASSOCIATION_ASSIGN);
    printf("[WebView::initUserController] 2\n");
    objc_msgSend(
        userController, sel_registerName("addScriptMessageHandler:name:"),
        scriptMessageHandler,
        objc_msgSend((id)objc_getClass("NSString"),
            sel_registerName("stringWithUTF8String:"), "invoke"));

    printf("[WebView::initUserController] 3\n");
    /***
        In order to maintain compatibility with the other 'webviews' we need to
        override `window.external.invoke` to call `webkit.messageHandlers.invoke.postMessage`
    ***/

    id windowExternalOverrideScript = objc_msgSend(
        (id)objc_getClass("WKUserScript"), sel_registerName("alloc"));
    objc_msgSend(
        windowExternalOverrideScript,
        sel_registerName("initWithSource:injectionTime:forMainFrameOnly:"),
        get_nsstring("window.external = this; invoke = function(arg){ "
                     "webkit.messageHandlers.invoke.postMessage(arg); };"),
        WKUserScriptInjectionTimeAtDocumentStart, 0);

    objc_msgSend(userController, sel_registerName("addUserScript:"),
        windowExternalOverrideScript);

    wkconfig = objc_msgSend((id)objc_getClass("WKWebViewConfiguration"),
        sel_registerName("new"));
    id processPool = objc_msgSend(wkconfig, sel_registerName("processPool"));
    objc_msgSend(processPool, sel_registerName("_setDownloadDelegate:"),
        downloadDelegate);
    objc_msgSend(wkconfig, sel_registerName("setProcessPool:"), processPool);
    objc_msgSend(wkconfig, sel_registerName("setUserContentController:"),
        userController);
    objc_msgSend(wkconfig, sel_registerName("setPreferences:"), wkPref);

    Class __NSWindowDelegate = objc_allocateClassPair(objc_getClass("NSObject"),
        "__NSWindowDelegate", 0);
    class_addProtocol(__NSWindowDelegate, objc_getProtocol("NSWindowDelegate"));
    class_replaceMethod(__NSWindowDelegate, sel_registerName("windowWillClose:"),
        (IMP)(webview_window_will_close), "v@:@");
    objc_registerClassPair(__NSWindowDelegate);

    winfo->priv.windowDelegate = objc_msgSend((id)__NSWindowDelegate, sel_registerName("new"));

    objc_setAssociatedObject(winfo->priv.windowDelegate, "webview", (id)winfo,
        OBJC_ASSOCIATION_ASSIGN);
    printf("[WebView::initUserController] after\n");
}

void WebView::initParentWindow()
{
    nsTitle = objc_msgSend((id)objc_getClass("NSString"),
        sel_registerName("stringWithUTF8String:"), (id)(winfo->title));

    // Create Rect for webview
    rect = CGRectMake(0, 0, winfo->width, winfo->height);

    unsigned int style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
    if (winfo->resizable) {
        style = style | NSWindowStyleMaskResizable;
    }

    winfo->priv.window = objc_msgSend((id)objc_getClass("NSWindow"), sel_registerName("alloc"));
    objc_msgSend(winfo->priv.window,
        sel_registerName("initWithContentRect:styleMask:backing:defer:"),
        rect, style, NSBackingStoreBuffered, 0);

    objc_msgSend(winfo->priv.window, sel_registerName("autorelease"));
    objc_msgSend(winfo->priv.window, sel_registerName("setTitle:"), nsTitle);
    objc_msgSend(winfo->priv.window, sel_registerName("setDelegate:"),
        winfo->priv.windowDelegate);
    objc_msgSend(winfo->priv.window, sel_registerName("center"));

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
    uiDel = objc_msgSend((id)__WKUIDelegate, sel_registerName("new"));
}

void WebView::initNavigation()
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
    navDel = objc_msgSend((id)__WKNavigationDelegate, sel_registerName("new"));
    nsURL = objc_msgSend((id)objc_getClass("NSURL"),
        sel_registerName("URLWithString:"),
        get_nsstring(webview_check_url(get_url().c_str())));
}

void WebView::initWKWebView()
{
    winfo->priv.webview = objc_msgSend((id)objc_getClass("WKWebView"), sel_registerName("alloc"));
    objc_msgSend(winfo->priv.webview,
        sel_registerName("initWithFrame:configuration:"), rect, wkconfig);
    objc_msgSend(winfo->priv.webview, sel_registerName("setUIDelegate:"), uiDel);
    objc_msgSend(winfo->priv.webview, sel_registerName("setNavigationDelegate:"),
        navDel);

    objc_msgSend(winfo->priv.webview, sel_registerName("loadRequest:"),
        objc_msgSend((id)objc_getClass("NSURLRequest"),
            sel_registerName("requestWithURL:"), nsURL));
    objc_msgSend(winfo->priv.webview, sel_registerName("setAutoresizesSubviews:"), 1);
    objc_msgSend(winfo->priv.webview, sel_registerName("setAutoresizingMask:"),
        (NSViewWidthSizable | NSViewHeightSizable));
    objc_msgSend(objc_msgSend(winfo->priv.window, sel_registerName("contentView")),
        sel_registerName("addSubview:"), winfo->priv.webview);
    objc_msgSend(winfo->priv.window, sel_registerName("orderFrontRegardless"));
}

void WebView::finishSetupApplication()
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

void WebView::finishSetupMenu()
{
    id menubar = objc_msgSend((id)objc_getClass("NSMenu"), sel_registerName("alloc"));
    objc_msgSend(menubar, sel_registerName("initWithTitle:"), get_nsstring(""));
    objc_msgSend(menubar, sel_registerName("autorelease"));

    id appName = objc_msgSend(objc_msgSend((id)objc_getClass("NSProcessInfo"),
                                  sel_registerName("processInfo")),
        sel_registerName("processName"));

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

void WebView::terminate()
{
    winfo->priv.should_exit = 1;
}

result_t WebView::open()
{
    printf("[here] WebView::open");
    m_bSilent = false;
    m_maximize = false;

    if (m_opt) {
    }

    // mount webview to it
    AddRef();

    // check if slicent

    // show window

    // navite to target url
    Navigate(m_url.c_str());
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

    if (s_activeWin == winfo->priv.windowDelegate)
        s_activeWin = NULL;

    winfo->priv.windowDelegate = NULL;
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

#undef OBJC_OLD_DISPATCH_PROTOTYPES

#endif /* __APPLE__ */