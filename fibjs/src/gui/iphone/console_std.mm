/*
 * console_std.cpp
 *
 *  Created on: Apr 25, 2014
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#ifdef iPhone

#import "AppDelegate.h"
#import "ViewController.h"
#import "ANSIString.h"

#include "console.h"
#include "../gui.h"

namespace fibjs {

void std_logger::out(exlib::string& txt, bool is_error)
{
    NSString* ansiString = [NSString stringWithUTF8String:txt.c_str()];
    NSAttributedString* attributedText = [ANSIString parseColorCodes:ansiString];

    fibjs::async([attributedText]() {
        AppDelegate* appDelegate = (AppDelegate*)[UIApplication sharedApplication].delegate;
        ViewController* rootViewController = appDelegate.window.rootViewController;
        [rootViewController appendToConsole:attributedText];
    },
        CALL_E_GUICALL);
}

result_t std_logger::write(AsyncEvent* ac)
{
    item* p1;

    while ((p1 = m_workinglogs.getHead()) != 0) {
        exlib::string txt;

        if (p1->m_priority == console_base::C_NOTICE)
            txt = logger::notice() + p1->m_msg + COLOR_RESET;
        else if (p1->m_priority == console_base::C_WARN)
            txt = logger::warn() + p1->m_msg + COLOR_RESET;
        else if (p1->m_priority <= console_base::C_ERROR)
            txt = logger::error() + p1->m_msg + COLOR_RESET;
        else if (p1->m_priority == console_base::C_PRINT)
            txt = p1->m_msg;
        else
            txt = p1->m_msg;

        out(txt, p1->m_priority <= console_base::C_WARN);

        delete p1;
    }
    fflush(stdout);

    return 0;
}
}

#endif