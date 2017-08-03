/*
 * path.h
 *
 *  Created on: Dec 7, 2015
 *      Author: lion
 */

#ifndef _PATH_UTIL_H_
#define _PATH_UTIL_H_

#include "ifs/path_posix.h"
#include "ifs/path_win32.h"
#include "ifs/process.h"
#include "utf8.h"

namespace fibjs {

#ifdef _WIN32
typedef path_win32_base path_base;
#else
typedef path_posix_base path_base;
#endif

class Path {
public:
    Path()
    {
    }

public:
    void resolveWin32(exlib::string other)
    {
        if (other.empty())
            return;

        const char* c_str = other.c_str();

        if (isWin32PathSlash(c_str[0]) && isWin32PathSlash(c_str[1])) {
            m_buf = other;
            return;
        }

        char diskId = 0;

        if (qisascii(c_str[0]) && c_str[1] == ':') {
            diskId = c_str[0];
            m_buf = m_disks[diskId & 0x1f];
            c_str += 2;
        }

        bool is_root = isWin32PathSlash(c_str[0]);
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
            if (!isWin32PathSlash(base.c_str()[base.length() - 1]))
                base.append(1, PATH_SLASH_WIN32);

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

    void resolvePosix(exlib::string other)
    {
        if (other.empty())
            return;

        const char* c_str = other.c_str();

        if (isPosixPathSlash(c_str[0]) || m_buf.empty())
            m_buf = other;
        else {
            if (!isPosixPathSlash(m_buf.c_str()[m_buf.length() - 1]))
                m_buf.append(1, PATH_SLASH_POSIX);

            m_buf.append(other);
        }
    }

    void joinWin32(exlib::string other)
    {
        if (other.empty())
            return;

        if (m_buf.length() > 0 && !isWin32PathSlash(m_buf.c_str()[m_buf.length() - 1]))
            m_buf.append(1, PATH_SLASH_WIN32);

        m_buf.append(other);
    }

    void joinPosix(exlib::string other)
    {
        if (other.empty())
            return;

        if (m_buf.length() > 0 && !isPosixPathSlash(m_buf.c_str()[m_buf.length() - 1]))
            m_buf.append(1, PATH_SLASH_POSIX);

        m_buf.append(other);
    }

public:
    exlib::string str()
    {
        return m_buf;
    }

private:
    exlib::string m_buf;
    exlib::string m_disks[27];
};

inline void resolvePath(exlib::string& strBuffer, exlib::string other)
{
    Path p;
#ifdef _WIN32
    p.resolveWin32(strBuffer);
    p.resolveWin32(other);
#else
    p.resolvePosix(strBuffer);
    p.resolvePosix(other);
#endif
    strBuffer = p.str();
}

inline const char* split_path(const char* p)
{
    char ch;
    const char* p1 = p;
    const char* p2 = NULL;

    if (p[0] != 0 && p[1] == ':') {
        p += 2;
        if (isPosixPathSlash(*p))
            p++;
        p2 = p;
    } else if (isPosixPathSlash(p[0]) && isPosixPathSlash(p[1])) {
        p += 2;
        while (*p && !isPosixPathSlash(*p))
            p++;

        if (*p) {
            p++;

            while (*p && !isPosixPathSlash(*p))
                p++;

            if (*p)
                p++;
        }

        p2 = p;
    }

    while (*p) {
        ch = *p++;
        if (isPosixPathSlash(ch) && *p)
            p2 = p - 1;
    }

    if (p2 == NULL)
        p2 = p1;

    if (isPosixPathSlash(*p2) && p2 == p1)
        p2++;

    return p2;
}

inline const char* split_path_win32(const char* p)
{
    char ch;
    const char* p1 = p;
    const char* p2 = NULL;

    if (p[0] != 0 && p[1] == ':') {
        p += 2;
        if (isWin32PathSlash(*p))
            p++;
        p2 = p;
    } else if (isWin32PathSlash(p[0]) && isWin32PathSlash(p[1])) {
        p += 2;
        while (*p && !isWin32PathSlash(*p))
            p++;

        if (*p) {
            p++;

            while (*p && !isWin32PathSlash(*p))
                p++;

            if (*p)
                p++;
        }

        p2 = p;
    }

    while (*p) {
        ch = *p++;
        if (isWin32PathSlash(ch) && *p)
            p2 = p - 1;
    }

    if (p2 == NULL)
        p2 = p1;

    if (isWin32PathSlash(*p2) && p2 == p1)
        p2++;

    return p2;
}

inline result_t _normalize(exlib::string path, exlib::string& retVal, bool bRemoveSlash = false)
{
    if (!path.length()) {
        retVal = ".";
        return 0;
    }

    exlib::string str;
    const char* p1 = path.c_str();
    char* pstr;
    int32_t pos = 0;
    int32_t root = 0;
    bool bRoot = false;

    str.resize(path.length());
    pstr = &str[0];

    if (isPosixPathSlash(p1[0])) {
        pstr[pos++] = PATH_SLASH_POSIX;
        p1++;
        bRoot = true;
    }

    root = pos;

    while (*p1) {
        if (isPosixPathSlash(p1[0])) {
            p1++;
        } else if (p1[0] == '.' && (!p1[1] || isPosixPathSlash(p1[1]))) {
            if (pos == 0 && (!p1[1] || !p1[2])) {
                pstr[pos++] = '.';
                if (p1[1])
                    pstr[pos++] = PATH_SLASH_POSIX;
            } else {
                if (pos > root && !p1[1])
                    pos--;
            }
            p1 += p1[1] ? 2 : 1;
        } else if ((p1[0] == '.') && (p1[1] == '.') && (!p1[2] || isPosixPathSlash(p1[2]))) {
            if (pos > root) {
                if ((pstr[pos - 2] == '.') && (pstr[pos - 3] == '.')
                    && ((root == pos - 3) || (pstr[pos - 4] == PATH_SLASH_POSIX))) {
                    pstr[pos++] = '.';
                    pstr[pos++] = '.';
                    pstr[pos++] = PATH_SLASH_POSIX;
                } else {
                    pos--;
                    while (pos > root && !isPosixPathSlash(pstr[pos - 1]))
                        pos--;
                }
            } else if (!bRoot) {
                pstr[pos++] = '.';
                pstr[pos++] = '.';
                pstr[pos++] = PATH_SLASH_POSIX;
            }

            p1 += p1[2] ? 3 : 2;
        } else {
            while (*p1 && !isPosixPathSlash(*p1))
                pstr[pos++] = *p1++;
            if (*p1) {
                p1++;
                pstr[pos++] = PATH_SLASH_POSIX;
            }
        }
    }

    if (bRemoveSlash && pos > root && isPosixPathSlash(pstr[pos - 1]))
        pos--;

    str.resize(pos);
    retVal = str;

    return 0;
}

inline result_t _normalize_win32(exlib::string path, exlib::string& retVal, bool bRemoveSlash = false)
{
    if (!path.length()) {
        retVal = ".";
        return 0;
    }

    exlib::string str;
    const char* p1 = path.c_str();
    char* pstr;
    int32_t pos = 0;
    int32_t root = 0;
    bool bRoot = false;

    str.resize(path.length());
    pstr = &str[0];

    if (p1[0] != 0 && p1[1] == ':') {
        pstr[pos++] = p1[0];
        pstr[pos++] = ':';
        p1 += 2;

        if (isWin32PathSlash(*p1)) {
            pstr[pos++] = PATH_SLASH_WIN32;
            p1++;
            bRoot = true;
        }
    } else if (isWin32PathSlash(p1[0]) && isWin32PathSlash(p1[1])) {
        pstr[pos++] = PATH_SLASH_WIN32;
        pstr[pos++] = PATH_SLASH_WIN32;
        p1 += 2;

        while (*p1 && !isWin32PathSlash(*p1))
            pstr[pos++] = *p1++;

        if (*p1) {
            pstr[pos++] = PATH_SLASH_WIN32;
            p1++;

            while (isWin32PathSlash(*p1))
                p1++;

            while (*p1 && !isWin32PathSlash(*p1))
                pstr[pos++] = *p1++;

            pstr[pos++] = PATH_SLASH_WIN32;
            bRoot = true;

            if (*p1)
                p1++;
        }
    } else if (isWin32PathSlash(p1[0])) {
        pstr[pos++] = PATH_SLASH_WIN32;
        p1++;
        bRoot = true;
    }

    root = pos;

    while (*p1) {
        if (isWin32PathSlash(p1[0])) {
            p1++;
        } else if (p1[0] == '.' && (!p1[1] || isWin32PathSlash(p1[1]))) {
            if (pos == 0 && (!p1[1] || !p1[2])) {
                pstr[pos++] = '.';
                if (p1[1])
                    pstr[pos++] = PATH_SLASH_WIN32;
            } else {
                if (pos > root && !p1[1])
                    pos--;
            }
            p1 += p1[1] ? 2 : 1;
        } else if ((p1[0] == '.') && (p1[1] == '.') && (!p1[2] || isWin32PathSlash(p1[2]))) {
            if (pos > root) {
                if ((pstr[pos - 2] == '.') && (pstr[pos - 3] == '.')
                    && ((root == pos - 3) || (pstr[pos - 4] == PATH_SLASH_WIN32))) {
                    pstr[pos++] = '.';
                    pstr[pos++] = '.';
                    pstr[pos++] = PATH_SLASH_WIN32;
                } else {
                    pos--;
                    while (pos > root && !isWin32PathSlash(pstr[pos - 1]))
                        pos--;
                }
            } else if (!bRoot) {
                pstr[pos++] = '.';
                pstr[pos++] = '.';
                pstr[pos++] = PATH_SLASH_WIN32;
            }

            p1 += p1[2] ? 3 : 2;
        } else {
            while (*p1 && !isWin32PathSlash(*p1))
                pstr[pos++] = *p1++;
            if (*p1) {
                p1++;
                pstr[pos++] = PATH_SLASH_WIN32;
            }
        }
    }

    if (bRemoveSlash && pos > root && isWin32PathSlash(pstr[pos - 1]))
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

    while (isPosixPathSlash(*p2))
        p2++;

    while (p3 > p2 && isPosixPathSlash(*(p3 - 1)))
        p3--;

    if (extlen && ((int32_t)(p3 - p2) >= extlen)
        && !qstrcmp(ext.c_str(), p3 - extlen, extlen))
        p3 -= extlen;

    retVal.assign(p2, (int32_t)(p3 - p2));

    return 0;
}

inline result_t _basename_win32(exlib::string path, exlib::string ext,
    exlib::string& retVal)
{
    const char* p1 = path.c_str();
    const char* p2 = split_path_win32(p1);
    const char* p3 = p1 + path.length();
    int32_t extlen = (int32_t)ext.length();

    while (isWin32PathSlash(*p2))
        p2++;

    while (p3 > p2 && isWin32PathSlash(*(p3 - 1)))
        p3--;

    if (extlen && ((int32_t)(p3 - p2) >= extlen)
        && !qstricmp(ext.c_str(), p3 - extlen, extlen))
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
        if (isPosixPathSlash(ch)) {
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

inline result_t _extname_win32(exlib::string path, exlib::string& retVal)
{
    char ch;
    const char* p1 = NULL;
    const char* c_str = path.c_str();

    if (*c_str == '.')
        c_str++;

    while (*c_str) {
        ch = *c_str++;
        if (isWin32PathSlash(ch)) {
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

inline result_t _dirname_win32(exlib::string path, exlib::string& retVal)
{
    const char* p1 = path.c_str();
    const char* p2 = split_path_win32(p1);

    retVal.assign(p1, (int32_t)(p2 - p1));

    return 0;
}

inline result_t _join(std::vector<v8::Local<v8::Value>>& ps, exlib::string& retVal)
{
    Path p;
    int32_t argc = (int32_t)ps.size();
    int32_t i;

    for (i = 0; i < argc; i++) {
        v8::String::Utf8Value s(ps[i]);
        p.joinPosix(*s);
    }

    return _normalize(p.str(), retVal);
}

inline result_t _join_win32(std::vector<v8::Local<v8::Value>>& ps, exlib::string& retVal)
{
    Path p;
    int32_t argc = (int32_t)ps.size();
    int32_t i;

    for (i = 0; i < argc; i++) {
        v8::String::Utf8Value s(ps[i]);
        p.joinWin32(*s);
    }

    return _normalize_win32(p.str(), retVal);
}

inline result_t _resolve(std::vector<v8::Local<v8::Value>>& ps, exlib::string& retVal)
{
    exlib::string str;
    process_base::cwd(str);

    Path p;
    p.resolvePosix(str);
    int32_t argc = (int32_t)ps.size();
    int32_t i;

    for (i = 0; i < argc; i++) {
        v8::String::Utf8Value s(ps[i]);
        p.resolvePosix(*s);
    }

    return _normalize(p.str(), retVal, true);
}

inline result_t _resolve_win32(std::vector<v8::Local<v8::Value>>& ps, exlib::string& retVal)
{
    exlib::string str;
    process_base::cwd(str);

    Path p;
    p.resolveWin32(str);
    int32_t argc = (int32_t)ps.size();
    int32_t i;

    for (i = 0; i < argc; i++) {
        v8::String::Utf8Value s(ps[i]);
        p.resolveWin32(*s);
    }

    return _normalize_win32(p.str(), retVal, true);
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

inline result_t _fullpath(exlib::string path, exlib::string& retVal)
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

    return 0;
#else
    return Runtime::setError("not supported on none Win32 platform !");
#endif
}

} /* namespace fibjs */

#endif /* _PATH_UTIL_H_ */
