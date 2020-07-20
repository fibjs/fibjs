/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-12 04:25:25
 * @modify date 2020-06-12 04:25:25
 * @desc WebView Implementation in OSX
 */

#if defined(__APPLE__) && !defined(FIBJS_DISABLE_GUI)

#include <Cocoa/Cocoa.h>
#include <Webkit/Webkit.h>

#include "object.h"
#include "ifs/gui.h"
#include "ifs/fs.h"
#include "ifs/os.h"
#include "ifs/json.h"
#include "ifs/console.h"
#include "path.h"
#include "WebView.h"
#include "EventInfo.h"
#include "utf8.h"
#include <exlib/include/thread.h>

#include "ns-api.h"

@implementation WVViewController
-(void)loadView
{
    self.view = [NSView new];
    [self.view setWantsLayer:YES];
}
-(void)viewDidAppear
{
    [super viewDidAppear];
}
-(BOOL)acceptsFirstResponder {
    return YES;
}
@end

namespace fibjs {

DECLARE_MODULE(gui);

void asyncLog(int32_t priority, exlib::string msg);

NSEvent* getEmptyCustomNSEvent () {
    return [NSEvent otherEventWithType: NSApplicationDefined
        location: NSMakePoint(0,0)
        modifierFlags: 0
        timestamp: 0.0
        windowNumber: 0
        context: nil
        subtype: 0
        data1: 0
        data2: 0];
}

void putGuiPool(AsyncEvent* ac)
{
    s_uiPool.putTail(ac);

    [[NSApplication sharedApplication]
        postEvent:getEmptyCustomNSEvent()
        atStart:YES
    ];
}

result_t asyncFinishedLaunchingApp(NSApplication* app)
{
    /**
     * @see https://developer.apple.com/documentation/appkit/nsapplicationactivationpolicy/nsapplicationactivationpolicyregular?language=objc
     *
     * @enum NSApplicationActivationPolicyRegular default, can be overrided by Info.plist
     * @enum NSApplicationActivationPolicyAccessory The application doesn’t appear in the Dock and doesn’t have a menu bar, but it may be activated programmatically or by clicking on one of its windows.
     * @enum NSApplicationActivationPolicyProhibited The application doesn’t appear in the Dock and may not create windows or be activated.
     */
    [app setActivationPolicy: NSApplicationActivationPolicyAccessory];
    [app finishLaunching];

    return 0;
}

void run_gui()
{
    @autoreleasepool {
        [NSApplication sharedApplication];
        [[NSApplication sharedApplication] setDelegate:[__NSApplicationDelegate new]];

        NSAppMainRunLoopThread* _thMainLoop = new NSAppMainRunLoopThread();

        asyncCall(asyncFinishedLaunchingApp, [NSApplication sharedApplication]);

        _thMainLoop->bindCurrent();
        s_thNSMainLoop = _thMainLoop;

        _thMainLoop->Run();
    }
}

id fetchEventFromNSRunLoop(int blocking)
{
    id until = blocking ? [NSDate distantFuture] : [NSDate distantPast];

    return [[NSApplication sharedApplication]
        nextEventMatchingMask:NSEventMaskAny
        untilDate:until
        inMode:@"kCFRunLoopDefaultMode"
        dequeue:YES
    ];
}

/**
 * replace [[NSApplication shareApplication] run] in GUI Thread;
 */
void NSAppMainRunLoopThread::Run()
{
    // initialize one fibjs runtime
    Runtime rt(NULL);

    while (true) {
        AsyncEvent* p = s_uiPool.getHead();

        if (p)
            p->invoke();

        id event = fetchEventFromNSRunLoop(1);
        [[NSApplication sharedApplication] sendEvent:event];
    }
}

// useless for darwin
result_t gui_base::setVersion(int32_t ver)
{
    return 0;
}

void maxmizeNSWindow (NSWindow* win) {
    [win setFrame:
        [[NSScreen mainScreen] visibleFrame]
        display:YES
    ];
}

result_t openWebViewInGUIThread(obj_ptr<fibjs::WebView> wv)
{
    wv->open();

    return 0;
}

// In Javascript Thread
result_t gui_base::open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    obj_ptr<NObject> o = new NObject();
    o->add(opt);

    obj_ptr<WebView> wv = new WebView(url, o);
    wv->wrap();

    asyncCall(openWebViewInGUIThread, wv, CALL_E_GUICALL);
    retVal = wv;

    return 0;
};

// Would Call In Javascript Thread
WebView::WebView(exlib::string url, NObject* opt)
    : m_WinW(640)
    , m_WinH(400)
    , m_visible(true)
    , m_bDebug(true)
    , m_initScriptDocAfter("")
    , m_bIScriptLoaded(false)
    , m_iUseContentViewController(true)
{
    holder()->Ref();

    m_url = url;
    m_opt = opt;

    m_fullscreenable = true;

    m_visible = true;
    m_bDebug = true;
    m_maximize = false;
    m_nsWinStyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSResizableWindowMask
        // | NSWindowStyleMaskTexturedBackground
        // | NSWindowStyleMaskFullSizeContentView
        // | NSWindowStyleMaskUtilityWindow
        // | NSWindowStyleMaskUnifiedTitleAndToolbar
        | NSWindowStyleMaskResizable;
    m_wkViewStyle = NSViewWidthSizable | NSViewHeightSizable;

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

        if (m_opt->get("resizable", v) == 0 && !v.isUndefined() && !v.boolVal()) {
            m_nsWinStyle ^= NSWindowStyleMaskResizable;
        }

        if (m_opt->get("fullscreenable", v) == 0)
            m_fullscreenable = v.boolVal();

        if (m_opt->get("maximize", v) == 0 && v.boolVal())
            m_maximize = true;

        if (m_opt->get("visible", v) == 0 && !v.boolVal())
            m_visible = false;

        if (m_opt->get("debug", v) == 0 && !v.isUndefined())
            m_bDebug = v.boolVal();

        if (m_opt->get("script_after", v) == 0)
            m_initScriptDocAfter = v.string();
    }

    m_ac = NULL;
}

WebView::~WebView()
{
    clear();
}

void WebView::setupAppMenubar()
{
    NSMenu* menubar = [NSMenu alloc];
    [menubar initWithTitle:@""];
    [menubar autorelease];

    NSString* appName = [[NSProcessInfo processInfo] processName];

    NSMenuItem* appMenuItem = [NSMenuItem alloc];
    [appMenuItem
        initWithTitle:appName
        action:NULL
        keyEquivalent:get_nsstring("")
    ];

    NSMenu* appMenu = [NSMenu alloc];
    [appMenu initWithTitle:appName];
    [appMenu autorelease];

    [appMenuItem setSubmenu:appMenu];
    [menubar addItem:appMenuItem];

    NSString* title = [get_nsstring("Hide ") stringByAppendingString:appName];
    NSMenuItem* item = createMenuItem(title, "hide:", "h");
    [appMenu addItem:item];

    item = createMenuItem(get_nsstring("Hide Others"), "hideOtherApplications:", "h");
    [item setKeyEquivalentModifierMask:(NSEventModifierFlagOption | NSEventModifierFlagCommand)];
    [appMenu addItem:item];

    item = createMenuItem(get_nsstring("Show All"), "unhideAllApplications:", "");
    [appMenu addItem:item];

    [appMenu addItem:[NSMenuItem separatorItem]];

    title = [get_nsstring("Quit ") stringByAppendingString:appName];
    item = createMenuItem(title, "terminate:", "q");
    [appMenu addItem:item];

    [[NSApplication sharedApplication] setMainMenu:menubar];
}

void _waitAsyncOperationInCurrentLoop(bool blocking = false) {
    if (blocking)
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
    else
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantPast]];
}

bool WebView::onNSWindowShouldClose()
{
    __block bool shouldClose = false;

    WebView* wv = this;
    if (!isInternalScriptLoaded()) {
        forceCloseWindow();
        return shouldClose;
    }

    evaluateWebviewJS(
        "!window.external || typeof window.external.onclose !== 'function' ? false : window.external.onclose()",
        ^(id result, NSError * _Nullable error) {
            if (error != nil) {
                if (wv->m_bDebug)
                    asyncLog(console_base::_DEBUG, NSStringToExString([error localizedDescription]));
                    
                wv->forceCloseWindow();
            } else {
                if (result == nil)
                    shouldClose = true;
                else if ([result boolValue] != NO)
                    shouldClose = true;
            }

            s_thNSMainLoop->m_sem.Post();
        }
    );

    do {
        _waitAsyncOperationInCurrentLoop(true);
    } while (!s_thNSMainLoop->m_sem.TryWait());

    return shouldClose;
}

void WebView::onWKWebViewPostMessage(WKScriptMessage* message)
{
    // TODO: escape it.
    const char* wvJsMsg = (const char*)([[message body] UTF8String]);

    _emit("message", wvJsMsg);
}

void WebView::onWKWebViewInwardMessage(WKScriptMessage* message)
{
    const char* inwardMsg = (const char*)([[message body] UTF8String]);

    if (!strcmp(inwardMsg, "inward:window.onLoad")) {
        _emit("load");
    } else if (!strcmp(inwardMsg, "inward:window.doClose")) {
        if (onNSWindowShouldClose())
            forceCloseWindow();
    } else if (!strcmp(inwardMsg, "inward:internal-script-loaded")) {
        m_bIScriptLoaded = true;
    }
}

static int32_t asyncOutputMessageFromWKWebview(exlib::string& jsonFmt)
{
    JSValue _logInfo;
    json_base::decode(jsonFmt, _logInfo);
    v8::Local<v8::Object> logInfo = v8::Local<v8::Object>::Cast(_logInfo);

    Isolate* isolate = Isolate::current(); 
    
    int32_t logLevel = JSValue(logInfo->Get(isolate->NewString("level")))->IntegerValue();

    v8::Local<v8::Value> _fmtMessage = logInfo->Get(isolate->NewString("fmt"));
    exlib::string fmtMessage(ToCString(v8::String::Utf8Value(_fmtMessage)));
    
    if (logLevel == console_base::_ERROR)
        fmtMessage = ("WebView Error: " + fmtMessage);

    asyncLog(logLevel, fmtMessage);

    return 0;
}

void WebView::onWKWebViewExternalLogMessage(WKScriptMessage* message)
{
    if (!m_bDebug)
        return ;

    const char* externalLogMsg = (const char*)([[message body] UTF8String]);
    exlib::string payload(externalLogMsg);

    syncCall(holder(), asyncOutputMessageFromWKWebview, payload);
}

extern const wchar_t* g_console_js;

extern const wchar_t* scriptStart_regExternal;
extern const wchar_t* scriptEnd_inwardPostMessage;
extern const wchar_t* scriptEnd_afterInternal;

id WebView::createWKUserContentController()
{
    WKUserContentController* wkUserCtrl = [WKUserContentController new];

    assignToWKUserContentController(wkUserCtrl);

    [wkUserCtrl addScriptMessageHandler:[__WKScriptMessageHandler new] name:get_nsstring(WEBVIEW_MSG_HANDLER_NAME_INVOKE)];
    [wkUserCtrl addScriptMessageHandler:[__WKScriptMessageHandler new] name:get_nsstring(WEBVIEW_MSG_HANDLER_NAME_INWARD)];
    [wkUserCtrl addScriptMessageHandler:[__WKScriptMessageHandler new] name:get_nsstring(WEBVIEW_MSG_HANDLER_NAME_EXTERNALLOG)];

    [wkUserCtrl addUserScript:[[WKUserScript alloc]
        initWithSource:w_get_nsstring(scriptStart_regExternal)
        injectionTime:WKUserScriptInjectionTimeAtDocumentStart
        forMainFrameOnly:FALSE
    ]];

    [wkUserCtrl addUserScript:[[WKUserScript alloc]
        initWithSource:w_get_nsstring(g_console_js)
        injectionTime:WKUserScriptInjectionTimeAtDocumentStart
        forMainFrameOnly:FALSE
    ]];

    [wkUserCtrl addUserScript:[[WKUserScript alloc]
        initWithSource:w_get_nsstring(scriptEnd_inwardPostMessage)
        injectionTime:WKUserScriptInjectionTimeAtDocumentEnd
        forMainFrameOnly:TRUE
    ]];

    [wkUserCtrl addUserScript:[[WKUserScript alloc]
        initWithSource:w_get_nsstring(scriptEnd_afterInternal)
        injectionTime:WKUserScriptInjectionTimeAtDocumentEnd
        forMainFrameOnly:TRUE
    ]];

    // TODO: support inject javascript
    [wkUserCtrl addUserScript:[[WKUserScript alloc]
        initWithSource:get_nsstring(m_initScriptDocAfter.c_str())
        injectionTime:WKUserScriptInjectionTimeAtDocumentEnd
        forMainFrameOnly:TRUE
    ]];

    return wkUserCtrl;
}

id WebView::createWKWebViewConfig()
{
    WKWebViewConfiguration* configuration = [WKWebViewConfiguration new];

    // WKProcessPool* processPool = [configuration processPool];
    // [configuration setProcessPool:processPool];
    [configuration setUserContentController:createWKUserContentController()];

    WKPreferences *preferences = [WKPreferences new];
    preferences.javaScriptCanOpenWindowsAutomatically = YES;
    preferences.tabFocusesLinks = FALSE;
    [preferences setValue:@TRUE forKey:@"allowFileAccessFromFileURLs"];
    if (m_bDebug) {
        [preferences setValue:@TRUE forKey:@"developerExtrasEnabled"];
    }
    // preferences.minimumFontSize = 40.0;

    configuration.preferences = preferences;

    /**
     * WKWebViewConfiguration would put the field value:
     * `Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_5) AppleWebKit/537.36 (KHTML, like Gecko) {applicationNameForUserAgent}`
     * 
     * we could set the field `applicationNameForUserAgent` to customize our AppName, like this
     * "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_5) AppleWebKit/605.1.15 (KHTML, like Gecko) Chrome/83.0.4103.97 Safari/537.36"
     * @real safari
     * "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.97 Safari/537.36"
     */
    configuration.applicationNameForUserAgent = @"Chrome/83.0.4103.97 Safari/537.36";

    /**
     * register customized protocol `fs://`
     * @warning `setURLSchemeHandler:forURLScheme:` is only available on macOS 10.13 or newer
     */
    NSOperatingSystemVersion macOS10_13 = (NSOperatingSystemVersion){10, 13, 0};
    if ([[NSProcessInfo processInfo] isOperatingSystemAtLeastVersion:macOS10_13]) {
        [configuration setURLSchemeHandler:[FileSystemWKURLSchemeHandler new] forURLScheme:@"fs"];
    }
    

    return configuration;
}

void WebView::initNSWindow()
{
    m_nsWindow = [[NSWindow alloc]
        initWithContentRect:m_nsWindowFrame
        styleMask:m_nsWinStyle
        backing:NSBackingStoreBuffered
        defer:YES
    ];

    if (m_fullscreenable)
        [m_nsWindow setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];

    [m_nsWindow autorelease];

    [m_nsWindow setDelegate:[__NSWindowDelegate new]];
    [m_nsWindow setTitle:[NSString stringWithUTF8String:m_title.c_str()]];

    assignToToNSWindow(m_nsWindow);
}

void WebView::centralizeWindow()
{
    [m_nsWindow center];
}

void WebView::initWKWebView()
{
    m_wkWebView = [
        [WKWebView alloc]
        initWithFrame:m_nsWindowFrame
        configuration:createWKWebViewConfig()
    ];

    assignToWKWebView(m_wkWebView);

    [m_wkWebView setUIDelegate:[__WKUIDelegate new]];
    [m_wkWebView setNavigationDelegate:[__WKNavigationDelegate new]];

    [m_wkWebView autorelease];
}

void WebView::navigateWKWebView()
{
    NSURL* nsURL = [NSURL
        URLWithString:get_nsstring(
            webview_check_url(m_url.c_str())
        )
    ];
    [m_wkWebView loadRequest:[NSURLRequest requestWithURL:nsURL]];
}

void WebView::startWKUI()
{
    [m_wkWebView setAutoresizesSubviews:TRUE];
    [m_wkWebView setAutoresizingMask:m_wkViewStyle];

    if (m_iUseContentViewController) {
        WVViewController* contentViewController = [WVViewController new];
        contentViewController.view = m_wkWebView;
        m_nsWindow.contentViewController = contentViewController;
    } else {
        m_nsWindow.contentView = m_wkWebView;
        [m_nsWindow.contentView setWantsLayer:YES];
    }

    [m_nsWindow makeKeyWindow];
    [m_nsWindow orderFrontRegardless];

    [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
}

int WebView::initializeWebView()
{
    initNSWindowRect();

    initNSWindow();
    centralizeWindow();

    initWKWebView();
    navigateWKWebView();

    // setupAppMenubar();

    startWKUI();

    return 0;
}


void WebView::evaluateWebviewJS(const char* js, JsEvaluateResultHdlr hdlr)
{
    if (hdlr == NULL) {
        hdlr = ^(id item, NSError * _Nullable error) {};
    }

    [m_wkWebView
        evaluateJavaScript:get_nsstring(js)
        completionHandler:hdlr
    ];
}

void WebView::setWindowColor(WebView* w, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    NSColor* color = [NSColor
        colorWithRed:((float)r / 255.0)
        green:((float)g / 255.0)
        blue:((float)b / 255.0)
        alpha:((float)a / 255.0)
    ];

    [m_nsWindow setBackgroundColor:color];

    if (0.5 >= ((r / 255.0 * 299.0) + (g / 255.0 * 587.0) + (b / 255.0 * 114.0)) / 1000.0) {
        [m_nsWindow
            setAppearance:[NSAppearance appearanceNamed:@"NSAppearanceNameVibrantDark"]
        ];
    } else {
        [m_nsWindow
            setAppearance:[NSAppearance appearanceNamed:@"NSAppearanceNameVibrantLight"]
        ];
    }

    [m_nsWindow setOpaque:FALSE];
    [m_nsWindow setTitlebarAppearsTransparent:YES];
}

int WebView::helperEncodeJS(const char* s, char* esc, size_t n)
{
    int r = 1; /* At least one byte for trailing zero */
    for (; *s; s++) {
        const unsigned char c = *s;
        if (c >= 0x20 && c < 0x80 && strchr("<>\\'\"", c) == NULL) {
            if (n > 0) {
                *esc++ = c;
                n--;
            }
            r++;
        } else {
            if (n > 0) {
                snprintf(esc, n, "\\x%02x", (int)c);
                esc += 4;
                n -= 4;
            }
            r += 4;
        }
    }
    return r;
}

NSMenuItem* WebView::createMenuItem(id title, const char* action, const char* key)
{    
    id item = [[NSMenuItem alloc]
        initWithTitle:title
        action:sel_registerName(action)
        keyEquivalent:get_nsstring(key)
    ];
    [item autorelease];

    return item;
}

void WebView::clear()
{
    if (m_ac) {
        m_ac->post(0);
        m_ac = NULL;
    }
}

result_t WebView::setHtml(exlib::string html, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    [m_wkWebView loadHTMLString:get_nsstring(html.c_str()) baseURL:nil];

    return 0;
}

// TODO: support it.
result_t WebView::print(int32_t mode, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    return CHECK_ERROR(CALL_E_EXCEPTION);
}

result_t WebView::close(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    [m_nsWindow performClose:nil];

    return 0;
}

void WebView::forceCloseWindow()
{
    [m_nsWindow close];
}

result_t WebView::postMessage(exlib::string msg)
{
    exlib::string c_jsstr;
    // TODO: we should escape it.
    c_jsstr.append("external.onmessage('");
    c_jsstr.append(msg);
    c_jsstr.append("')");

    evaluateWebviewJS(c_jsstr.c_str());

    return 0;
}

result_t WebView::postMessage(exlib::string msg, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    return postMessage(msg);
}

// result_t WebView::get_fullscreen(bool& retVal)
// {
//     unsigned long windowStyleMask = (unsigned long)[m_nsWindow styleMask];

//     retVal = !!(windowStyleMask & NSWindowStyleMaskFullScreen) == NSWindowStyleMaskFullScreen;
//     return 0;
// }

// result_t WebView::set_fullscreen(bool newVal)
// {
//     bool bNowFull;
//     get_fullscreen(bNowFull);
//     if (bNowFull == newVal)
//         return 0;

//     m_fullscreen = newVal;

//     return 0;
// }

result_t WebView::get_visible(bool& retVal)
{
    retVal = m_visible;
    return 0;
}

result_t WebView::asyncToggleVisible(WebView* wv)
{
    if (wv->m_visible) {
        if (wv->m_maximize) maxmizeNSWindow(wv->m_nsWindow);
        wv->m_maximize = false;
    }

    [wv->m_nsWindow setIsVisible:(wv->m_visible ? YES : NO)];

    return 0;
}

result_t WebView::set_visible(bool newVal)
{
    m_visible = newVal;

    asyncCall(asyncToggleVisible, this, CALL_E_GUICALL);

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
    // NOT IMPLEMENTED
    return;
}

void WebView::Refresh()
{
    // NOT IMPLEMENTED
    return;
}

void WebView::Navigate(exlib::string szUrl)
{
    // NOT IMPLEMENTED
    return;
}

}

#endif /* __APPLE__ */