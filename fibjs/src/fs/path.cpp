/*
 * path.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "ifs/process.h"
#include "utf8.h"
#include <sstream>
#include "path.h"

namespace fibjs
{

DECLARE_MODULE(path);

result_t path_base::basename(const char *path, const char *ext,
                             qstring &retVal)
{
    char ch;
    const char *p1 = path;
    int32_t extlen = (int32_t)qstrlen(ext);

    while (*path)
    {
        ch = *path++;
        if (isPathSlash(ch))
            p1 = path;
    }

    if (extlen && ((int32_t) (path - p1) >= extlen)
            && !pathcmp(ext, path - extlen, extlen))
        path -= extlen;

    retVal.assign(p1, (int32_t) (path - p1));

    return 0;
}

result_t path_base::extname(const char *path, qstring &retVal)
{
    char ch;
    const char *p1 = NULL;

    if (*path == '.')
        path++;

    while (*path)
    {
        ch = *path++;
        if (isPathSlash(ch))
        {
            if (*path == '.')
                path++;
            p1 = NULL;
        }
        else if (ch == '.')
            p1 = path - 1;
    }

    if (p1)
        retVal.assign(p1, (int32_t) (path - p1));

    return 0;
}

result_t path_base::dirname(const char *path, qstring &retVal)
{
    char ch;
    const char *p1 = path;
    const char *p2 = NULL;

#ifdef _WIN32
    if (path[0] != 0 && path[1] == ':')
    {
        path += 2;
        if (isPathSlash(*path))
            path++;
        p2 = path;
    }
    else if (isPathSlash(path[0]) && isPathSlash(path[1]))
    {
        path += 2;
        while (*path && !isPathSlash(*path))
            path++;

        if (*path)
        {
            path++;

            while (*path && !isPathSlash(*path))
                path++;

            if (*path)
                path++;
        }

        p2 = path;
    }
#endif

    while (*path)
    {
        ch = *path++;
        if (isPathSlash(ch) && *path)
            p2 = path - 1;
    }

    if (p2 == NULL)
        p2 = p1;

    if (isPathSlash(*p2) && p2 == p1)
        p2++;

    retVal.assign(p1, (int32_t) (p2 - p1));

    return 0;
}

inline char fixChar(char ch)
{
    return isPathSlash(ch) ? PATH_SLASH : ch;
}

result_t path_base::normalize(const char *path, qstring &retVal)
{
    qstring str;
    const char *p1 = path;
    char *pstr;
    int32_t pos = 0;
    int32_t root = 0;
    bool bRoot = false;

    str.resize(qstrlen(p1));
    pstr = &str[0];

#ifdef _WIN32
    if (p1[0] != 0 && p1[1] == ':')
    {
        pstr[pos++] = p1[0];
        pstr[pos++] = ':';
        p1 += 2;

        if (isPathSlash(*p1))
        {
            pstr[pos++] = PATH_SLASH;
            p1++;
            bRoot = true;
        }
    }
    else if (isPathSlash(p1[0]) && isPathSlash(p1[1]))
    {
        pstr[pos++] = PATH_SLASH;
        pstr[pos++] = PATH_SLASH;
        p1 += 2;

        while (*p1 && !isPathSlash(*p1))
            pstr[pos++] = *p1++;

        if (*p1)
        {
            pstr[pos++] = PATH_SLASH;
            p1++;

            while (*p1 && !isPathSlash(*p1))
                pstr[pos++] = *p1++;

            if (*p1)
            {
                pstr[pos++] = PATH_SLASH;
                p1++;
                bRoot = true;
            }
        }
    }
    else
#endif
        if (isPathSlash(p1[0]))
        {
            pstr[pos++] = PATH_SLASH;
            p1++;
            bRoot = true;
        }

    root = pos;

    while (*p1)
    {
        if (isPathSlash(p1[0]))
        {
            p1++;
        }
        else if (p1[0] == '.' && (!p1[1] || isPathSlash(p1[1])))
        {
            p1 += p1[1] ? 2 : 1;
        }
        else if ((p1[0] == '.') && (p1[1] == '.') && (!p1[2] || isPathSlash(p1[2])))
        {
            if (pos > root)
            {
                if ((pstr[pos - 2] == '.') && (pstr[pos - 3] == '.')
                        && ((root == pos - 3) || (pstr[pos - 4] == PATH_SLASH)))
                {
                    pstr[pos++] = '.';
                    pstr[pos++] = '.';
                    pstr[pos++] = PATH_SLASH;
                }
                else
                {
                    pos--;
                    while (pos > root && !isPathSlash(pstr[pos - 1]))
                        pos--;
                }
            }
            else if (!bRoot)
            {
                pstr[pos++] = '.';
                pstr[pos++] = '.';
                pstr[pos++] = PATH_SLASH;
            }

            p1 += p1[2] ? 3 : 2;
        }
        else
        {
            while (*p1 && !isPathSlash(*p1))
                pstr[pos++] = *p1++;
            if (*p1)
            {
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

result_t path_base::join(const v8::FunctionCallbackInfo<v8::Value> &args, qstring &retVal)
{
    qstring strBuffer;
    int32_t argc = args.Length();
    int32_t i;

    for (i = 0; i < argc; i++)
    {
        v8::String::Utf8Value s(args[i]);
        pathAdd(strBuffer, *s);
    }

    return normalize(strBuffer.c_str(), retVal);
}

result_t path_base::fullpath(const char *path, qstring &retVal)
{
#ifdef _WIN32
    wstring str = utf8to16String(path);
    wchar utf16_buffer[MAX_PATH];

    DWORD utf16_len = GetFullPathNameW(str.c_str(), MAX_PATH, utf16_buffer, NULL);
    if (!utf16_len)
        return CHECK_ERROR(LastError());

    retVal = utf16to8String(utf16_buffer, (int32_t)utf16_len);
    return 0;
#else
    if (isPathSlash(path[0]))
        return normalize(path, retVal);

    qstring str;

    process_base::cwd(str);
    str.append(1, PATH_SLASH);
    str.append(path);

    return normalize(str.c_str(), retVal);
#endif
}

result_t path_base::get_sep(qstring &retVal)
{
    retVal.clear();
    retVal.append(1, PATH_SLASH);
    return 0;
}

result_t path_base::get_delimiter(qstring &retVal)
{
    retVal.clear();
    retVal.append(1, PATH_DELIMITER);
    return 0;
}

}
