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
#include "lib.h"

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
    const char* get_url()
    {
        return m_url.c_str();
    };

private:
    void clear();

public:
    typedef void (*external_invoke_cb_t)(WebView* w, const char* arg);
    id nso;
    // Cocoa infos
    // struct fibjs::webview winfo;
    /* priv about: start */;
    id priv_pool;
    id priv_window;
    id priv_webview;
    id priv_windowDelegate;
    int priv_should_exit;
    /* priv about: end */

    /* webview about :start */
    const char* webview__url;
    const char* webview__title;
    int webview__width;
    int webview__height;
    int webview__resizable;
    int webview__debug;
    WebView::external_invoke_cb_t webview__external_invoke_cb;
    void* webview__userdata;
    /* webview about :end */

    id userController;
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

    WebView::external_invoke_cb_t _onmessage;
    WebView::external_invoke_cb_t _onclose;

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