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
    if (m_menu_obj) {
        retVal = m_menu_obj;
        return result_t();
    }

    return result_t::return_null();
}

result_t Tray::async_open()
{
    isolate_ref();
    m_self = new ValueHolder(wrap());
    async([this]() {
        createTray();
        m_ready->set();
    },
        result_t::guiloop());

    // The icon exists when the call returns: the creation task runs on the host
    // loop while this fiber is parked.
    m_ready->ac_wait();

    return ensure_ready();
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

result_t gui_base::createTray(TrayOptions* opt, obj_ptr<Tray_base>& retVal)
{
    result_t gui_hr = check_gui_available();
    if (gui_hr.failed())
        return gui_hr;

    if (!opt)
        return result_t(CALL_E_PARAMNOTOPTIONAL);

    obj_ptr<Tray> tray = new Tray();
    retVal = tray;

    tray->m_options = opt;

    result_t hr;
    if (opt->menu.has_value()) {
        obj_ptr<Menu> menu;
        hr = GetArgumentValue(Isolate::current(), opt->menu.value(), menu);
        if (hr.failed())
            return hr;
        tray->m_menu_obj = menu;
    }

    Variant var;
    hr = fs_base::ac_readFile(opt->icon, "", var);
    if (hr.failed())
        return hr;

    tray->m_icon = (Buffer*)var.object();
    if (!tray->m_icon)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("Tray icon is empty");

    if (tray->m_menu_obj)
        tray->m_menu = new ValueHolder(tray->m_menu_obj->wrap());

    return tray->async_open();
}
}

#endif