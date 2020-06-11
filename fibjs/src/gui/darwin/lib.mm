#import <Cocoa/Cocoa.h>

#import "utils.h"
#import "lib.h"

FIBJS_EXTERN_C_START

id get_nsstring(const char* c_str)
{
    return (id)[NSString stringWithUTF8String:c_str];
}

FIBJS_EXTERN_C_END