
/*
 * dl_appindicator.cpp
 *
 *  Created on: Oct 12, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#if defined(Linux) && defined(OS_DESKTOP)

#include <exlib/include/dl_func.h>
#include "ldconfig.h"

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include "app-indicator.h"

const char* appindicator_str()
{
    static std::string str = ldconfig("appindicator3");
    return str.c_str();
}

static void* appindicator_handle = NULL;
#define appindicator_func(func) dl_def_func(appindicator_handle, func, appindicator_str())

void dl_appindicator_init()
{
}

AppIndicator* app_indicator_new(const gchar* id, const gchar* icon_name, AppIndicatorCategory category)
{
    appindicator_func(app_indicator_new);
    return s_app_indicator_new(id, icon_name, category);
}

void app_indicator_set_icon_full(AppIndicator* self, const gchar* icon_name, const gchar* icon_desc)
{
    appindicator_func(app_indicator_set_icon_full);
    s_app_indicator_set_icon_full(self, icon_name, icon_desc);
}

void app_indicator_set_label(AppIndicator* self, const gchar* label, const gchar* guide)
{
    appindicator_func(app_indicator_set_label);
    s_app_indicator_set_label(self, label, guide);
}

void app_indicator_set_title(AppIndicator* self, const gchar* title)
{
    appindicator_func(app_indicator_set_title);
    s_app_indicator_set_title(self, title);
}

void app_indicator_set_status(AppIndicator* self, AppIndicatorStatus status)
{
    appindicator_func(app_indicator_set_status);
    s_app_indicator_set_status(self, status);
}

void app_indicator_set_menu(AppIndicator* self, GtkMenu* menu)
{
    appindicator_func(app_indicator_set_menu);
    s_app_indicator_set_menu(self, menu);
}

#endif
