/*
 * Tray.cpp
 *
 *  Created on: Oct 12, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#if defined(Linux) && defined(OS_DESKTOP)

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <glib.h>
#include <gtk/gtk.h>
#include "dl/app-indicator.h"

#include "object.h"
#include "ifs/gui.h"
#include "Tray.h"
#include "EventInfo.h"

namespace fibjs {

static void onClick(GtkStatusIcon* status_icon, gpointer user_data)
{
    Tray* _tray = static_cast<Tray*>(user_data);
    obj_ptr<EventInfo> ei = new EventInfo(_tray, "click");
    _tray->_emit("click", ei);
}

std::string calculateHash(const uint8_t* data, size_t length)
{
    std::hash<std::string_view> hasher;
    size_t hash = hasher(std::string_view((const char*)data, length));
    return std::to_string(hash);
}

void Tray::createTray()
{
    std::string tempFileName = "/tmp/fibjs_tray_icon_" + calculateHash(m_icon->data(), m_icon->length());
    int fd = open(tempFileName.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
    if (fd != -1) {
        write(fd, m_icon->data(), m_icon->length());
        ::close(fd);
    }

    AppIndicator* tray;
    m_tray = tray = app_indicator_new("example-simple-client", "indicator-messages", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);

    app_indicator_set_icon_full(tray, tempFileName.c_str(), "Tray Icon");

    if (m_options->title.has_value())
        app_indicator_set_label(tray, m_options->title.value().c_str(), NULL);

    if (m_options->tooltip.has_value())
        app_indicator_set_title(tray, m_options->tooltip.value().c_str());

    app_indicator_set_status(tray, APP_INDICATOR_STATUS_ACTIVE);

    if (m_options->menu.has_value()) {
        GtkWidget* menu = (GtkWidget*)m_options->menu.value()->create_os_menu();
        gtk_widget_show_all(menu);
        app_indicator_set_menu(tray, GTK_MENU(menu));
    }
}

result_t Tray::close(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    app_indicator_set_status((AppIndicator*)m_tray, APP_INDICATOR_STATUS_PASSIVE);
    g_object_unref(G_OBJECT(m_tray));
    release();

    return 0;
}

} // namespace fibjs

#endif // OS_DESKTOP