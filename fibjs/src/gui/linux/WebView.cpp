/*
 * WebView.cpp
 *
 *  Created on: Sep 22, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#if defined(Linux) && defined(OS_DESKTOP)

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <glib.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "object.h"
#include "ifs/gui.h"
#include "ifs/encoding.h"
#include "WebView.h"
#include "EventInfo.h"

namespace fibjs {

result_t WebView::loadURL(exlib::string url, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(m_webview), url.c_str());

    return 0;
}

result_t WebView::getUrl(exlib::string& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    const gchar* uri = webkit_web_view_get_uri(WEBKIT_WEB_VIEW(m_webview));
    retVal = uri;

    return 0;
}

result_t WebView::setHtml(exlib::string html, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    webkit_web_view_load_html(WEBKIT_WEB_VIEW(m_webview), html.c_str(), nullptr);

    return 0;
}

result_t WebView::reload(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    webkit_web_view_reload(WEBKIT_WEB_VIEW(m_webview));

    return 0;
}

result_t WebView::goBack(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    webkit_web_view_go_back(WEBKIT_WEB_VIEW(m_webview));

    return 0;
}

result_t WebView::goForward(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    webkit_web_view_go_forward(WEBKIT_WEB_VIEW(m_webview));

    return 0;
}

result_t WebView::eval(exlib::string code, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    webkit_web_view_run_javascript(WEBKIT_WEB_VIEW(m_webview), code.c_str(), nullptr, nullptr, nullptr);

    return 0;
}

result_t WebView::setTitle(exlib::string title, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    gtk_window_set_title(GTK_WINDOW(m_window), title.c_str());

    return 0;
}

result_t WebView::getTitle(exlib::string& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    const char* title = gtk_window_get_title(GTK_WINDOW(m_window));
    if (title) {
        retVal = title;
    } else {
        retVal = "";
    }

    return 0;
}

result_t WebView::close(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    gtk_window_close(GTK_WINDOW(m_window));

    return 0;
}

result_t WebView::postMessage(exlib::string msg, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    encoding_base::jsstr(msg, false, msg);
    exlib::string jsCode = "window.dispatchEvent(new MessageEvent('message', { data: '" + msg + "' }))";

    webkit_web_view_run_javascript(WEBKIT_WEB_VIEW(m_webview), jsCode.c_str(), nullptr, nullptr, nullptr);

    return 0;
}

}

#endif
