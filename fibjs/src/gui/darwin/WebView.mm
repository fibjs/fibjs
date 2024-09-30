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
#include "ifs/encoding.h"
#include "WebView.h"
#import <WebKit/WebKit.h>

namespace fibjs {

result_t WebView::loadURL(exlib::string url, AsyncEvent* ac)
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

    if ([(WKWebView*)m_webview URL])
        retVal = [[((WKWebView*)m_webview).URL absoluteString] UTF8String];

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

result_t WebView::eval(exlib::string code, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    [(WKWebView*)m_webview evaluateJavaScript:[NSString stringWithUTF8String:code.c_str()] completionHandler:nil];

    return 0;
}

result_t WebView::setTitle(exlib::string title, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    NSString* nsTitle = [NSString stringWithUTF8String:title.c_str()];
    [(NSWindow*)m_window setTitle:nsTitle];

    return 0;
}

result_t WebView::getTitle(exlib::string& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    NSString* nsTitle = [(NSWindow*)m_window title];
    retVal = [nsTitle UTF8String];

    return 0;
}

result_t WebView::close(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    [(NSWindow*)m_window close];

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
