/*
 * message.mm
 *
 *  Created on: Oct 25, 2024
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

result_t gui_base::alert(exlib::string message)
{
    return alert("", message);
}

result_t gui_base::alert(exlib::string title, exlib::string message)
{
    result_t gui_hr = check_gui_available();
    if (gui_hr.failed())
        return gui_hr;

    gui_prepare_call();

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

    return result_t();
}

result_t gui_base::confirm(exlib::string message, bool& retVal)
{
    return confirm("", message, retVal);
}

result_t gui_base::confirm(exlib::string title, exlib::string message, bool& retVal)
{
    result_t gui_hr = check_gui_available();
    if (gui_hr.failed())
        return gui_hr;

    gui_prepare_call();

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

    if (--s_window_count == 0)
        [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyAccessory];

    if (response == NSAlertFirstButtonReturn) {
        retVal = true;
    } else {
        retVal = false;
    }

    return result_t();
}

}

#endif
