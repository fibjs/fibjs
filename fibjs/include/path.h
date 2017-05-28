/*
 * path.h
 *
 *  Created on: Dec 7, 2015
 *      Author: lion
 */

#ifndef _PATH_UTIL_H_
#define _PATH_UTIL_H_

#include "ifs/path.h"
#include "ifs/process.h"

namespace fibjs {

#ifdef _WIN32

#define pathcmpchr qchricmp
#define pathcmp qstricmp

#else

#define pathcmp qstrcmp

inline bool pathcmpchr(char ch1, char ch2)
{
    return ch1 - ch2;
}

#endif

class Path {
public:
    Path()
    {
    }

    Path(exlib::string init)
    {
        append(init);
    }

public:
#ifdef _WIN32
    void append(exlib::string other)
    {
        if (other.empty())
            return;

        const char* c_str = other.c_str();

        if (isPathSlash(c_str[0]) && isPathSlash(c_str[1])) {
            m_buf = other;
            return;
        }

        char diskId = 0;

        if (qisascii(c_str[0]) && c_str[1] == ':') {
            diskId = c_str[0];
            m_buf = m_disks[diskId & 0x1f];
            c_str += 2;
        }

        bool is_root = isPathSlash(c_str[0]);
        exlib::string base;
        const char* c_strBuf = m_buf.c_str();
        if (*c_strBuf) {
            if (qisascii(c_strBuf[0]) && c_strBuf[1] == ':') {
                diskId = c_strBuf[0];

                if (!is_root)
                    base = m_buf.substr(2);
            } else if (!is_root)
                base = m_buf;
        }

        if (is_root || base.empty())
            base = c_str;
        else {
            if (!isPathSlash(base.c_str()[base.length() - 1]))
                base.append(1, PATH_SLASH);

            base.append(c_str);
        }

        if (diskId) {
            m_buf.assign(1, diskId);
            m_buf.append(1, ':');
            m_buf.append(base);

            m_disks[diskId & 0x1f] = m_buf;
        } else
            m_buf = base;
    }

private:
    exlib::string m_disks[27];

#else
    void append(exlib::string other)
    {
        if (other.empty())
            return;

        const char* c_str = other.c_str();

        if (isPathSlash(c_str[0]) || m_buf.empty())
            m_buf = other;
        else {
            if (!isPathSlash(m_buf.c_str()[m_buf.length() - 1]))
                m_buf.append(1, PATH_SLASH);

            m_buf.append(other);
        }
    }

#endif

public:
    exlib::string str()
    {
        return m_buf;
    }

private:
    exlib::string m_buf;
};

inline void pathAdd(exlib::string& strBuffer, exlib::string other)
{
    Path p(strBuffer);

    p.append(other);
    strBuffer = p.str();
}

inline char fixChar(char ch)
{
    return isPathSlash(ch) ? PATH_SLASH : ch;
}

inline const char* split_path(const char* p)
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

inline result_t _fullpath_posix(exlib::string path, exlib::string& retVal)
{
    if (isPathSlash(path.c_str()[0]))
        return _normalize(path, retVal);

    exlib::string str;

    process_base::cwd(str);
    str.append(1, PATH_SLASH);
    str.append(path);

    return _normalize(str, retVal);
}

inline result_t _fullpath_win32(exlib::string path, exlib::string& retVal)
{
#ifdef _WIN32
    exlib::wstring str = utf8to16String(path);
    exlib::wchar utf16_buffer[MAX_PATH];

    DWORD utf16_len = GetFullPathNameW(str.c_str(), MAX_PATH, utf16_buffer, NULL);
    if (!utf16_len)
        return CHECK_ERROR(LastError());

    retVal = utf16to8String(utf16_buffer, (int32_t)utf16_len);
#else
    ThrowError("not supported on none Win32 platform !");
#endif
    return 0;
}

} /* namespace fibjs */

#endif /* _PATH_UTIL_H_ */
