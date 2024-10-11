/*
 * dl_init.cpp
 *
 *  Created on: Oct 11, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#if defined(Linux) && defined(OS_DESKTOP)

extern void dl_gobj_init();
extern void dl_gtk_init();
extern void dl_webkit_init();
extern void dl_js_init();

void dl_init()
{
    dl_gobj_init();
    dl_gtk_init();
    dl_webkit_init();
    dl_js_init();
}

#endif
