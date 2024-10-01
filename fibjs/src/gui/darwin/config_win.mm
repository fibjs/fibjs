/*
 * config_win.mm
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#if defined(OS_DESKTOP)

#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>

#include "object.h"
#include "EventInfo.h"
#include "WebView.h"

static fibjs::WebView* getWebViewFromNSWindow(NSWindow* win)
{
    return (fibjs::WebView*)objc_getAssociatedObject(win, "webview");
}

@interface GuiWindowDelegate : NSObject <NSWindowDelegate>
- (void)windowWillClose:(NSNotification*)willCloseNotification;
- (void)windowDidMove:(NSNotification*)didMoveNotification;
- (void)windowDidResize:(NSNotification*)notification;
@end

@implementation GuiWindowDelegate
- (void)windowWillClose:(NSNotification*)willCloseNotification
{
    NSWindow* currentWindow = willCloseNotification.object;
    fibjs::WebView* webview = getWebViewFromNSWindow(currentWindow);

    if (webview == NULL)
        return;

    webview->release();
}

- (void)windowDidMove:(NSNotification*)didMoveNotification
{
    NSWindow* currentWindow = didMoveNotification.object;
    fibjs::WebView* wv = getWebViewFromNSWindow(currentWindow);

    if (wv == NULL)
        return;

    CGPoint wcoord = currentWindow.frame.origin;
    CGSize ws = currentWindow.frame.size;
    NSRect screen_rect = [[NSScreen mainScreen] frame];

    fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(wv, "move");
    ei->add("left", (int32_t)(wcoord.x - screen_rect.origin.x));
    ei->add("top", (int32_t)(screen_rect.size.height + screen_rect.origin.y - (ws.height + wcoord.y)));

    wv->_emit("move", ei);
}

- (void)windowDidResize:(NSNotification*)notification
{
    NSWindow* currentWindow = notification.object;
    fibjs::WebView* wv = getWebViewFromNSWindow(currentWindow);

    if (wv == NULL)
        return;

    CGSize ws = currentWindow.frame.size;

    fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(wv, "resize");
    ei->add("width", (int32_t)ws.width);
    ei->add("height", (int32_t)ws.height);

    wv->_emit("resize", ei);
}
@end

namespace fibjs {

static dispatch_queue_t mainQueue = dispatch_get_main_queue();
void putGuiPool(AsyncEvent* ac)
{
    dispatch_async(mainQueue, ^{ ac->invoke(); });
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

void WebView::run_os_gui(exlib::Event& gui_ready)
{
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        [app finishLaunching];
        [app activateIgnoringOtherApps:YES];

        gui_ready.set();

        [app run];
    }
}

void WebView::internal_close()
{
    NSWindow* window = (NSWindow*)m_window;
    [window close];
}

#define CW_USEDEFAULT -1
void WebView::config()
{
    NSWindow* window = (NSWindow*)m_window;
    int32_t x = CW_USEDEFAULT;
    int32_t y = CW_USEDEFAULT;
    int32_t nWidth = CW_USEDEFAULT;
    int32_t nHeight = CW_USEDEFAULT;
    NSWindowStyleMask mask = NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
    bool _maximize = false;
    bool _fullscreen = false;

    if (m_opt) {
        Variant v;

        if (m_opt->get("left", v) == 0)
            x = v.intVal();
        if (m_opt->get("top", v) == 0)
            y = v.intVal();
        if (m_opt->get("width", v) == 0)
            nWidth = v.intVal();
        if (m_opt->get("height", v) == 0)
            nHeight = v.intVal();

        if (!(m_opt->get("frame", v) == 0 && !v.boolVal())) {
            if (!(m_opt->get("caption", v) == 0 && !v.boolVal()))
                mask |= NSWindowStyleMaskTitled;

            if (!(m_opt->get("resizable", v) == 0 && !v.boolVal()))
                mask |= NSWindowStyleMaskResizable;
        } else
            mask |= NSWindowStyleMaskBorderless | NSWindowStyleMaskFullSizeContentView;

        if (m_opt->get("maximize", v) == 0 && v.boolVal())
            _maximize = true;

        if (m_opt->get("fullscreen", v) == 0 && v.boolVal()) {
            mask = NSWindowStyleMaskResizable;
            _fullscreen = true;
        }
    } else
        mask |= NSWindowStyleMaskResizable | NSWindowStyleMaskTitled;

    NSRect screen_rect = [[NSScreen mainScreen] frame];

    if (nWidth == CW_USEDEFAULT)
        nWidth = screen_rect.size.width * 3 / 4;

    if (nHeight == CW_USEDEFAULT)
        nHeight = screen_rect.size.height * 3 / 4;

    if (x == CW_USEDEFAULT)
        x = (screen_rect.size.width - nWidth) / 2;

    if (y == CW_USEDEFAULT)
        y = (screen_rect.size.height - nHeight) / 2;

    x = screen_rect.origin.x + x;
    y = screen_rect.size.height + screen_rect.origin.y - (nHeight + y);

    window.styleMask = mask;

    if (_fullscreen) {
        [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
        [window toggleFullScreen:nil];
    } else if (_maximize)
        [window setFrame:[[NSScreen mainScreen] visibleFrame] display:YES];
    else
        [window setFrame:CGRectMake(x, y, nWidth, nHeight) display:YES];

    objc_setAssociatedObject(window, "webview", (id)this, OBJC_ASSOCIATION_ASSIGN);
    [window setDelegate:[GuiWindowDelegate new]];

    [window makeKeyAndOrderFront:window];
    [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];

    Ref();
    m_ready->set();
    _emit("open");
}
}

#endif
