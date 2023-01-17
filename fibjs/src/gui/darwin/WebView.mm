/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-12 04:25:25
 * @modify date 2020-06-12 04:25:25
 * @desc WebView Implementation in OSX
 */

#if defined(macosx)

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
#include "../os_gui.h"

#include "ns-api.h"

namespace fibjs {

void asyncLog(int32_t priority, exlib::string msg);

NSEvent* getEmptyCustomNSEvent()
{
    return [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                              location:NSMakePoint(0, 0)
                         modifierFlags:0
                             timestamp:0.0
                          windowNumber:0
                               context:nil
                               subtype:0
                                 data1:0
                                 data2:0];
}

static exlib::LockedList<AsyncEvent> s_uiPool;

void os_putGuiPool(AsyncEvent* ac)
{
    s_uiPool.putTail(ac);

    [[NSApplication sharedApplication]
        postEvent:getEmptyCustomNSEvent()
          atStart:YES];
}

id fetchEventFromNSRunLoop(int blocking)
{
    id until = blocking ? [NSDate distantFuture] : [NSDate distantPast];

    return [[NSApplication sharedApplication]
        nextEventMatchingMask:NSEventMaskAny
                    untilDate:until
                       inMode:@"kCFRunLoopDefaultMode"
                      dequeue:YES];
}

void run_os_gui()
{
    @autoreleasepool {
        [NSApplication sharedApplication];
        [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyRegular];
        [[NSApplication sharedApplication] finishLaunching];
        [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];

        while (true) {
            AsyncEvent* p = s_uiPool.getHead();

            if (p)
                p->invoke();

            id event = fetchEventFromNSRunLoop(1);
            [[NSApplication sharedApplication] sendEvent:event];
        }
    }
}

// useless for darwin
result_t os_gui_setVersion(int32_t ver)
{
    return 0;
}

void maxmizeNSWindow(NSWindow* win)
{
    [win setFrame:
             [[NSScreen mainScreen] visibleFrame]
          display:YES];
}

result_t openWebViewInGUIThread(obj_ptr<fibjs::WebView> wv)
{
    wv->open();

    return 0;
}

// In Javascript Thread
result_t os_gui_open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
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
    : m_bIScriptLoaded(false)
    , m_bDebug(true)
{
    isolate_ref();

    m_opt = opt;
    m_url = url;

    if (m_opt) {
        Variant v;

        if (m_opt->get("debug", v) == 0 && !v.isUndefined())
            m_bDebug = v.boolVal();

        if (m_opt->get("script_after", v) == 0)
            m_initScriptDocAfter = v.string();
    }
}

WebView::~WebView()
{
    [m_nsWindow autorelease];
    [m_wkWebView autorelease];
}

bool WebView::onNSWindowShouldClose()
{
    if (!isInternalScriptLoaded()) {
        forceCloseWindow();
        return false;
    }

    evaluateWebviewJS(
        "!window.external || typeof window.external.onclose !== 'function' ? false : window.external.onclose()",
        ^(id result, NSError* _Nullable error) {
            if (error != nil) {
                if (m_bDebug)
                    asyncLog(console_base::C_DEBUG, NSStringToExString([error localizedDescription]));

                forceCloseWindow();
            } else {
                if (result == nil)
                    forceCloseWindow();
                else if ([result boolValue] != NO)
                    forceCloseWindow();
            }
        });

    return false;
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
    v8::Local<v8::Context> context = isolate->context();

    int32_t logLevel = isolate->toInteger(JSValue(logInfo->Get(context, isolate->NewString("level"))));

    JSValue _fmtMessage = logInfo->Get(context, isolate->NewString("fmt"));
    exlib::string fmtMessage(isolate->toString(_fmtMessage));

    if (logLevel == console_base::C_ERROR)
        fmtMessage = ("WebView Error: " + fmtMessage);

    asyncLog(logLevel, fmtMessage);

    return 0;
}

void WebView::onWKWebViewExternalLogMessage(WKScriptMessage* message)
{
    if (!m_bDebug)
        return;

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
                                  forMainFrameOnly:FALSE]];

    [wkUserCtrl addUserScript:[[WKUserScript alloc]
                                    initWithSource:w_get_nsstring(g_console_js)
                                     injectionTime:WKUserScriptInjectionTimeAtDocumentStart
                                  forMainFrameOnly:FALSE]];

    [wkUserCtrl addUserScript:[[WKUserScript alloc]
                                    initWithSource:w_get_nsstring(scriptEnd_inwardPostMessage)
                                     injectionTime:WKUserScriptInjectionTimeAtDocumentEnd
                                  forMainFrameOnly:TRUE]];

    [wkUserCtrl addUserScript:[[WKUserScript alloc]
                                    initWithSource:w_get_nsstring(scriptEnd_afterInternal)
                                     injectionTime:WKUserScriptInjectionTimeAtDocumentEnd
                                  forMainFrameOnly:TRUE]];

    // TODO: support inject javascript
    [wkUserCtrl addUserScript:[[WKUserScript alloc]
                                    initWithSource:get_nsstring(m_initScriptDocAfter.c_str())
                                     injectionTime:WKUserScriptInjectionTimeAtDocumentEnd
                                  forMainFrameOnly:TRUE]];

    return wkUserCtrl;
}

id WebView::createWKWebViewConfig()
{
    WKWebViewConfiguration* configuration = [WKWebViewConfiguration new];

    // WKProcessPool* processPool = [configuration processPool];
    // [configuration setProcessPool:processPool];
    [configuration setUserContentController:createWKUserContentController()];

    WKPreferences* preferences = [WKPreferences new];
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
    NSOperatingSystemVersion macOS10_13 = (NSOperatingSystemVersion){ 10, 13, 0 };
    if ([[NSProcessInfo processInfo] isOperatingSystemAtLeastVersion:macOS10_13]) {
        [configuration setURLSchemeHandler:[FileSystemWKURLSchemeHandler new] forURLScheme:@"fs"];
    }

    return configuration;
}

void WebView::initNSWindow()
{
    m_nsWindow = [[NSWindow alloc]
        initWithContentRect:CGRectMake(0, 0, 400, 300)
                  styleMask:0x0f
                    backing:NSBackingStoreBuffered
                      defer:YES];

    os_config_window(this, m_nsWindow, m_opt);
}

void WebView::initWKWebView()
{
    m_wkWebView = [[WKWebView alloc]
        initWithFrame:CGRectMake(0, 0, 400, 300)
        configuration:createWKWebViewConfig()];

    assignToWKWebView(m_wkWebView);
}

void WebView::navigateWKWebView(exlib::string url)
{
    NSURL* nsURL = [NSURL
        URLWithString:get_nsstring(
                          webview_check_url(url.c_str()))];
    [m_wkWebView loadRequest:[NSURLRequest requestWithURL:nsURL]];
}

void WebView::startWKUI()
{
    [m_wkWebView setAutoresizesSubviews:TRUE];

    m_nsWindow.contentView = m_wkWebView;
    [m_nsWindow.contentView setWantsLayer:YES];
}

int WebView::initializeWebView()
{
    initNSWindow();

    initWKWebView();
    navigateWKWebView(m_url);

    startWKUI();

    return 0;
}

void WebView::evaluateWebviewJS(const char* js, JsEvaluateResultHdlr hdlr)
{
    if (hdlr == NULL) {
        hdlr = ^(id item, NSError* _Nullable error) {
        };
    }

    [m_wkWebView
        evaluateJavaScript:get_nsstring(js)
         completionHandler:hdlr];
}

result_t WebView::loadUrl(exlib::string url, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (!m_wkWebView)
        return 0;

    navigateWKWebView(url);

    return 0;
}

result_t WebView::getUrl(exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (!m_wkWebView)
        return 0;

    if ([m_wkWebView URL])
        retVal = [[m_wkWebView.URL absoluteString] UTF8String];

    return 0;
}

result_t WebView::setHtml(exlib::string html, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (!m_wkWebView)
        return 0;

    [m_wkWebView loadHTMLString:get_nsstring(html.c_str()) baseURL:nil];

    return 0;
}

result_t WebView::reload(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (!m_wkWebView)
        return 0;

    [m_wkWebView reload];

    return 0;
}

result_t WebView::goBack(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (!m_wkWebView)
        return 0;

    [m_wkWebView goBack];

    return 0;
}

result_t WebView::goForward(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (!m_wkWebView)
        return 0;

    [m_wkWebView goForward];

    return 0;
}

// TODO: support it.
result_t WebView::print(int32_t mode, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t WebView::executeJavaScript(exlib::string code, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (!m_nsWindow)
        return 0;

    evaluateWebviewJS(code.c_str(), nil);

    return 0;
}

result_t WebView::close(AsyncEvent* ac)
{
    if (ac && ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (!m_nsWindow)
        return 0;

    onNSWindowShouldClose();

    return 0;
}

void WebView::forceCloseWindow()
{
    [m_nsWindow close];
}

result_t WebView::postMessage(exlib::string msg, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    exlib::string c_jsstr;
    // TODO: we should escape it.
    c_jsstr.append("external.onmessage('");
    c_jsstr.append(msg);
    c_jsstr.append("')");

    evaluateWebviewJS(c_jsstr.c_str());

    return 0;
}

}

#endif /* __APPLE__ */