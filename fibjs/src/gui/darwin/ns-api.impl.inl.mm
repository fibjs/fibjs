/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-12 04:25:25
 * @modify date 2020-06-12 04:25:25
 * @desc WebView Implementation in OSX
 */

#if defined(__APPLE__) && !defined(FIBJS_DISABLE_GUI)

#import "ns-api.h"

#import "WebView.h"

#include "ifs/fs.h"
#include "path.h"
#include "Buffer.h"

using fibjs::obj_ptr;
using fibjs::EventInfo;
using fibjs::fs_base;
using fibjs::path_base;
using fibjs::result_t;
using fibjs::Buffer_base;

void assignWinSizeInfoToResizeAboutEventInfo (CGSize ws, EventInfo* ei) {
    ei->add("width", ws.width);
    ei->add("height", ws.height);
}

@implementation __NSApplicationDelegate
// -(void)applicationWillTerminate:(id)app
// {
// }
// -(void)applicationDidFinishLaunching:(id)app
// {
// }
-(bool)applicationShouldTerminate:(id)app
{
    return NO;
}
-(bool)applicationShouldTerminateAfterLastWindowClosed:(id)app
{
    return NO;
}
@end

@implementation __NSWindowDelegate
-(void)windowWillClose:(NSNotification *)willCloseNotification
{
    NSWindow *currentWindow = willCloseNotification.object;
    fibjs::WebView* wv = fibjs::WebView::getWebViewFromNSWindow(currentWindow);

    if (wv != NULL)
        wv->onNSWindowWillClose();
}
-(bool)windowShouldClose:(NSWindow *)window
{
    fibjs::WebView* wv = fibjs::WebView::getWebViewFromNSWindow(window);

    if (wv == NULL)
        return YES;

    return wv->onNSWindowShouldClose();
}
-(void)windowDidMove:(NSNotification *)didMoveNotification
{
    NSWindow *currentWindow = didMoveNotification.object;
    fibjs::WebView* wv = fibjs::WebView::getWebViewFromNSWindow(currentWindow);

    if (wv == NULL)
        return;

    obj_ptr<EventInfo> ei = new EventInfo(wv, "move");
    CGPoint wcoord = currentWindow.frame.origin;
    ei->add("x", wcoord.x);
    ei->add("y", wcoord.y);

    wv->_emit("move", ei);
}
-(void)windowWillStartLiveResize:(NSNotification *)notification
{
    NSWindow *currentWindow = notification.object;
    fibjs::WebView* wv = fibjs::WebView::getWebViewFromNSWindow(currentWindow);

    if (wv == NULL)
        return ;

    obj_ptr<EventInfo> ei = new EventInfo(wv, "resizestart");
    assignWinSizeInfoToResizeAboutEventInfo(currentWindow.frame.size, ei);
    wv->_emit("resizestart", ei);
}
-(void)windowDidEndLiveResize:(NSNotification *)notification
{
    NSWindow *currentWindow = notification.object;
    fibjs::WebView* wv = fibjs::WebView::getWebViewFromNSWindow(currentWindow);

    if (wv == NULL)
        return ;

    obj_ptr<EventInfo> ei = new EventInfo(wv, "resizeend");
    assignWinSizeInfoToResizeAboutEventInfo(currentWindow.frame.size, ei);
    wv->_emit("resizeend", ei);
}
-(void)windowDidResize:(NSNotification *)notification
{
    NSWindow *currentWindow = notification.object;
    fibjs::WebView* wv = fibjs::WebView::getWebViewFromNSWindow(currentWindow);

    if (wv == NULL)
        return ;

    obj_ptr<EventInfo> ei = new EventInfo(wv, "resize");
    assignWinSizeInfoToResizeAboutEventInfo(currentWindow.frame.size, ei);

    wv->_emit("resize", ei);
}
@end

@implementation __WKScriptMessageHandler
- (void)userContentController:(WKUserContentController *)userContentController
      didReceiveScriptMessage:(WKScriptMessage *)message
{
    fibjs::WebView* wv = fibjs::WebView::getWebViewFromWKUserContentController(userContentController);

    if (wv == NULL)
        return;

    const char* wkScriptName = [[message name] UTF8String];

    if (!strcmp(wkScriptName, WEBVIEW_MSG_HANDLER_NAME_INVOKE)) {
        wv->onWKWebViewPostMessage(message);
    } else if (!strcmp(wkScriptName, WEBVIEW_MSG_HANDLER_NAME_INWARD)) {
        wv->onWKWebViewInwardMessage(message);
    } else if (!strcmp(wkScriptName, WEBVIEW_MSG_HANDLER_NAME_EXTERNALLOG)) {
        wv->onWKWebViewExternalLogMessage(message);
    }
}
@end

@implementation __WKUIDelegate
// run_open_panel
-(void)webView:(WKWebView *)webView
runOpenPanelWithParameters:(WKOpenPanelParameters *)parameters
initiatedByFrame:(WKFrameInfo *)frame
completionHandler:(void (^)(NSArray<NSURL *> *URLs))completionHandler
{
    NSOpenPanel* openPanel = [NSOpenPanel openPanel];
    [openPanel setAllowsMultipleSelection:[parameters allowsMultipleSelection]];
    [openPanel setCanChooseFiles:1];
    [openPanel
        beginWithCompletionHandler:^(NSInteger result) {
            if (result == NSModalResponseOK) {
                completionHandler([openPanel URLs]);
            } else {
                completionHandler(nil);
            }
        }];
}

// run_alert_panel
-(void)webView:(WKWebView *)webView
runJavaScriptAlertPanelWithMessage:(NSString *)message
initiatedByFrame:(WKFrameInfo *)frame
completionHandler:(void (^)(void))completionHandler
{
    NSAlert* alert = [NSAlert new];

    [alert setIcon:[NSImage imageNamed:@"NSCaution"]];

    [alert setShowsHelp:FALSE];
    [alert setInformativeText:message];
    [alert addButtonWithTitle:@"OK"];
    [alert runModal];
    [alert release];
    completionHandler();
}

// run_confirmation_panel
-(void)webView:(WKWebView *)webView
runJavaScriptConfirmPanelWithMessage:(NSString *)message
initiatedByFrame:(WKFrameInfo *)frame
completionHandler:(void (^)(BOOL result))completionHandler;
{
    NSAlert* alert = [NSAlert new];

    [alert setIcon:[NSImage imageNamed:@"NSCaution"]];

    [alert setShowsHelp:FALSE];
    [alert setInformativeText:message];
    [alert addButtonWithTitle:@"OK"];
    [alert addButtonWithTitle:@"Cancel"];

    if ([alert runModal] == NSAlertFirstButtonReturn) {
        completionHandler(true);
    } else {
        completionHandler(false);
    }
    [alert release];
    objc_msgSend(alert, sel_registerName("release"));
}
@end

@implementation __WKNavigationDelegate
- (void)webView:(WKWebView *)webView
decidePolicyForNavigationResponse:(WKNavigationResponse *)navigationResponse
decisionHandler:(void (^)(WKNavigationResponsePolicy))decisionHandler
{
    if ([navigationResponse canShowMIMEType] == 0) {
        // decisionHandler(WKNavigationActionPolicyDownload);
        decisionHandler(WKNavigationResponsePolicyCancel);
    } else {
        decisionHandler(WKNavigationResponsePolicyAllow);
    }
}
@end

@implementation FileSystemWKURLSchemeHandler
-(void)webView:(WKWebView *)webView 
      startURLSchemeTask:(id<WKURLSchemeTask>)urlSchemeTask
{
    // cut 'fs://' from input
    NSString* localFileName = [[urlSchemeTask.request.URL absoluteString] substringFromIndex:5];

    exlib::string target = (const char*)[localFileName UTF8String];
    path_base::normalize(target, target);

    exlib::string baseName;
    path_base::basename(target, "", baseName);

    fibjs::WebView* wv = fibjs::WebView::getWebViewFromWKWebView(webView);
    
    fibjs::Variant varContent;
    exlib::string bufStr("");

    result_t hr = fs_base::cc_readFile(target, "utf8", varContent, wv->holder());

    NSHTTPURLResponse *response;
    NSData* data;

    if (hr == CALL_E_FILE_NOT_FOUND) {
        NSDictionary *responseHeader = @{
            @"Content-length":[NSString stringWithFormat:@"%lu", (unsigned long)0]
        };
            
        response = [[NSHTTPURLResponse alloc]
            initWithURL:[NSURL URLWithString:[urlSchemeTask.request.URL absoluteString]]
            statusCode: 404
            HTTPVersion:@"HTTP/1.1"
            headerFields:responseHeader
        ];
        
        data = [@"" dataUsingEncoding:NSUTF8StringEncoding];

        [urlSchemeTask didReceiveResponse:response];
        [urlSchemeTask didReceiveData:data];

        [urlSchemeTask didFinish];
        return ;
    }

    if (hr == CALL_RETURN_NULL)
        hr = 0;
    else
        bufStr = varContent.string();

    data = [get_nsstring(bufStr.c_str()) dataUsingEncoding:NSUTF8StringEncoding];

    NSString* fileMIME = [self getMIMETypeWithCAPIAtFilePath:localFileName];
    NSDictionary *responseHeader = @{
        @"Content-type":fileMIME,
        @"Content-length":[NSString stringWithFormat:@"%lu", (unsigned long)[data length]]
    };
    response = [[NSHTTPURLResponse alloc]
        initWithURL:[NSURL URLWithString:[urlSchemeTask.request.URL absoluteString]]
        statusCode: 200
        HTTPVersion:@"HTTP/1.1"
        headerFields:responseHeader
    ];

    [urlSchemeTask didReceiveResponse:response];
    [urlSchemeTask didReceiveData:data];

    [urlSchemeTask didFinish];
}
- (NSString *)getMIMETypeWithCAPIAtFilePath:(NSString *)path
{
    return [self getMIMETypeWithCAPIAtFilePath:path allowNonExisted:true];
}
- (NSString *)getMIMETypeWithCAPIAtFilePath:(NSString *)path
    allowNonExisted:(bool)isAllowNonExisted
{
    if (![[[NSFileManager alloc] init] fileExistsAtPath:path] && !isAllowNonExisted) {
        return nil;
    }
    CFStringRef UTI = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, (__bridge CFStringRef)[path pathExtension], NULL);
    CFStringRef MIMEType = UTTypeCopyPreferredTagWithClass (UTI, kUTTagClassMIMEType);
    CFRelease(UTI);
    if (!MIMEType) {
        return @"application/octet-stream";
    }
    return (__bridge NSString *)(MIMEType);
}
@end /* FileSystemWKURLSchemeHandler */

#endif // __APPLE__