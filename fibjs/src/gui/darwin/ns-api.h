/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-12 04:26:38
 * @modify date 2020-06-12 04:26:38

 * @desc 
 */
#ifdef __APPLE__

#import <Webkit/Webkit.h>
#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>
#import <objc/message.h>

#import "darwin.h"

/**
 * @see https://developer.apple.com/documentation/appkit/nsapplicationdelegate
 */
@interface __NSApplicationDelegate : NSObject<NSApplicationDelegate>
- (void)applicationWillTerminate:(id)app;
- (void)applicationDidFinishLaunching:(id)app;
- (void)applicationShouldTerminate:(id)app;
- (void)applicationShouldTerminateAfterLastWindowClosed:(id)app;
@end

/**
 * @see https://developer.apple.com/documentation/appkit/nswindowdelegate
 */
@interface __NSWindowDelegate : NSObject<NSWindowDelegate>
- (void)windowWillClose:(NSNotification *)willCloseNotification;
- (bool)windowShouldClose:(NSWindow *)window;
- (void)windowDidMove:(NSNotification *)didMoveNotification;
- (void)windowWillStartLiveResize:(NSNotification *)notification;
- (void)windowDidEndLiveResize:(NSNotification *)notification;
- (void)windowDidResize:(NSNotification *)notification;
@end

@interface __WKUIDelegate : NSObject<WKUIDelegate>
@end

@interface __WKNavigationDelegate : NSObject<WKNavigationDelegate>
@end

@interface __WKScriptMessageHandler : NSObject<WKScriptMessageHandler>
- (void)userContentController:(WKUserContentController *)userContentController 
      didReceiveScriptMessage:(WKScriptMessage *)message;
@end

// @interface __WKPreferences : NSObject<WKPreferences>
// @end

@interface __WKDownloadDelegate : NSObject/* <NSURLDownloadDelegate> */
@end

#endif // __APPLE__