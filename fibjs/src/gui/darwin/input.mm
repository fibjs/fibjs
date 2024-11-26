/*
 * input.mm
 *
 *  Created on: Nov 22, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#ifndef iPhone

#include "object.h"
#include "ifs/gui.h"
#include "../gui.h"
#import <Cocoa/Cocoa.h>

extern int32_t s_window_count;

namespace fibjs {

inline NSString* toNSString(const exlib::string& str)
{
    return [NSString stringWithUTF8String:str.c_str()];
}

result_t gui_base::input(exlib::string message, bool password, exlib::string& retVal, AsyncEvent* ac)
{
    return input("", message, password, retVal, ac);
}

result_t gui_base::input(exlib::string title, exlib::string message, bool password, exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    NSAlert* alert = [[NSAlert alloc] init];
    [alert setMessageText:toNSString(title)];
    [alert setInformativeText:toNSString(message)];
    [alert setAlertStyle:NSAlertStyleInformational];
    [alert setIcon:[NSImage imageNamed:NSImageNameInfo]];
    [alert addButtonWithTitle:@"OK"];
    [alert addButtonWithTitle:@"Cancel"];

    NSTextField* input;
    if (password) {
        input = [[NSSecureTextField alloc] initWithFrame:NSMakeRect(0, 0, 200, 24)];
    } else {
        input = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 200, 24)];
    }

    [alert setAccessoryView:input];

    if (++s_window_count == 1)
        [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyRegular];

    [[NSRunningApplication currentApplication] activateWithOptions:NSApplicationActivateIgnoringOtherApps];

    NSModalResponse response = [alert runModal];

    if (--s_window_count == 0)
        [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyAccessory];

    if (response == NSAlertFirstButtonReturn) {
        retVal = [[[alert accessoryView] stringValue] UTF8String];
    } else {
        return CALL_RETURN_UNDEFINED;
    }

    return 0;
}

}

#endif
