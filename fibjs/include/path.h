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
#include "vector"
#include "SimpleObject.h"

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

        if (other.length() > 2) {
            if (isWin32PathSlash(c_str[0]) && isWin32PathSlash(c_str[1])
                && !isWin32PathSlash(c_str[2])) {
                for (int i = 3; i < (int32_t)other.length(); i++) {
                    if (isWin32PathSlash(c_str[i])) {
                        m_buf = other;
                        return;
                    }
                }
            }
        }

        char diskId = 0;

        if (qisascii(c_str[0]) && c_str[1] == ':') {
            diskId = c_str[0];
            m_buf = m_disks[diskId & 0x1f];
            m_buf[0] = c_str[0];
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
            if (!isWin32PathSlash(base[base.length() - 1]))
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
            if (!isPosixPathSlash(m_buf[m_buf.length() - 1]))
                m_buf.append(1, PATH_SLASH_POSIX);

            m_buf.append(other);
        }
    }

    void joinWin32(exlib::string other)
    {
        if (other.empty())
            return;

        if (m_buf.length() == 0 && other.length() == 2
            && isWin32PathSlash(other[0]) && isWin32PathSlash(other[1])) {
            m_buf.append(1, PATH_SLASH_WIN32);
            return;
        } else if (m_buf.length() == 1 && isWin32PathSlash(m_buf[0])) {
            if (isWin32PathSlash(other[0]))
                m_buf.clear();
        } else if (m_buf.length() > 0 && !isWin32PathSlash(m_buf[m_buf.length() - 1]))
            m_buf.append(1, PATH_SLASH_WIN32);

        m_buf.append(other);
    }

    void joinPosix(exlib::string other)
    {
        if (other.empty())
            return;

        if (m_buf.length() > 0 && !isPosixPathSlash(m_buf[m_buf.length() - 1]))
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

inline const char* split_path(const char* p, int32_t len)
{
    if (len == 0)
        return p;

    const char* p1 = p + len - 1;

    while (p1 > p && isPosixPathSlash(*p1))
        p1--;

    while (p1 > p && !isPosixPathSlash(*p1))
        p1--;

    return p1;
}

inline const char* split_path_win32(const char* p, int32_t len, bool share = false)
{
    const char* p1 = p + len;

    if (qisascii(p[0]) && p[1] == ':') {
        p += 2;
        if (isWin32PathSlash(p[0]))
            p++;
    } else if (len > 3 && isWin32PathSlash(p[0]) && isWin32PathSlash(p[1])
        && !isWin32PathSlash(p[2])) {

        p += 3;
        while (p < p1 && !isWin32PathSlash(*p))
            p++;

        if (p < p1) {
            p++;
            if (share) {
                while (p < p1 && !isWin32PathSlash(*p))
                    p++;

                if (p < p1)
                    p++;
            }
        }
    }

    if (p1 > p)
        p1--;

    while (p1 > p && isWin32PathSlash(*p1))
        p1--;

    while (p1 > p && !isWin32PathSlash(*p1))
        p1--;

    return p1;
}

inline void _normalize_array(std::vector<exlib::string>& a, bool removeSlash)
{
    int32_t i;

    if ((int32_t)a.size() == 1 && (a[0].empty() || a[0] == ".")) {
        a[0] = ".";
        return;
    }

    i = 1;
    while (i < (int32_t)a.size() - 1)
        if (a[i].empty())
            a.erase(a.begin() + i);
        else
            i++;

    i = 0;
    while (i < (int32_t)a.size()) {
        exlib::string n = a[i];

        if (n == ".") {
            if ((int32_t)a.size() == 1)
                return;

            if (i == 0 && (int32_t)a.size() == 2 && a[1].empty())
                return;

            if (i == 1 && (int32_t)a.size() == 2 && a[0].empty()) {
                a[1] = "";
                return;
            }

            a.erase(a.begin() + i);
        } else if (n == "..") {
            if (i == 1 && a[0].empty()) {
                if ((int32_t)a.size() == 2) {
                    a[1] = "";
                    return;
                }

                a.erase(a.begin() + i);
            } else if (i > 0 && !a[i - 1].empty() && a[i - 1] != "..") {
                a.erase(a.begin() + i - 1, a.begin() + i + 1);
                i--;
            } else
                i++;
        } else
            i++;
    }

    if (removeSlash && a.size() > 1) {
        i = (int32_t)a.size();
        if (i > 1 && a[i - 1].empty() && !a[i - 2].empty())
            a.erase(a.begin() + i - 1);
    }
}

inline void _path_array(exlib::string path, std::vector<exlib::string>& a)
{
    const char* p = path.c_str();
    int32_t len = (int32_t)path.length();

    int32_t p1, i;

    p1 = 0;
    for (i = 0; i < len; i++)
        if (isPosixPathSlash(p[i])) {
            a.push_back(exlib::string(p + p1, i - p1));
            p1 = i + 1;
        }
    a.push_back(exlib::string(p + p1, i - p1));
}

inline result_t _normalize(exlib::string path, exlib::string& retVal, bool removeSlash = false, bool root = false)
{
    int32_t i;

    std::vector<exlib::string> a;

    _path_array(path, a);
    _normalize_array(a, removeSlash);

    retVal.clear();

    for (i = 0; i < (int32_t)a.size(); i++) {
        if (i > 0)
            retVal.append(1, PATH_SLASH_POSIX);
        retVal.append(a[i]);
    }

    if (root && a.size() == 1 && a[0].empty())
        retVal.append(1, PATH_SLASH_POSIX);

    return 0;
}

inline void _path_array_win32(exlib::string path, std::vector<exlib::string>& a, char& drv_no,
    exlib::string& domain, exlib::string& share)
{
    const char* p = path.c_str();
    int32_t len = (int32_t)path.length();
    int32_t p1, i;

    if (qisascii(p[0]) && p[1] == ':') {
        drv_no = p[0];
        p += 2;
        len -= 2;
    }

    p1 = 0;
    for (i = 0; i < len; i++)
        if (isWin32PathSlash(p[i])) {
            a.push_back(exlib::string(p + p1, i - p1));
            p1 = i + 1;
        }
    a.push_back(exlib::string(p + p1, i - p1));

    if (drv_no == 0 && (int32_t)a.size() > 2 && a[0].empty() && a[1].empty()) {
        domain = a[2];
        if (!domain.empty()) {
            i = 3;
            while (i < (int32_t)a.size() && a[i].empty())
                i++;

            if (i < (int32_t)a.size()) {
                share = a[i++];
                a.erase(a.begin() + 2, a.begin() + i);
            }
        }
    }
}

inline result_t _normalize_win32(exlib::string path, exlib::string& retVal, bool removeSlash = false, bool root = false)
{
    int32_t i;

    std::vector<exlib::string> a;
    char drv_no = 0;
    exlib::string domain;
    exlib::string share;

    _path_array_win32(path, a, drv_no, domain, share);
    _normalize_array(a, removeSlash);

    retVal.clear();

    if (drv_no) {
        retVal.append(1, drv_no);
        retVal.append(1, ':');
    } else if (!share.empty()) {
        retVal.append(1, PATH_SLASH_WIN32);
        retVal.append(1, PATH_SLASH_WIN32);
        retVal.append(domain);
        retVal.append(1, PATH_SLASH_WIN32);
        retVal.append(share);
    }

    for (i = 0; i < (int32_t)a.size(); i++) {
        if (i > 0)
            retVal.append(1, PATH_SLASH_WIN32);
        retVal.append(a[i]);
    }

    if (root && a.size() == 1 && a[0].empty())
        retVal.append(1, PATH_SLASH_WIN32);

    return 0;
}

inline result_t _basename(exlib::string path, exlib::string ext, exlib::string& retVal)
{
    const char* p1 = path.c_str();
    const char* p2 = split_path(p1, (int32_t)path.length());
    const char* p3 = p1 + path.length();
    int32_t extlen = (int32_t)ext.length();

    while (isPosixPathSlash(*p2))
        p2++;

    while (p3 > p2 && isPosixPathSlash(*(p3 - 1)))
        p3--;

    if (extlen) {
        if (p1 == p2 && extlen == (int32_t)path.length()
            && !qstrcmp(ext.c_str(), p3 - extlen, extlen))
            p3 -= extlen;
        else if ((int32_t)(p3 - p2) > extlen
            && !qstrcmp(ext.c_str(), p3 - extlen, extlen))
            p3 -= extlen;
    }

    retVal.assign(p2, (int32_t)(p3 - p2));

    return 0;
}

inline result_t _basename_win32(exlib::string path, exlib::string ext, exlib::string& retVal)
{
    const char* p1 = path.c_str();
    const char* p2 = split_path_win32(p1, (int32_t)path.length());
    const char* p3 = p1 + path.length();
    int32_t extlen = (int32_t)ext.length();

    while (isWin32PathSlash(*p2))
        p2++;

    while (p3 > p2 && isWin32PathSlash(*(p3 - 1)))
        p3--;

    if (extlen) {
        if (p1 == p2 && extlen == (int32_t)path.length()
            && !qstrcmp(ext.c_str(), p3 - extlen, extlen))
            p3 -= extlen;
        else if ((int32_t)(p3 - p2) > extlen
            && !qstrcmp(ext.c_str(), p3 - extlen, extlen))
            p3 -= extlen;
    }

    retVal.assign(p2, (int32_t)(p3 - p2));

    return 0;
}

inline result_t _extname(exlib::string path, exlib::string& retVal)
{
    const char* p1 = path.c_str();
    const char* p2 = split_path(p1, (int32_t)path.length());
    const char* p3 = p1 + path.length();
    const char* p4;

    while (isPosixPathSlash(*p2))
        p2++;

    while (p3 > p2 && isPosixPathSlash(*(p3 - 1)))
        p3--;

    if (p3 - p2 == 2 && p2[0] == '.' && p2[1] == '.')
        return 0;

    p4 = p3;
    while (p4 > p2 + 1 && *(p4 - 1) != '.')
        p4--;

    if (p4 > p2 + 1)
        retVal.assign(p4 - 1, (int32_t)(p3 - p4 + 1));

    return 0;
}

inline result_t _extname_win32(exlib::string path, exlib::string& retVal)
{
    const char* p1 = path.c_str();
    const char* p2 = split_path_win32(p1, (int32_t)path.length());
    const char* p3 = p1 + path.length();
    const char* p4;

    while (isWin32PathSlash(*p2))
        p2++;

    while (p3 > p2 && isWin32PathSlash(*(p3 - 1)))
        p3--;

    if (p3 - p2 == 2 && p2[0] == '.' && p2[1] == '.')
        return 0;

    p4 = p3;
    while (p4 > p2 + 1 && *(p4 - 1) != '.')
        p4--;

    if (p4 > p2 + 1)
        retVal.assign(p4 - 1, (int32_t)(p3 - p4 + 1));

    return 0;
}

inline result_t _universal_format(exlib::string sep, v8::Local<v8::Object> pathObject, exlib::string& retVal)
{
    result_t hr;
    Isolate* isolate = Isolate::current();

    exlib::string dir;
    hr = GetConfigValue(isolate->m_isolate, pathObject, "dir", dir, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    exlib::string root;

    hr = GetConfigValue(isolate->m_isolate, pathObject, "root", root, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    bool dirFromRoot = false;
    if (!dir.length()) {
        dir = root;
        dirFromRoot = true;
    }

    exlib::string base;
    hr = GetConfigValue(isolate->m_isolate, pathObject, "base", base, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (!base.length()) {
        exlib::string tmp;
        hr = GetConfigValue(isolate->m_isolate, pathObject, "name", tmp, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        base += tmp;

        hr = GetConfigValue(isolate->m_isolate, pathObject, "ext", tmp, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        base += tmp;
    }

    if (!dir.length())
        retVal = base;
    else
        retVal = (dirFromRoot || dir == root) ? (dir + base) : (dir + sep + base);

    return 0;
}

inline void fillPathObject(obj_ptr<NObject>& po)
{
    po->add("root", "");
    po->add("dir", "");
    po->add("base", "");
    po->add("ext", "");
    po->add("name", "");
}

inline result_t castPathObject(obj_ptr<NObject> po, obj_ptr<NObject>& retVal)
{
    retVal = po;

    return 0;
}

inline result_t _parse(exlib::string path, obj_ptr<NObject>& retVal)
{
    obj_ptr<NObject> ret = new NObject();
    fillPathObject(ret);

    if (!path.length()) {
        retVal = ret;
        return 0;
    }

    bool isAbsolute = path[0] == CHAR_FORWARD_SLASH;
    int start;
    if (isAbsolute) {
        ret->add("root", "/");
        start = 1;
    } else
        start = 0;
    int startDot = -1;
    int startPart = 0;
    int end = -1;
    bool matchedSlash = true;
    int i = path.length() - 1;

    int preDotState = 0;

    for (; i >= start; --i) {
        int code = path[i];
        if (code == CHAR_FORWARD_SLASH) {
            if (!matchedSlash) {
                startPart = i + 1;
                break;
            }
            continue;
        }
        if (end == -1) {
            matchedSlash = false;
            end = i + 1;
        }
        if (code == CHAR_DOT) {
            if (startDot == -1)
                startDot = i;
            else if (preDotState != 1)
                preDotState = 1;
        } else if (startDot != -1)
            preDotState = -1;
    }

    if (end != -1) {
        int start = startPart == 0 && isAbsolute ? 1 : startPart;
        if (startDot == -1 || preDotState == 0 || (preDotState == 1 && startDot == end - 1 && startDot == startPart + 1)) {
            exlib::string tmp = path.substr(start, end - start);
            ret->add("base", tmp);
            ret->add("name", tmp);
        } else {
            ret->add("name", path.substr(start, startDot - start));
            ret->add("base", path.substr(start, end - start));
            ret->add("ext", path.substr(startDot, end - startDot));
        }
    }

    if (startPart > 0)
        ret->add("dir", path.substr(0, startPart - 1));
    else if (isAbsolute)
        ret->add("dir", "/");

    retVal = ret;
    return 0;
}

inline result_t _parse_win32(exlib::string path, obj_ptr<NObject>& retVal)
{
    obj_ptr<NObject> ret = new NObject();
    fillPathObject(ret);

    if (!path.length()) {
        retVal = ret;
        return 0;
    }

    int len = path.length();
    int rootEnd = 0;
    int code = path[0];

    if (len == 1) {
        if (isPathSeparator(code)) {
            ret->add("root", path);
            ret->add("dir", path);
        } else {
            ret->add("base", path);
            ret->add("name", path);
        }

        retVal = ret;
        return 0;
    }
    if (isPathSeparator(code)) {
        rootEnd = 1;
        if (isPathSeparator(path[1])) {
            int j = 2;
            int last = j;
            while (j < len && !isPathSeparator(path[j]))
                j++;
            if (j < len && j != last) {
                last = j;
                while (j < len && isPathSeparator(path[j]))
                    j++;
                if (j < len && j != last) {
                    last = j;
                    while (j < len && !isPathSeparator(path[j]))
                        j++;
                    if (j == len)
                        rootEnd = j;
                    else if (j != last)
                        rootEnd = j + 1;
                }
            }
        }
    } else if (isWindowsDeviceRoot(code) && path[1] == CHAR_COLON) {
        if (len <= 2) {
            ret->add("root", path);
            ret->add("dir", path);

            retVal = ret;
            return 0;
        }
        rootEnd = 2;
        if (isPathSeparator(path[2])) {
            if (len == 3) {
                ret->add("root", path);
                ret->add("dir", path);

                retVal = ret;
                return 0;
            }
            rootEnd = 3;
        }
    }
    if (rootEnd > 0)
        ret->add("root", path.substr(0, rootEnd));

    int startDot = -1;
    int startPart = rootEnd;
    int end = -1;
    int matchedSlash = true;
    int i = path.length() - 1;

    int preDotState = 0;

    for (; i >= rootEnd; --i) {
        code = path[i];
        if (isPathSeparator(code)) {
            if (!matchedSlash) {
                startPart = i + 1;
                break;
            }
            continue;
        }
        if (end == -1) {
            matchedSlash = false;
            end = i + 1;
        }
        if (code == CHAR_DOT) {
            if (startDot == -1)
                startDot = i;
            else if (preDotState != 1)
                preDotState = 1;
        } else if (startDot != -1)
            preDotState = -1;
    }

    if (end != -1) {
        if (startDot == -1 || preDotState == 0 || (preDotState == 1 && startDot == end - 1 && startDot == startPart + 1)) {

            exlib::string tmp = path.substr(startPart, end);
            ret->add("base", tmp);
            ret->add("name", tmp);
        } else {
            ret->add("name", path.substr(startPart, startDot));
            ret->add("base", path.substr(startPart, end));
            ret->add("ext", path.substr(startDot, end));
        }
    }

    if (startPart > 0 && startPart != rootEnd)
        ret->add("dir", path.substr(0, startPart - 1));
    else {
        Variant v;
        ret->get("root", v);
        ret->add("dir", v.string());
    }

    retVal = ret;
    return 0;
}

inline result_t _dirname(exlib::string path, exlib::string& retVal)
{
    const char* p1 = path.c_str();
    const char* p2 = split_path(p1, (int32_t)path.length());

    if (p2 == p1 && !isPosixPathSlash(*p2)) {
        retVal = ".";
        return 0;
    }

    retVal.assign(p1, (int32_t)(p2 - p1));
    if (retVal.empty())
        retVal.assign(p2, 1);

    return 0;
}

inline result_t _dirname_win32(exlib::string path, exlib::string& retVal)
{
    const char* p1 = path.c_str();
    const char* p2 = split_path_win32(p1, (int32_t)path.length(), true);

    if (p2 == p1 && !isWin32PathSlash(*p2)) {
        retVal = ".";
        return 0;
    }

    retVal.assign(p1, (int32_t)(p2 - p1));
    if (retVal.empty())
        retVal.assign(p2, 1);

    return 0;
}

inline result_t _join(OptArgs ps, exlib::string& retVal)
{
    Path p;
    int32_t argc = ps.Length();
    int32_t i;
    Isolate* isolate = Isolate::current();

    for (i = 0; i < argc; i++) {
        exlib::string s;
        GetArgumentValue(isolate->m_isolate, ps[i], s);
        p.joinPosix(s);
    }

    return _normalize(p.str(), retVal);
}

inline result_t _join_win32(OptArgs ps, exlib::string& retVal)
{
    Path p;
    int32_t argc = ps.Length();
    int32_t i;
    Isolate* isolate = Isolate::current();

    for (i = 0; i < argc; i++) {
        exlib::string s;
        GetArgumentValue(isolate->m_isolate, ps[i], s);
        p.joinWin32(s);
    }

    return _normalize_win32(p.str(), retVal);
}

inline result_t _resolve(OptArgs ps, exlib::string& retVal)
{
    exlib::string str;
    process_base::cwd(str);

    Path p;
    p.resolvePosix(str);
    int32_t argc = ps.Length();
    int32_t i;
    Isolate* isolate = Isolate::current();

    for (i = 0; i < argc; i++) {
        exlib::string s;
        GetArgumentValue(isolate->m_isolate, ps[i], s);
        p.resolvePosix(s);
    }

    return _normalize(p.str(), retVal, true, true);
}

inline result_t _resolve(exlib::string& path)
{
    exlib::string str;
    process_base::cwd(str);

    Path p;
    p.resolvePosix(str);
    p.resolvePosix(path);

    return _normalize(p.str(), path, true, true);
}

inline result_t _resolve_win32(OptArgs ps, exlib::string& retVal)
{
    exlib::string str;
    process_base::cwd(str);

    Path p;
    p.resolveWin32(str);
    int32_t argc = ps.Length();
    int32_t i;
    Isolate* isolate = Isolate::current();

    for (i = 0; i < argc; i++) {
        exlib::string s;
        GetArgumentValue(isolate->m_isolate, ps[i], s);
        p.resolveWin32(s);
    }

    return _normalize_win32(p.str(), retVal, true, true);
}

inline result_t _resolve_win32(exlib::string& path)
{
    exlib::string str;
    process_base::cwd(str);

    Path p;
    p.resolveWin32(str);
    p.resolveWin32(path);

    return _normalize_win32(p.str(), path, true);
}

inline result_t _relative(exlib::string from, exlib::string to, exlib::string& retVal)
{
    if (from == to)
        return 0;

    result_t hr;

    hr = _resolve(from);
    if (hr < 0)
        return hr;

    hr = _resolve(to);
    if (hr < 0)
        return hr;

    if (from == to)
        return 0;

    // Trim any leading backslashes
    int32_t fromStart = 1;
    for (; fromStart < (int32_t)from.length(); ++fromStart) {
        if (!isPosixPathSlash(from[fromStart]))
            break;
    }
    int32_t fromEnd = (int32_t)from.length();
    int32_t fromLen = (fromEnd - fromStart);

    // Trim any leading backslashes
    int32_t toStart = 1;
    for (; toStart < (int32_t)to.length(); ++toStart) {
        if (!isPosixPathSlash(to[toStart]))
            break;
    }
    int32_t toEnd = (int32_t)to.length();
    int32_t toLen = (toEnd - toStart);

    // Compare paths to find the longest common path from root
    int32_t length = (fromLen < toLen ? fromLen : toLen);
    int32_t lastCommonSep = -1;
    int32_t i = 0;
    for (; i <= length; ++i) {
        if (i == length) {
            if (toLen > length) {
                if (isPosixPathSlash(to[toStart + i])) {
                    // We get here if `from` is the exact base path for `to`.
                    // For example: from='/foo/bar'; to='/foo/bar/baz'
                    return _normalize(to.substr(toStart + i + 1), retVal, false);
                } else if (i == 0) {
                    // We get here if `from` is the root
                    // For example: from='/'; to='/foo'
                    return _normalize(to.substr(toStart + i), retVal, false);
                }

            } else if (fromLen > length) {
                if (isPosixPathSlash(from[fromStart + i])) {
                    // We get here if `to` is the exact base path for `from`.
                    // For example: from='/foo/bar/baz'; to='/foo/bar'
                    lastCommonSep = i;
                } else if (i == 0) {
                    // We get here if `to` is the root.
                    // For example: from='/foo'; to='/'
                    lastCommonSep = 0;
                }
            }
            break;
        }
        char fromChar = from[fromStart + i];
        char toChar = to[toStart + i];
        if (fromChar != toChar)
            break;
        else if (isPosixPathSlash(fromChar))
            lastCommonSep = i;
    }

    exlib::string out = "";
    // Generate the relative path based on the path difference between `to`
    // and `from`
    for (i = fromStart + lastCommonSep + 1; i <= fromEnd; ++i) {
        if (i == fromEnd || isPosixPathSlash(from[i])) {
            if (out.length() == 0)
                out += "..";
            else
                out += "/..";
        }
    }

    // Lastly, append the rest of the destination (`to`) path that comes after
    // the common path parts
    if (out.length() > 0) {
        return _normalize(out + to.substr(toStart + lastCommonSep), retVal, false);
    } else {
        toStart += lastCommonSep;
        if (isPosixPathSlash(to[toStart]))
            ++toStart;

        return _normalize(to.substr(toStart), retVal, false);
    }

    return 0;
}

inline result_t _relative_win32(exlib::string from, exlib::string to, exlib::string& retVal)
{
    if (from == to)
        return 0;

    exlib::string fromOrig = "" + from;
    exlib::string toOrig = "" + to;

    exlib::string str;
    int32_t hr;

    hr = _resolve_win32(from);
    if (hr < 0)
        return hr;
    hr = _resolve_win32(to);
    if (hr < 0)
        return hr;

    if (fromOrig == toOrig)
        return 0;

    from = "" + fromOrig;
    from.tolower();
    to = "" + toOrig;
    to.tolower();

    if (from == to)
        return 0;

    // Trim any leading backslashes
    int32_t fromStart = 0;
    for (; fromStart < (int32_t)from.length(); ++fromStart) {
        if (!isWin32PathSlash(from[fromStart]))
            break;
    }
    // Trim trailing backslashes (applicable to UNC paths only)
    int32_t fromEnd = (int32_t)from.length();
    for (; fromEnd - 1 > fromStart; --fromEnd) {
        if (!isWin32PathSlash(from[fromEnd - 1]))
            break;
    }
    int32_t fromLen = (fromEnd - fromStart);

    // Trim any leading backslashes
    int32_t toStart = 0;
    for (; toStart < (int32_t)to.length(); ++toStart) {
        if (!isWin32PathSlash(to[toStart]))
            break;
    }
    // Trim trailing backslashes (applicable to UNC paths only)
    int32_t toEnd = (int32_t)to.length();
    for (; toEnd - 1 > toStart; --toEnd) {
        if (!isWin32PathSlash(to[toEnd - 1]))
            break;
    }
    int32_t toLen = (toEnd - toStart);

    // Compare paths to find the longest common path from root
    int32_t length = (fromLen < toLen ? fromLen : toLen);
    int32_t lastCommonSep = -1;
    int32_t i = 0;
    for (; i <= length; ++i) {
        if (i == length) {
            if (toLen > length) {
                if (isWin32PathSlash(to[toStart + i])) {
                    // We get here if `from` is the exact base path for `to`.
                    // For example: from='C:\\foo\\bar'; to='C:\\foo\\bar\\baz'
                    return _normalize_win32(toOrig.substr(toStart + i + 1), retVal, true);
                } else if (i == 2) {
                    // We get here if `from` is the device root.
                    // For example: from='C:\\'; to='C:\\foo'
                    return _normalize_win32(toOrig.substr(toStart + i), retVal, true);
                }
            }
            if (fromLen > length) {
                if (isWin32PathSlash(from[fromStart + i])) {
                    // We get here if `to` is the exact base path for `from`.
                    // For example: from='C:\\foo\\bar'; to='C:\\foo'
                    lastCommonSep = i;
                } else if (i == 2) {
                    // We get here if `to` is the device root.
                    // For example: from='C:\\foo\\bar'; to='C:\\'
                    lastCommonSep = 3;
                }
            }
            break;
        }
        char fromChar = from[fromStart + i];
        char toChar = to[toStart + i];
        if (fromChar != toChar)
            break;
        else if (isWin32PathSlash(fromChar))
            lastCommonSep = i;
    }

    // We found a mismatch before the first common path separator was seen, so
    // return the original `to`.
    if (i != length && lastCommonSep == -1) {
        return _normalize_win32(toOrig, retVal, true);
    }

    exlib::string out = "";
    if (lastCommonSep == -1)
        lastCommonSep = 0;
    // Generate the relative path based on the path difference between `to` and
    // `from`
    for (i = fromStart + lastCommonSep + 1; i <= fromEnd; ++i) {
        if (i == fromEnd || isWin32PathSlash(from[i])) {
            if (out.length() == 0)
                out += "..";
            else
                out += "\\..";
        }
    }

    // Lastly, append the rest of the destination (`to`) path that comes after
    // the common path parts
    if (out.length() > 0) {
        return _normalize_win32(out + toOrig.substr(toStart + lastCommonSep), retVal, true);
    } else {
        toStart += lastCommonSep;
        if (isWin32PathSlash(toOrig[toStart]))
            ++toStart;
        return _normalize_win32(toOrig.substr(toStart), retVal, true);
    }
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
    if (isPathSlash(path[0]))
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
