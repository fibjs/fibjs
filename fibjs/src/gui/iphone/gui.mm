/*
 * gui.mm
 *
 *  Created on: Nov 11, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#ifdef iPhone

#import <UIKit/UIKit.h>
#import "AppDelegate.h"
#import "ViewController.h"

#include "object.h"
#include "ifs/gui.h"
#include "ifs/url.h"
#include "../gui.h"
#include "WebView.h"

@implementation AppDelegate

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    ViewController* viewController = [[ViewController alloc] init];
    self.window.rootViewController = viewController;
    [self.window makeKeyAndVisible];

    fibjs::g_gui_ready.set();

    return YES;
}

@end

namespace fibjs {

void gui_log(const char* msg)
{
    NSString* nsmsg = [NSString stringWithUTF8String:msg];
    NSLog(@"%@", nsmsg);
}

static dispatch_queue_t mainQueue = dispatch_get_main_queue();
void os_putGuiPool(AsyncEvent* ac)
{
    dispatch_async(mainQueue, ^{ ac->invoke(); });
}

void run_os_gui()
{
    int argc = 0;
    char* argv[] = { NULL };
    UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
}

}

#endif
