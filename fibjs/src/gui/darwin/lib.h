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

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WEBVIEW_STATIC
#define WEBVIEW_API static
#else
#define WEBVIEW_API extern
#endif

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

static int webview_eval(struct webview* w, const char* js);
WEBVIEW_API int webview_inject_css(struct webview* w, const char* css);
WEBVIEW_API void webview_set_title(struct webview* w, const char* title);
WEBVIEW_API void webview_set_fullscreen(struct webview* w, int fullscreen);
WEBVIEW_API void webview_set_color(struct webview* w, uint8_t r, uint8_t g,
    uint8_t b, uint8_t a);
WEBVIEW_API void webview_dialog(struct webview* w,
    enum webview_dialog_type dlgtype, int flags,
    const char* title, const char* arg,
    char* result, size_t resultsz);
WEBVIEW_API void webview_dispatch(struct webview* w, webview_dispatch_fn fn,
    void* arg);
WEBVIEW_API void webview_terminate(struct webview* w);
WEBVIEW_API void webview_debug(const char* format, ...);
WEBVIEW_API void webview_print_log(const char* s);

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

WEBVIEW_API int webview_inject_css(struct webview* w, const char* css)
{
    int n = webview_js_encode(css, NULL, 0);
    char* esc = (char*)calloc(1, sizeof(CSS_INJECT_FUNCTION) + n + 4);
    if (esc == NULL) {
        return -1;
    }
    char* js = (char*)calloc(1, n);
    webview_js_encode(css, js, n);
    snprintf(esc, sizeof(CSS_INJECT_FUNCTION) + n + 4, "%s(\"%s\")",
        CSS_INJECT_FUNCTION, js);
    int r = webview_eval(w, esc);
    free(js);
    free(esc);
    return r;
}

#define NSAlertStyleWarning 0
#define NSAlertStyleCritical 2
#define NSWindowStyleMaskResizable 8
#define NSWindowStyleMaskMiniaturizable 4
#define NSWindowStyleMaskTitled 1
#define NSWindowStyleMaskClosable 2
#define NSWindowStyleMaskFullScreen (1 << 14)
#define NSViewWidthSizable 2
#define NSViewHeightSizable 16
#define NSBackingStoreBuffered 2
#define NSEventMaskAny ULONG_MAX
#define NSEventModifierFlagCommand (1 << 20)
#define NSEventModifierFlagOption (1 << 19)
#define NSAlertStyleInformational 1
#define NSAlertFirstButtonReturn 1000
#define WKNavigationActionPolicyDownload 2
#define NSModalResponseOK 1
#define WKNavigationActionPolicyDownload 2
#define WKNavigationResponsePolicyAllow 1
#define WKUserScriptInjectionTimeAtDocumentStart 0
#define NSApplicationActivationPolicyRegular 0

static id get_nsstring(const char* c_str)
{
    return objc_msgSend((id)objc_getClass("NSString"),
        sel_registerName("stringWithUTF8String:"), c_str);
}

static id create_menu_item(id title, const char* action, const char* key)
{
    id item = objc_msgSend((id)objc_getClass("NSMenuItem"), sel_registerName("alloc"));
    objc_msgSend(item, sel_registerName("initWithTitle:action:keyEquivalent:"),
        title, sel_registerName(action), get_nsstring(key));
    objc_msgSend(item, sel_registerName("autorelease"));

    return item;
}

static void run_open_panel(id self, SEL cmd, id webView, id parameters,
    id frame, void (^completionHandler)(id))
{

    id openPanel = objc_msgSend((id)objc_getClass("NSOpenPanel"),
        sel_registerName("openPanel"));

    objc_msgSend(
        openPanel, sel_registerName("setAllowsMultipleSelection:"),
        objc_msgSend(parameters, sel_registerName("allowsMultipleSelection")));

    objc_msgSend(openPanel, sel_registerName("setCanChooseFiles:"), 1);
    objc_msgSend(
        openPanel, sel_registerName("beginWithCompletionHandler:"), ^(id result) {
            if (result == (id)NSModalResponseOK) {
                completionHandler(objc_msgSend(openPanel, sel_registerName("URLs")));
            } else {
                completionHandler(nil);
            }
        });
}

static void run_save_panel(id self, SEL cmd, id download, id filename,
    void (^completionHandler)(int allowOverwrite,
        id destination))
{
    id savePanel = objc_msgSend((id)objc_getClass("NSSavePanel"),
        sel_registerName("savePanel"));
    objc_msgSend(savePanel, sel_registerName("setCanCreateDirectories:"), 1);
    objc_msgSend(savePanel, sel_registerName("setNameFieldStringValue:"),
        filename);
    objc_msgSend(savePanel, sel_registerName("beginWithCompletionHandler:"),
        ^(id result) {
            if (result == (id)NSModalResponseOK) {
                id url = objc_msgSend(savePanel, sel_registerName("URL"));
                id path = objc_msgSend(url, sel_registerName("path"));
                completionHandler(1, path);
            } else {
                completionHandler(NO, nil);
            }
        });
}

static void run_confirmation_panel(id self, SEL cmd, id webView, id message,
    id frame, void (^completionHandler)(bool))
{

    id alert = objc_msgSend((id)objc_getClass("NSAlert"), sel_registerName("new"));
    objc_msgSend(alert, sel_registerName("setIcon:"),
        objc_msgSend((id)objc_getClass("NSImage"),
            sel_registerName("imageNamed:"),
            get_nsstring("NSCaution")));
    objc_msgSend(alert, sel_registerName("setShowsHelp:"), 0);
    objc_msgSend(alert, sel_registerName("setInformativeText:"), message);
    objc_msgSend(alert, sel_registerName("addButtonWithTitle:"),
        get_nsstring("OK"));
    objc_msgSend(alert, sel_registerName("addButtonWithTitle:"),
        get_nsstring("Cancel"));
    if (objc_msgSend(alert, sel_registerName("runModal")) == (id)NSAlertFirstButtonReturn) {
        completionHandler(true);
    } else {
        completionHandler(false);
    }
    objc_msgSend(alert, sel_registerName("release"));
}

static void run_alert_panel(id self, SEL cmd, id webView, id message, id frame,
    void (^completionHandler)(void))
{
    id alert = objc_msgSend((id)objc_getClass("NSAlert"), sel_registerName("new"));
    objc_msgSend(alert, sel_registerName("setIcon:"),
        objc_msgSend((id)objc_getClass("NSImage"),
            sel_registerName("imageNamed:"),
            get_nsstring("NSCaution")));
    objc_msgSend(alert, sel_registerName("setShowsHelp:"), 0);
    objc_msgSend(alert, sel_registerName("setInformativeText:"), message);
    objc_msgSend(alert, sel_registerName("addButtonWithTitle:"),
        get_nsstring("OK"));
    objc_msgSend(alert, sel_registerName("runModal"));
    objc_msgSend(alert, sel_registerName("release"));
    completionHandler();
}

static void download_failed(id self, SEL cmd, id download, id error)
{
    printf("%s",
        (const char*)objc_msgSend(
            objc_msgSend(error, sel_registerName("localizedDescription")),
            sel_registerName("UTF8String")));
}

static void make_nav_policy_decision(id self, SEL cmd, id webView, id response,
    void (^decisionHandler)(int))
{
    if (objc_msgSend(response, sel_registerName("canShowMIMEType")) == 0) {
        decisionHandler(WKNavigationActionPolicyDownload);
    } else {
        decisionHandler(WKNavigationResponsePolicyAllow);
    }
}

static int webview_eval(struct webview* w, const char* js)
{
    objc_msgSend(w->priv.webview,
        sel_registerName("evaluateJavaScript:completionHandler:"),
        get_nsstring(js), NULL);

    return 0;
}

WEBVIEW_API void webview_set_title(struct webview* w, const char* title)
{
    objc_msgSend(w->priv.window, sel_registerName("setTitle"),
        get_nsstring(title));
}

WEBVIEW_API void webview_set_fullscreen(struct webview* w, int fullscreen)
{
    unsigned long windowStyleMask = (unsigned long)objc_msgSend(
        w->priv.window, sel_registerName("styleMask"));
    int b = (((windowStyleMask & NSWindowStyleMaskFullScreen) == NSWindowStyleMaskFullScreen)
            ? 1
            : 0);
    if (b != fullscreen) {
        objc_msgSend(w->priv.window, sel_registerName("toggleFullScreen:"), NULL);
    }
}

WEBVIEW_API void webview_set_color(struct webview* w, uint8_t r, uint8_t g,
    uint8_t b, uint8_t a)
{

    id color = objc_msgSend((id)objc_getClass("NSColor"),
        sel_registerName("colorWithRed:green:blue:alpha:"),
        (float)r / 255.0, (float)g / 255.0, (float)b / 255.0,
        (float)a / 255.0);

    objc_msgSend(w->priv.window, sel_registerName("setBackgroundColor:"), color);

    if (0.5 >= ((r / 255.0 * 299.0) + (g / 255.0 * 587.0) + (b / 255.0 * 114.0)) / 1000.0) {
        objc_msgSend(w->priv.window, sel_registerName("setAppearance:"),
            objc_msgSend((id)objc_getClass("NSAppearance"),
                sel_registerName("appearanceNamed:"),
                get_nsstring("NSAppearanceNameVibrantDark")));
    } else {
        objc_msgSend(w->priv.window, sel_registerName("setAppearance:"),
            objc_msgSend((id)objc_getClass("NSAppearance"),
                sel_registerName("appearanceNamed:"),
                get_nsstring("NSAppearanceNameVibrantLight")));
    }
    objc_msgSend(w->priv.window, sel_registerName("setOpaque:"), 0);
    objc_msgSend(w->priv.window,
        sel_registerName("setTitlebarAppearsTransparent:"), 1);
}

WEBVIEW_API void webview_dialog(struct webview* w,
    enum webview_dialog_type dlgtype, int flags,
    const char* title, const char* arg,
    char* result, size_t resultsz)
{
    if (dlgtype == WEBVIEW_DIALOG_TYPE_OPEN || dlgtype == WEBVIEW_DIALOG_TYPE_SAVE) {
        id panel = (id)objc_getClass("NSSavePanel");
        if (dlgtype == WEBVIEW_DIALOG_TYPE_OPEN) {
            id openPanel = objc_msgSend((id)objc_getClass("NSOpenPanel"),
                sel_registerName("openPanel"));
            if (flags & WEBVIEW_DIALOG_FLAG_DIRECTORY) {
                objc_msgSend(openPanel, sel_registerName("setCanChooseFiles:"), 0);
                objc_msgSend(openPanel, sel_registerName("setCanChooseDirectories:"),
                    1);
            } else {
                objc_msgSend(openPanel, sel_registerName("setCanChooseFiles:"), 1);
                objc_msgSend(openPanel, sel_registerName("setCanChooseDirectories:"),
                    0);
            }
            objc_msgSend(openPanel, sel_registerName("setResolvesAliases:"), 0);
            objc_msgSend(openPanel, sel_registerName("setAllowsMultipleSelection:"),
                0);
            panel = openPanel;
        } else {
            panel = objc_msgSend((id)objc_getClass("NSSavePanel"),
                sel_registerName("savePanel"));
        }

        objc_msgSend(panel, sel_registerName("setCanCreateDirectories:"), 1);
        objc_msgSend(panel, sel_registerName("setShowsHiddenFiles:"), 1);
        objc_msgSend(panel, sel_registerName("setExtensionHidden:"), 0);
        objc_msgSend(panel, sel_registerName("setCanSelectHiddenExtension:"), 0);
        objc_msgSend(panel, sel_registerName("setTreatsFilePackagesAsDirectories:"),
            1);
        objc_msgSend(
            panel, sel_registerName("beginSheetModalForWindow:completionHandler:"),
            w->priv.window, ^(id result) {
                objc_msgSend(objc_msgSend((id)objc_getClass("NSApplication"),
                                 sel_registerName("sharedApplication")),
                    sel_registerName("stopModalWithCode:"), result);
            });

        if (objc_msgSend(objc_msgSend((id)objc_getClass("NSApplication"),
                             sel_registerName("sharedApplication")),
                sel_registerName("runModalForWindow:"),
                panel)
            == (id)NSModalResponseOK) {
            id url = objc_msgSend(panel, sel_registerName("URL"));
            id path = objc_msgSend(url, sel_registerName("path"));
            const char* filename = (const char*)objc_msgSend(path, sel_registerName("UTF8String"));
            strlcpy(result, filename, resultsz);
        }
    } else if (dlgtype == WEBVIEW_DIALOG_TYPE_ALERT) {
        id a = objc_msgSend((id)objc_getClass("NSAlert"), sel_registerName("new"));
        switch (flags & WEBVIEW_DIALOG_FLAG_ALERT_MASK) {
        case WEBVIEW_DIALOG_FLAG_INFO:
            objc_msgSend(a, sel_registerName("setAlertStyle:"),
                NSAlertStyleInformational);
            break;
        case WEBVIEW_DIALOG_FLAG_WARNING:
            printf("Warning\n");
            objc_msgSend(a, sel_registerName("setAlertStyle:"), NSAlertStyleWarning);
            break;
        case WEBVIEW_DIALOG_FLAG_ERROR:
            printf("Error\n");
            objc_msgSend(a, sel_registerName("setAlertStyle:"), NSAlertStyleCritical);
            break;
        }
        objc_msgSend(a, sel_registerName("setShowsHelp:"), 0);
        objc_msgSend(a, sel_registerName("setShowsSuppressionButton:"), 0);
        objc_msgSend(a, sel_registerName("setMessageText:"), get_nsstring(title));
        objc_msgSend(a, sel_registerName("setInformativeText:"), get_nsstring(arg));
        objc_msgSend(a, sel_registerName("addButtonWithTitle:"),
            get_nsstring("OK"));
        objc_msgSend(a, sel_registerName("runModal"));
        objc_msgSend(a, sel_registerName("release"));
    }
}

static void webview_dispatch_cb(void* arg)
{
    struct webview_dispatch_arg* context = (struct webview_dispatch_arg*)arg;
    (context->fn)(context->w, context->arg);
    free(context);
}

WEBVIEW_API void webview_dispatch(struct webview* w, webview_dispatch_fn fn,
    void* arg)
{
    struct webview_dispatch_arg* context = (struct webview_dispatch_arg*)malloc(
        sizeof(struct webview_dispatch_arg));
    context->w = w;
    context->arg = arg;
    context->fn = fn;
    dispatch_async_f(dispatch_get_main_queue(), context, webview_dispatch_cb);
}

WEBVIEW_API void webview_print_log(const char* s) { printf("%s\n", s); }

#ifdef __cplusplus
}
#endif

#endif /* WEBVIEW_COCOA_H */
