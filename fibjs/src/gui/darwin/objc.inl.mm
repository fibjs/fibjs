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

#import "lib.h"

/**
 * @see https://developer.apple.com/documentation/appkit/nsapplicationdelegate
 */
@interface __NSApplicationDelegate : NSObject/* , NSApplicationDelegate */
-(void)applicationWillTerminate:(id)app;
-(void)applicationDidFinishLaunching:(id)app;
-(void)applicationShouldTerminate:(id)app;
-(void)applicationShouldTerminateAfterLastWindowClosed:(id)app;
@end

@implementation __NSApplicationDelegate
-(void)applicationWillTerminate:(id)app
{
    printf("[webview_applicationWillTerminate] 看看 appDelegate 生效没 \n");
    return;
}
-(void)applicationDidFinishLaunching:(id)app
{
    printf("[webview_applicationDidFinishLaunching] 看看 appDelegate 生效没\n");

    // WebView* wv = WebView::getCurrentWebViewInstance();
    // if (wv)
    //     syncCall(
    //         wv->holder(),
    //         [](WebView* wv) {
    //             wv->_emit("load");

    //             return 0;
    //         },
    //         wv);
}
-(int)applicationShouldTerminate:(id)app
{
    printf("[webview_applicationShouldTerminate] 看看 appDelegate 生效没 \n");
    // NSTerminateNow = 1
    // NSTerminateLater = 2
    return 1;
}
-(bool)applicationShouldTerminateAfterLastWindowClosed:(id)app
{
    printf("[webview_applicationShouldTerminateAfterLastWindowClosed] 看看 appDelegate 生效没 \n");
    return false;
}
@end

/**
 * @see https://developer.apple.com/documentation/appkit/nswindowdelegate
 */
@interface __NSWindowDelegate : NSObject<NSWindowDelegate>
-(void)windowWillClose:(id)willCloseNotification;
-(void)windowDidMove:(id)didMoveNotification;
-(bool)windowShouldClose:(id)window;
@end

@implementation __NSWindowDelegate
-(void)windowWillClose:(id)willCloseNotification
{
    printf("[webview_windowWillClose] before \n");
    // struct webview* w = (struct webview*)objc_getAssociatedObject(self, "webview");

    // if (w != NULL)
    //     WebView::on_webview_say_close(w);

    // asyncCall(WebView::on_webview_say_close, w, CALL_E_GUICALL);
    printf("[webview_windowWillClose] after \n");
}
-(void)windowDidMove:(id)didMoveNotification
{
    struct webview* w = (struct webview*)objc_getAssociatedObject(self, "webview");
    if (w == NULL)
        return;

    // WebView* wv = getClsWebView(w);
    // if (wv == NULL)
    //     return;

    // TODO: use information in didMoveNotification
    printf("[onWindowDidMove]\n");

    // obj_ptr<EventInfo> ei = new EventInfo(wv, "move");
    // wv->_emit("move", ei);

    // wv->_emit("move");
}
// webview_windowShouldClose
-(bool)windowShouldClose:(id)window
{
    printf("[webview_windowShouldClose] 看看 winDelegate 生效没 \n");

    // id alert = objc_msgSend((id)objc_getClass("NSAlert"), sel_registerName("new"));
    // objc_msgSend(alert, sel_registerName("setAlertStyle:"), NSAlertStyleWarning);
    // objc_msgSend(alert, sel_registerName("setMessageText:"), get_nsstring("确定退出吗?"));
    // objc_msgSend(alert, sel_registerName("addButtonWithTitle:"), get_nsstring("退出"));
    // objc_msgSend(alert, sel_registerName("addButtonWithTitle:"), get_nsstring("取消"));

    // unsigned long result = (unsigned long)objc_msgSend(alert, sel_registerName("runModal"));
    // objc_msgSend(alert, sel_registerName("release"));

    // if (result != NSAlertFirstButtonReturn) {
    //     return NO;
    // }
    return YES;
}
@end

@interface __WKUIDelegate : NSObject<WKUIDelegate>
// run_open_panel
-(void)webView:(WKWebView *)webView 
runOpenPanelWithParameters:(WKOpenPanelParameters *)parameters 
initiatedByFrame:(WKFrameInfo *)frame 
completionHandler:(void (^)(NSArray<NSURL *> *URLs))completionHandler;

// run_alert_panel
- (void)webView:(WKWebView *)webView 
runJavaScriptAlertPanelWithMessage:(NSString *)message 
initiatedByFrame:(WKFrameInfo *)frame 
completionHandler:(void (^)(void))completionHandler;

// run_confirmation_panel
- (void)webView:(WKWebView *)webView 
runJavaScriptConfirmPanelWithMessage:(NSString *)message 
initiatedByFrame:(WKFrameInfo *)frame 
completionHandler:(void (^)(BOOL result))completionHandler;
@end

@implementation __WKUIDelegate
-(void)webView:(WKWebView *)webView 
runOpenPanelWithParameters:(WKOpenPanelParameters *)parameters 
initiatedByFrame:(WKFrameInfo *)frame 
completionHandler:(void (^)(NSArray<NSURL *> *URLs))completionHandler
{
    id openPanel = objc_msgSend((id)objc_getClass("NSOpenPanel"),
        sel_registerName("openPanel"));

    objc_msgSend(
        openPanel, sel_registerName("setAllowsMultipleSelection:"),
        objc_msgSend(parameters, sel_registerName("allowsMultipleSelection")));

    objc_msgSend(openPanel, sel_registerName("setCanChooseFiles:"), 1);
    objc_msgSend(
        openPanel, sel_registerName("beginWithCompletionHandler:"), ^(id result) {
            if (result == (id)NSModalResponseOK) {
                completionHandler(objc_msgSend(openPanel, sel_registerName("URLs")));
            } else {
                completionHandler(nil);
            }
        });
}

- (void)webView:(WKWebView *)webView 
runJavaScriptAlertPanelWithMessage:(NSString *)message 
initiatedByFrame:(WKFrameInfo *)frame 
completionHandler:(void (^)(void))completionHandler
{
    id alert = objc_msgSend((id)objc_getClass("NSAlert"), sel_registerName("new"));
    objc_msgSend(alert, sel_registerName("setIcon:"),
        objc_msgSend((id)objc_getClass("NSImage"),
            sel_registerName("imageNamed:"),
            get_nsstring("NSCaution")));
    objc_msgSend(alert, sel_registerName("setShowsHelp:"), 0);
    objc_msgSend(alert, sel_registerName("setInformativeText:"), message);
    objc_msgSend(alert, sel_registerName("addButtonWithTitle:"),
        get_nsstring("OK"));
    objc_msgSend(alert, sel_registerName("runModal"));
    objc_msgSend(alert, sel_registerName("release"));
    completionHandler();
}

- (void)webView:(WKWebView *)webView 
runJavaScriptConfirmPanelWithMessage:(NSString *)message 
initiatedByFrame:(WKFrameInfo *)frame 
completionHandler:(void (^)(BOOL result))completionHandler;
{
    id alert = objc_msgSend((id)objc_getClass("NSAlert"), sel_registerName("new"));
    objc_msgSend(alert, sel_registerName("setIcon:"),
        objc_msgSend((id)objc_getClass("NSImage"),
            sel_registerName("imageNamed:"),
            get_nsstring("NSCaution")));
    objc_msgSend(alert, sel_registerName("setShowsHelp:"), 0);
    objc_msgSend(alert, sel_registerName("setInformativeText:"), message);
    objc_msgSend(alert, sel_registerName("addButtonWithTitle:"),
        get_nsstring("OK"));
    objc_msgSend(alert, sel_registerName("addButtonWithTitle:"),
        get_nsstring("Cancel"));
    if (objc_msgSend(alert, sel_registerName("runModal")) == (id)NSAlertFirstButtonReturn) {
        completionHandler(true);
    } else {
        completionHandler(false);
    }
    objc_msgSend(alert, sel_registerName("release"));
}
@end

#endif // __APPLE__