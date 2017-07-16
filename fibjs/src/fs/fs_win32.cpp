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

result_t _create_junction(const WCHAR* path, const WCHAR* new_path)
{
/* MinGW already has a definition for REPARSE_DATA_BUFFER, but mingw-w64 does
 * not.
 */
#if defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR)
    typedef struct _REPARSE_DATA_BUFFER {
        ULONG ReparseTag;
        USHORT ReparseDataLength;
        USHORT Reserved;
        union {
            struct {
                USHORT SubstituteNameOffset;
                USHORT SubstituteNameLength;
                USHORT PrintNameOffset;
                USHORT PrintNameLength;
                ULONG Flags;
                WCHAR PathBuffer[1];
            } SymbolicLinkReparseBuffer;
            struct {
                USHORT SubstituteNameOffset;
                USHORT SubstituteNameLength;
                USHORT PrintNameOffset;
                USHORT PrintNameLength;
                WCHAR PathBuffer[1];
            } MountPointReparseBuffer;
            struct {
                UCHAR DataBuffer[1];
            } GenericReparseBuffer;
        };
    } REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;
#endif

#define LONG_PATH_PREFIX L"\\\\?\\"
#define LONG_PATH_PREFIX_LEN 4
#define JUNCTION_PREFIX L"\\??\\"
#define JUNCTION_PREFIX_LEN 4
#define IS_LETTER(c) (((c) >= L'a' && (c) <= L'z') || ((c) >= L'A' && (c) <= L'Z'))
    HANDLE handle = INVALID_HANDLE_VALUE;
    REPARSE_DATA_BUFFER* buffer = NULL;
    int created = 0;
    int target_len;
    int is_absolute, is_long_path;
    int needed_buf_size, used_buf_size, used_data_size, path_buf_len;
    int start, len, i;
    int add_slash;
    DWORD bytes;
    WCHAR* path_buf;

    target_len = (int)wcslen(path);
    is_long_path = wcsncmp(path, LONG_PATH_PREFIX, LONG_PATH_PREFIX_LEN) == 0;

    if (is_long_path) {
        is_absolute = 1;
    } else {
        is_absolute = target_len >= 3 && IS_LETTER(path[0]) && path[1] == L':' && path[2] == L'\\';
    }

    if (!is_absolute) {
        /* Not supporting relative paths */
        return CHECK_ERROR(CALL_E_INVALIDARG);
    }

    /* Do a pessimistic calculation of the required buffer size */
    needed_buf_size = FIELD_OFFSET(REPARSE_DATA_BUFFER, MountPointReparseBuffer.PathBuffer) + JUNCTION_PREFIX_LEN * sizeof(WCHAR) + 2 * (target_len + 2) * sizeof(WCHAR);

    /* Allocate the buffer */
    buffer = (REPARSE_DATA_BUFFER*)malloc(needed_buf_size);
    if (!buffer)
        return CHECK_ERROR(CALL_E_OVERFLOW);

    /* Grab a pointer to the part of the buffer where filenames go */
    path_buf = (WCHAR*)&(buffer->MountPointReparseBuffer.PathBuffer);
    path_buf_len = 0;

    /* Copy the substitute (internal) target path */
    start = path_buf_len;

    wcsncpy((WCHAR*)&path_buf[path_buf_len], JUNCTION_PREFIX,
        JUNCTION_PREFIX_LEN);
    path_buf_len += JUNCTION_PREFIX_LEN;

    add_slash = 0;
    for (i = is_long_path ? LONG_PATH_PREFIX_LEN : 0; path[i] != L'\0'; i++) {
        if (path[i] == L'\\') {
            add_slash = 1;
            continue;
        }

        if (add_slash) {
            path_buf[path_buf_len++] = L'\\';
            add_slash = 0;
        }

        path_buf[path_buf_len++] = path[i];
    }
    path_buf[path_buf_len++] = L'\\';
    len = path_buf_len - start;

    /* Set the info about the substitute name */
    buffer->MountPointReparseBuffer.SubstituteNameOffset = (USHORT)start * sizeof(WCHAR);
    buffer->MountPointReparseBuffer.SubstituteNameLength = (USHORT)len * sizeof(WCHAR);

    /* Insert null terminator */
    path_buf[path_buf_len++] = L'\0';

    /* Copy the print name of the target path */
    start = path_buf_len;
    add_slash = 0;
    for (i = is_long_path ? LONG_PATH_PREFIX_LEN : 0; path[i] != L'\0'; i++) {
        if (path[i] == L'\\') {
            add_slash = 1;
            continue;
        }

        if (add_slash) {
            path_buf[path_buf_len++] = L'\\';
            add_slash = 0;
        }

        path_buf[path_buf_len++] = path[i];
    }
    len = path_buf_len - start;
    if (len == 2) {
        path_buf[path_buf_len++] = L'\\';
        len++;
    }

    /* Set the info about the print name */
    buffer->MountPointReparseBuffer.PrintNameOffset = (USHORT)start * sizeof(WCHAR);
    buffer->MountPointReparseBuffer.PrintNameLength = (USHORT)len * sizeof(WCHAR);

    /* Insert another null terminator */
    path_buf[path_buf_len++] = L'\0';

    /* Calculate how much buffer space was actually used */
    used_buf_size = FIELD_OFFSET(REPARSE_DATA_BUFFER, MountPointReparseBuffer.PathBuffer) + path_buf_len * sizeof(WCHAR);
    used_data_size = used_buf_size - FIELD_OFFSET(REPARSE_DATA_BUFFER, MountPointReparseBuffer);

    /* Put general info in the data buffer */
    buffer->ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
    buffer->ReparseDataLength = used_data_size;
    buffer->Reserved = 0;

    /* Create a new directory */
    if (!CreateDirectoryW(new_path, NULL))
        goto error;

    created = 1;

    /* Open the directory */
    handle = CreateFileW(new_path,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
        NULL);
    if (handle == INVALID_HANDLE_VALUE)
        goto error;

    /* Create the actual reparse point */
    if (!DeviceIoControl(handle,
            FSCTL_SET_REPARSE_POINT,
            buffer,
            used_buf_size,
            NULL,
            0,
            &bytes,
            NULL))
        goto error;

    /* Clean up */
    CloseHandle(handle);
    free(buffer);

    return 0;
error:
    free(buffer);

    if (handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
    }

    if (created) {
        RemoveDirectoryW(new_path);
    }
    return CHECK_ERROR(LastError());
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
    if (!isDir) {
        if (CreateSymbolicLinkW(UTF8_W(linkpath), UTF8_W(target), isDir ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0) == 0)
            return CHECK_ERROR(LastError());
    } else {
        hr = _create_junction(UTF8_W(target), UTF8_W(linkpath));
        if (hr < 0)
            return hr;
    }

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
