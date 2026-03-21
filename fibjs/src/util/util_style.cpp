/*
 * util_style.cpp
 *
 *  Created on: Mar 22, 2026
 *      Author: lion
 */

#include "ifs/util.h"
#include "ifs/colors.h"

namespace fibjs {

struct AnsiCode {
    int32_t open;
    int32_t close;
};

static const struct {
    const char* name;
    AnsiCode code;
} s_ansi_codes[] = {
    { "bold", { 1, 22 } },
    { "italic", { 3, 23 } },
    { "underline", { 4, 24 } },
    { "strikethrough", { 9, 29 } },
    { "hidden", { 8, 28 } },
    { "black", { 30, 39 } },
    { "red", { 31, 39 } },
    { "green", { 32, 39 } },
    { "yellow", { 33, 39 } },
    { "blue", { 34, 39 } },
    { "magenta", { 35, 39 } },
    { "cyan", { 36, 39 } },
    { "white", { 37, 39 } },
    { "bgBlack", { 40, 49 } },
    { "bgRed", { 41, 49 } },
    { "bgGreen", { 42, 49 } },
    { "bgYellow", { 43, 49 } },
    { "bgBlue", { 44, 49 } },
    { "bgMagenta", { 45, 49 } },
    { "bgCyan", { 46, 49 } },
    { "bgWhite", { 47, 49 } },
    { "gray", { 90, 39 } },
    { "grey", { 90, 39 } },
    { "blackBright", { 90, 39 } },
    { "redBright", { 91, 39 } },
    { "greenBright", { 92, 39 } },
    { "yellowBright", { 93, 39 } },
    { "blueBright", { 94, 39 } },
    { "magentaBright", { 95, 39 } },
    { "cyanBright", { 96, 39 } },
    { "whiteBright", { 97, 39 } },
};

static const AnsiCode* lookup_ansi(const exlib::string& name)
{
    for (size_t i = 0; i < sizeof(s_ansi_codes) / sizeof(s_ansi_codes[0]); i++)
        if (name == s_ansi_codes[i].name)
            return &s_ansi_codes[i].code;
    return nullptr;
}

static void apply_ansi(const AnsiCode& code, exlib::string& result)
{
    char open_buf[16], close_buf[16];
    snprintf(open_buf, sizeof(open_buf), "\x1b[%dm", code.open);
    snprintf(close_buf, sizeof(close_buf), "\x1b[%dm", code.close);
    result = exlib::string(open_buf) + result + exlib::string(close_buf);
}

result_t util_base::styleText(std::vector<exlib::string>& format, exlib::string text, exlib::string& retVal)
{
    bool hc;
    colors_base::get_hasColors(hc);
    if (!hc) {
        retVal = text;
        return 0;
    }

    exlib::string result = text;
    // Apply in reverse so first format ends up as the outermost wrapper
    for (int32_t i = (int32_t)format.size() - 1; i >= 0; i--) {
        const AnsiCode* code = lookup_ansi(format[i]);
        if (code)
            apply_ansi(*code, result);
    }

    retVal = result;
    return 0;
}

result_t util_base::styleText(exlib::string format, exlib::string text, exlib::string& retVal)
{
    bool hc;
    colors_base::get_hasColors(hc);
    if (!hc) {
        retVal = text;
        return 0;
    }

    exlib::string result = text;
    const AnsiCode* code = lookup_ansi(format);
    if (code)
        apply_ansi(*code, result);

    retVal = result;
    return 0;
}

} /* namespace fibjs */
