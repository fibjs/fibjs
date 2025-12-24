/*
 * path.cpp
 *
 *  Created on: May 30, 2017
 *      Author: lion
 */

#include "object.h"
#include "path.h"

namespace fibjs {

#ifdef _WIN32
DECLARE_MODULE_EX(path, path_win32);

bool path_isAbsolute(exlib::string path)
{
    bool retVal;
    path_win32_base::isAbsolute(path, retVal);
    return retVal;
}
#endif

result_t path_win32_base::normalize(exlib::string path, exlib::string& retVal)
{
    return _normalize_win32(path, retVal);
}

result_t path_win32_base::basename(exlib::string path, exlib::string ext, exlib::string& retVal)
{
    return _basename_win32(path, ext, retVal);
}

result_t path_win32_base::format(v8::Local<v8::Object> pathObject, exlib::string& retVal)
{
    exlib::string sep;
    sep.assign(1, PATH_SLASH_WIN32);
    return _universal_format(sep, pathObject, retVal);
}

result_t path_win32_base::parse(exlib::string path, obj_ptr<NObject>& retVal)
{
    return _parse_win32(path, retVal);
}

result_t path_win32_base::extname(exlib::string path, exlib::string& retVal)
{
    return _extname_win32(path, retVal);
}

result_t path_win32_base::dirname(exlib::string path, exlib::string& retVal)
{
    return _dirname_win32(path, retVal);
}

result_t path_win32_base::fullpath(exlib::string path, exlib::string& retVal)
{
    return _fullpath_win32(path, retVal);
}

result_t path_win32_base::isAbsolute(exlib::string path, bool& retVal)
{
    const char* c_str = path.c_str();

    if (isWin32PathSlash(c_str[0])) {
        retVal = true;
        return 0;
    }

    if (qisascii(c_str[0]) && c_str[1] == ':' && isWin32PathSlash(c_str[2])) {
        retVal = true;
        return 0;
    }

    retVal = false;

    return 0;
}

result_t path_win32_base::join(OptArgs ps, exlib::string& retVal)
{
    return _join_win32(ps, retVal);
}

result_t path_win32_base::resolve(OptArgs ps, exlib::string& retVal)
{
    return _resolve_win32(ps, retVal);
}

result_t path_win32_base::relative(exlib::string _from, exlib::string to, exlib::string& retVal)
{
    return _relative_win32(_from, to, retVal);
}

result_t path_win32_base::toNamespacedPath(v8::Local<v8::Value> path,
    v8::Local<v8::Value>& retVal)
{
    if (!path->IsString()) {
        retVal = path;
        return 0;
    }

    Isolate* isolate = Isolate::current();

    exlib::string str;
    GetArgumentValue(isolate, path, str);

    if (str.length() >= 3) {
        // For paths starting with \\?\, normalize forward slashes to backslashes
        // but don't resolve them as it might change the path structure
        if (str.length() >= 4 && str[0] == '\\' && str[1] == '\\' && str[2] == '?') {
            // Convert forward slashes to backslashes in the path part
            for (size_t i = 4; i < str.length(); i++) {
                if (str[i] == '/') {
                    str[i] = '\\';
                }
            }

#ifdef _WIN32
            // Check if this is a UNC path (\\?\UNC\...)
            bool isUNCPath = (str.length() >= 8 && str.substr(4, 4) == "UNC\\");

            // Check if this is a drive path (\\?\X:\...)
            bool isDrivePath = false;
            if (!isUNCPath && str.length() >= 7) { // \\?\X:\ minimum length
                size_t colonPos = str.find(':', 4);
                if (colonPos != exlib::string::npos && colonPos <= 6) {
                    // This looks like a drive path (\\?\c:\...)
                    isDrivePath = true;
                }
            }

            // Add trailing backslash only for simple device paths (not UNC or drive paths)
            if (!isUNCPath && !isDrivePath && str[str.length() - 1] != '\\') {
                str += '\\';
            }
#endif
            // For paths that already have \\?\ prefix, return after processing
            retVal = GetReturnValue(isolate, str);
            return 0;
        }

        result_t hr = _resolve_win32(str);
        if (hr < 0)
            return hr;

        if (str[0] == '\\' && str[1] == '\\') {
            if (str[2] != '?' && str[2] != '.') {
                str = "\\\\?\\UNC" + str.substr(1);
                retVal = GetReturnValue(isolate, str);
                return 0;
            }
        } else if (qisascii(str[0])) {
            if (str[1] == ':' && str[2] == '\\') {
                str = "\\\\?\\" + str.substr(0);
                retVal = GetReturnValue(isolate, str);
                return 0;
            }
        }
    }

    retVal = path;
    return 0;
}

result_t path_win32_base::get_posix(v8::Local<v8::Object>& retVal)
{
    retVal = path_posix_base::class_info().getModule(Isolate::current());
    return 0;
}

result_t path_win32_base::get_win32(v8::Local<v8::Object>& retVal)
{
    retVal = path_win32_base::class_info().getModule(Isolate::current());
    return 0;
}
}