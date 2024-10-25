/*
 * message.mm
 *
 *  Created on: Oct 25, 2024
 *      Author: lion
 */

#if defined(OS_DESKTOP)

#include "object.h"
#include "ifs/gui.h"
#include "../gui.h"
#import <Cocoa/Cocoa.h>

extern int32_t s_window_count;

namespace fibjs {

NSString* toNSString(const exlib::string& str)
{
    return [NSString stringWithUTF8String:str.c_str()];
}

result_t gui_base::alert(exlib::string message, AsyncEvent* ac)
{
    return alert("", message, ac);
}

result_t gui_base::alert(exlib::string title, exlib::string message, AsyncEvent* ac)
{
    result_t hr = check_gui(ac);
    if (hr < 0)
        return hr;

    NSAlert* alert = [[NSAlert alloc] init];
    [alert setMessageText:toNSString(title)];
    [alert setInformativeText:toNSString(message)];
    [alert setAlertStyle:NSAlertStyleInformational];
    [alert setIcon:[NSImage imageNamed:NSImageNameCaution]];

    if (++s_window_count == 1)
        [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyRegular];
    [[NSRunningApplication currentApplication] activateWithOptions:NSApplicationActivateIgnoringOtherApps];
    [alert runModal];
    if (--s_window_count == 0)
        [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyAccessory];

    return 0;
}

result_t gui_base::confirm(exlib::string message, bool& retVal, AsyncEvent* ac)
{
    return confirm("", message, retVal, ac);
}

result_t gui_base::confirm(exlib::string title, exlib::string message, bool& retVal, AsyncEvent* ac)
{
    result_t hr = check_gui(ac);
    if (hr < 0)
        return hr;

    NSAlert* alert = [[NSAlert alloc] init];
    [alert setMessageText:toNSString(title)];
    [alert setInformativeText:toNSString(message)];
    [alert setAlertStyle:NSAlertStyleInformational];
    [alert setIcon:[NSImage imageNamed:NSImageNameInfo]];
    [alert addButtonWithTitle:@"OK"];
    [alert addButtonWithTitle:@"Cancel"];

    if (++s_window_count == 1)
        [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyRegular];
    [[NSRunningApplication currentApplication] activateWithOptions:NSApplicationActivateIgnoringOtherApps];

    NSModalResponse response = [alert runModal];
    if (response == NSAlertFirstButtonReturn) {
        retVal = true;
    } else {
        retVal = false;
    }

    if (--s_window_count == 0)
        [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyAccessory];

    return 0;
}

}

#endif
