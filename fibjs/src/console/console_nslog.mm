/*
 * console_nslog.mm
 *
 *  Created on: Apr 25, 2014
 *      Author: lion
 */

#include "console.h"
#include "Buffer.h"
#include "UVStream.h"
#include "ifs/tty.h"
#include <stdio.h>
#import <Foundation/Foundation.h>

namespace fibjs {

void nslog_logger::out(exlib::string& txt, bool is_error)
{
    // 使用 NSLog 输出日志
    NSString *nsTxt = [NSString stringWithUTF8String:txt.c_str()];
    if (is_error) {
        NSLog(@"[ERROR] %@", nsTxt);
    } else {
        NSLog(@"%@", nsTxt);
    }
}

result_t nslog_logger::write(AsyncEvent* ac)
{
    item* p1;

    while ((p1 = m_workinglogs.getHead()) != 0) {
        exlib::string txt;

        if (p1->m_priority == console_base::C_NOTICE)
            txt = logger::notice() + p1->m_msg + COLOR_RESET + "\n";
        else if (p1->m_priority == console_base::C_WARN)
            txt = logger::warn() + p1->m_msg + COLOR_RESET + "\n";
        else if (p1->m_priority <= console_base::C_ERROR)
            txt = logger::error() + p1->m_msg + COLOR_RESET + "\n";
        else if (p1->m_priority == console_base::C_PRINT)
            txt = p1->m_msg;
        else
            txt = p1->m_msg + "\n";

        out(txt, p1->m_priority <= console_base::C_WARN);

        delete p1;
    }
    fflush(stdout);

    return 0;
}
}
