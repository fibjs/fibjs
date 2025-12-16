/*
 * colors.cpp
 *
 *  Created on: Dec 18, 2024
 *      Author: lion
 */

#include "object.h"
#include "ifs/colors.h"
#include "ifs/tty.h"
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#define _fileno fileno
#endif

namespace fibjs {

DECLARE_MODULE(colors);

exlib::string COLOR_RESET = "";
exlib::string COLOR_BLACK = "";
exlib::string COLOR_RED = "";
exlib::string COLOR_GREEN = "";
exlib::string COLOR_YELLOW = "";
exlib::string COLOR_BLUE = "";
exlib::string COLOR_MAGENTA = "";
exlib::string COLOR_CYAN = "";
exlib::string COLOR_WHITE = "";
exlib::string COLOR_NORMAL = "";
exlib::string COLOR_GRAY = "";

exlib::string COLOR_LIGHTRED = "";
exlib::string COLOR_LIGHTGREEN = "";
exlib::string COLOR_LIGHTYELLOW = "";
exlib::string COLOR_LIGHTBLUE = "";
exlib::string COLOR_LIGHTMAGENTA = "";
exlib::string COLOR_LIGHTCYAN = "";
exlib::string COLOR_LIGHTWHITE = "";

exlib::string COLOR_BOLD = "";

static bool hasColors = false;

class color_initer {
public:
    color_initer()
    {
        hasColors = false;
        tty_base::isatty(_fileno(stdout), hasColors);

        char buf[4096];
        size_t sz = sizeof(buf);
        if (uv_os_getenv("NO_COLOR", buf, &sz) == 0) {
            if (buf[0] != '\0')
                hasColors = false;

            uv_os_unsetenv("NO_COLOR");
        }

        sz = sizeof(buf);
        if (uv_os_getenv("FORCE_COLOR", buf, &sz) == 0) {
            if (buf[0] != '\0')
                hasColors = true;

            uv_os_unsetenv("FORCE_COLOR");
        }

        if (hasColors) {
            COLOR_RESET = "\x1b[0m";
            COLOR_BLACK = "\x1b[0;30m"; /* Black */
            COLOR_RED = "\x1b[0;31m"; /* Red */
            COLOR_GREEN = "\x1b[0;32m"; /* Green */
            COLOR_YELLOW = "\x1b[0;33m"; /* Yellow */
            COLOR_BLUE = "\x1b[0;34m"; /* Blue */
            COLOR_MAGENTA = "\x1b[0;35m"; /* Magenta */
            COLOR_CYAN = "\x1b[0;36m"; /* Cyan */
            COLOR_WHITE = "\x1b[0;37m"; /* White */
            COLOR_NORMAL = "\x1b[0;39m"; /* Normal */
            COLOR_GRAY = "\x1B[90m"; /* Grey */

            COLOR_LIGHTRED = "\x1b[1;31m"; /* Red */
            COLOR_LIGHTGREEN = "\x1b[1;32m"; /* Green */
            COLOR_LIGHTYELLOW = "\x1b[1;33m"; /* Yellow */
            COLOR_LIGHTBLUE = "\x1b[1;34m"; /* Blue */
            COLOR_LIGHTMAGENTA = "\x1b[1;35m"; /* Magenta */
            COLOR_LIGHTCYAN = "\x1b[1;36m"; /* Cyan */
            COLOR_LIGHTWHITE = "\x1b[1;37m"; /* White */

            COLOR_BOLD = "\x1B[1;39m";
        }
    }
} s_color_initer;

result_t colors_base::get_hasColors(bool& retVal)
{
    retVal = hasColors;
    return 0;
}

result_t colors_base::get_clear(exlib::string& retVal)
{
    retVal = COLOR_RESET;
    return 0;
}

result_t colors_base::get_normal(exlib::string& retVal)
{
    retVal = COLOR_NORMAL;
    return 0;
}

result_t colors_base::get_black(exlib::string& retVal)
{
    retVal = COLOR_BLACK;
    return 0;
}

result_t colors_base::get_gray(exlib::string& retVal)
{
    retVal = COLOR_GRAY;
    return 0;
}

result_t colors_base::get_red(exlib::string& retVal)
{
    retVal = COLOR_RED;
    return 0;
}

result_t colors_base::get_green(exlib::string& retVal)
{
    retVal = COLOR_GREEN;
    return 0;
}

result_t colors_base::get_yellow(exlib::string& retVal)
{
    retVal = COLOR_YELLOW;
    return 0;
}

result_t colors_base::get_blue(exlib::string& retVal)
{
    retVal = COLOR_BLUE;
    return 0;
}

result_t colors_base::get_magenta(exlib::string& retVal)
{
    retVal = COLOR_MAGENTA;
    return 0;
}

result_t colors_base::get_cyan(exlib::string& retVal)
{
    retVal = COLOR_CYAN;
    return 0;
}

result_t colors_base::get_white(exlib::string& retVal)
{
    retVal = COLOR_WHITE;
    return 0;
}

result_t colors_base::get_lightred(exlib::string& retVal)
{
    retVal = COLOR_LIGHTRED;
    return 0;
}

result_t colors_base::get_lightgreen(exlib::string& retVal)
{
    retVal = COLOR_LIGHTGREEN;
    return 0;
}

result_t colors_base::get_lightyellow(exlib::string& retVal)
{
    retVal = COLOR_LIGHTYELLOW;
    return 0;
}

result_t colors_base::get_lightblue(exlib::string& retVal)
{
    retVal = COLOR_LIGHTBLUE;
    return 0;
}

result_t colors_base::get_lightmagenta(exlib::string& retVal)
{
    retVal = COLOR_LIGHTMAGENTA;
    return 0;
}

result_t colors_base::get_lightcyan(exlib::string& retVal)
{
    retVal = COLOR_LIGHTCYAN;
    return 0;
}

result_t colors_base::get_lightwhite(exlib::string& retVal)
{
    retVal = COLOR_LIGHTWHITE;
    return 0;
}

result_t colors_base::get_bold(exlib::string& retVal)
{
    retVal = COLOR_BOLD;
    return 0;
}

} // namespace fibjs
