#pragma once

#include "utils.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <objc/objc-runtime.h>
#include <objc/message.h>
#include <CoreGraphics/CoreGraphics.h>
#include <limits.h>

#define DEFAULT_URL                                                              \
    "data:text/"                                                                 \
    "html,%3C%21DOCTYPE%20html%3E%0A%3Chtml%20lang=%22en%22%3E%0A%3Chead%3E%"    \
    "3Cmeta%20charset=%22utf-8%22%3E%3Cmeta%20http-equiv=%22X-UA-Compatible%22%" \
    "20content=%22IE=edge%22%3E%3C%2Fhead%3E%0A%3Cbody%3E%3Cdiv%20id=%22app%22%" \
    "3E%3C%2Fdiv%3E%3Cscript%20type=%22text%2Fjavascript%22%3E%3C%2Fscript%3E%"  \
    "3C%2Fbody%3E%0A%3C%2Fhtml%3E"

#define CSS_INJECT_FUNCTION                                             \
    "(function(e){var "                                                 \
    "t=document.createElement('style'),d=document.head||document."      \
    "getElementsByTagName('head')[0];t.setAttribute('type','text/"      \
    "css'),t.styleSheet?t.styleSheet.cssText=e:t.appendChild(document." \
    "createTextNode(e)),d.appendChild(t)})"

inline const char* webview_check_url(const char* url)
{
    if (url == NULL || strlen(url) == 0)
        return DEFAULT_URL;

    return url;
}

inline NSString* get_nsstring(const char* c_str)
{
    return [NSString stringWithUTF8String:c_str];
}

inline NSString* w_get_nsstring(const wchar_t* wstr)
{
    return [[NSString alloc]
        initWithBytes:wstr
               length:wcslen(wstr) * sizeof(*wstr)
             encoding:NSUTF32LittleEndianStringEncoding];
}

static const char* WEBVIEW_MSG_HANDLER_NAME_INVOKE = "invoke";
static const char* WEBVIEW_MSG_HANDLER_NAME_INWARD = "__inward";
static const char* WEBVIEW_MSG_HANDLER_NAME_EXTERNALLOG = "__externalLog";
