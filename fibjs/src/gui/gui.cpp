/*
 * gui.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#ifdef OS_DESKTOP

#include "object.h"
#include "ifs/gui.h"

#include "WebView.h"

namespace fibjs {

DECLARE_MODULE(gui);

static exlib::Event s_gui;
static exlib::Event s_gui_ready;

void run_gui(int argc, char* argv[])
{
    exlib::OSThread th;
    th.bindCurrent();

    Runtime rt(NULL);

    s_gui.wait();
    WebView::run_os_gui(s_gui_ready);
}

result_t WebView::open(NObject* opt, obj_ptr<WebView_base>& retVal)
{
    s_gui.set();
    s_gui_ready.wait();

    obj_ptr<WebView> w = new WebView(opt);
    w->wrap();

    w->Ref();
    asyncCall([](WebView* w) {
        w->open();
        w->Unref();
    },
        w, CALL_E_GUICALL);
    retVal = w;

    return 0;
}

result_t gui_base::open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    obj_ptr<NObject> o = new NObject();
    o->add(opt);
    o->add("url", url);

    return WebView::open(o, retVal);
}

result_t gui_base::openFile(exlib::string file, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    obj_ptr<NObject> o = new NObject();
    o->add(opt);
    o->add("file", file);
    o->remove("url");

    return WebView::open(o, retVal);
}

result_t gui_base::open(v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    obj_ptr<NObject> o = new NObject();
    o->add(opt);

    return WebView::open(o, retVal);
}

}

#endif
