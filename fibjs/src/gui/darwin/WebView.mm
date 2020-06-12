/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-12 04:25:25
 * @modify date 2020-06-12 04:25:25
 * @desc WebView Implementation in OSX
 */

#ifdef __APPLE__

#include <Cocoa/Cocoa.h>
#include <Webkit/Webkit.h>

#include "object.h"
#include "ifs/gui.h"
#include "ifs/fs.h"
#include "ifs/os.h"
#include "path.h"
#include "WebView.h"
#include "EventInfo.h"
#include "utf8.h"
#include <exlib/include/thread.h>

#include "objc.inl.mm"

namespace fibjs {

DECLARE_MODULE(gui);

class gui_thread : public exlib::OSThread {
public:
    // Run In GUI Thread, get AsyncEvent from s_uiPool to invoke
    virtual void Run()
    {
        // initialize one fibjs runtime
        Runtime rt(NULL);

        printf("gui_thread->Run 1\n");
        WebView::RegNSApplicationDelegations();
        WebView::SetupAppMenubar();

        /**
         * 目前是: 
         * - 另一方面, 关闭窗口后, 从 webview_windowWillClose 回调中执行了 WebView::clear, 之后, 这个 Loop 又开始运行了(如果此时系统中还有 isolate 在运行的话)
         */
        while (true) {
            AsyncEvent* p = s_uiPool.getHead();

            if (p) {
                // printf("[gui_thread::Run] in loop, it's p \n");
                p->invoke();
                // p->js_invoke();
            }
            // 从 sharedApplication 的事件循环中中取得事件
            id event = WebView::webview_get_event_from_mainloop(0);
            WebView::send_event_to_sharedApplicatoin_and_check_should_exit(event);

            struct webview* w = WebView::getCurrentWebViewStruct();

            if (w) {
                // printf("[gui_thread::Run] in loop, webview struct not NULL \n");

                if (!WebView::should_exit(w))
                    continue;

                // if (WebView::webview_loop(w, 0) == 0)
                //     continue;

                // if (WebView::should_exit(w) == 0)
                //     continue;
            }
        }
        printf("gui_thread->Run 2\n");
    }

public:
    // OTHERS
    result_t AddRef(void)
    {
        return 1;
    }
    result_t Release(void)
    {
        return 1;
    }
};

void run_gui()
{
    gui_thread* _thGUI = new gui_thread();

    _thGUI->bindCurrent();
    s_thGUI = _thGUI;

    _thGUI->Run();
    // _thGUI->suspend();
}

// useless for darwin
result_t gui_base::setVersion(int32_t ver)
{
    return 0;
}

// In Javascript Thread
result_t gui_base::open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    // printf("--- [here] gui_base::open 1 --- \n");
    obj_ptr<NObject> o = new NObject();
    o->add(opt);

    obj_ptr<WebView> wv = new WebView(url, o);
    wv->wrap();

    asyncCall(WebView::async_open, wv, CALL_E_GUICALL);
    // printf("--- [here] gui_base::open 4 --- \n");
    retVal = wv;

    return 0;
};

// Would Call In Javascript Thread
WebView::WebView(exlib::string url, NObject* opt)
{
    printf("[WebView::WebView] before\n");
    holder()->Ref();

    m_url = url;
    m_opt = opt;

    m_title = "[WIP] Darwin WebView";
    m_WinW = 640;
    m_WinH = 400;
    m_bResizable = true;

    m_bDebug = false;

    m_ac = NULL;
    // _onmessage = NULL;
    // _onclose = NULL;

    m_visible = true;

    printf("[WebView::WebView] after\n");
}

WebView::~WebView()
{
    clear();
}

id WebView::webview_get_event_from_mainloop(int blocking)
{
    id until = blocking ? [NSDate distantFuture] : [NSDate distantPast];

    return [[NSApplication sharedApplication]
        nextEventMatchingMask:ULONG_MAX
        untilDate:until
        inMode:get_nsstring("kCFRunLoopDefaultMode")
        dequeue:true
    ];
}

void WebView::send_event_to_sharedApplicatoin_and_check_should_exit(id event)
{
    if (event) {
        [[NSApplication sharedApplication] sendEvent:event];
    }
}

void WebView::RegNSApplicationDelegations() {
    // id appDelegate = [[__NSApplicationDelegate alloc] init];
    id appDelegate = [__NSApplicationDelegate new];

    [[NSApplication sharedApplication] setDelegate:appDelegate];

    WebView::SetupAppMenubar();
}

void WebView::objc_nsAppInit(struct webview* w)
{
    w->priv.pool = [NSAutoreleasePool new];
    [NSApplication sharedApplication];
}

id WebView::prepareWKPreferences(struct webview* w)
{
    Class __WKPreferences
        = objc_allocateClassPair(objc_getClass("WKPreferences"),
            "__WKPreferences", 0);
    objc_property_attribute_t type = { "T", "c" };
    objc_property_attribute_t ownership = { "N", "" };
    objc_property_attribute_t attrs[] = { type, ownership };
    class_replaceProperty(__WKPreferences, "developerExtrasEnabled", attrs, 2);
    objc_registerClassPair(__WKPreferences);

    id webviewid_wkPref = [__WKPreferences new];
    [webviewid_wkPref
        setValue:[NSNumber numberWithBool:!!w->debug]
        forKey:[NSString stringWithUTF8String:"developerExtrasEnabled"]
    ];

    return webviewid_wkPref;
}

id WebView::getWKUserController(struct webview* w)
{
    id webviewid_wkUserController = [WKUserContentController new];

    objc_setAssociatedObject(webviewid_wkUserController, "webview", (id)(w),
        OBJC_ASSOCIATION_ASSIGN);

    [webviewid_wkUserController
        addScriptMessageHandler:[__WKScriptMessageHandler new]
        name:get_nsstring(WEBVIEW_MSG_HANDLER_NAME)
    ];

    id windowExternalOverrideScript = [[WKUserScript alloc]
        initWithSource:@"window.external = this;postMessage = function(arg){ webkit.messageHandlers.invoke.postMessage(arg); };"
        injectionTime:WKUserScriptInjectionTimeAtDocumentStart
        forMainFrameOnly:FALSE
    ];

    [webviewid_wkUserController addUserScript:windowExternalOverrideScript];

    return webviewid_wkUserController;
}

id WebView::prepareWKWebViewConfig(struct webview* w)
{
    // id webviewid_wkwebviewconfig = objc_msgSend((id)objc_getClass("WKWebViewConfiguration"), sel_registerName("new"));
    id webviewid_wkwebviewconfig = [WKWebViewConfiguration new];

    id processPool = [webviewid_wkwebviewconfig processPool];
    [processPool _setDownloadDelegate:[__WKDownloadDelegate new]];
    [webviewid_wkwebviewconfig setProcessPool:processPool];
    [webviewid_wkwebviewconfig setUserContentController:getWKUserController(w)];
    [webviewid_wkwebviewconfig setPreferences:prepareWKPreferences(w)];

    return webviewid_wkwebviewconfig;
}

void WebView::initWindow(struct webview* w)
{
    unsigned int style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
    if (w->resizable) style = style | NSWindowStyleMaskResizable;

    s_activeWinObjcId = w->priv.window = [[NSWindow alloc]
        initWithContentRect:this->webview_window_rect
        styleMask:style
        backing:NSBackingStoreBuffered
        defer:FALSE
    ];
    printf("[WebView::initWindow] s_activeWinObjcId assigned\n");
    objc_setAssociatedObject(s_activeWinObjcId, "webview", (id)(w), OBJC_ASSOCIATION_ASSIGN);

    [w->priv.window autorelease];
}

void WebView::setupWindowDelegation(struct webview* w)
{
    w->priv.windowDelegate = [__NSWindowDelegate new];
    objc_setAssociatedObject(w->priv.windowDelegate, "webview", (id)(w), OBJC_ASSOCIATION_ASSIGN);
    [w->priv.window setDelegate:w->priv.windowDelegate];
}

void WebView::setupWindowTitle(struct webview* w)
{
    [w->priv.window setTitle:[NSString stringWithUTF8String:w->title]];
}

id WebView::getWKWebView(struct webview* w)
{
    id webview = [
        [WKWebView alloc]
        initWithFrame:this->webview_window_rect configuration:prepareWKWebViewConfig(w)
    ];

    [webview setUIDelegate:[__WKUIDelegate new]];
    [webview setNavigationDelegate:[__WKNavigationDelegate new]];

    return webview;
}

void WebView::navigateWKWebView(struct webview* w)
{
    id nsURL = [NSURL URLWithString:get_nsstring(webview_check_url(w->url))];
    [w->priv.webview loadRequest:[NSURLRequest requestWithURL:nsURL]];
}

void WebView::setWKWebViewStyle(id webview)
{
    [webview setAutoresizesSubviews:TRUE];
    [webview setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
}

void WebView::linkWindowWithWebview(struct webview* w)
{
    [[w->priv.window contentView]
        addSubview:w->priv.webview
    ];
}

void WebView::putWindowToTopOrder(struct webview* w)
{
    [w->priv.window orderFrontRegardless];
}

void WebView::activeApp()
{
    id app = [NSApplication sharedApplication];
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];
    [app finishLaunching];
    [app activateIgnoringOtherApps:YES];
}

int WebView::webview_eval(struct webview* w, const char* js)
{
    [w->priv.webview
        evaluateJavaScript:get_nsstring(js)
        completionHandler:NULL
    ];

    return 0;
}

void WebView::webview_set_color(struct webview* w, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
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

void WebView::webview_dialog(struct webview* w, enum webview_dialog_type dlgtype, int flags, const char* title, const char* arg, char* result, size_t resultsz)
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

int WebView::webview_js_encode(const char* s, char* esc, size_t n)
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

id WebView::create_menu_item(id title, const char* action, const char* key)
{
    id item = objc_msgSend((id)objc_getClass("NSMenuItem"), sel_registerName("alloc"));
    objc_msgSend(item, sel_registerName("initWithTitle:action:keyEquivalent:"),
        title, sel_registerName(action), get_nsstring(key));
    objc_msgSend(item, sel_registerName("autorelease"));

    return item;
}

void WebView::clear()
{
    printf("[WebView::clear] \n");
    // if (_onmessage) {
    //     // _onmessage->Release();
    //     _onmessage = NULL;
    // }

    // if (_onclose) {
    //     // _onclose->Release();
    //     _onclose = NULL;
    // }

    if (m_ac) {
        m_ac->post(0);
        m_ac = NULL;
    }

    if (m_webview) {
        m_webview = NULL;
    }

    if (s_activeWinObjcId) {
        s_activeWinObjcId = NULL;
        printf("[WebView::clear]s_activeWinObjcId set as NULL \n");
    }

    // if (s_activeWinObjcId == this->priv_windowDelegate)
    //     s_activeWinObjcId = NULL;
}

result_t WebView::setHtml(exlib::string html, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    return 0;
}

result_t WebView::print(int32_t mode, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    return 0;
}

result_t WebView::close(AsyncEvent* ac)
{
    isolate_unref();

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    return 0;
}

static int async_webview_eval(const char* jsscript)
{
    struct webview* w = WebView::getCurrentWebViewStruct();
    if (w != NULL) {
        printf("[async_webview_eval] would eval \n");
        WebView::webview_eval(w, jsscript);
    }

    return 0;
}

result_t WebView::postMessage(exlib::string msg)
{
    printf("[WebView::postMessage] view view argument %s \n", msg.c_str());

    struct webview* w = WebView::getCurrentWebViewStruct();
    if (w != NULL) {
        exlib::string jsstr = "if (this.onmessage) { this.onmessage(";
        // TODO: maybe escape here?
        jsstr.append("\"");
        jsstr.append(msg.c_str());
        jsstr.append("\"");
        jsstr.append("); }");

        printf("[WebView::postMessage] simple WebView::postMessage %s \n", jsstr.c_str());

        asyncCall(async_webview_eval, jsstr.c_str(), CALL_E_GUICALL);
    } else {
        printf("[WebView::postMessage] no s_activeWinObjcId, no webview struct \n");
    }

    return 0;
}

result_t WebView::postMessage(exlib::string msg, AsyncEvent* ac)
{
    if (ac->isSync()) {
        printf("[WebView::postMessage] I would go into CALL_E_GUICALL\n");
        return CHECK_ERROR(CALL_E_GUICALL);
    }

    return postMessage(msg);
}

result_t WebView::get_visible(bool& retVal)
{
    retVal = m_visible;
    return 0;
}

result_t WebView::set_visible(bool newVal)
{
    m_visible = newVal;

    return 0;
}

// ----- Control methods -----

void WebView::GoBack()
{
    // DO REAL THING
    return;
}

void WebView::GoForward()
{
    // DO REAL THING
    return;
}

void WebView::Refresh()
{
    // DO REAL THING
    return;
}

void WebView::Navigate(exlib::string szUrl)
{
    // bstr_t url(UTF8_W(szUrl));
    // variant_t flags(0x02u); //navNoHistory
    // DO REAL THING
    return;
}

result_t WebView::AddRef(void)
{
    Ref();
    return 1;
}

result_t WebView::Release(void)
{
    Unref();
    return 1;
}

extern const wchar_t* g_console_js;
}

#endif /* __APPLE__ */