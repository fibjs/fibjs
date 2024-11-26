/*
 * message.mm
 *
 *  Created on: Nov 11, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#ifdef iPhone

#include "object.h"
#include "ifs/gui.h"
#include "../gui.h"
#import <UIKit/UIKit.h>

namespace fibjs {

inline NSString* toNSString(const exlib::string& str)
{
    return [NSString stringWithUTF8String:str.c_str()];
}

result_t gui_base::alert(exlib::string message, AsyncEvent* ac)
{
    return alert("", message, ac);
}

result_t gui_base::alert(exlib::string title, exlib::string message, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    UIAlertController* alert = [UIAlertController alertControllerWithTitle:toNSString(title)
                                                                   message:toNSString(message)
                                                            preferredStyle:UIAlertControllerStyleAlert];

    UIAlertAction* okAction = [UIAlertAction actionWithTitle:@"OK"
                                                       style:UIAlertActionStyleDefault
                                                     handler:^(UIAlertAction* action) {
                                                         ac->post(0);
                                                     }];

    [alert addAction:okAction];

    UIViewController* rootViewController = [UIApplication sharedApplication].keyWindow.rootViewController;
    [rootViewController presentViewController:alert animated:YES completion:nil];

    return CALL_E_PENDDING;
}

result_t gui_base::confirm(exlib::string message, bool& retVal, AsyncEvent* ac)
{
    return confirm("", message, retVal, ac);
}

result_t gui_base::confirm(exlib::string title, exlib::string message, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    UIAlertController* alert = [UIAlertController alertControllerWithTitle:toNSString(title)
                                                                   message:toNSString(message)
                                                            preferredStyle:UIAlertControllerStyleAlert];

    UIAlertAction* okAction = [UIAlertAction actionWithTitle:@"OK"
                                                       style:UIAlertActionStyleDefault
                                                     handler:^(UIAlertAction* action) {
                                                         retVal = true;
                                                         ac->post(0);
                                                     }];

    UIAlertAction* cancelAction = [UIAlertAction actionWithTitle:@"Cancel"
                                                           style:UIAlertActionStyleCancel
                                                         handler:^(UIAlertAction* action) {
                                                             retVal = false;
                                                             ac->post(0);
                                                         }];

    [alert addAction:okAction];
    [alert addAction:cancelAction];

    UIViewController* rootViewController = [UIApplication sharedApplication].keyWindow.rootViewController;
    [rootViewController presentViewController:alert animated:YES completion:nil];

    return CALL_E_PENDDING;
}

}

#endif