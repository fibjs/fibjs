#import <Cocoa/Cocoa.h>

#import "utils.h"
#import "lib.h"

FIBJS_EXTERN_C_START

id get_nsstring(const char* c_str)
{
    return (id)[NSString stringWithUTF8String:c_str];
}

id create_menu_item(id title, const char* action, const char* key)
{
    id item = objc_msgSend((id)objc_getClass("NSMenuItem"), sel_registerName("alloc"));
    objc_msgSend(item, sel_registerName("initWithTitle:action:keyEquivalent:"),
        title, sel_registerName(action), get_nsstring(key));
    objc_msgSend(item, sel_registerName("autorelease"));

    return item;
}

void run_save_panel(id self, SEL cmd, id download, id filename,
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

void make_nav_policy_decision(id self, SEL cmd, id webView, id response,
    void (^decisionHandler)(int))
{
    if (objc_msgSend(response, sel_registerName("canShowMIMEType")) == 0) {
        decisionHandler(WKNavigationActionPolicyDownload);
    } else {
        decisionHandler(WKNavigationResponsePolicyAllow);
    }
}

void download_failed(id self, SEL cmd, id download, id error)
{
    printf("%s",
        (const char*)objc_msgSend(
            objc_msgSend(error, sel_registerName("localizedDescription")),
            sel_registerName("UTF8String")));
}

int webview_eval(struct webview* w, const char* js)
{
    [w->priv.webview
        evaluateJavaScript:get_nsstring(js)
        completionHandler:NULL];

    return 0;
}

int webview_inject_css(struct webview* w, const char* css)
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

void webview_set_title(struct webview* w, const char* title)
{
    [w->priv.window setTitle:get_nsstring(title)];
}

void webview_set_fullscreen(struct webview* w, int fullscreen)
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

void webview_set_color(struct webview* w, uint8_t r, uint8_t g,
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

void webview_dialog(struct webview* w,
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

void webview_dispatch(struct webview* w, webview_dispatch_fn fn,
    void* arg)
{
    struct webview_dispatch_arg* context = (struct webview_dispatch_arg*)malloc(
        sizeof(struct webview_dispatch_arg));
    context->w = w;
    context->arg = arg;
    context->fn = fn;
    dispatch_async_f(dispatch_get_main_queue(), context, webview_dispatch_cb);
}

FIBJS_EXTERN_C_END