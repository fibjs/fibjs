/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-12 04:26:38
 * @modify date 2020-06-12 04:26:38
 * @desc 
 */


#ifdef __APPLE__

#include <Cocoa/Cocoa.h>

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

#endif // __APPLE__