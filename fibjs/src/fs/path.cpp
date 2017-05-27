/*
 * path.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/process.h"
#include "PathPosix.h"
#include "PathWin32.h"
#include "utf8.h"
#include <sstream>
#include "path.h"

namespace fibjs {

DECLARE_MODULE(path);

inline char fixChar(char ch)
{
    return isPathSlash(ch) ? PATH_SLASH : ch;
}

const char* split_path(const char* p)
{
    char ch;
    const char* p1 = p;
    const char* p2 = NULL;

    if (p[0] != 0 && p[1] == ':') {
        p += 2;
        if (isPathSlash(*p))
            p++;
        p2 = p;
    } else if (isPathSlash(p[0]) && isPathSlash(p[1])) {
        p += 2;
        while (*p && !isPathSlash(*p))
            p++;

        if (*p) {
            p++;

            while (*p && !isPathSlash(*p))
                p++;

            if (*p)
                p++;
        }

        p2 = p;
    }

    while (*p) {
        ch = *p++;
        if (isPathSlash(ch) && *p)
            p2 = p - 1;
    }

    if (p2 == NULL)
        p2 = p1;

    if (isPathSlash(*p2) && p2 == p1)
        p2++;

    return p2;
}

inline result_t _normalize(exlib::string path, exlib::string& retVal)
{
    exlib::string str;
    const char* p1 = path.c_str();
    char* pstr;
    int32_t pos = 0;
    int32_t root = 0;
    bool bRoot = false;

    str.resize(qstrlen(p1));
    pstr = &str[0];

    if (p1[0] != 0 && p1[1] == ':') {
        pstr[pos++] = p1[0];
        pstr[pos++] = ':';
        p1 += 2;

        if (isPathSlash(*p1)) {
            pstr[pos++] = PATH_SLASH;
            p1++;
            bRoot = true;
        }
    } else if (isPathSlash(p1[0]) && isPathSlash(p1[1])) {
        pstr[pos++] = PATH_SLASH;
        pstr[pos++] = PATH_SLASH;
        p1 += 2;

        while (*p1 && !isPathSlash(*p1))
            pstr[pos++] = *p1++;

        if (*p1) {
            pstr[pos++] = PATH_SLASH;
            p1++;

            while (isPathSlash(*p1))
                p1++;

            while (*p1 && !isPathSlash(*p1))
                pstr[pos++] = *p1++;

            if (*p1) {
                pstr[pos++] = PATH_SLASH;
                p1++;
                bRoot = true;
            }
        }
    } else if (isPathSlash(p1[0])) {
        pstr[pos++] = PATH_SLASH;
        p1++;
        bRoot = true;
    }

    root = pos;

    while (*p1) {
        if (isPathSlash(p1[0])) {
            p1++;
        } else if (p1[0] == '.' && (!p1[1] || isPathSlash(p1[1]))) {
            p1 += p1[1] ? 2 : 1;
        } else if ((p1[0] == '.') && (p1[1] == '.') && (!p1[2] || isPathSlash(p1[2]))) {
            if (pos > root) {
                if ((pstr[pos - 2] == '.') && (pstr[pos - 3] == '.')
                    && ((root == pos - 3) || (pstr[pos - 4] == PATH_SLASH))) {
                    pstr[pos++] = '.';
                    pstr[pos++] = '.';
                    pstr[pos++] = PATH_SLASH;
                } else {
                    pos--;
                    while (pos > root && !isPathSlash(pstr[pos - 1]))
                        pos--;
                }
            } else if (!bRoot) {
                pstr[pos++] = '.';
                pstr[pos++] = '.';
                pstr[pos++] = PATH_SLASH;
            }

            p1 += p1[2] ? 3 : 2;
        } else {
            while (*p1 && !isPathSlash(*p1))
                pstr[pos++] = *p1++;
            if (*p1) {
                p1++;
                pstr[pos++] = PATH_SLASH;
            }
        }
    }

    if (pos > root && isPathSlash(pstr[pos - 1]))
        pos--;

    str.resize(pos);

    retVal = str;

    return 0;
}

inline result_t _basename(exlib::string path, exlib::string ext,
    exlib::string& retVal)
{
    const char* p1 = path.c_str();
    const char* p2 = split_path(p1);
    const char* p3 = p1 + path.length();
    int32_t extlen = (int32_t)ext.length();

    while (isPathSlash(*p2))
        p2++;

    while (p3 > p2 && isPathSlash(*(p3 - 1)))
        p3--;

    if (extlen && ((int32_t)(p3 - p2) >= extlen)
        && !pathcmp(ext.c_str(), p3 - extlen, extlen))
        p3 -= extlen;

    retVal.assign(p2, (int32_t)(p3 - p2));

    return 0;
}

inline result_t _extname(exlib::string path, exlib::string& retVal)
{
    char ch;
    const char* p1 = NULL;
    const char* c_str = path.c_str();

    if (*c_str == '.')
        c_str++;

    while (*c_str) {
        ch = *c_str++;
        if (isPathSlash(ch)) {
            if (*c_str == '.')
                c_str++;
            p1 = NULL;
        } else if (ch == '.')
            p1 = c_str - 1;
    }

    if (p1)
        retVal.assign(p1, (int32_t)(c_str - p1));

    return 0;
}

inline result_t _dirname(exlib::string path, exlib::string& retVal)
{
    const char* p1 = path.c_str();
    const char* p2 = split_path(p1);

    retVal.assign(p1, (int32_t)(p2 - p1));

    return 0;
}

inline result_t _join(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal)
{
    Path p;
    int32_t argc = args.Length();
    int32_t i;

    for (i = 0; i < argc; i++) {
        v8::String::Utf8Value s(args[i]);
        p.append(*s);
    }

    return _normalize(p.str(), retVal);
}

inline result_t _resolve(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal)
{
    exlib::string str;
    process_base::cwd(str);

    Path p(str);
    int32_t argc = args.Length();
    int32_t i;

    for (i = 0; i < argc; i++) {
        v8::String::Utf8Value s(args[i]);
        p.append(*s);
    }

    return _normalize(p.str(), retVal);
}

inline result_t _sep(exlib::string& retVal)
{
    retVal.assign(1, PATH_SLASH);
    return 0;
}

inline result_t _delimiter(exlib::string& retVal)
{
    retVal.assign(1, PATH_DELIMITER);
    return 0;
}

result_t path_base::normalize(exlib::string path, exlib::string& retVal)
{
    return _normalize(path, retVal);
}

result_t path_base::basename(exlib::string path, exlib::string ext,
    exlib::string& retVal)
{
    return _basename(path, ext, retVal);
}

result_t path_base::extname(exlib::string path, exlib::string& retVal)
{
    return _extname(path, retVal);
}

result_t path_base::dirname(exlib::string path, exlib::string& retVal)
{
    return _dirname(path, retVal);
}

extern result_t _fullpath_win32(exlib::string path, exlib::string& retVal);
extern result_t _fullpath_posix(exlib::string path, exlib::string& retVal);

result_t path_base::fullpath(exlib::string path, exlib::string& retVal)
{
#ifdef _WIN32
    return _fullpath_win32(path, retVal);
#else
    return _fullpath_posix(path, retVal);
#endif
}

result_t path_base::join(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal)
{
    return _join(args, retVal);
}

result_t path_base::resolve(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal)
{
    return _resolve(args, retVal);
}

result_t path_base::get_sep(exlib::string& retVal)
{
    return _sep(retVal);
}

result_t path_base::get_delimiter(exlib::string& retVal)
{
    return _delimiter(retVal);
}

result_t path_base::get_posix(obj_ptr<PathPosix_base>& retVal)
{
    static obj_ptr<PathPosix_base> obj;
    if (!obj) {
        PathPosix_base::_new(obj);
    }
    retVal = obj;
    return 0;
}

result_t path_base::get_win32(obj_ptr<PathWin32_base>& retVal)
{
    static obj_ptr<PathWin32_base> obj;
    if (!obj) {
        PathWin32_base::_new(obj);
    }
    retVal = obj;
    return 0;
}
}
