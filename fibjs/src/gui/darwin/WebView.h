/**
 * @author Richard
 * @email richardo2016@gmail.com
 * @create date 2018-04-23 03:25:07
 * @modify date 2018-04-23 03:25:42
 * @desc WebView Object for Mac OSX
 */
#ifdef __APPLE__

#ifndef WEBVIEW_APPLE_H_
#define WEBVIEW_APPLE_H_

#include "ifs/WebView.h"
#include "darwin_utils.h"

namespace fibjs {

class WebView : public WebView_base {
    FIBER_FREE();

public:
    WebView(exlib::string url, NObject* opt);
    ~WebView();

    result_t open();

    EVENT_SUPPORT();

public:
    // WebView_base
    virtual result_t setHtml(exlib::string html, AsyncEvent* ac);
    virtual result_t print(int32_t mode, AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac);
    virtual result_t get_visible(bool& retVal);
    virtual result_t set_visible(bool newVal);

public:
    EVENT_FUNC(load);
    EVENT_FUNC(move);
    EVENT_FUNC(resize);
    EVENT_FUNC(closed);
    EVENT_FUNC(message);

private:
    void GoBack();
    void GoForward();
    void Refresh();
    void Navigate(exlib::string szUrl);

public:
    result_t AddRef(void);
    result_t Release(void);

public:
    // Message Flow
    // static id GetIDsOfNames(REFIID riid, OLECHAR** rgszNames, unsigned int cNames, LCID lcid, DISPID* rgdispid);
    void Invoke(id self, SEL cmd, id contentController, id message);

public:
    exlib::string get_url()
    {
        return m_url;
    };

private:
    void clear();

public:
    // Cocoa infos
    struct webview * winfo;
    id scriptMessageHandler;
    id downloadDelegate;
    id wkPref;
    id nsTitle;
    id nsURL;
    id navDel;
    id uiDel;
    // @WKWebViewConfiguration
    id wkconfig;
    id processPool;

    CGRect rect;

    webview_external_invoke_cb_t _onmessage;
    webview_external_invoke_cb_t _onclose;

    void terminate();

public:
    // Cocoa internal call
    // void webview_init();
    
    // void RegNSApplication();
    void initDownloadDelegate();
    void initPreference();
    void initUserController();
    void initParentWindow();
    void initNavigation();
    void initWKWebView();
    void finishSetupApplication();
    void finishSetupMenu();

protected:
    exlib::string m_url;
    obj_ptr<NObject> m_opt;

    bool m_visible;
    bool m_maximize;
    bool m_bSilent;
    bool m_isdebug;

    AsyncEvent* m_ac;
};
} /* namespace fibjs */

#endif /* WEBVIEW_APPLE_H_ */
#endif /* __APPLE__ */