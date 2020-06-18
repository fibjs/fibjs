/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-12 04:25:25
 * @modify date 2020-06-12 04:25:25
 * @desc WebView Implementation in OSX
 */

#ifdef __APPLE__

#include <Cocoa/Cocoa.h>
#include <Webkit/Webkit.h>

#include "object.h"
#include "ifs/gui.h"
#include "ifs/fs.h"
#include "ifs/os.h"
#include "ifs/json.h"
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
    // printf("viewDidAppear \n");
    [super viewDidAppear];
    // [self showResponsederInfo];
}
-(void)showResponsederInfo
{
    NSResponder *responder = self.view.nextResponder;

    int i = 0;
    while (responder) {
        NSLog(@"%d responder - %@", i, responder);
        
        responder = responder.nextResponder;
    }
}
#pragma mark - mouse event
-(void)mouseDown:(NSEvent *)event{
    NSLog(@"WVViewController::mouseDown %s", __FUNCTION__);
    [self showResponsederInfo];
}
#pragma mark - keyboard event
-(void)keyDown:(NSEvent *)event{
    NSLog(@"WVViewController::keyDown %@", event);
    [super keyDown:event];
}
-(BOOL)acceptsFirstResponder {
    return YES;
}
@end

@implementation __WKPreferences : WKPreferences
-(bool)_fullScreenEnabled
{
    return true;
}
@end

namespace fibjs {

DECLARE_MODULE(gui);

void putGuiPool(AsyncEvent* ac)
{
    s_uiPool.putTail(ac);
}

void run_gui()
{
    [NSAutoreleasePool new];
    [NSApplication sharedApplication];

    NSAppMainLoopThread* _thMainLoop = new NSAppMainLoopThread();

    _thMainLoop->bindCurrent();
    s_thNSMainLoop = _thMainLoop;

    id app = [NSApplication sharedApplication];
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];
    [app activateIgnoringOtherApps:YES];

    _thMainLoop->Run();
    // [app finishLaunching];
}

id fetchEventFromNSMainLoop(int blocking)
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
void NSAppMainLoopThread::Run()
{
    // initialize one fibjs runtime
    Runtime rt(NULL);

    [[NSApplication sharedApplication] setDelegate:[__NSApplicationDelegate new]];
    WebView::setupAppMenubar();

    while (true) {
        AsyncEvent* p = s_uiPool.getHead();

        if (p)
            p->invoke();

        id event = fetchEventFromNSMainLoop(0);
        if (event)
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
    @autoreleasepool {
        wv->open();
    }

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
    , m_bDebug(false)
    , m_initScriptDocAfter("")
    , m_bIScriptLoaded(false)
    , m_iUseContentViewController(true)
{
    holder()->Ref();

    m_url = url;
    m_opt = opt;

    m_fullscreenable = true;

    m_visible = true;
    m_bDebug = false;
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

        if (m_opt->get("debug", v) == 0)
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
    id menubar = [NSMenu alloc];
    [menubar initWithTitle:@""];
    [menubar autorelease];

    id appName = [[NSProcessInfo processInfo] processName];

    id appMenuItem = [NSMenuItem alloc];
    [appMenuItem
        initWithTitle:appName
        action:NULL
        keyEquivalent:get_nsstring("")
    ];

    id appMenu = [NSMenu alloc];
    [appMenu initWithTitle:appName];
    [appMenu autorelease];

    [appMenuItem setSubmenu:appMenu];
    [menubar addItem:appMenuItem];

    id title = [get_nsstring("Hide ") stringByAppendingString:appName];
    id item = create_menu_item(title, "hide:", "h");
    [appMenu addItem:item];

    item = create_menu_item(get_nsstring("Hide Others"), "hideOtherApplications:", "h");
    [item setKeyEquivalentModifierMask:(NSEventModifierFlagOption | NSEventModifierFlagCommand)];
    [appMenu addItem:item];

    item = create_menu_item(get_nsstring("Show All"), "unhideAllApplications:", "");
    [appMenu addItem:item];

    [appMenu addItem:[NSMenuItem separatorItem]];

    title = [get_nsstring("Quit ") stringByAppendingString:appName];
    item = create_menu_item(title, "terminate:", "q");
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
        return false;
    }

    // TODO: use fibjs native API to resolve it.
    evaluateWebviewJS("!window.external || typeof window.external.onclose !== 'function' ? false : window.external.onclose()", ^(id result, NSError * _Nullable error) {
        if (error != nil) {
            // NSLog(@"evaluateJavaScript error : %@", error.localizedDescription);
            NSLog(@"evaluateJavaScript error : %@", error);
            // shouldClose = true;
            wv->forceCloseWindow();
        } else {
            if (result == nil)
                shouldClose = true;
            else if ([result boolValue] != NO)
                shouldClose = true;
            NSLog(@"evaluateJavaScript result : %@", result);
        }

        s_thNSMainLoop->m_sem.Post();
    });

    do {
        _waitAsyncOperationInCurrentLoop(true);
    } while (s_thNSMainLoop->m_sem.TryWait());

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

    if (!strcmp(inwardMsg, "inward:window.load")) {
        _emit("load");
    } else if (!strcmp(inwardMsg, "inward:internal-script-loaded")) {
        m_bIScriptLoaded = true;
    }
}

void asyncLog(int32_t priority, exlib::string msg);

static int32_t asyncOutputMessageFromWKWebview(exlib::string& jsonFmt)
{
    JSValue _logInfo;
    json_base::decode(jsonFmt, _logInfo);
    v8::Local<v8::Object> logInfo = v8::Local<v8::Object>::Cast(_logInfo);

    Isolate* isolate = Isolate::current(); 
    
    int32_t logLevel = JSValue(logInfo->Get(isolate->NewString("level")))->IntegerValue();

    v8::Local<v8::Value> _fmtMessage = logInfo->Get(isolate->NewString("fmt"));
    exlib::string fmtMessage(ToCString(v8::String::Utf8Value(_fmtMessage)));

    asyncLog(logLevel, fmtMessage);

    return 0;
}

void WebView::onWKWebViewExternalLogMessage(WKScriptMessage* message)
{
    const char* externalLogMsg = (const char*)([[message body] UTF8String]);
    exlib::string payload(externalLogMsg);

    syncCall(holder(), asyncOutputMessageFromWKWebview, payload);
}

extern const wchar_t* g_console_js;

extern const wchar_t* script_regExternal;
extern const wchar_t* script_inwardPostMessage;
extern const wchar_t* script_afterInternal;

id WebView::createWKUserContentController()
{
    WKUserContentController* wkUserCtrl = [WKUserContentController new];

    assignToWKUserContentController(wkUserCtrl);

    [wkUserCtrl addScriptMessageHandler:[__WKScriptMessageHandler new] name:get_nsstring(WEBVIEW_MSG_HANDLER_NAME_INVOKE)];
    [wkUserCtrl addScriptMessageHandler:[__WKScriptMessageHandler new] name:get_nsstring(WEBVIEW_MSG_HANDLER_NAME_INWARD)];
    [wkUserCtrl addScriptMessageHandler:[__WKScriptMessageHandler new] name:get_nsstring(WEBVIEW_MSG_HANDLER_NAME_EXTERNALLOG)];

    [wkUserCtrl addUserScript:[[WKUserScript alloc]
        initWithSource:w_get_nsstring(script_regExternal)
        injectionTime:WKUserScriptInjectionTimeAtDocumentStart
        forMainFrameOnly:FALSE
    ]];

    [wkUserCtrl addUserScript:[[WKUserScript alloc]
        initWithSource:w_get_nsstring(g_console_js)
        injectionTime:WKUserScriptInjectionTimeAtDocumentStart
        forMainFrameOnly:FALSE
    ]];

    [wkUserCtrl addUserScript:[[WKUserScript alloc]
        initWithSource:w_get_nsstring(script_inwardPostMessage)
        injectionTime:WKUserScriptInjectionTimeAtDocumentEnd
        forMainFrameOnly:TRUE
    ]];

    [wkUserCtrl addUserScript:[[WKUserScript alloc]
        initWithSource:w_get_nsstring(script_afterInternal)
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

    id processPool = [configuration processPool];
    [processPool _setDownloadDelegate:[__WKDownloadDelegate new]];
    [configuration setProcessPool:processPool];
    [configuration setUserContentController:createWKUserContentController()];

    WKPreferences *preferences = [WKPreferences new];
    preferences.javaScriptCanOpenWindowsAutomatically = YES;
    preferences.tabFocusesLinks = FALSE;
    if (m_bDebug) {
        [preferences setValue:@TRUE forKey:@"allowFileAccessFromFileURLs"];
        [preferences setValue:@TRUE forKey:@"developerExtrasEnabled"];
    }
    // preferences.minimumFontSize = 40.0;

    configuration.preferences = preferences;

    configuration.applicationNameForUserAgent = @"Chrome/83.0.4103.97 Safari/537.36";

    // "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.97 Safari/537.36";
    // "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_5) AppleWebKit/605.1.15 (KHTML, like Gecko) Chrome/83.0.4103.97 Safari/537.36"
    

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

    [m_wkWebView setUIDelegate:[__WKUIDelegate new]];
    [m_wkWebView setNavigationDelegate:[__WKNavigationDelegate new]];

    [m_wkWebView autorelease];
}

void WebView::navigateWKWebView()
{
    id nsURL = [NSURL
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
    // [m_nsWindow makeKeyAndOrderFront:nil];
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

    setupAppMenubar();

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

int WebView::injectCSS(WebView* w, const char* css)
{
    // int n = helperEncodeJS(css, NULL, 0);
    // char* esc = (char*)calloc(1, sizeof(CSS_INJECT_FUNCTION) + n + 4);
    // if (esc == NULL) {
    //     return -1;
    // }
    // char* js = (char*)calloc(1, n);
    // helperEncodeJS(css, js, n);
    // snprintf(esc, sizeof(CSS_INJECT_FUNCTION) + n + 4, "%s(\"%s\")",
    //     CSS_INJECT_FUNCTION, js);
    // int r = evaluateWebviewJS(w, esc);
    // free(js);
    // free(esc);
    // return r;

    return 0;
}

// void WebView::toggleFullScreen(int nextFull)
// {
//     unsigned long windowStyleMask = (unsigned long)[m_nsWindow styleMask];
//     int b = (((windowStyleMask & NSWindowStyleMaskFullScreen) == NSWindowStyleMaskFullScreen)
//             ? 1
//             : 0);
//     if (b != nextFull) {
//         [m_nsWindow toggleFullScreen:NULL];
//     }
// }

void WebView::webview_set_color(WebView* w, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    // id color = [NSColor
    //     colorWithRed:((float)r / 255.0)
    //     green:((float)g / 255.0)
    //     blue:((float)b / 255.0)
    //     alpha:((float)a / 255.0)
    // ];

    // [w->priv.window setBackgroundColor:color];

    // if (0.5 >= ((r / 255.0 * 299.0) + (g / 255.0 * 587.0) + (b / 255.0 * 114.0)) / 1000.0) {
    //     [w->priv.window
    //         setAppearance:[NSAppearance appearanceNamed:get_nsstring("NSAppearanceNameVibrantDark")]
    //     ];
    // } else {
    //     [w->priv.window
    //         setAppearance:[NSAppearance appearanceNamed:get_nsstring("NSAppearanceNameVibrantLight")]
    //     ];
    // }

    // [w->priv.window setOpaque:FALSE];
    // [w->priv.window setTitlebarAppearsTransparent:YES];
}

void WebView::webview_dialog(WebView* w, enum webview_dialog_type dlgtype, int flags, const char* title, const char* arg, char* result, size_t resultsz)
{
    // if (dlgtype == WEBVIEW_DIALOG_TYPE_OPEN || dlgtype == WEBVIEW_DIALOG_TYPE_SAVE) {
    //     id panel = (id)objc_getClass("NSSavePanel");
    //     if (dlgtype == WEBVIEW_DIALOG_TYPE_OPEN) {
    //         id openPanel = [NSOpenPanel openPanel];
            
    //         if (flags & WEBVIEW_DIALOG_FLAG_DIRECTORY) {
    //             [openPanel setCanChooseFiles:FALSE];
    //             [openPanel setCanChooseDirectories:TRUE];
    //         } else {
    //             [openPanel setCanChooseFiles:TRUE];
    //             [openPanel setCanChooseDirectories:FALSE];
    //         }

    //         [openPanel setResolvesAliases:FALSE];
    //         [openPanel setAllowsMultipleSelection:FALSE];
    //         panel = openPanel;
    //     } else {
    //         panel = [NSSavePanel savePanel];
    //     }

    //     [panel setCanCreateDirectories:TRUE];
    //     [panel setShowsHiddenFiles:TRUE];
    //     [panel setExtensionHidden:FALSE];
    //     [panel setCanSelectHiddenExtension:FALSE];
    //     [panel setTreatsFilePackagesAsDirectories:TRUE];

    //     [panel
    //         beginSheetModalForWindow:w->priv.window
    //         completionHandler:^(NSModalResponse result) {
    //             [[NSApplication sharedApplication] stopModalWithCode:result];
    //         }
    //     ];

    //     if (
    //         [[NSApplication sharedApplication] runModalForWindow:panel] == NSModalResponseOK
    //     ) {
    //         id url = [panel URL];
    //         id path = [url path];
    //         const char* filename = (const char*)[path UTF8String];
    //         strlcpy(result, filename, resultsz);
    //     }
    // } else if (dlgtype == WEBVIEW_DIALOG_TYPE_ALERT) {
    //     id a = [NSAlert new];
    //     switch (flags & WEBVIEW_DIALOG_FLAG_ALERT_MASK) {
    //     case WEBVIEW_DIALOG_FLAG_INFO:
    //         [a setAlertStyle:NSAlertStyleInformational];
    //         break;
    //     case WEBVIEW_DIALOG_FLAG_WARNING:
    //         printf("Warning\n");
    //         [a setAlertStyle:NSAlertStyleWarning];
    //         break;
    //     case WEBVIEW_DIALOG_FLAG_ERROR:
    //         printf("Error\n");
    //         [a setAlertStyle:NSAlertStyleCritical];
    //         break;
    //     }

    //     [a setShowsHelp:FALSE];
    //     [a setShowsSuppressionButton:FALSE];
    //     [a setMessageText:get_nsstring(title)];
    //     [a setInformativeText:get_nsstring(arg)];

    //     [a addButtonWithTitle:get_nsstring("OK")];
    //     [a runModal];
    //     [a release];
    // }
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

id WebView::create_menu_item(id title, const char* action, const char* key)
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
    printf("WebView::get_visible [1] : %s \n", m_visible ? "yes" : "no");

    retVal = m_visible;
    return 0;
}

result_t WebView::set_visible(bool newVal)
{
    m_visible = newVal;

    printf("WebView::set_visible [1] : %s \n", newVal ? "yes" : "no");
    printf("WebView::set_visible [2] : %s \n", m_visible ? "yes" : "no");

    if (m_visible) {
        if (m_maximize) maxmizeNSWindow(m_nsWindow);
        m_maximize = false;
    }

    [m_nsWindow setIsVisible:(m_visible ? YES : NO)];

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

}

#endif /* __APPLE__ */