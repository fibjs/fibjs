#ifdef _WIN32

#include "ifs/fs.h"
#include "List.h"
#include "path.h"
#include "File.h"

#include <stdio.h>

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
#define UNC_PATH_PREFIX L"\\\\?\\UNC\\"
#define UNC_PATH_PREFIX_LEN 8
#define JUNCTION_PREFIX L"\\??\\"
#define JUNCTION_PREFIX_LEN 4
#define IS_LETTER(c) (((c) >= L'a' && (c) <= L'z') || ((c) >= L'A' && (c) <= L'Z'))

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
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    char buffer[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
    REPARSE_DATA_BUFFER* reparse_data = (REPARSE_DATA_BUFFER*) buffer;
    WCHAR* w_target;
    DWORD w_target_len;
    DWORD bytes;
    HANDLE handle;

    handle = CreateFileW(UTF8_W(path),
                         0,
                         0,
                         NULL,
                         OPEN_EXISTING,
                         FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS,
                         NULL);

    if (handle == INVALID_HANDLE_VALUE)
        return CHECK_ERROR(LastError());

    if (!DeviceIoControl(handle,
                         FSCTL_GET_REPARSE_POINT,
                         NULL,
                         0,
                         buffer,
                         sizeof buffer,
                         &bytes,
                         NULL)) {
        CloseHandle(handle);
        return CHECK_ERROR(LastError());
    }

    if (reparse_data->ReparseTag == IO_REPARSE_TAG_SYMLINK) {
        /* Real symlink */
        w_target = reparse_data->SymbolicLinkReparseBuffer.PathBuffer +
                   (reparse_data->SymbolicLinkReparseBuffer.SubstituteNameOffset /
                    sizeof(WCHAR));
        w_target_len =
            reparse_data->SymbolicLinkReparseBuffer.SubstituteNameLength /
            sizeof(WCHAR);

        /* Real symlinks can contain pretty much everything, but the only thing */
        /* we really care about is undoing the implicit conversion to an NT */
        /* namespaced path that CreateSymbolicLink will perform on absolute */
        /* paths. If the path is win32-namespaced then the user must have */
        /* explicitly made it so, and we better just return the unmodified */
        /* reparse data. */
        if (w_target_len >= 4 &&
                w_target[0] == L'\\' &&
                w_target[1] == L'?' &&
                w_target[2] == L'?' &&
                w_target[3] == L'\\') {
            /* Starts with \??\ */
            if (w_target_len >= 6 &&
                    ((w_target[4] >= L'A' && w_target[4] <= L'Z') ||
                     (w_target[4] >= L'a' && w_target[4] <= L'z')) &&
                    w_target[5] == L':' &&
                    (w_target_len == 6 || w_target[6] == L'\\')) {
                /* \??\<drive>:\ */
                w_target += 4;
                w_target_len -= 4;

            } else if (w_target_len >= 8 &&
                       (w_target[4] == L'U' || w_target[4] == L'u') &&
                       (w_target[5] == L'N' || w_target[5] == L'n') &&
                       (w_target[6] == L'C' || w_target[6] == L'c') &&
                       w_target[7] == L'\\') {
                /* \??\UNC\<server>\<share>\ - make sure the final path looks like */
                /* \\<server>\<share>\ */
                w_target += 6;
                w_target[0] = L'\\';
                w_target_len -= 6;
            }
        }

    } else if (reparse_data->ReparseTag == IO_REPARSE_TAG_MOUNT_POINT) {
        /* Junction. */
        w_target = reparse_data->MountPointReparseBuffer.PathBuffer +
                   (reparse_data->MountPointReparseBuffer.SubstituteNameOffset /
                    sizeof(WCHAR));
        w_target_len = reparse_data->MountPointReparseBuffer.SubstituteNameLength /
                       sizeof(WCHAR);

        /* Only treat junctions that look like \??\<drive>:\ as symlink. */
        /* Junctions can also be used as mount points, like \??\Volume{<guid>}, */
        /* but that's confusing for programs since they wouldn't be able to */
        /* actually understand such a path when returned by uv_readlink(). */
        /* UNC paths are never valid for junctions so we don't care about them. */
        if (!(w_target_len >= 6 &&
                w_target[0] == L'\\' &&
                w_target[1] == L'?' &&
                w_target[2] == L'?' &&
                w_target[3] == L'\\' &&
                ((w_target[4] >= L'A' && w_target[4] <= L'Z') ||
                 (w_target[4] >= L'a' && w_target[4] <= L'z')) &&
                w_target[5] == L':' &&
                (w_target_len == 6 || w_target[6] == L'\\'))) {
            CloseHandle(handle);
            return CHECK_ERROR(Runtime::setError("Symlink not supported"));
        }

        /* Remove leading \??\ */
        w_target += 4;
        w_target_len -= 4;

    } else {
        /* Reparse tag does not indicate a symlink. */
        CloseHandle(handle);
        return CHECK_ERROR(Runtime::setError("Symlink not supported"));
    }

    retVal = utf16to8String(w_target, w_target_len);

    CloseHandle(handle);
    return 0;
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
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    DWORD w_realpath_len;
    WCHAR* w_realpath_ptr = NULL;
    WCHAR* w_realpath_buf;
    HANDLE handle;

    handle = CreateFileW(UTF8_W(path),
                         0,
                         0,
                         NULL,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
                         NULL);
    if (handle == INVALID_HANDLE_VALUE)
        return CHECK_ERROR(LastError());

    w_realpath_len = GetFinalPathNameByHandleW(handle, NULL, 0, VOLUME_NAME_DOS);
    if (w_realpath_len == 0) {
        CloseHandle(handle);
        return CHECK_ERROR(LastError());
    }

    w_realpath_buf = (WCHAR*)malloc((w_realpath_len + 1) * sizeof(WCHAR));
    if (w_realpath_buf == NULL) {
        CloseHandle(handle);
        return CHECK_ERROR(CALL_E_OVERFLOW);
    }
    w_realpath_ptr = w_realpath_buf;

    if (GetFinalPathNameByHandleW(handle,
                                   w_realpath_ptr,
                                   w_realpath_len,
                                   VOLUME_NAME_DOS) == 0) {
        CloseHandle(handle);
        free(w_realpath_buf);
        return CHECK_ERROR(Runtime::setError("Invalid File Handle"));
    }

    /* convert UNC path to long path */
    if (wcsncmp(w_realpath_ptr,
                UNC_PATH_PREFIX,
                UNC_PATH_PREFIX_LEN) == 0) {
        w_realpath_ptr += 6;
        *w_realpath_ptr = L'\\';
        w_realpath_len -= 6;
    } else if (wcsncmp(w_realpath_ptr,
                       LONG_PATH_PREFIX,
                       LONG_PATH_PREFIX_LEN) == 0) {
        w_realpath_ptr += 4;
        w_realpath_len -= 4;
    } else {
        free(w_realpath_buf);
        CloseHandle(handle);
        return CHECK_ERROR(Runtime::setError("Invalid File Handle"));
    }

    retVal = utf16to8String(w_realpath_ptr, w_realpath_len);

    free(w_realpath_buf);
    CloseHandle(handle);
    return 0;
}

result_t fs_base::fchmod(int32_t fd, int32_t mode, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::fchown(int32_t fd, int32_t uid, int32_t gid, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::fdatasync(int32_t fd, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (FlushFileBuffers((HANDLE)_get_osfhandle(fd)))
        return CHECK_ERROR(LastError());
    return 0;
}

result_t fs_base::fsync(int32_t fd, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (FlushFileBuffers((HANDLE)_get_osfhandle(fd)))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t _create_junction(const WCHAR* path, const WCHAR* new_path)
{

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

    if (!is_absolute)
        return CHECK_ERROR(Runtime::setError("Not supporting relative paths"));

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

result_t fs_base::symlink(exlib::string target, exlib::string linkpath, exlib::string type, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    bool isDir = type == "dir";

    if (isDir || type == "file")
    {
        if (CreateSymbolicLinkW(UTF8_W(linkpath), UTF8_W(target), isDir ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0) == 0)
            return CHECK_ERROR(LastError());
    }
    else if (type == "junction")
    {
        path_base::fullpath(target, target);
        hr = _create_junction(UTF8_W(target), UTF8_W(linkpath));
        if (hr < 0)
            return hr;
    }
    else
        return CHECK_ERROR(CALL_E_INVALIDARG);

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
