#ifdef __APPLE__

#ifndef WEBVIEW_DARWIN_UTILS_H
#define WEBVIEW_DARWIN_UTILS_H

#ifndef OBJC_OLD_DISPATCH_PROTOTYPES
#define OBJC_OLD_DISPATCH_PROTOTYPES 1
#endif

#ifdef __cplusplus
extern "C" {
#include <stdlib.h>
#include <stdint.h>

#include <objc/objc-runtime.h>
#include <CoreGraphics/CoreGraphics.h>
#include <limits.h>
}
#endif

#ifdef __cplusplus
#endif

typedef id objc_id;

namespace fibjs {
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

struct webview_priv {
    id pool;
    id window;
    id webview;
    id windowDelegate;
    int should_exit;
};

struct webview;

typedef void (*webview_external_invoke_cb_t)(struct webview* w,
    const char* arg);

struct webview {
    const char* url;
    const char* title;
    int width;
    int height;
    int resizable;
    int debug;
    webview_external_invoke_cb_t external_invoke_cb;
    struct webview_priv priv;
    void* parent;
    void* userdata;
};

#define WEBVIEW_DIALOG_FLAG_FILE (0 << 0)
#define WEBVIEW_DIALOG_FLAG_DIRECTORY (1 << 0)

#define WEBVIEW_DIALOG_FLAG_INFO (1 << 1)
#define WEBVIEW_DIALOG_FLAG_WARNING (2 << 1)
#define WEBVIEW_DIALOG_FLAG_ERROR (3 << 1)
#define WEBVIEW_DIALOG_FLAG_ALERT_MASK (3 << 1)

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
} /* namespace fibjs */

#endif /* WEBVIEW_DARWIN_UTILS_H */
#endif /* __APPLE__ */