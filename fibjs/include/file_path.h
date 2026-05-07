#pragma once

#include "path.h"
#include "ifs/url.h"

namespace fibjs {

inline bool is_file_url_input(const exlib::string& input)
{
    return input.length() >= 5
        && ((input[0] | 0x20) == 'f')
        && ((input[1] | 0x20) == 'i')
        && ((input[2] | 0x20) == 'l')
        && ((input[3] | 0x20) == 'e')
        && input[4] == ':';
}

inline result_t coerce_file_path_like(exlib::string input, exlib::string& out)
{
    out = input;

    if (!is_file_url_input(input))
        return 0;

    return url_base::fileURLToPath(input, v8::Local<v8::Object>(), out);
}

inline result_t normalize_file_path_like(exlib::string input, exlib::string& out)
{
    result_t hr = coerce_file_path_like(input, out);
    if (hr < 0)
        return hr;

    path_base::normalize(out, out);
    return 0;
}

inline result_t absolute_file_path_like(exlib::string input, exlib::string& out)
{
    result_t hr = normalize_file_path_like(input, out);
    if (hr < 0)
        return hr;

    bool isAbs = false;
    path_base::isAbsolute(out, isAbs);
    if (!isAbs)
        _resolve(out);

    return 0;
}

} // namespace fibjs