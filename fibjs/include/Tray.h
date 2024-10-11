/*
 * Tray.h
 *
 *  Created on: Oct 12, 2024
 *      Author: lion
 */

#pragma once

#include <boost/preprocessor.hpp>

#include "ifs/Tray.h"
#include "Buffer.h"
#include "Event.h"
#include "Menu.h"

namespace fibjs {

class Tray : public Tray_base {
public:
    class OpenOptions : public obj_base {
    public:
        LOAD_OPTIONS(OpenOptions, (icon)(title)(tooltip)(menu));

    public:
        std::optional<exlib::string> icon;
        std::optional<exlib::string> title;
        std::optional<exlib::string> tooltip;
        std::optional<obj_ptr<Menu>> menu;
    };

public:
    Tray()
    {
        m_ready = new Event();
        isolate_ref();
    }

    EVENT_SUPPORT();

public:
    // Tray_base
    virtual result_t getMenu(obj_ptr<Menu_base>& retVal);
    virtual result_t close(AsyncEvent* ac);

public:
    result_t  async_open();
    void createTray();
    void release()
    {
        if (m_tray) {
            m_tray = nullptr;

            _emit("close");

            isolate_unref();
            Unref();
        }
    }

    result_t check_status(AsyncEvent* ac)
    {
        if (ac->isSync()) {
            m_ready->ac_wait();
            return CHECK_ERROR(CALL_E_GUICALL);
        }

        if (!m_tray)
            return Runtime::setError("Tray: tray is closed");

        return 0;
    }

public:
    obj_ptr<OpenOptions> m_options;

    obj_ptr<Buffer> m_icon;

    void* m_tray = nullptr;

    obj_ptr<Event_base> m_ready;
};

}
