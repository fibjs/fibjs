/*
 * gui_mac.mm
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#include "gui_handler.h"
#import <Cocoa/Cocoa.h>
#include "include/cef_application_mac.h"
#include "include/cef_browser.h"
#include "include/cef_app.h"
#include "gui_handler.h"

@interface GuiAppDelegate : NSObject <NSApplicationDelegate> {
@private
    bool with_chrome_runtime_;
}

- (id)initWithChromeRuntime:(bool)with_chrome_runtime;
- (void)createApplication:(id)object;
- (void)tryToTerminateApplication:(NSApplication*)app;
@end

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

- (void)terminate:(id)sender
{
    GuiAppDelegate* delegate = static_cast<GuiAppDelegate*>([NSApp delegate]);
    [delegate tryToTerminateApplication:self];
}
@end

@implementation GuiAppDelegate

- (id)initWithChromeRuntime:(bool)with_chrome_runtime
{
    if (self = [super init]) {
        with_chrome_runtime_ = with_chrome_runtime;
    }
    return self;
}

- (void)createApplication:(id)object
{
    if (!with_chrome_runtime_) {

        [[NSBundle mainBundle] loadNibNamed:@"MainMenu"
                                      owner:NSApp
                            topLevelObjects:nil];
    }

    [[NSApplication sharedApplication] setDelegate:self];
}

- (void)tryToTerminateApplication:(NSApplication*)app
{
    // fibjs::GuiHandler* handler = fibjs::GuiHandler::GetInstance();
    // if (handler && !handler->IsClosing())
    //     handler->CloseAllBrowsers(false);
}

- (NSApplicationTerminateReply)applicationShouldTerminate:
    (NSApplication*)sender
{
    return NSTerminateNow;
}
@end

namespace fibjs {

void MacRunMessageLoop(const CefMainArgs& args, const CefSettings& settings, CefRefPtr<CefApp> application)
{
    @autoreleasepool {
        [GuiApplication sharedApplication];

        CefInitialize(args, settings, application, nullptr);

        NSObject* delegate = [[GuiAppDelegate alloc] initWithChromeRuntime:false];
        [delegate performSelectorOnMainThread:@selector(createApplication:)
                                   withObject:nil
                                waitUntilDone:NO];

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