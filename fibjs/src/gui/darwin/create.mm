/*
 * WebView.mm
 *
 *  Created on: Sep 22, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#ifndef iPhone

#include "object.h"
#include "ifs/fs.h"
#include "ifs/url.h"
#include "ifs/mime.h"
#include "ifs/gui.h"
#include "WebView.h"
#include "Buffer.h"
#include "EventInfo.h"
#import <WebKit/WebKit.h>

@interface WKWebViewHandler : WKWebView <WKScriptMessageHandler, WKURLSchemeHandler, WKNavigationDelegate>
@property (nonatomic, assign) fibjs::WebView* webView;
- (instancetype)initWithWebView:(fibjs::WebView*)webView;
- (void)removeFromSuperview;
@end

@implementation WKWebViewHandler
- (instancetype)initWithWebView:(fibjs::WebView*)webView
{
    self = [super init];
    if (self) {
        _webView = webView;
    }
    return self;
}

- (void)removeFromSuperview
{
    _webView = nil;
    [super removeFromSuperview];
}

- (void)observeValueForKeyPath:(NSString*)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValueChangeKey, id>*)change context:(void*)context
{
    if (_webView) {
        if ([keyPath isEqualToString:@"title"] && object == _webView->m_webview) {
            NSString* newTitle = change[NSKeyValueChangeNewKey];
            [_webView->m_window setTitle:newTitle];
        }
    }
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
                    [_webView->m_window performClose:nil];
                else if ([message.body isEqualToString:@"minimize"])
                    _webView->internal_minimize();
                else if ([message.body isEqualToString:@"maximize"])
                    _webView->internal_maximize();
                else if ([message.body isEqualToString:@"drag"])
                    [_webView->m_window performWindowDragWithEvent:[_webView->m_window currentEvent]];
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

    [(id)urlSchemeTask retain];

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

                @try {
                    NSURLResponse* response = [[NSURLResponse alloc] initWithURL:url
                                                                        MIMEType:[NSString stringWithUTF8String:mtype.c_str()]
                                                           expectedContentLength:[data length]
                                                                textEncodingName:nil];

                    [urlSchemeTask didReceiveResponse:response];
                    [urlSchemeTask didReceiveData:data];
                    [urlSchemeTask didFinish];
                } @catch (NSException* exception) {
                }

                [(id)urlSchemeTask release];
            },
                CALL_E_GUICALL);
        } else {
            fibjs::async([fname, urlSchemeTask]() {
                NSString* errorHTML = [NSString stringWithFormat:@"<html><body><h1>Error</h1><p>File not found at path:<br>%@</p></body></html>",
                                                [NSString stringWithUTF8String:fname.c_str()]];
                NSData* data = [errorHTML dataUsingEncoding:NSUTF8StringEncoding];

                @try {
                    NSURLResponse* response = [[NSURLResponse alloc] initWithURL:[NSURL URLWithString:@""]
                                                                        MIMEType:@"text/html"
                                                           expectedContentLength:[data length]
                                                                textEncodingName:@"UTF-8"];
                    [urlSchemeTask didReceiveResponse:response];
                    [urlSchemeTask didReceiveData:data];
                    [urlSchemeTask didFinish];
                } @catch (NSException* exception) {
                }

                [(id)urlSchemeTask release];
            },
                CALL_E_GUICALL);
        }
    });
}

- (void)webView:(WKWebView*)webView stopURLSchemeTask:(id<WKURLSchemeTask>)urlSchemeTask
{
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

static WKProcessPool* globalProcessPool = [[WKProcessPool alloc] init];
result_t WebView::createWebView()
{
    NSWindow* window = [[NSWindow alloc] initWithContentRect:CGRectZero
                                                   styleMask:(NSWindowStyleMaskTitled
                                                                 | NSWindowStyleMaskClosable
                                                                 | NSWindowStyleMaskResizable
                                                                 | NSWindowStyleMaskMiniaturizable)
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO];
    m_window = window;

    WKWebViewConfiguration* configuration = [[WKWebViewConfiguration alloc] init];
    configuration.processPool = globalProcessPool;

    WKWebViewHandler* webView = [[WKWebViewHandler alloc] initWithWebView:this];
    m_webview = webView;

    WKUserContentController* userContentController = [[WKUserContentController alloc] init];
    [userContentController addScriptMessageHandler:webView name:@"message"];
    [userContentController addScriptMessageHandler:webView name:@"command"];

    WKUserScript* userScript = [[WKUserScript alloc] initWithSource:s_bridge_code
                                                      injectionTime:WKUserScriptInjectionTimeAtDocumentStart
                                                   forMainFrameOnly:NO];
    [userContentController addUserScript:userScript];

    configuration.userContentController = userContentController;
    [configuration setURLSchemeHandler:webView forURLScheme:@"fs"];

    if (m_options->devtools.value()) {
        WKPreferences* preferences = [[WKPreferences alloc] init];
        [preferences setValue:@YES forKey:@"developerExtrasEnabled"];
        configuration.preferences = preferences;
    }

    static std::string safariVersion = readSafariVersion();
    configuration.applicationNameForUserAgent = [NSString stringWithUTF8String:safariVersion.c_str()];

    [webView initWithFrame:CGRectZero configuration:configuration];
    [webView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];

    [webView addObserver:webView forKeyPath:@"title" options:NSKeyValueObservingOptionNew context:nil];
    [webView setNavigationDelegate:webView];

    [window.contentView addSubview:webView];

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

    NSURL* nsurl = [NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]];
    NSURLRequest* request = [NSURLRequest requestWithURL:nsurl];
    [m_webview loadRequest:request];

    config();

    return 0;
}

}

#endif
