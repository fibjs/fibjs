/*
 * WebView.mm
 *
 *  Created on: Sep 22, 2024
 *      Author: lion
 */

#if defined(OS_DESKTOP)

#include "object.h"
#include "ifs/url.h"
#include "ifs/gui.h"
#include "WebView.h"
#include "EventInfo.h"
#import <WebKit/WebKit.h>

@interface MessageHandler : NSObject <WKScriptMessageHandler>
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

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValueChangeKey,id> *)change context:(void *)context
{
    if ([keyPath isEqualToString:@"title"] && object == _webView->m_webview) {
        NSString *newTitle = change[NSKeyValueChangeNewKey];
        [_webView->m_window setTitle:newTitle];
    } else {
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
    }
}

- (void)userContentController:(WKUserContentController*)userContentController didReceiveScriptMessage:(WKScriptMessage*)message
{
    if ([message.name isEqualToString:@"fibjs"]) {
        fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(_webView, "message");
        ei->add("data", [message.body UTF8String]);
        _webView->_emit("message", ei);
    } else if ([message.name isEqualToString:@"close"]) {
        _webView->internal_close();
    }
}
@end

namespace fibjs {

result_t WebView::open()
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
    [userContentController addScriptMessageHandler:messageHandler name:@"fibjs"];
    [userContentController addScriptMessageHandler:messageHandler name:@"close"];

    NSString* jsCode = @"window.postMessage = function(message) { window.webkit.messageHandlers.fibjs.postMessage(message); };"
                        "window.close = function() { window.webkit.messageHandlers.close.postMessage(''); };";
    WKUserScript* userScript = [[WKUserScript alloc] initWithSource:jsCode injectionTime:WKUserScriptInjectionTimeAtDocumentStart forMainFrameOnly:NO];
    [userContentController addUserScript:userScript];

    configuration.userContentController = userContentController;

    WKWebView* webView = [[WKWebView alloc] initWithFrame:CGRectZero configuration:configuration];
    [webView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
    [window.contentView addSubview:webView];
    m_webview = webView;

    [m_webview addObserver:messageHandler forKeyPath:@"title" options:NSKeyValueObservingOptionNew context:nil];

    exlib::string url;
    Variant v;

    if (m_opt->get("url", v) == 0)
        url = v.string();
    else if (m_opt->get("file", v) == 0) {
        obj_ptr<UrlObject_base> u;
        result_t hr = url_base::pathToFileURL(v.string(), u);
        if (hr < 0)
            return hr;

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
