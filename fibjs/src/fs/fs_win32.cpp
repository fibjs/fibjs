#ifdef _WIN32

#include "ifs/fs.h"
#include "List.h"
#include "File.h"

#include <stdio.h>

namespace fibjs {

result_t fs_base::exists(exlib::string path, bool& retVal, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = _waccess(UTF8_W(path), 0) == 0;
    return 0;
}

result_t fs_base::access(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    DWORD attr = GetFileAttributesW(UTF8_W(path));

    if (attr == INVALID_FILE_ATTRIBUTES)
        return CHECK_ERROR(LastError());

    if (mode & W_OK && attr & FILE_ATTRIBUTE_READONLY && !(attr & FILE_ATTRIBUTE_DIRECTORY))
        return CHECK_ERROR(CALL_E_PERMIT);

    return 0;
}

result_t fs_base::link(exlib::string oldPath, exlib::string newPath, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::CreateHardLinkW(UTF8_W(newPath), UTF8_W(oldPath), NULL) == 0)
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::unlink(exlib::string path, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::_wunlink(UTF8_W(path)))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::readlink(exlib::string path, exlib::string& retVal, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::mkdir(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::_wmkdir(UTF8_W(path)))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::rmdir(exlib::string path, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::_wrmdir(UTF8_W(path)))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::chmod(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::lchmod(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::chown(exlib::string path, int32_t uid, int32_t gid, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::lchown(exlib::string path, int32_t uid, int32_t gid, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::realpath(exlib::string path, exlib::string& retVal, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::symlink(exlib::string target, exlib::string linkpath, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    bool isDir;
    obj_ptr<Stat> pStat = new Stat();

    result_t hr = pStat->getStat(target);
    if (hr < 0)
        return hr;

    pStat->isDirectory(isDir);
    if (CreateSymbolicLinkW(UTF8_W(linkpath), UTF8_W(target), isDir ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0) == 0)
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::truncate(exlib::string path, int32_t len, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    HANDLE file;

    if ((file = CreateFileW(UTF8_W(path),
             GENERIC_WRITE,
             FILE_SHARE_WRITE,
             NULL,
             CREATE_NEW | OPEN_EXISTING,
             FILE_ATTRIBUTE_NORMAL,
             NULL))
        == INVALID_HANDLE_VALUE)
        return CHECK_ERROR(LastError());

    if (SetFilePointer(file, (LONG)len, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        return CHECK_ERROR(LastError());

    if (!SetEndOfFile(file))
        return CHECK_ERROR(LastError());

    CloseHandle(file);
    return 0;
}

result_t fs_base::rename(exlib::string from, exlib::string to, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::_wrename(UTF8_W(from), UTF8_W(to)))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::copy(exlib::string from, exlib::string to, AsyncEvent* ac)
{
    if (!CopyFileW(UTF8_W(from), UTF8_W(to), TRUE))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::readdir(exlib::string path, obj_ptr<List_base>& retVal, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    WIN32_FIND_DATAW fd;
    HANDLE hFind;
    exlib::wstring fpath;
    obj_ptr<List> oa;

    fpath = utf8to16String(path);
    fpath.append(L"\\*", 2);

    hFind = FindFirstFileW(fpath.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE)
        return CHECK_ERROR(LastError());

    oa = new List();

    do {
        if (qstrcmp(fd.cFileName, L".") && qstrcmp(fd.cFileName, L"..")) {
            exlib::string name = UTF8_A(fd.cFileName);
            oa->append(name);
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);

    retVal = oa;

    return 0;
}
}

#endif
