#ifdef _WIN32

#include "ifs/fs.h"
#include "path.h"
#include "File.h"

#include <stdio.h>

namespace fibjs {

result_t fs_base::lchmod(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::truncate(exlib::string path, int32_t len, AsyncEvent* ac)
{
    if (ac->isSync())
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

result_t fs_base::copy(exlib::string from, exlib::string to, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!CopyFileW(UTF8_W(from), UTF8_W(to), TRUE))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::readdir(exlib::string path, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!ac->isolate()->m_enable_FileSystem)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    WIN32_FIND_DATAW fd;
    HANDLE hFind;
    exlib::wstring fpath;
    obj_ptr<NArray> oa;

    fpath = utf8to16String(path);
    fpath.append(L"\\*", 2);

    hFind = FindFirstFileW(fpath.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE)
        return CHECK_ERROR(LastError());

    oa = new NArray();

    do {
        if (qstrcmp(fd.cFileName, L".") && qstrcmp(fd.cFileName, L"..")) {
            exlib::string name = utf16to8String(fd.cFileName);
            oa->append(name);
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);

    retVal = oa;

    return 0;
}
}

#endif
