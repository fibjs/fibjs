/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-12 04:25:25
 * @modify date 2020-06-12 04:25:25
 * @desc WebView Implementation in OSX
 */

#include <exlib/include/osconfig.h>
#if defined(Darwin) && !defined(iPhone)

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

@implementation __WKScriptMessageHandler
- (void)userContentController:(WKUserContentController*)userContentController
      didReceiveScriptMessage:(WKScriptMessage*)message
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

@implementation FileSystemWKURLSchemeHandler
- (void)webView:(WKWebView*)webView
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

    NSHTTPURLResponse* response;
    NSData* data;

    if (hr == CALL_E_FILE_NOT_FOUND) {
        NSDictionary* responseHeader = @{
            @"Content-length" : [NSString stringWithFormat:@"%lu", (unsigned long)0]
        };

        response = [[NSHTTPURLResponse alloc]
             initWithURL:[NSURL URLWithString:[urlSchemeTask.request.URL absoluteString]]
              statusCode:404
             HTTPVersion:@"HTTP/1.1"
            headerFields:responseHeader];

        data = [@"" dataUsingEncoding:NSUTF8StringEncoding];

        [urlSchemeTask didReceiveResponse:response];
        [urlSchemeTask didReceiveData:data];

        [urlSchemeTask didFinish];
        return;
    }

    if (hr == CALL_RETURN_NULL)
        hr = 0;
    else
        bufStr = varContent.string();

    data = [get_nsstring(bufStr.c_str()) dataUsingEncoding:NSUTF8StringEncoding];

    NSString* fileMIME = [self getMIMETypeWithCAPIAtFilePath:localFileName];
    NSDictionary* responseHeader = @{
        @"Content-type" : fileMIME,
        @"Content-length" : [NSString stringWithFormat:@"%lu", (unsigned long)[data length]]
    };
    response = [[NSHTTPURLResponse alloc]
         initWithURL:[NSURL URLWithString:[urlSchemeTask.request.URL absoluteString]]
          statusCode:200
         HTTPVersion:@"HTTP/1.1"
        headerFields:responseHeader];

    [urlSchemeTask didReceiveResponse:response];
    [urlSchemeTask didReceiveData:data];

    [urlSchemeTask didFinish];
}
- (NSString*)getMIMETypeWithCAPIAtFilePath:(NSString*)path
{
    return [self getMIMETypeWithCAPIAtFilePath:path allowNonExisted:true];
}
- (NSString*)getMIMETypeWithCAPIAtFilePath:(NSString*)path
                           allowNonExisted:(bool)isAllowNonExisted
{
    if (![[[NSFileManager alloc] init] fileExistsAtPath:path] && !isAllowNonExisted) {
        return nil;
    }
    CFStringRef UTI = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, (__bridge CFStringRef)[path pathExtension], NULL);
    CFStringRef MIMEType = UTTypeCopyPreferredTagWithClass(UTI, kUTTagClassMIMEType);
    CFRelease(UTI);
    if (!MIMEType) {
        return @"application/octet-stream";
    }
    return (__bridge NSString*)(MIMEType);
}
@end /* FileSystemWKURLSchemeHandler */

#endif // __APPLE__