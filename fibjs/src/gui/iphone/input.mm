/*
 * input.mm
 *
 *  Created on: Nov 22, 2024
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

result_t gui_base::input(exlib::string message, bool password, exlib::string& retVal, AsyncEvent* ac)
{
    return input("", message, password, retVal, ac);
}

result_t gui_base::input(exlib::string title, exlib::string message, bool password, exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    UIAlertController* alert = [UIAlertController alertControllerWithTitle:toNSString(title)
                                                                   message:toNSString(message)
                                                            preferredStyle:UIAlertControllerStyleAlert];

    [alert addTextFieldWithConfigurationHandler:^(UITextField* textField) {
        textField.secureTextEntry = password;
    }];

    UIAlertAction* okAction = [UIAlertAction actionWithTitle:@"OK"
                                                       style:UIAlertActionStyleDefault
                                                     handler:^(UIAlertAction* action) {
                                                         UITextField* textField = alert.textFields.firstObject;
                                                         retVal = [textField.text UTF8String];
                                                         ac->post(0);
                                                     }];

    UIAlertAction* cancelAction = [UIAlertAction actionWithTitle:@"Cancel"
                                                           style:UIAlertActionStyleCancel
                                                         handler:^(UIAlertAction* action) {
                                                             ac->post(CALL_RETURN_UNDEFINED);
                                                         }];

    [alert addAction:okAction];
    [alert addAction:cancelAction];

    UIViewController* topViewController = [UIApplication sharedApplication].keyWindow.rootViewController;
    while (topViewController.presentedViewController) {
        topViewController = topViewController.presentedViewController;
    }
    [topViewController presentViewController:alert animated:YES completion:nil];

    return CALL_E_PENDDING;
}

}

#endif
