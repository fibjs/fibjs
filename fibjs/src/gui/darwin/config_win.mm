/*
 * config_win.mm
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#ifndef iPhone

#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>

#include "object.h"
#include "../gui.h"
#include "options.h"
#include "EventInfo.h"
#include "WebView.h"
#import <WebKit/WebKit.h>

int32_t s_window_count = 0;

static fibjs::WebView* getWebViewFromNSWindow(NSWindow* win)
{
    return (fibjs::WebView*)objc_getAssociatedObject(win, "webview");
}

@interface GuiWindowDelegate : NSObject <NSWindowDelegate>
- (BOOL)windowShouldClose:(id)sender;
- (void)windowWillClose:(NSNotification*)willCloseNotification;
- (void)windowDidMove:(NSNotification*)didMoveNotification;
- (void)windowDidResize:(NSNotification*)notification;
- (void)windowDidBecomeMain:(NSNotification*)notification;
- (void)windowDidResignMain:(NSNotification*)notification;
- (void)windowDidMiniaturize:(NSNotification*)notification;
- (void)windowDidDeminiaturize:(NSNotification*)notification;
- (void)windowDidEnterFullScreen:(NSNotification*)notification;
- (void)windowDidExitFullScreen:(NSNotification*)notification;
@end

@implementation GuiWindowDelegate

- (BOOL)windowShouldClose:(id)sender
{
    NSWindow* currentWindow = (NSWindow*)sender;
    fibjs::WebView* webview = getWebViewFromNSWindow(currentWindow);

    if (webview == NULL)
        return YES;

    if (webview->m_options->hideOnClose.value()) {
        if ([currentWindow isVisible]) {
            [currentWindow orderOut:nil];
            if (--s_window_count == 0)
                [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyAccessory];
        }

        return NO;
    }

    // A close-requested listener may cancel; a programmatic close sets the
    // allow flag first and is never consulted.
    return webview->should_cancel_close() ? NO : YES;
}

- (void)windowWillClose:(NSNotification*)willCloseNotification
{
    NSWindow* currentWindow = willCloseNotification.object;
    fibjs::WebView* webview = getWebViewFromNSWindow(currentWindow);

    if (webview == NULL)
        return;

    WKWebView* webView = (WKWebView*)webview->m_webview;
    [webView stopLoading];
    [webView removeFromSuperview];
    webView.navigationDelegate = nil;

    webview->release();

    if ([currentWindow isVisible]) {
        if (--s_window_count == 0)
            [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyAccessory];
    }
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

- (void)windowDidBecomeMain:(NSNotification*)notification
{
    NSWindow* currentWindow = notification.object;
    fibjs::WebView* wv = getWebViewFromNSWindow(currentWindow);

    if (wv == NULL)
        return;

    if (wv->m_options->menu.has_value()) {
        NSMenu* menu = (NSMenu*)wv->m_options->menu.value()->create_os_menu(false);
        if (menu) {
            [[NSApplication sharedApplication] setMainMenu:menu];
        } else {
            NSLog(@"Failed to create menu");
        }
    }

    if (wv->m_icon) {
        NSImage* icon = [[NSImage alloc]
            initWithData:[NSData dataWithBytes:wv->m_icon->data() length:wv->m_icon->length()]];
        if (icon) {
            [[NSApplication sharedApplication] setApplicationIconImage:icon];
            [icon release];
        }
    }

    fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(wv, "focus");
    wv->_emit("focus", ei);
}

- (void)windowDidResignMain:(NSNotification*)notification
{
    NSWindow* currentWindow = notification.object;
    fibjs::WebView* wv = getWebViewFromNSWindow(currentWindow);

    if (wv == NULL)
        return;

    fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(wv, "blur");
    wv->_emit("blur", ei);
}

- (void)windowDidMiniaturize:(NSNotification*)notification
{
    fibjs::WebView* wv = getWebViewFromNSWindow(notification.object);
    if (wv == NULL)
        return;

    fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(wv, "minimize");
    wv->_emit("minimize", ei);
}

- (void)windowDidDeminiaturize:(NSNotification*)notification
{
    fibjs::WebView* wv = getWebViewFromNSWindow(notification.object);
    if (wv == NULL)
        return;

    fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(wv, "restore");
    wv->_emit("restore", ei);
}

- (void)windowDidEnterFullScreen:(NSNotification*)notification
{
    fibjs::WebView* wv = getWebViewFromNSWindow(notification.object);
    if (wv == NULL)
        return;

    fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(wv, "enter-fullscreen");
    wv->_emit("enter-fullscreen", ei);
}

- (void)windowDidExitFullScreen:(NSNotification*)notification
{
    fibjs::WebView* wv = getWebViewFromNSWindow(notification.object);
    if (wv == NULL)
        return;

    fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(wv, "leave-fullscreen");
    wv->_emit("leave-fullscreen", ei);
}
@end

@interface GuiApplication : NSApplication
@end

@implementation GuiApplication

- (void)sendEvent:(NSEvent*)event
{
    if (event.type == NSKeyDown) {
        NSString* inputKey = [event.charactersIgnoringModifiers lowercaseString];
        if ((event.modifierFlags & NSDeviceIndependentModifierFlagsMask) == NSCommandKeyMask
            || (event.modifierFlags & NSDeviceIndependentModifierFlagsMask) == (NSCommandKeyMask | NSAlphaShiftKeyMask)) {
            if ([inputKey isEqualToString:@"x"]) {
                if ([self sendAction:@selector(cut:) to:nil from:self])
                    return;
            } else if ([inputKey isEqualToString:@"c"]) {
                if ([self sendAction:@selector(copy:) to:nil from:self])
                    return;
            } else if ([inputKey isEqualToString:@"v"]) {
                if ([self sendAction:@selector(paste:) to:nil from:self])
                    return;
            } else if ([inputKey isEqualToString:@"a"]) {
                if ([self sendAction:@selector(selectAll:) to:nil from:self])
                    return;
            } else if ([inputKey isEqualToString:@"z"]) {
                if ([self sendAction:NSSelectorFromString(@"undo:") to:nil from:self])
                    return;
            }
        } else if ((event.modifierFlags & NSDeviceIndependentModifierFlagsMask) == (NSCommandKeyMask | NSShiftKeyMask)
            || (event.modifierFlags & NSDeviceIndependentModifierFlagsMask) == (NSCommandKeyMask | NSShiftKeyMask | NSAlphaShiftKeyMask)) {
            if ([inputKey isEqualToString:@"z"]) {
                if ([self sendAction:NSSelectorFromString(@"redo:") to:nil from:self])
                    return;
            }
        }
    }
    [super sendEvent:event];
}

@end

namespace fibjs {

static CFRunLoopSourceRef s_host_source = NULL;

// AppKit owns the host thread only once fibjs itself uses the GUI. Until then
// the host runs a bare CFRunLoop, which leaves NSApplication free for a native
// addon that embeds its own UI (it needs to install its own subclass and ivars;
// it could never take over an instance we already created).
static bool s_app_loop = false;
static bool s_app_launched = false;

static void host_source_perform(void* info)
{
    // Initialize AppKit before draining, so the first window is created against a
    // launched application. The switch to [NSApp run] itself happens once this
    // callback returns (CFRunLoopStop below), never in the middle of JS.
    if (!s_app_loop && gui_has_pending())
        ensure_os_gui_initialized();

    gui_dispatch_pending();

    // JS can queue GUI work during the drain (gui.open() from a fiber).
    if (!s_app_loop && gui_has_pending())
        ensure_os_gui_initialized();

    // A native addon may have created its own NSApplication (GPUIApplication).
    // Now that the current JS turn is over, run the AppKit loop it expects, so
    // system events reach its windows. This never creates an application:
    // ensure_os_gui_initialized() only borrows the existing one.
    bool more = g_main_js_service && g_main_js_service->runPending(64);

    if (!s_app_loop && NSApp != nil)
        ensure_os_gui_initialized();

    if (more)
        CFRunLoopSourceSignal(s_host_source);
}

void os_prepare_host()
{
    // Only the wakeup source is created here. AppKit is started on demand (see
    // ensure_os_gui_initialized), which lets a native addon claim NSApplication
    // first and keeps GUI-less scripts from paying for AppKit at all.
    CFRunLoopSourceContext ctx = {};
    ctx.perform = host_source_perform;

    s_host_source = CFRunLoopSourceCreate(kCFAllocatorDefault, 0, &ctx);
    CFRunLoopAddSource(CFRunLoopGetMain(), s_host_source, kCFRunLoopCommonModes);
}

void os_wake_host()
{
    if (s_host_source) {
        CFRunLoopSourceSignal(s_host_source);
        CFRunLoopWakeUp(CFRunLoopGetMain());
    }
}

void os_run_host()
{
    @autoreleasepool {
        // Bare run loop until fibjs needs its own GUI. ensure_os_gui_initialized()
        // stops it and sets s_app_loop on the first GUI event or inline GUI call.
        while (!s_app_loop)
            CFRunLoopRun();

        // The switching callback consumed its own signal; re-arm so the queued
        // GUI event is drained inside the AppKit loop.
        os_wake_host();

        [NSApp run];
    }
}

void ensure_os_gui_initialized()
{
    if (!s_app_loop) {
        s_app_loop = true;

        // os_run_host() is blocked in CFRunLoopRun(): stop it so it can switch
        // to [NSApp run]. Harmless when the app loop is already the host.
        CFRunLoopStop(CFRunLoopGetMain());
    }

    if (s_app_launched)
        return;

    s_app_launched = true;

    // NSApp already exists: a native addon (GPUIApplication, for example) or
    // another library created it. It owns its own launching/initialization, so
    // only borrow the instance to run the host loop.
    if (NSApp != nil)
        return;

    GuiApplication* app = [GuiApplication sharedApplication];

    // Accessory keeps a pure CLI run free of a Dock icon; opening a window
    // switches to Regular (WebView.mm) and closing the last one restores this.
    [app setActivationPolicy:NSApplicationActivationPolicyAccessory];
    [app finishLaunching];
}

id fetchEventFromNSRunLoop(int blocking)
{
    id until = blocking ? [NSDate distantFuture] : [NSDate distantPast];

    return [[GuiApplication sharedApplication]
        nextEventMatchingMask:NSEventMaskAny
                    untilDate:until
                       inMode:@"kCFRunLoopDefaultMode"
                      dequeue:YES];
}

// Graceful quit: asks the application to terminate, which closes the windows
// and returns from [NSApp run]; the process then leaves through the normal
// shutdown path.
void os_quit()
{
    if (NSApp != nil)
        [NSApp terminate:nil];
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

    if (m_options->left.has_value())
        x = m_options->left.value();
    if (m_options->top.has_value())
        y = m_options->top.value();
    if (m_options->width.has_value())
        nWidth = m_options->width.value();
    if (m_options->height.has_value())
        nHeight = m_options->height.value();

    if (m_options->fullscreen.value())
        mask = NSWindowStyleMaskResizable;
    else {
        if (m_options->frame.value()) {
            obj_ptr<TitlebarOptions> titlebar_opt = std::get<obj_ptr<TitlebarOptions>>(m_options->titlebar.value());
            exlib::string& titlebar_style = titlebar_opt->style.value();

            if (titlebar_style != "hide")
                mask |= NSWindowStyleMaskTitled;

            if (titlebar_style == "transparent") {
                mask |= NSFullSizeContentViewWindowMask;
                window.titlebarAppearsTransparent = true;
            }

            if (titlebar_opt->height.value() == "tall") {
                NSToolbar* toolbar = [[NSToolbar alloc] initWithIdentifier:@"MainToolbar"];
                toolbar.showsBaselineSeparator = NO;
                window.toolbar = toolbar;
                [window setToolbarStyle:NSWindowToolbarStyleUnified];
            }

            if (m_options->resizable.value())
                mask |= NSWindowStyleMaskResizable;

        } else
            mask |= NSWindowStyleMaskBorderless | NSWindowStyleMaskFullSizeContentView;
    }

    if ((mask & NSWindowStyleMaskTitled) == 0) {
        mask |= NSWindowStyleMaskTitled | NSFullSizeContentViewWindowMask;

        window.titlebarAppearsTransparent = true;
        window.titleVisibility = NSWindowTitleHidden;

        [[window standardWindowButton:NSWindowCloseButton] setHidden:YES];
        [[window standardWindowButton:NSWindowMiniaturizeButton] setHidden:YES];
        [[window standardWindowButton:NSWindowZoomButton] setHidden:YES];
        [[window standardWindowButton:NSWindowFullScreenButton] setHidden:YES];
    }

    NSRect screen_rect = [[NSScreen mainScreen] frame];

    if (nWidth == CW_USEDEFAULT)
        nWidth = screen_rect.size.width * 3 / 4;

    if (nHeight == CW_USEDEFAULT)
        nHeight = screen_rect.size.height * 3 / 4;

    if (m_options->minWidth.has_value())
        nWidth = std::max(nWidth, m_options->minWidth.value());

    if (m_options->minHeight.has_value())
        nHeight = std::max(nHeight, m_options->minHeight.value());

    if (m_options->maxWidth.has_value())
        nWidth = std::min(nWidth, m_options->maxWidth.value());

    if (m_options->maxHeight.has_value())
        nHeight = std::min(nHeight, m_options->maxHeight.value());

    if (x == CW_USEDEFAULT)
        x = (screen_rect.size.width - nWidth) / 2;

    if (y == CW_USEDEFAULT)
        y = (screen_rect.size.height - nHeight) / 2;

    x = screen_rect.origin.x + x;
    y = screen_rect.size.height + screen_rect.origin.y - (nHeight + y);

    if (m_options->visible.value()) {
        if (++s_window_count == 1)
            [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyRegular];
    }

    window.styleMask = mask;

    [window setContentMinSize:NSMakeSize(m_options->minWidth.value(), m_options->minHeight.value())];
    [window setContentMaxSize:NSMakeSize(m_options->maxWidth.value_or(__INT32_MAX__), m_options->maxHeight.value_or(__INT32_MAX__))];

    if (m_options->fullscreen.value()) {
        [window setFrame:[[NSScreen mainScreen] visibleFrame] display:YES];
        [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
        [window toggleFullScreen:nil];
    } else if (m_options->maximize.value())
        [window setFrame:[[NSScreen mainScreen] visibleFrame] display:YES];
    else
        [window setFrame:CGRectMake(x, y, nWidth, nHeight) display:YES];

    objc_setAssociatedObject(window, "webview", (id)this, OBJC_ASSOCIATION_ASSIGN);
    [window setDelegate:[GuiWindowDelegate new]];

    if (m_options->menu.has_value()) {
        NSMenu* menu = (NSMenu*)m_options->menu.value()->create_os_menu(false);
        if (menu) {
            if (m_options->visible.value())
                [[NSApplication sharedApplication] setMainMenu:menu];
        } else {
            NSLog(@"Failed to create menu");
        }
    }

    if (m_options->visible.value()) {
        [window makeKeyAndOrderFront:window];
        [[GuiApplication sharedApplication] activateIgnoringOtherApps:YES];
    }

    if (m_icon) {
        NSImage* icon = [[NSImage alloc]
            initWithData:[NSData dataWithBytes:m_icon->data() length:m_icon->length()]];
        if (icon) {
            if (m_options->visible.value())
                [[NSApplication sharedApplication] setApplicationIconImage:icon];
            [icon release];
        }
    }

    m_ready->set();
}
}

#endif
