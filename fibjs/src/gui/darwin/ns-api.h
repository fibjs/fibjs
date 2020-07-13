/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-12 04:26:38
 * @modify date 2020-06-12 04:26:38

 * @desc 
 */
#if defined(__APPLE__) && !defined(FIBJS_DISABLE_GUI)

#ifndef WEBVIEW_NS_API_H
#define WEBVIEW_NS_API_H

#import <Webkit/Webkit.h>
#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>
#import <objc/message.h>

#import "darwin.h"

static exlib::string NSStringToExString(NSString* str) {
      return exlib::string((const char*)([str UTF8String]));
}

@interface FibjsNSWindow : NSWindow
@end

/**
 * @see https://developer.apple.com/documentation/appkit/nsapplicationdelegate
 */
@interface __NSApplicationDelegate : NSObject<NSApplicationDelegate>
- (bool)applicationShouldTerminate:(id)app;
- (bool)applicationShouldTerminateAfterLastWindowClosed:(id)app;
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

// @interface __WKWindowFeatures : NSObject<WKWindowFeatures>
// @end

@interface __WKNavigationDelegate : NSObject<WKNavigationDelegate>
@end

@interface __WKScriptMessageHandler : NSObject<WKScriptMessageHandler>
- (void)userContentController:(WKUserContentController *)userContentController 
      didReceiveScriptMessage:(WKScriptMessage *)message;
@end

@interface __WKDownloadDelegate : NSObject/* <NSURLDownloadDelegate> */
@end

@interface WVViewController: NSViewController
- (void)loadView;
@end

@interface FileSystemWKURLSchemeHandler: NSObject<WKURLSchemeHandler>
- (void)webView:(WKWebView *)webView 
      startURLSchemeTask:(id<WKURLSchemeTask>)urlSchemeTask;
// - (void)webView:(WKWebView *)webView 
//       stopURLSchemeTask:(id<WKURLSchemeTask>)urlSchemeTask;
@end

#endif // WEBVIEW_NS_API_H

#endif // __APPLE__
