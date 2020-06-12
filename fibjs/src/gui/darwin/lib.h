/*
 * MIT License
 *
 * Copyright (c) 2017 Serge Zaitsev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef WEBVIEW_COCOA_H
#define WEBVIEW_COCOA_H

#include "utils.h"

FIBJS_EXTERN_C_START

// #ifdef WEBVIEW_STATIC
#define WEBVIEW_API static
// #else
// #define WEBVIEW_API extern
// #endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <objc/objc-runtime.h>
#include <objc/message.h>
#include <CoreGraphics/CoreGraphics.h>
#include <limits.h>

struct webview_priv {
    // id pool;
    id window;
    id webview;
    // id appDelegate;
    int should_exit;
};

struct webview {
    // const char* url;
    // const char* title;
    // int width;
    // int height;
    // int resizable;
    // int debug;
    // void* clsWebView;
    struct webview_priv priv;
    // void* userdata;
};

enum webview_dialog_type {
    WEBVIEW_DIALOG_TYPE_OPEN = 0,
    WEBVIEW_DIALOG_TYPE_SAVE = 1,
    WEBVIEW_DIALOG_TYPE_ALERT = 2
};

#define WEBVIEW_DIALOG_FLAG_FILE (0 << 0)
#define WEBVIEW_DIALOG_FLAG_DIRECTORY (1 << 0)

#define WEBVIEW_DIALOG_FLAG_INFO (1 << 1)
#define WEBVIEW_DIALOG_FLAG_WARNING (2 << 1)
#define WEBVIEW_DIALOG_FLAG_ERROR (3 << 1)
#define WEBVIEW_DIALOG_FLAG_ALERT_MASK (3 << 1)

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

static const char* webview_check_url(const char* url)
{
    if (url == NULL || strlen(url) == 0) {
        return DEFAULT_URL;
    }
    return url;
}

// #ifndef NSAlertStyleWarning
// #define NSAlertStyleWarning 0
// #endif
// #ifndef NSAlertStyleCritical
// #define NSAlertStyleCritical 2
// #endif
#ifndef NSWindowStyleMaskResizable
#define NSWindowStyleMaskResizable 8
#endif
#ifndef NSWindowStyleMaskMiniaturizable
#define NSWindowStyleMaskMiniaturizable 4
#endif
#ifndef NSWindowStyleMaskTitled
#define NSWindowStyleMaskTitled 1
#endif
#ifndef NSWindowStyleMaskClosable
#define NSWindowStyleMaskClosable 2
#endif
#ifndef NSWindowStyleMaskFullScreen
#define NSWindowStyleMaskFullScreen (1 << 14)
#endif
#ifndef NSViewWidthSizable
#define NSViewWidthSizable 2
#endif
#ifndef NSViewHeightSizable
#define NSViewHeightSizable 16
#endif
// #ifndef NSBackingStoreBuffered
// #define NSBackingStoreBuffered 2
// #endif
#ifndef NSEventMaskAny
#define NSEventMaskAny ULONG_MAX
#endif
#ifndef NSEventModifierFlagCommand
#define NSEventModifierFlagCommand (1 << 20)
#endif
#ifndef NSEventModifierFlagOption
#define NSEventModifierFlagOption (1 << 19)
#endif
// #ifndef NSAlertStyleInformational
// #define NSAlertStyleInformational 1
// #endif
#ifndef NSAlertFirstButtonReturn
#define NSAlertFirstButtonReturn 1000
#endif
#ifndef WKNavigationActionPolicyDownload
#define WKNavigationActionPolicyDownload 2
#endif
#ifndef NSModalResponseOK
#define NSModalResponseOK 1
#endif
// #ifndef WKNavigationResponsePolicyAllow
// #define WKNavigationResponsePolicyAllow 1
// #endif
// #ifndef WKUserScriptInjectionTimeAtDocumentStart
// #define WKUserScriptInjectionTimeAtDocumentStart 0
// #endif
// #ifndef NSApplicationActivationPolicyRegular
// #define NSApplicationActivationPolicyRegular 0
// #endif

id get_nsstring(const char* c_str);

FIBJS_EXTERN_C_END

#endif /* WEBVIEW_COCOA_H */
