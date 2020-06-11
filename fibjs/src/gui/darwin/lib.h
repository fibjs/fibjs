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
    id pool;
    id window;
    id webview;
    id windowDelegate;
    id appDelegate;
    int should_exit;
};

struct webview;

typedef void (*webview_external_cb_t)(struct webview* w,
    const char* arg);

typedef void (*webview_external_invoke_cb_t)(struct webview* w,
    const char* arg);

struct webview {
    const char* url;
    const char* title;
    int width;
    int height;
    int resizable;
    int debug;
    void* clsWebView;
    struct webview_priv priv;
    void* userdata;
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

typedef void (*gui_dispatch_fn)(id event, void* arg);

struct guid_dispatch_arg {
    gui_dispatch_fn fn;
    id event;
    void* arg;
};

static void gui_dispatch_cb(void* arg)
{
    struct guid_dispatch_arg* context = (struct guid_dispatch_arg*)arg;
    (context->fn)(context->event, context->arg);
    free(context);
}
static void gui_dispatch(
    id event,
    gui_dispatch_fn fn,
    void* arg)
{
    struct guid_dispatch_arg* context = (struct guid_dispatch_arg*)malloc(
        sizeof(struct guid_dispatch_arg));
    context->event = event;
    context->arg = arg;
    context->fn = fn;
    dispatch_async_f(dispatch_get_main_queue(), context, gui_dispatch_cb);
}

typedef void (*webview_dispatch_fn)(struct webview* w, void* arg);

struct webview_dispatch_arg {
    webview_dispatch_fn fn;
    struct webview* w;
    void* arg;
};

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

int webview_eval(struct webview* w, const char* js);
int webview_inject_css(struct webview* w, const char* css);
void webview_set_title(struct webview* w, const char* title);
void webview_set_fullscreen(struct webview* w, int fullscreen);
void webview_set_color(struct webview* w, uint8_t r, uint8_t g,
    uint8_t b, uint8_t a);
void webview_dialog(struct webview* w,
    enum webview_dialog_type dlgtype, int flags,
    const char* title, const char* arg,
    char* result, size_t resultsz);
void webview_dispatch(struct webview* w, webview_dispatch_fn fn,
    void* arg);
// void webview_terminate(struct webview* w);
WEBVIEW_API void webview_print_log(const char* s) { printf("%s\n", s); }

WEBVIEW_API void webview_debug(const char* format, ...)
{
    char buf[4096];
    va_list ap;
    va_start(ap, format);
    vsnprintf(buf, sizeof(buf), format, ap);
    webview_print_log(buf);
    va_end(ap);
}

static int webview_js_encode(const char* s, char* esc, size_t n)
{
    int r = 1; /* At least one byte for trailing zero */
    for (; *s; s++) {
        const unsigned char c = *s;
        if (c >= 0x20 && c < 0x80 && strchr("<>\\'\"", c) == NULL) {
            if (n > 0) {
                *esc++ = c;
                n--;
            }
            r++;
        } else {
            if (n > 0) {
                snprintf(esc, n, "\\x%02x", (int)c);
                esc += 4;
                n -= 4;
            }
            r += 4;
        }
    }
    return r;
}

#ifndef NSAlertStyleWarning
#define NSAlertStyleWarning 0
#endif
#ifndef NSAlertStyleCritical
#define NSAlertStyleCritical 2
#endif
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
#ifndef NSAlertStyleInformational
#define NSAlertStyleInformational 1
#endif
#ifndef NSAlertFirstButtonReturn
#define NSAlertFirstButtonReturn 1000
#endif
#ifndef WKNavigationActionPolicyDownload
#define WKNavigationActionPolicyDownload 2
#endif
#ifndef NSModalResponseOK
#define NSModalResponseOK 1
#endif
#ifndef WKNavigationResponsePolicyAllow
#define WKNavigationResponsePolicyAllow 1
#endif
// #ifndef WKUserScriptInjectionTimeAtDocumentStart
// #define WKUserScriptInjectionTimeAtDocumentStart 0
// #endif
#ifndef NSApplicationActivationPolicyRegular
#define NSApplicationActivationPolicyRegular 0
#endif

id get_nsstring(const char* c_str);

id create_menu_item(id title, const char* action, const char* key);

void run_open_panel(id self, SEL cmd, id webView, id parameters,
    id frame, void (^completionHandler)(id));

void run_save_panel(id self, SEL cmd, id download, id filename,
    void (^completionHandler)(int allowOverwrite,
        id destination));

void run_confirmation_panel(id self, SEL cmd, id webView, id message,
    id frame, void (^completionHandler)(bool));

void run_alert_panel(id self, SEL cmd, id webView, id message, id frame,
    void (^completionHandler)(void));

void download_failed(id self, SEL cmd, id download, id error);

void make_nav_policy_decision(id self, SEL cmd, id webView, id response,
    void (^decisionHandler)(int));


static void webview_dispatch_cb(void* arg)
{
    struct webview_dispatch_arg* context = (struct webview_dispatch_arg*)arg;
    (context->fn)(context->w, context->arg);
    free(context);
}

FIBJS_EXTERN_C_END

#endif /* WEBVIEW_COCOA_H */
