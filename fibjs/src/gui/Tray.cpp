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

    isolate_ref();
    m_self = new ValueHolder(wrap());
    async([this]() {
        createTray();
        m_ready->set();
    },
        CALL_E_GUICALL);

    return 0;
}

void Tray::release()
{
    if (m_tray) {
        m_tray = nullptr;

        _emit("close");

        m_self.Release();
        isolate_unref();
    }
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

    if (tray->m_options->menu.has_value())
        tray->m_menu = new ValueHolder(tray->m_options->menu.value()->wrap());

    return tray->async_open();
}
}

#endif