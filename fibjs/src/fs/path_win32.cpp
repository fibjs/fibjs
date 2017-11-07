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
#endif

result_t path_win32_base::normalize(exlib::string path, exlib::string& retVal)
{
    return _normalize_win32(path, retVal);
}

result_t path_win32_base::basename(exlib::string path, exlib::string ext, exlib::string& retVal)
{
    return _basename_win32(path, ext, retVal);
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

result_t path_win32_base::toNamespacedPath(v8::Local<v8::Value> path,
    v8::Local<v8::Value>& retVal)
{
    if (!path->IsString()) {
        retVal = path;
        return 0;
    }

    exlib::string str;
    GetArgumentValue(path, str);

    if (str.length() >= 3) {
        Isolate* isolate = Isolate::current();
        Path p;
        p.resolveWin32(str);
        result_t hr = normalize(p.str(), str);
        if (hr < 0)
            return hr;
        if (str[0] == '\\' && str[1] == '\\') {
            if (str[2] != '?' && str[2] != '.') {
                str = "\\\\?\\UNC\\" + str.substr(2);
                retVal = GetReturnValue(isolate->m_isolate, str);
                return 0;
            }
        } else if (qisascii(str[0])) {
            if (str[1] == ':' && str[2] == '\\') {
                str = "\\\\?\\" + str.substr(0);
                retVal = GetReturnValue(isolate->m_isolate, str);
                return 0;
            }
        }
    }

    retVal = path;
    return 0;
}

result_t path_win32_base::get_sep(exlib::string& retVal)
{
    retVal.assign(1, PATH_SLASH_WIN32);
    return 0;
}

result_t path_win32_base::get_delimiter(exlib::string& retVal)
{
    retVal.assign(1, PATH_DELIMITER_WIN32);
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