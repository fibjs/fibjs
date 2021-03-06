/*
 * config_win.mm
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>
#include "object.h"
#include "EventInfo.h"
#include "ifs/WebView.h"

NSRect s_screen_rect;

static fibjs::WebView_base* getWebViewFromNSWindow(NSWindow* win)
{
    return (fibjs::WebView_base*)objc_getAssociatedObject(win, "webview");
}

@interface GuiWindowDelegate : NSObject <NSWindowDelegate>
- (void)windowWillClose:(NSNotification*)willCloseNotification;
- (bool)windowShouldClose:(NSWindow*)window;
- (void)windowDidMove:(NSNotification*)didMoveNotification;
- (void)windowDidResize:(NSNotification*)notification;
@end

@implementation GuiWindowDelegate
- (void)windowWillClose:(NSNotification*)willCloseNotification
{
    NSWindow* currentWindow = willCloseNotification.object;
    fibjs::WebView_base* wv = getWebViewFromNSWindow(currentWindow);

    if (wv == NULL)
        return;

    wv->_emit("closed");
    wv->holder()->Unref();
    wv->Unref();
}

- (bool)windowShouldClose:(NSWindow*)window
{
    fibjs::WebView_base* wv = getWebViewFromNSWindow(window);

    if (wv == NULL)
        return YES;

    if(wv->close(nil))
    {
        [window setIsVisible:NO];
        return YES;
    }

    return NO;
}

- (void)windowDidMove:(NSNotification*)didMoveNotification
{
    NSWindow* currentWindow = didMoveNotification.object;
    fibjs::WebView_base* wv = getWebViewFromNSWindow(currentWindow);

    if (wv == NULL)
        return;

    fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(wv, "move");
    CGPoint wcoord = currentWindow.frame.origin;
    ei->add("left", (int32_t)wcoord.x);
    ei->add("top", (int32_t)(s_screen_rect.size.height - (currentWindow.frame.size.height + wcoord.y)));

    wv->_emit("move", ei);
}

- (void)windowDidResize:(NSNotification*)notification
{
    NSWindow* currentWindow = notification.object;
    fibjs::WebView_base* wv = getWebViewFromNSWindow(currentWindow);

    if (wv == NULL)
        return;

    fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(wv, "resize");
    CGSize ws = currentWindow.frame.size;
    ei->add("width", (int32_t)ws.width);
    ei->add("height", (int32_t)ws.height);

    wv->_emit("resize", ei);
}
@end

namespace fibjs {

#define CW_USEDEFAULT -1

void os_config_window(WebView_base* webview, void* _window, NObject* opt)
{
    NSWindow* window = (NSWindow*)_window;
    int32_t x = CW_USEDEFAULT;
    int32_t y = CW_USEDEFAULT;
    int32_t nWidth = CW_USEDEFAULT;
    int32_t nHeight = CW_USEDEFAULT;
    NSWindowStyleMask mask = NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
    bool _maximize = false;

    if (opt) {
        Variant v;

        if (opt->get("left", v) == 0)
            x = v.intVal();
        if (opt->get("top", v) == 0)
            y = v.intVal();
        if (opt->get("width", v) == 0)
            nWidth = v.intVal();
        if (opt->get("height", v) == 0)
            nHeight = v.intVal();

        if (!(opt->get("border", v) == 0 && !v.boolVal())) {
            if (!(opt->get("caption", v) == 0 && !v.boolVal()))
                mask |= NSWindowStyleMaskTitled;

            if (!(opt->get("resizable", v) == 0 && !v.boolVal()))
                mask |= NSResizableWindowMask;
        } else
            mask |= NSWindowStyleMaskBorderless | NSFullSizeContentViewWindowMask;

        if (opt->get("maximize", v) == 0 && v.boolVal())
            _maximize = true;
    } else
        mask |= NSResizableWindowMask | NSWindowStyleMaskTitled;

    s_screen_rect = [[NSScreen mainScreen] frame];

    if (nWidth == CW_USEDEFAULT)
        nWidth = s_screen_rect.size.width * 3 / 4;

    if (nHeight == CW_USEDEFAULT)
        nHeight = s_screen_rect.size.height * 3 / 4;

    if (x == CW_USEDEFAULT)
        x = (s_screen_rect.size.width - nWidth) / 2;

    if (y == CW_USEDEFAULT)
        y = (s_screen_rect.size.height - nHeight) / 2;
    y = s_screen_rect.size.height - (nHeight + y);

    window.styleMask = mask;

    if (_maximize)
        [window setFrame:[[NSScreen mainScreen] visibleFrame] display:YES];
    else
        [window setFrame:CGRectMake(x, y, nWidth, nHeight) display:YES];

    objc_setAssociatedObject(window, "webview", (id)webview, OBJC_ASSOCIATION_ASSIGN);
    [window setDelegate:[GuiWindowDelegate new]];

    webview->Ref();
    webview->_emit("open");
}
}