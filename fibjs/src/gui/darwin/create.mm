/*
 * WebView.mm
 *
 *  Created on: Sep 22, 2024
 *      Author: lion
 */

#if defined(OS_DESKTOP)

#include "object.h"
#include "ifs/fs.h"
#include "ifs/url.h"
#include "ifs/mime.h"
#include "ifs/gui.h"
#include "WebView.h"
#include "Buffer.h"
#include "EventInfo.h"
#import <WebKit/WebKit.h>

@interface MessageHandler : NSObject <WKScriptMessageHandler, WKURLSchemeHandler>
@property (nonatomic, assign) fibjs::WebView* webView;
- (instancetype)initWithWebView:(fibjs::WebView*)webView;
@end

@implementation MessageHandler
- (instancetype)initWithWebView:(fibjs::WebView*)webView
{
    self = [super init];
    if (self) {
        _webView = webView;
    }
    return self;
}

- (void)observeValueForKeyPath:(NSString*)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValueChangeKey, id>*)change context:(void*)context
{
    if ([keyPath isEqualToString:@"title"] && object == _webView->m_webview) {
        NSString* newTitle = change[NSKeyValueChangeNewKey];
        [_webView->m_window setTitle:newTitle];
    } else {
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
    }
}

- (void)userContentController:(WKUserContentController*)userContentController didReceiveScriptMessage:(WKScriptMessage*)message
{
    if ([message.body isKindOfClass:[NSString class]]) {
        if ([message.name isEqualToString:@"message"]) {
            fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(_webView, "message");
            ei->add("data", [message.body UTF8String]);
            _webView->_emit("message", ei);
        } else if ([message.name isEqualToString:@"command"]) {
            if ([message.body isEqualToString:@"close"])
                _webView->internal_close();
            else if ([message.body isEqualToString:@"minimize"])
                _webView->internal_minimize();
            else if ([message.body isEqualToString:@"maximize"])
                _webView->internal_maximize();
            else if ([message.body isEqualToString:@"drag"])
                [_webView->m_window performWindowDragWithEvent:[_webView->m_window currentEvent]];
        }
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
    NSWindow* window = [[NSWindow alloc] initWithContentRect:CGRectZero
                                                   styleMask:(NSWindowStyleMaskTitled
                                                                 | NSWindowStyleMaskClosable
                                                                 | NSWindowStyleMaskResizable
                                                                 | NSWindowStyleMaskMiniaturizable)
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO];
    m_window = window;

    WKWebViewConfiguration* configuration = [[WKWebViewConfiguration alloc] init];
    WKUserContentController* userContentController = [[WKUserContentController alloc] init];

    MessageHandler* messageHandler = [[MessageHandler alloc] initWithWebView:this];
    [userContentController addScriptMessageHandler:messageHandler name:@"message"];
    [userContentController addScriptMessageHandler:messageHandler name:@"command"];

    NSString* jsCode = @"window.postMessage = function(message) { window.webkit.messageHandlers.message.postMessage(message); };"
                        "window.close = function() { window.webkit.messageHandlers.command.postMessage('close'); };"
                        "window.minimize = function() { window.webkit.messageHandlers.command.postMessage('minimize'); };"
                        "window.maximize = function() { window.webkit.messageHandlers.command.postMessage('maximize'); };"
                        "window.drag = function() { window.webkit.messageHandlers.command.postMessage('drag'); };";
    WKUserScript* userScript = [[WKUserScript alloc] initWithSource:jsCode injectionTime:WKUserScriptInjectionTimeAtDocumentStart forMainFrameOnly:NO];
    [userContentController addUserScript:userScript];

    configuration.userContentController = userContentController;
    [configuration setURLSchemeHandler:messageHandler forURLScheme:@"fs"];

    if (m_options->devtools.value()) {
        WKPreferences* preferences = [[WKPreferences alloc] init];
        [preferences setValue:@YES forKey:@"developerExtrasEnabled"];
        configuration.preferences = preferences;
    }

    static std::string safariVersion = readSafariVersion();
    configuration.applicationNameForUserAgent = [NSString stringWithUTF8String:safariVersion.c_str()];

    WKWebView* webView = [[WKWebView alloc] initWithFrame:CGRectZero configuration:configuration];
    [webView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
    [window.contentView addSubview:webView];
    m_webview = webView;

    [m_webview addObserver:messageHandler forKeyPath:@"title" options:NSKeyValueObservingOptionNew context:nil];

    exlib::string url;
    if (m_options->url.has_value())
        url = m_options->url.value();
    else if (m_options->file.has_value()) {
        obj_ptr<UrlObject_base> u;
        result_t hr = url_base::pathToFileURL(m_options->file.value(), u);
        if (hr < 0)
            return hr;

        u->set_protocol("fs:");
        u->set_slashes(true);

        u->get_href(url);
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
