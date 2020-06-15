#import <Cocoa/Cocoa.h>

#import "utils.h"
#import "darwin.h"

FIBJS_EXTERN_C_START

id get_nsstring(const char* c_str)
{
    return (id)[NSString stringWithUTF8String:c_str];
}

id w_get_nsstring(const wchar_t* wstr)
{
    return [[NSString alloc]
        initWithBytes:wstr
        length:wcslen(wstr)*sizeof(*wstr)
        encoding:NSUTF32LittleEndianStringEncoding
    ];
}

FIBJS_EXTERN_C_END