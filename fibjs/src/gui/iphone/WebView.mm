/*
 * WebView.mm
 *
 *  Created on: Nov 11, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#ifdef iPhone

#include "object.h"
#include "ifs/fs.h"
#include "ifs/url.h"
#include "ifs/mime.h"
#include "ifs/gui.h"
#include "ifs/encoding.h"
#include "EventInfo.h"
#include "WebView.h"

#import <WebKit/WebKit.h>
#import "AppDelegate.h"
#import "ViewController.h"
#include <objc/runtime.h>

#ifndef NSInternalSpecifierError
#define NSInternalSpecifierError 5
#endif

@interface WKWebViewHandler : WKWebView <WKScriptMessageHandler, WKURLSchemeHandler, WKNavigationDelegate>
@property (nonatomic, assign) fibjs::WebView* webView;
- (instancetype)initWithWebView:(fibjs::WebView*)webView configuration:(WKWebViewConfiguration*)configuration;
@end

@implementation WKWebViewHandler
- (instancetype)initWithWebView:(fibjs::WebView*)webView configuration:(WKWebViewConfiguration*)configuration
{
    self = [super initWithFrame:CGRectMake(0, 0, 0, 0) configuration:configuration];
    if (self) {
        _webView = webView;
    }
    return self;
}

- (void)userContentController:(WKUserContentController*)userContentController didReceiveScriptMessage:(WKScriptMessage*)message
{
    if (_webView) {
        if ([message.body isKindOfClass:[NSString class]]) {
            if ([message.name isEqualToString:@"message"]) {
                fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(_webView, "message");
                ei->add("data", [message.body UTF8String]);
                ei->emit();
            } else if ([message.name isEqualToString:@"command"]) {
                if ([message.body isEqualToString:@"close"])
                    _webView->internal_close();
                else if ([message.body isEqualToString:@"minimize"])
                    _webView->internal_minimize();
                else if ([message.body isEqualToString:@"maximize"])
                    _webView->internal_maximize();
                else if ([message.body isEqualToString:@"drag"])
                    ;
                else {
                    _webView->app_rpc([message.body UTF8String]);
                }
            }
        }
    }
}

- (void)webView:(WKWebView*)webView didStartProvisionalNavigation:(WKNavigation*)navigation
{
    if (_webView) {
        fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(_webView, "loading");
        ei->add("url", [[[webView URL] absoluteString] UTF8String]);
        ei->emit();
    }
}

- (void)webView:(WKWebView*)webView didFinishNavigation:(WKNavigation*)navigation
{
    if (_webView) {
        fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(_webView, "load");
        exlib::string url = [[[webView URL] absoluteString] UTF8String];
        ei->add("url", url);
        ei->emit();
        _webView->postWaitFor(url);
    }
}

- (void)webView:(WKWebView*)webView startURLSchemeTask:(id<WKURLSchemeTask>)urlSchemeTask
{
    NSURLRequest* request = [urlSchemeTask request];
    NSURL* url = [request URL];

    exlib::string fname = [[url path] UTF8String];
    fibjs::async([fname, url, urlSchemeTask]() {
        fibjs::Variant var;
        fibjs::result_t hr = fibjs::fs_base::cc_readFile(fname, "", var, fibjs::Isolate::main());

        if (hr >= 0) {
            fibjs::async([fname, url, urlSchemeTask, var]() {
                fibjs::Buffer* _buf = (fibjs::Buffer*)var.object();
                NSData* data = _buf ? [NSData dataWithBytes:_buf->data() length:_buf->length()] : [NSData data];

                exlib::string mtype;
                fibjs::mime_base::getType(fname, mtype);

                NSURLResponse* response = [[NSURLResponse alloc] initWithURL:url MIMEType:[NSString stringWithUTF8String:mtype.c_str()] expectedContentLength:[data length] textEncodingName:nil];
                [urlSchemeTask didReceiveResponse:response];
                [urlSchemeTask didReceiveData:data];
                [urlSchemeTask didFinish];
            },
                CALL_E_GUICALL);
        } else {
            fibjs::async([fname, urlSchemeTask]() {
                NSString* errorHTML = [NSString stringWithFormat:@"<html><body><h1>Error</h1><p>File not found at path:<br>%@</p></body></html>", [NSString stringWithUTF8String:fname.c_str()]];
                NSData* data = [errorHTML dataUsingEncoding:NSUTF8StringEncoding];
                NSURLResponse* response = [[NSURLResponse alloc] initWithURL:[NSURL URLWithString:@""] MIMEType:@"text/html" expectedContentLength:[data length] textEncodingName:@"UTF-8"];
                [urlSchemeTask didReceiveResponse:response];
                [urlSchemeTask didReceiveData:data];
                [urlSchemeTask didFinish];
            },
                CALL_E_GUICALL);
        }
    });
}
@end

namespace fibjs {

static NSString* s_bridge_code
    = @"window.app = (function(){"
       "    function postRequest(req) { window.webkit.messageHandlers.command.postMessage(JSON.stringify(req)); }"
       "    const pending = {};"
       "    function generateId() {"
       "        while(true) { const id = Math.random().toString(36).substring(2); if(!pending[id]) return id; }"
       "    }"
       "    function wrap(m, fn) {"
       "        return new Proxy(fn, {"
       "            get: function(target, prop) {"
       "                const method = m === '' ? prop : m + '.' + prop;"
       "                return wrap(method, function(...params) { return new Promise((resolve, reject) => {"
       "                    const id = generateId(); pending[id] = {resolve, reject};"
       "                    postRequest({id, method, params});"
       "                });});"
       "            },"
       "            set: function(target, method, value) { throw new Error('not allowed'); }"
       "        });"
       "    }"
       "    return wrap('', function(res) {"
       "        const p = pending[res.id];"
       "        if (p) {"
       "            delete pending[res.id];"
       "            if (res.error) { p.reject(new Error(res.error)); } else { p.resolve(res.result); }"
       "        }"
       "    });"
       "})();"
       "window.postMessage = function(message) { window.webkit.messageHandlers.message.postMessage(message); };"
       "window.close = function() { window.webkit.messageHandlers.command.postMessage('close'); };"
       "window.minimize = function() { window.webkit.messageHandlers.command.postMessage('minimize'); };"
       "window.maximize = function() { window.webkit.messageHandlers.command.postMessage('maximize'); };"
       "window.drag = function() { window.webkit.messageHandlers.command.postMessage('drag'); };";

std::string readSafariVersion()
{
    CFURLRef appURL = CFURLCreateWithString(kCFAllocatorDefault, CFSTR("/Applications/Safari.app"), NULL);
    if (appURL) {
        CFDictionaryRef infoDict = CFBundleCopyInfoDictionaryForURL(appURL);
        if (infoDict) {
            CFStringRef versionString = (CFStringRef)CFDictionaryGetValue(infoDict, CFSTR("CFBundleShortVersionString"));
            if (versionString) {
                NSString* version = (__bridge NSString*)versionString;
                CFRelease(infoDict);
                CFRelease(appURL);

                return "Version/" + std::string([version UTF8String]) + " Safari/605.1.15";
            }
            CFRelease(infoDict);
        }
        CFRelease(appURL);
    }

    return "Version/17.0 Safari/605.1.15";
}

result_t WebView::createWebView()
{
    AppDelegate* appDelegate = (AppDelegate*)[UIApplication sharedApplication].delegate;
    ViewController* rootViewController = appDelegate.window.rootViewController;

    WKWebViewHandler* webView = [WKWebViewHandler alloc];

    WKUserContentController* userContentController = [[WKUserContentController alloc] init];
    [userContentController addScriptMessageHandler:webView name:@"message"];
    [userContentController addScriptMessageHandler:webView name:@"command"];

    WKUserScript* userScript = [[WKUserScript alloc] initWithSource:s_bridge_code
                                                      injectionTime:WKUserScriptInjectionTimeAtDocumentStart
                                                   forMainFrameOnly:NO];
    [userContentController addUserScript:userScript];

    WKWebViewConfiguration* configuration = [[WKWebViewConfiguration alloc] init];
    configuration.userContentController = userContentController;
    [configuration setURLSchemeHandler:webView forURLScheme:@"fs"];

    if (m_options->devtools.value()) {
        WKPreferences* preferences = [[WKPreferences alloc] init];
        [preferences setValue:@YES forKey:@"developerExtrasEnabled"];
        configuration.preferences = preferences;
    }

    static std::string safariVersion = readSafariVersion();
    configuration.applicationNameForUserAgent = [NSString stringWithUTF8String:safariVersion.c_str()];

    [webView initWithWebView:this configuration:configuration];
    [webView setNavigationDelegate:webView];

    [rootViewController addWebView:webView];

    m_window = rootViewController;
    m_webview = webView;

    exlib::string url;
    if (m_options->url.has_value())
        url = m_options->url.value();
    else if (m_options->file.has_value()) {
        obj_ptr<UrlObject_base> u;
        result_t hr = url_base::pathToFileURL(m_options->file.value(), v8::Local<v8::Object>(), u);
        if (hr < 0)
            return hr;

        u->get_href(url);
        url = "fs:" + url.substr(5);
    } else
        url = "about:blank";

    [webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]]]];

    if (!m_options->visible.value())
        [webView setHidden:YES];

    m_ready->set();

    return 0;
}

void WebView::internal_close()
{
    fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(this, "close");
    ei->emit();

    ViewController* rootViewController = (ViewController*)m_window;
    [rootViewController removeWebView:(WKWebView*)m_webview];

    m_window = nullptr;
    m_webview = nullptr;
}

void WebView::internal_minimize()
{
}

void WebView::internal_maximize()
{
}

bool WebView::internal_isReady()
{
    return [(WKWebView*)m_webview isLoading] == NO;
}

void WebView::internal_eval(exlib::string code)
{
    [(WKWebView*)m_webview evaluateJavaScript:[NSString stringWithUTF8String:code.c_str()] completionHandler:nil];
}

exlib::string WebView::internal_getUrl()
{
    if ([(WKWebView*)m_webview URL])
        return [[((WKWebView*)m_webview).URL absoluteString] UTF8String];

    return "";
}

result_t WebView::loadUrl(exlib::string url, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    NSURL* nsurl = [NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]];
    NSURLRequest* request = [NSURLRequest requestWithURL:nsurl];

    WKWebView* webView = (WKWebView*)m_webview;
    [webView loadRequest:request];

    return 0;
}

result_t WebView::getUrl(exlib::string& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    retVal = internal_getUrl();

    return 0;
}

result_t WebView::setHtml(exlib::string html, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    [(WKWebView*)m_webview loadHTMLString:[NSString stringWithUTF8String:html.c_str()] baseURL:nil];

    return 0;
}

result_t WebView::getHtml(exlib::string& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    WKWebView* webView = (WKWebView*)m_webview;
    [webView evaluateJavaScript:@"document.documentElement.outerHTML.toString()"
              completionHandler:^(NSString* html, NSError* error) {
                  if (error == nil)
                      retVal = [html UTF8String];
                  ac->post(0);
              }];

    return CALL_E_PENDDING;
}

result_t WebView::isReady(bool& retVal, AsyncEvent* ac)
{
    bool is_win_ready = false;
    m_ready->isSet(is_win_ready);
    if (!is_win_ready) {
        retVal = false;
        return 0;
    }

    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    retVal = internal_isReady();

    return 0;
}

result_t WebView::reload(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    [(WKWebView*)m_webview reload];

    return 0;
}

result_t WebView::goBack(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    [(WKWebView*)m_webview goBack];

    return 0;
}

result_t WebView::goForward(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    [(WKWebView*)m_webview goForward];

    return 0;
}

static void js2Variant(id result, Variant& retVal)
{
    if ([result isKindOfClass:[NSArray class]]) {
        obj_ptr<NArray> array = new NArray();
        for (id obj in result) {
            Variant v;
            js2Variant(obj, v);
            array->append(v);
        }
        retVal = array;
    } else if ([result isKindOfClass:[NSDictionary class]]) {
        obj_ptr<NObject> obj = new NObject();
        for (id key in result) {
            Variant v;
            js2Variant(result[key], v);
            obj->add([key UTF8String], v);
        }
        retVal = obj;
    } else if ([result isKindOfClass:[NSDate class]]) {
        retVal = (date_t)(1000 * [(NSDate*)result timeIntervalSince1970]);
    } else if ([result isKindOfClass:[NSNumber class]]) {
        const char* type = [result objCType];
        if (strcmp(type, @encode(int)) == 0) {
            retVal = [result intValue];
        } else if (strcmp(type, @encode(double)) == 0) {
            retVal = [result doubleValue];
        } else if (strcmp(type, @encode(char)) == 0) {
            retVal = [result boolValue] ? true : false;
        } else {
            retVal = [result UTF8String];
        }
    } else if ([result isKindOfClass:[NSString class]]) {
        retVal = [result UTF8String];
    } else if ([result isKindOfClass:[NSNull class]]) {
        retVal.setNull();
    }
}

static NSString* const WKJavaScriptExceptionMessage = @"WKJavaScriptExceptionMessage";
result_t WebView::eval(exlib::string code, Variant& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    [(WKWebView*)m_webview evaluateJavaScript:[NSString stringWithUTF8String:code.c_str()]
                            completionHandler:^(id result, NSError* error) {
                                if (error) {
                                    if (NSInternalSpecifierError == error.code) {
                                        ac->post(0);
                                    } else {
                                        NSString* jsExceptionMessage = error.userInfo[WKJavaScriptExceptionMessage];
                                        if (jsExceptionMessage) {
                                            ac->post(Runtime::setError([jsExceptionMessage UTF8String]));
                                        } else {
                                            NSString* errorDescription = [error localizedDescription];
                                            ac->post(Runtime::setError([errorDescription UTF8String]));
                                        }
                                    }
                                } else {
                                    js2Variant(result, retVal);
                                    ac->post(0);
                                }
                            }];

    return CALL_E_PENDDING;
}

result_t WebView::setTitle(exlib::string title, AsyncEvent* ac)
{
    return 0;
}

result_t WebView::getTitle(exlib::string& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    NSString* nsTitle = [(WKWebView*)m_webview title];
    retVal = [nsTitle UTF8String];

    return 0;
}

result_t WebView::isVisible(bool& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    WKWebView* webView = (WKWebView*)m_webview;
    retVal = !webView.hidden;
    return 0;
}

result_t WebView::show(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    WKWebView* webView = (WKWebView*)m_webview;
    [webView setHidden:NO];

    return 0;
}

result_t WebView::hide(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    WKWebView* webView = (WKWebView*)m_webview;
    [webView setHidden:YES];

    return 0;
}

result_t WebView::setSize(int32_t width, int32_t height, AsyncEvent* ac)
{
    return 0;
}

result_t WebView::getSize(obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    CGSize size = [(WKWebView*)m_webview view].frame.size;

    retVal = new NArray();
    retVal->append(size.width);
    retVal->append(size.height);

    return 0;
}

result_t WebView::setPosition(int32_t left, int32_t top, AsyncEvent* ac)
{
    return 0;
}

result_t WebView::getPosition(obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    CGRect frame = [(WKWebView*)m_webview view].frame;

    retVal = new NArray();
    retVal->append((int32_t)frame.origin.x);
    retVal->append((int32_t)frame.origin.y);

    return 0;
}

result_t WebView::isActived(bool& retVal, AsyncEvent* ac)
{
    retVal = true;
    return 0;
}

result_t WebView::active(AsyncEvent* ac)
{
    return 0;
}

result_t WebView::takeScreenshot(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    return CALL_E_INVALID_CALL;
}

result_t WebView::close(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    internal_close();

    return 0;
}

result_t WebView::postMessage(exlib::string msg, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    encoding_base::jsstr(msg, false, msg);
    exlib::string _jsCode = "window.dispatchEvent(new MessageEvent('message', { data: '" + msg + "' }))";

    NSString* jsCode = [NSString stringWithUTF8String:_jsCode.c_str()];
    [(WKWebView*)m_webview evaluateJavaScript:jsCode completionHandler:nil];

    return 0;
}

}

#endif
