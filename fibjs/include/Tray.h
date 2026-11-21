/*
 * Tray.h
 *
 *  Created on: Oct 12, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/Tray.h"
#include "ifs/TrayOptions.h"
#include "Buffer.h"
#include "Event.h"
#include "Menu.h"

namespace fibjs {

class Tray : public Tray_base {
public:
    Tray()
    {
        m_ready = new Event();
    }

public:
    // Tray_base
    virtual result_t getMenu(obj_ptr<Menu_base>& retVal);
    virtual result_t close();

public:
    result_t async_open();
    void createTray();
    void release();

    result_t ensure_ready()
    {
        // createTray blocks until the native icon exists, so a programmatic
        // tray is ready here; a released one is not.
        if (!m_tray)
            return result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code("ERR_GUI_CLOSED").with_message("Tray: tray is closed");

        return result_t();
    }

public:
    obj_ptr<TrayOptions> m_options;

    obj_ptr<ValueHolder> m_self;
    obj_ptr<ValueHolder> m_menu;
    obj_ptr<Menu> m_menu_obj;

    obj_ptr<Buffer> m_icon;

    void* m_tray = nullptr;

    obj_ptr<Event_base> m_ready;
};

}
