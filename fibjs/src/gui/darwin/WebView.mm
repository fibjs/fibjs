/*
 * WebView.mm
 *
 *  Created on: Sep 22, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#ifndef iPhone

#include "object.h"
#include "ifs/url.h"
#include "ifs/gui.h"
#include "ifs/encoding.h"
#include "WebView.h"
#import <WebKit/WebKit.h>
#include <memory>
#include <unordered_set>

extern int32_t s_window_count;

namespace fibjs {

void WebView::internal_close()
{
    NSWindow* window = (NSWindow*)m_window;
    [window close];
}

void WebView::internal_minimize()
{
    NSWindow* window = (NSWindow*)m_window;
    [window miniaturize:nil];
}

void WebView::internal_maximize()
{
    NSWindow* window = (NSWindow*)m_window;
    [window zoom:nil];
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

result_t WebView::loadUrl(exlib::string url)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    WKWebView* webView = (WKWebView*)m_webview;
    NSURL* nsurl = [NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]];

    // Stop any ongoing navigation first: the cancelled load reports a failure
    // that has to be discarded before the new one arms the wait.
    [webView stopLoading];
    begin_load();
    [webView loadRequest:[NSURLRequest requestWithURL:nsurl]];

    return finish_load();
}

result_t WebView::getUrl(exlib::string& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = internal_getUrl();

    return result_t();
}

result_t WebView::setHtml(exlib::string html)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    WKWebView* webView = (WKWebView*)m_webview;

    [webView stopLoading];
    begin_load();
    [webView loadHTMLString:[NSString stringWithUTF8String:html.c_str()] baseURL:nil];

    return finish_load();
}

result_t WebView::getHtml(exlib::string& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    // The WebKit completion runs on this thread while the fiber is parked, so
    // the frame stays alive and the references below stay valid. The block
    // itself is heap-copied, hence the shared box for the state it writes.
    struct Box {
        exlib::Event ev;
        exlib::string err;
    };
    auto box = std::make_shared<Box>();

    [(WKWebView*)m_webview evaluateJavaScript:@"document.documentElement.outerHTML.toString()"
                            completionHandler:^(NSString* html, NSError* error) {
                                if (error == nil)
                                    retVal = html ? [html UTF8String] : "";
                                else
                                    box->err = [[error localizedDescription] UTF8String];
                                box->ev.set();
                            }];

    box->ev.wait();

    if (!box->err.empty())
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_GUI_ERROR").with_message(box->err);

    return result_t();
}

result_t WebView::reload()
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    WKWebView* webView = (WKWebView*)m_webview;

    begin_load();
    [webView reload];

    return finish_load();
}

result_t WebView::goBack(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    WKWebView* webView = (WKWebView*)m_webview;

    retVal = [webView canGoBack] == YES;
    if (!retVal)
        return result_t();

    begin_load();
    [webView goBack];

    return finish_load();
}

result_t WebView::goForward(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    WKWebView* webView = (WKWebView*)m_webview;

    retVal = [webView canGoForward] == YES;
    if (!retVal)
        return result_t();

    begin_load();
    [webView goForward];

    return finish_load();
}

static const int kMaxJs2VariantDepth = 20;
static const size_t kMaxJs2VariantElements = 10000;

static void js2VariantInternal(id result, Variant& retVal, int depth, size_t& count, std::unordered_set<const void*>& visited)
{
    if (!result) {
        retVal.setNull();
        return;
    }

    if (depth > kMaxJs2VariantDepth || count > kMaxJs2VariantElements) {
        retVal.setNull();
        return;
    }

    const void* ptr = (__bridge const void*)result;
    if (visited.find(ptr) != visited.end()) {
        retVal.setNull();
        return;
    }

    if ([result isKindOfClass:[NSArray class]]) {
        visited.insert(ptr);
        obj_ptr<NArray> array = new NArray();
        for (id obj in result) {
            if (++count > kMaxJs2VariantElements) {
                retVal.setNull();
                return;
            }
            Variant v;
            js2VariantInternal(obj, v, depth + 1, count, visited);
            array->append(v);
        }
        retVal = array;
    } else if ([result isKindOfClass:[NSDictionary class]]) {
        visited.insert(ptr);
        obj_ptr<NObject> obj = new NObject();
        for (id key in result) {
            if (++count > kMaxJs2VariantElements) {
                retVal.setNull();
                return;
            }
            Variant v;
            js2VariantInternal(result[key], v, depth + 1, count, visited);
            if ([key isKindOfClass:[NSString class]])
                obj->add([key UTF8String], v);
            else
                obj->add([[key description] UTF8String], v);
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
            retVal = [[result stringValue] UTF8String];
        }
    } else if ([result isKindOfClass:[NSString class]]) {
        retVal = [result UTF8String];
    } else if ([result isKindOfClass:[NSNull class]]) {
        retVal.setNull();
    }
}

static void js2Variant(id result, Variant& retVal)
{
    size_t count = 0;
    std::unordered_set<const void*> visited;
    js2VariantInternal(result, retVal, 0, count, visited);
}

static NSString* const WKJavaScriptExceptionMessage = @"WKJavaScriptExceptionMessage";
result_t WebView::eval(exlib::string code, Variant& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    struct Box {
        exlib::Event ev;
        exlib::string err;
    };
    auto box = std::make_shared<Box>();

    [(WKWebView*)m_webview evaluateJavaScript:[NSString stringWithUTF8String:code.c_str()]
                            completionHandler:^(id ret, NSError* error) {
                                if (error) {
                                    if (NSInternalSpecifierError == error.code) {
                                        // Empty completion for a script that returned nothing.
                                    } else {
                                        NSString* jsExceptionMessage = error.userInfo[WKJavaScriptExceptionMessage];
                                        if (jsExceptionMessage)
                                            box->err = [jsExceptionMessage UTF8String];
                                        else
                                            box->err = [[error localizedDescription] UTF8String];
                                    }
                                } else {
                                    js2Variant(ret, retVal);
                                }
                                box->ev.set();
                            }];

    box->ev.wait();

    if (!box->err.empty())
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_GUI_ERROR").with_message(box->err);

    return result_t();
}

result_t WebView::setTitle(exlib::string title)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    NSString* nsTitle = [NSString stringWithUTF8String:title.c_str()];
    [(NSWindow*)m_window setTitle:nsTitle];

    return result_t();
}

result_t WebView::getTitle(exlib::string& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    NSString* nsTitle = [(NSWindow*)m_window title];
    retVal = [nsTitle UTF8String];

    return result_t();
}

result_t WebView::isVisible(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = [(NSWindow*)m_window isVisible];
    return result_t();
}

result_t WebView::show()
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    if (![(NSWindow*)m_window isVisible]) {
        if (++s_window_count == 1)
            [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyRegular];
        [(NSWindow*)m_window makeKeyAndOrderFront:nil];
        [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
    }

    return result_t();
}

result_t WebView::hide()
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    if ([(NSWindow*)m_window isVisible]) {
        [(NSWindow*)m_window orderOut:nil];
        if (--s_window_count == 0)
            [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyAccessory];
    }

    return result_t();
}

result_t WebView::setSize(int32_t width, int32_t height)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    NSRect screen_rect = [[NSScreen mainScreen] frame];
    NSRect frame = [(NSWindow*)m_window frame];

    // The caller's y is the top edge; a taller frame keeps its bottom edge, so
    // the origin has to move to leave the top where it was.
    double top = screen_rect.size.height + screen_rect.origin.y - (frame.size.height + frame.origin.y);

    frame.size = NSMakeSize(width, height);
    frame.origin.y = screen_rect.size.height + screen_rect.origin.y - (frame.size.height + top);

    [(NSWindow*)m_window setFrame:frame display:YES animate:NO];

    return result_t();
}

result_t WebView::getSize(std::vector<double>& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    NSSize size = [(NSWindow*)m_window frame].size;

    retVal.push_back(size.width);
    retVal.push_back(size.height);

    return result_t();
}

result_t WebView::setPosition(int32_t left, int32_t top)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    NSRect screen_rect = [[NSScreen mainScreen] frame];
    NSRect frame = [(NSWindow*)m_window frame];

    left = screen_rect.origin.x + left;
    top = screen_rect.size.height + screen_rect.origin.y - (frame.size.height + top);

    frame.origin = NSMakePoint(left, top);
    [(NSWindow*)m_window setFrame:frame display:YES animate:NO];

    return result_t();
}

result_t WebView::getPosition(std::vector<double>& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    NSRect frame = [(NSWindow*)m_window frame];

    NSRect screen_rect = [[NSScreen mainScreen] frame];

    retVal.push_back((int32_t)(frame.origin.x - screen_rect.origin.x));
    retVal.push_back((int32_t)(screen_rect.size.height + screen_rect.origin.y - (frame.size.height + frame.origin.y)));

    return result_t();
}

result_t WebView::isActived(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = [(NSWindow*)m_window isKeyWindow];

    return result_t();
}

result_t WebView::active()
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    [(NSWindow*)m_window makeKeyAndOrderFront:nil];

    return result_t();
}

result_t WebView::get_focused(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = [(NSWindow*)m_window isKeyWindow];
    return result_t();
}

static exlib::string window_state(NSWindow* window)
{
    if ([window isMiniaturized])
        return "minimized";

    if ([window styleMask] & NSWindowStyleMaskFullScreen)
        return "fullscreen";

    if ([window isZoomed])
        return "maximized";

    return "normal";
}

result_t WebView::get_state(exlib::string& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = window_state((NSWindow*)m_window);
    return result_t();
}

result_t WebView::set_state(exlib::string newVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    NSWindow* window = (NSWindow*)m_window;
    exlib::string cur = window_state(window);

    if (newVal == "normal") {
        if (cur == "minimized")
            [window deminiaturize:nil];
        else if (cur == "fullscreen")
            [window toggleFullScreen:nil];
        else if (cur == "maximized")
            [window zoom:nil];
    } else if (newVal == "minimized") {
        if (cur != "minimized")
            [window miniaturize:nil];
    } else if (newVal == "maximized") {
        if (cur == "fullscreen")
            [window toggleFullScreen:nil];
        if ([window isMiniaturized])
            [window deminiaturize:nil];
        if (![window isZoomed])
            [window zoom:nil];
    } else if (newVal == "fullscreen") {
        if (cur != "fullscreen")
            [window toggleFullScreen:nil];
    } else
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("WebView: state must be 'normal', 'minimized', 'maximized' or 'fullscreen', but got '" + newVal + "'");

    return result_t();
}

result_t WebView::getBounds(obj_ptr<GetBoundsType>& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    NSRect frame = [(NSWindow*)m_window frame];
    NSRect screen_rect = [[NSScreen mainScreen] frame];

    obj_ptr<GetBoundsType> bounds = new GetBoundsType();
    bounds->x = frame.origin.x - screen_rect.origin.x;
    bounds->y = screen_rect.size.height + screen_rect.origin.y - (frame.size.height + frame.origin.y);
    bounds->width = frame.size.width;
    bounds->height = frame.size.height;

    retVal = bounds;
    return result_t();
}

result_t WebView::setBounds(WindowBounds* bounds)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    NSRect screen_rect = [[NSScreen mainScreen] frame];
    NSRect frame = [(NSWindow*)m_window frame];

    // Keep the caller's top edge when only the size changes: the native origin
    // is the bottom-left corner, so a taller frame moves it down.
    double top = screen_rect.size.height + screen_rect.origin.y - (frame.size.height + frame.origin.y);

    if (bounds->width.has_value())
        frame.size.width = bounds->width.value();
    if (bounds->height.has_value())
        frame.size.height = bounds->height.value();
    if (bounds->y.has_value())
        top = bounds->y.value();

    frame.origin.y = screen_rect.size.height + screen_rect.origin.y - (frame.size.height + top);

    if (bounds->x.has_value())
        frame.origin.x = screen_rect.origin.x + bounds->x.value();

    [(NSWindow*)m_window setFrame:frame display:YES animate:NO];
    return result_t();
}

result_t WebView::focus(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    NSWindow* window = (NSWindow*)m_window;

    [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
    [window makeKeyAndOrderFront:nil];

    retVal = [window isKeyWindow] == YES;
    return result_t();
}

result_t WebView::takeScreenshot(bool fullPage, obj_ptr<Buffer_base>& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    if (fullPage)
        return result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code("ERR_NOT_SUPPORTED").with_message("fullPage screenshot is not supported on macOS");

    struct Box {
        exlib::Event ev;
        exlib::string err;
        obj_ptr<Buffer> data;
    };
    auto box = std::make_shared<Box>();

    WKWebView* webView = (WKWebView*)m_webview;
    // Capture only the visible area without changing the frame
    [webView takeSnapshotWithConfiguration:nil
                         completionHandler:^(NSImage* snapshotImage, NSError* error) {
                             if (snapshotImage) {
                                 NSBitmapImageRep* rep = [[NSBitmapImageRep alloc] initWithData:[snapshotImage TIFFRepresentation]];
                                 NSData* data = [rep representationUsingType:NSBitmapImageFileTypePNG properties:@ {}];

                                 box->data = new Buffer([data bytes], [data length]);

                                 [rep release];
                             } else {
                                 box->err = [[error localizedDescription] UTF8String];
                             }
                             box->ev.set();
                         }];

    box->ev.wait();

    if (!box->err.empty())
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_GUI_ERROR").with_message(box->err);

    retVal = box->data;
    return result_t();
}

result_t WebView::postMessage(exlib::string msg)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    encoding_base::jsstr(msg, false, msg);
    exlib::string _jsCode = "window.dispatchEvent(new MessageEvent('message', { data: '" + msg + "' }))";

    NSString* jsCode = [NSString stringWithUTF8String:_jsCode.c_str()];
    [(WKWebView*)m_webview evaluateJavaScript:jsCode completionHandler:nil];

    return result_t();
}

}

#endif
