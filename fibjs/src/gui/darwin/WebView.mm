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
            retVal = [result boolValue];
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
                                    NSString* jsExceptionMessage = error.userInfo[WKJavaScriptExceptionMessage];
                                    if (jsExceptionMessage) {
                                        ac->post(Runtime::setError([jsExceptionMessage UTF8String]));
                                    } else {
                                        NSString* errorDescription = [error localizedDescription];
                                        ac->post(Runtime::setError([errorDescription UTF8String]));
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

result_t WebView::setSize(int32_t width, int32_t height, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    NSRect frame = [(NSWindow*)m_window frame];
    frame.size = NSMakeSize(width, height);
    [(NSWindow*)m_window setFrame:frame display:YES animate:NO];

    return 0;
}

result_t WebView::getSize(obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    NSSize size = [(NSWindow*)m_window frame].size;

    retVal = new NArray();
    retVal->append(size.width);
    retVal->append(size.height);

    return 0;
}

result_t WebView::setPosition(int32_t left, int32_t top, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    NSRect screen_rect = [[NSScreen mainScreen] frame];
    NSRect frame = [(NSWindow*)m_window frame];

    left = screen_rect.origin.x + left;
    top = screen_rect.size.height + screen_rect.origin.y - (frame.size.height + top);

    frame.origin = NSMakePoint(left, top);
    [(NSWindow*)m_window setFrame:frame display:YES animate:NO];

    return 0;
}

result_t WebView::getPosition(obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    NSRect frame = [(NSWindow*)m_window frame];

    NSRect screen_rect = [[NSScreen mainScreen] frame];

    retVal = new NArray();
    retVal->append((int32_t)(frame.origin.x - screen_rect.origin.x));
    retVal->append((int32_t)(screen_rect.size.height + screen_rect.origin.y - (frame.size.height + frame.origin.y)));

    return 0;
}

result_t WebView::isActived(bool& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    retVal = [(NSWindow*)m_window isKeyWindow];

    return 0;
}

result_t WebView::active(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    [(NSWindow*)m_window makeKeyAndOrderFront:nil];

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
