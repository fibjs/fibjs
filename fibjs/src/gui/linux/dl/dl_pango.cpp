
/*
 * dl_pango.cpp
 *
 *  Created on: Sep 21, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#if defined(Linux) && defined(OS_DESKTOP)

#include <exlib/include/dl_func.h>
#include "ldconfig.h"

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <pango/pango.h>

const char* pango_str()
{
    static std::string str = ldconfig("pango-1.");
    return str.c_str();
}

static void* pango_handle = NULL;
#define pango_func(func) dl_def_func(pango_handle, func, pango_str())

void dl_pango_init()
{
}

PangoFontDescription* pango_font_description_copy(const PangoFontDescription* desc)
{
    pango_func(pango_font_description_copy);
    return s_pango_font_description_copy(desc);
}

int pango_font_description_get_size(const PangoFontDescription* desc)
{
    pango_func(pango_font_description_get_size);
    return s_pango_font_description_get_size(desc);
}

void pango_font_description_set_size(PangoFontDescription* desc, int size)
{
    pango_func(pango_font_description_set_size);
    s_pango_font_description_set_size(desc, size);
}

void pango_font_description_free(PangoFontDescription* desc)
{
    pango_func(pango_font_description_free);
    s_pango_font_description_free(desc);
}

#endif
