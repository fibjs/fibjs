/*
 * Tray.cpp
 *
 *  Created on: Nov 2, 2024
 *      Author: lion
 */

#ifdef OS_DESKTOP

#include "object.h"
#include "ifs/gui.h"
#include "gui.h"

#include "Tray.h"

namespace fibjs {

result_t Tray::getMenu(obj_ptr<Menu_base>& retVal)
{
    if (m_options->menu.has_value()) {
        retVal = m_options->menu.value();
        return 0;
    }

    return CALL_RETURN_NULL;
}

result_t Tray::async_open()
{
    start_gui();

    wrap();

    Ref();
    async([this]() {
        createTray();
        m_ready->set();

        Unref();
    },
        CALL_E_GUICALL);

    return 0;
}

result_t gui_base::createTray(v8::Local<v8::Object> opt, obj_ptr<Tray_base>& retVal)
{
    obj_ptr<Tray> tray = new Tray();
    retVal = tray;

    Isolate* isolate = Isolate::current(opt);
    result_t hr = Tray::OpenOptions::load(isolate, opt, tray->m_options);
    if (hr < 0)
        return hr;

    if (!tray->m_options->icon.has_value())
        return Runtime::setError("Tray icon is required");

    Variant var;
    hr = fs_base::ac_readFile(tray->m_options->icon.value(), "", var);
    if (hr)
        return hr;

    tray->m_icon = (Buffer*)var.object();
    if (!tray->m_icon)
        return Runtime::setError("Tray icon is empty");

    return tray->async_open();
}
}

#endif