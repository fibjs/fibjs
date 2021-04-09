/*
 * gui_mac.mm
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#import <Cocoa/Cocoa.h>
#include "include/cef_application_mac.h"
#include "include/cef_browser.h"
#include "include/cef_app.h"
#include "gui_handler.h"
#include "CefWebView.h"
#include "../os_gui.h"

@interface GuiApplication : NSApplication <CefAppProtocol> {
@private
    BOOL handlingSendEvent_;
}
@end

@implementation GuiApplication
- (BOOL)isHandlingSendEvent
{
    return handlingSendEvent_;
}

- (void)setHandlingSendEvent:(BOOL)handlingSendEvent
{
    handlingSendEvent_ = handlingSendEvent;
}

- (void)sendEvent:(NSEvent*)event
{
    CefScopedSendingEvent sendingEventScoper;
    [super sendEvent:event];
}
@end

namespace fibjs {
void CefWebView::config_window()
{
    NSView* view = CAST_CEF_WINDOW_HANDLE_TO_NSVIEW(m_browser->GetHost()->GetWindowHandle());
    NSWindow* window = [view window];

    os_config_window(this, window, m_opt);
}

result_t CefWebView::close(AsyncEvent* ac)
{
    if (ac && ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (!m_browser)
        return 0;

    NSView* view = CAST_CEF_WINDOW_HANDLE_TO_NSVIEW(m_browser->GetHost()->GetWindowHandle());
    if (view) {
        NSWindow* window = [view window];

        if (!(window.styleMask & NSWindowStyleMaskTitled)) {
            [window close];
            return 0;
        }
    }

    return m_browser->GetHost()->TryCloseBrowser();
}

void MacRunMessageLoop(const CefMainArgs& args, const CefSettings& settings, CefRefPtr<CefApp> application)
{
    @autoreleasepool {
        [GuiApplication sharedApplication];
        [[GuiApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyRegular];
        [[GuiApplication sharedApplication] finishLaunching];

        CefInitialize(args, settings, application, nullptr);
        CefRunMessageLoop();
    }
}

void GuiHandler::PlatformTitleChange(CefRefPtr<CefBrowser> browser,
    const CefString& title)
{
    NSView* view = CAST_CEF_WINDOW_HANDLE_TO_NSVIEW(browser->GetHost()->GetWindowHandle());
    NSWindow* window = [view window];
    std::string titleStr(title);
    NSString* str = [NSString stringWithUTF8String:titleStr.c_str()];
    [window setTitle:str];
}
}