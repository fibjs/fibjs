/*
 * pty_spawn_win.c
 *
 *  Created on: Aug 13, 2025) conpty_data_t;

// Function declarations
static int load_conpty_api(void);
static int create_anonymous_pipes(conpty_data_t* data);
static int create_conpty(conpty_data_t* data, int cols, int rows);

// Initialize ConPTY API function pointers      Author: lion
 */

#ifdef _WIN32

#include <uv/include/uv.h>
#include <windows.h>

#include <assert.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <limits.h>
#include <wchar.h>
#include <malloc.h> /* alloca */
#include <fcntl.h> /* for _O_RDONLY, _O_WRONLY */

#include "uv/src/win/internal.h"
#include "uv/src/win/handle-inl.h"
#include "uv/src/win/req-inl.h"

#include <exlib/include/ex_assert.h>

// ConPTY API definitions for compatibility with Windows 10 1809+
#ifndef PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE
#define PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE \
    ProcThreadAttributeValue(22, FALSE, TRUE, FALSE)
#endif

typedef VOID* HPCON;
typedef HRESULT(__stdcall* PFNCREATEPSEUDOCONSOLE)(COORD c, HANDLE hIn, HANDLE hOut, DWORD dwFlags, HPCON* phpcon);
typedef HRESULT(__stdcall* PFNRESIZEPSEUDOCONSOLE)(HPCON hpc, COORD newSize);
typedef VOID(__stdcall* PFNCLOSEPSEUDOCONSOLE)(HPCON hpc);

// Global ConPTY function pointers and state
static PFNCREATEPSEUDOCONSOLE pfnCreatePseudoConsole = NULL;
static PFNRESIZEPSEUDOCONSOLE pfnResizePseudoConsole = NULL;
static PFNCLOSEPSEUDOCONSOLE pfnClosePseudoConsole = NULL;
static HMODULE hKernel32 = NULL;
static int conpty_api_loaded = 0;

// Structure to manage ConPTY state
typedef struct conpty_data {
    HPCON hpc;
    HANDLE hPipeIn; // ConPTY input handle (ConPTY reads from this)
    HANDLE hPipeOut; // ConPTY output handle (ConPTY writes to this)
    HANDLE hParentIn; // Parent input handle (parent reads from this)
    HANDLE hParentOut; // Parent output handle (parent writes to this)
    wchar_t* inPipeName;
    wchar_t* outPipeName;
} conpty_data_t;

// Initialize ConPTY API function pointers
static int load_conpty_api(void)
{
    if (conpty_api_loaded)
        return conpty_api_loaded > 0 ? 0 : -1;

    hKernel32 = LoadLibraryW(L"kernel32.dll");
    if (!hKernel32) {
        conpty_api_loaded = -1;
        return -1;
    }

    pfnCreatePseudoConsole = (PFNCREATEPSEUDOCONSOLE)GetProcAddress(hKernel32, "CreatePseudoConsole");
    pfnResizePseudoConsole = (PFNRESIZEPSEUDOCONSOLE)GetProcAddress(hKernel32, "ResizePseudoConsole");
    pfnClosePseudoConsole = (PFNCLOSEPSEUDOCONSOLE)GetProcAddress(hKernel32, "ClosePseudoConsole");

    if (!pfnCreatePseudoConsole || !pfnResizePseudoConsole || !pfnClosePseudoConsole) {
        FreeLibrary(hKernel32);
        hKernel32 = NULL;
        conpty_api_loaded = -1;
        return -1;
    }

    conpty_api_loaded = 1;
    return 0;
}

// Create named pipes for ConPTY communication
static int create_anonymous_pipes(conpty_data_t* data)
{
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE; // Make inheritable for child process
    sa.lpSecurityDescriptor = NULL;

    // Create anonymous pipes instead of named pipes
    HANDLE hChildStdinRead, hChildStdinWrite;
    HANDLE hChildStdoutRead, hChildStdoutWrite;

    // Create pipe for ConPTY input (parent writes, ConPTY reads)
    if (!CreatePipe(&hChildStdinRead, &hChildStdinWrite, &sa, 0)) {
        return -1;
    }

    // Create pipe for ConPTY output (ConPTY writes, parent reads)
    if (!CreatePipe(&hChildStdoutRead, &hChildStdoutWrite, &sa, 0)) {
        CloseHandle(hChildStdinRead);
        CloseHandle(hChildStdinWrite);
        return -1;
    }

    // Store handles for ConPTY (ConPTY uses the child ends)
    data->hPipeIn = hChildStdoutWrite; // ConPTY writes to this
    data->hPipeOut = hChildStdinRead; // ConPTY reads from this

    // Store handles for parent (parent uses the other ends)
    data->hParentIn = hChildStdoutRead; // Parent reads from this
    data->hParentOut = hChildStdinWrite; // Parent writes to this

    return 0;
}

// Create ConPTY console
static int create_conpty(conpty_data_t* data, int cols, int rows)
{
    COORD consoleSize = { (SHORT)cols, (SHORT)rows };
    HRESULT hr = pfnCreatePseudoConsole(
        consoleSize,
        data->hPipeOut, // hIn: ConPTY reads from this (we write to it - our stdin to ConPTY)
        data->hPipeIn, // hOut: ConPTY writes to this (we read from it - ConPTY's output to us)
        0, // dwFlags
        &data->hpc);

    if (FAILED(hr)) {
        return (int)hr;
    }

    return 0;
}

// External functions from libuv
int make_program_args(char** args, int verbatim_arguments, WCHAR** dst_ptr);
int env_strncmp(const wchar_t* a, int na, const wchar_t* b);
int make_program_env(char* env_block[], WCHAR** dst_ptr);

// Forward declarations

#define SIGKILL 9

typedef struct env_var {
    const WCHAR* const wide;
    const WCHAR* const wide_eq;
    const size_t len; /* including null or '=' */
} env_var_t;

#define E_V(str) { L##str, L##str L"=", sizeof(str) }

static const env_var_t required_vars[] = {
    /* keep me sorted */
    E_V("HOMEDRIVE"),
    E_V("HOMEPATH"),
    E_V("LOGONSERVER"),
    E_V("PATH"),
    E_V("SYSTEMDRIVE"),
    E_V("SYSTEMROOT"),
    E_V("TEMP"),
    E_V("USERDOMAIN"),
    E_V("USERNAME"),
    E_V("USERPROFILE"),
    E_V("WINDIR"),
};

static HANDLE uv_global_job_handle_;
static uv_once_t uv_global_job_handle_init_guard_ = UV_ONCE_INIT;

static void uv__init_global_job_handle(void)
{
    SECURITY_ATTRIBUTES attr;
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION info;

    memset(&attr, 0, sizeof attr);
    attr.bInheritHandle = FALSE;

    memset(&info, 0, sizeof info);
    info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_BREAKAWAY_OK | JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK | JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION | JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

    uv_global_job_handle_ = CreateJobObjectW(&attr, NULL);
    if (uv_global_job_handle_ == NULL)
        uv_fatal_error(GetLastError(), "CreateJobObjectW");

    if (!SetInformationJobObject(uv_global_job_handle_,
            JobObjectExtendedLimitInformation,
            &info,
            sizeof info))
        uv_fatal_error(GetLastError(), "SetInformationJobObject");
}

static int uv__utf8_to_utf16_alloc(const char* s, WCHAR** ws_ptr)
{
    int ws_len, r;
    WCHAR* ws;

    ws_len = MultiByteToWideChar(CP_UTF8,
        0,
        s,
        -1,
        NULL,
        0);
    if (ws_len <= 0) {
        return GetLastError();
    }

    ws = (WCHAR*)uv__malloc(ws_len * sizeof(WCHAR));
    if (ws == NULL) {
        return ERROR_OUTOFMEMORY;
    }

    r = MultiByteToWideChar(CP_UTF8,
        0,
        s,
        -1,
        ws,
        ws_len);
    ex_assert(r == ws_len);

    *ws_ptr = ws;
    return 0;
}

static void uv__process_init(uv_loop_t* loop, uv_process_t* handle)
{
    uv__handle_init(loop, (uv_handle_t*)handle, UV_PROCESS);
    handle->exit_cb = NULL;
    handle->pid = 0;
    handle->exit_signal = 0;
    handle->wait_handle = INVALID_HANDLE_VALUE;
    handle->process_handle = INVALID_HANDLE_VALUE;
    handle->child_stdio_buffer = NULL;
    handle->exit_cb_pending = 0;

    UV_REQ_INIT(&handle->exit_req, UV_PROCESS_EXIT);
    handle->exit_req.data = handle;
}

/*
 * Path search functions
 */

/*
 * Helper function for search_path
 */
static WCHAR* search_path_join_test(const WCHAR* dir,
    size_t dir_len,
    const WCHAR* name,
    size_t name_len,
    const WCHAR* ext,
    size_t ext_len,
    const WCHAR* cwd,
    size_t cwd_len)
{
    WCHAR *result, *result_pos;
    DWORD attrs;
    if (dir_len > 2 && ((dir[0] == L'\\' || dir[0] == L'/') && (dir[1] == L'\\' || dir[1] == L'/'))) {
        /* It's a UNC path so ignore cwd */
        cwd_len = 0;
    } else if (dir_len >= 1 && (dir[0] == L'/' || dir[0] == L'\\')) {
        /* It's a full path without drive letter, use cwd's drive letter only */
        cwd_len = 2;
    } else if (dir_len >= 2 && dir[1] == L':' && (dir_len < 3 || (dir[2] != L'/' && dir[2] != L'\\'))) {
        /* It's a relative path with drive letter (ext.g. D:../some/file)
         * Replace drive letter in dir by full cwd if it points to the same drive,
         * otherwise use the dir only.
         */
        if (cwd_len < 2 || _wcsnicmp(cwd, dir, 2) != 0) {
            cwd_len = 0;
        } else {
            dir += 2;
            dir_len -= 2;
        }
    } else if (dir_len > 2 && dir[1] == L':') {
        /* It's an absolute path with drive letter
         * Don't use the cwd at all
         */
        cwd_len = 0;
    }

    /* Allocate buffer for output */
    result = result_pos = (WCHAR*)uv__malloc(sizeof(WCHAR) * (cwd_len + 1 + dir_len + 1 + name_len + 1 + ext_len + 1));

    /* Copy cwd */
    wcsncpy(result_pos, cwd, cwd_len);
    result_pos += cwd_len;

    /* Add a path separator if cwd didn't end with one */
    if (cwd_len && wcsrchr(L"\\/:", result_pos[-1]) == NULL) {
        result_pos[0] = L'\\';
        result_pos++;
    }

    /* Copy dir */
    wcsncpy(result_pos, dir, dir_len);
    result_pos += dir_len;

    /* Add a separator if the dir didn't end with one */
    if (dir_len && wcsrchr(L"\\/:", result_pos[-1]) == NULL) {
        result_pos[0] = L'\\';
        result_pos++;
    }

    /* Copy filename */
    wcsncpy(result_pos, name, name_len);
    result_pos += name_len;

    if (ext_len) {
        /* Add a dot if the filename didn't end with one */
        if (name_len && result_pos[-1] != '.') {
            result_pos[0] = L'.';
            result_pos++;
        }

        /* Copy extension */
        wcsncpy(result_pos, ext, ext_len);
        result_pos += ext_len;
    }

    /* Null terminator */
    result_pos[0] = L'\0';

    attrs = GetFileAttributesW(result);

    if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        return result;
    }

    uv__free(result);
    return NULL;
}

/*
 * Helper function for search_path
 */
static WCHAR* path_search_walk_ext(const WCHAR* dir,
    size_t dir_len,
    const WCHAR* name,
    size_t name_len,
    WCHAR* cwd,
    size_t cwd_len,
    int name_has_ext)
{
    WCHAR* result;

    /* If the name itself has a nonempty extension, try this extension first */
    if (name_has_ext) {
        result = search_path_join_test(dir, dir_len,
            name, name_len,
            L"", 0,
            cwd, cwd_len);
        if (result != NULL) {
            return result;
        }
    }

    /* Try .com extension */
    result = search_path_join_test(dir, dir_len,
        name, name_len,
        L"com", 3,
        cwd, cwd_len);
    if (result != NULL) {
        return result;
    }

    /* Try .exe extension */
    result = search_path_join_test(dir, dir_len,
        name, name_len,
        L"exe", 3,
        cwd, cwd_len);
    if (result != NULL) {
        return result;
    }

    return NULL;
}

static WCHAR* search_path(const WCHAR* file,
    WCHAR* cwd,
    const WCHAR* path)
{
    int file_has_dir;
    WCHAR* result = NULL;
    WCHAR* file_name_start;
    WCHAR* dot;
    const WCHAR *dir_start, *dir_end, *dir_path;
    size_t dir_len;
    int name_has_ext;

    size_t file_len = wcslen(file);
    size_t cwd_len = wcslen(cwd);

    /* If the caller supplies an empty filename,
     * we're not gonna return c:\windows\.exe -- GFY!
     */
    if (file_len == 0
        || (file_len == 1 && file[0] == L'.')) {
        return NULL;
    }

    /* Find the start of the filename so we can split the directory from the
     * name. */
    for (file_name_start = (WCHAR*)file + file_len;
        file_name_start > file
        && file_name_start[-1] != L'\\'
        && file_name_start[-1] != L'/'
        && file_name_start[-1] != L':';
        file_name_start--)
        ;

    file_has_dir = file_name_start != file;

    /* Check if the filename includes an extension */
    dot = wcschr(file_name_start, L'.');
    name_has_ext = (dot != NULL && dot[1] != L'\0');

    if (file_has_dir) {
        /* The file has a path inside, don't use path */
        result = path_search_walk_ext(
            file, file_name_start - file,
            file_name_start, file_len - (file_name_start - file),
            cwd, cwd_len,
            name_has_ext);

    } else {
        dir_end = path;

        /* The file is really only a name; look in cwd first, then scan path */
        result = path_search_walk_ext(L"", 0,
            file, file_len,
            cwd, cwd_len,
            name_has_ext);

        while (result == NULL) {
            if (*dir_end == L'\0') {
                break;
            }

            /* Skip the separator that dir_end now points to */
            if (dir_end != path || *path == L';') {
                dir_end++;
            }

            /* Next slice starts just after where the previous one ended */
            dir_start = dir_end;

            /* If path is quoted, find quote end */
            if (*dir_start == L'"' || *dir_start == L'\'') {
                dir_end = wcschr(dir_start + 1, *dir_start);
                if (dir_end == NULL) {
                    dir_end = wcschr(dir_start, L'\0');
                }
            }
            /* Slice until the next ; or \0 is found */
            dir_end = wcschr(dir_end, L';');
            if (dir_end == NULL) {
                dir_end = wcschr(dir_start, L'\0');
            }

            /* If the slice is zero-length, don't bother */
            if (dir_end - dir_start == 0) {
                continue;
            }

            dir_path = dir_start;
            dir_len = dir_end - dir_start;

            /* Adjust if the path is quoted. */
            if (dir_path[0] == '"' || dir_path[0] == '\'') {
                ++dir_path;
                --dir_len;
            }

            if (dir_path[dir_len - 1] == '"' || dir_path[dir_len - 1] == '\'') {
                --dir_len;
            }

            result = path_search_walk_ext(dir_path, dir_len,
                file, file_len,
                cwd, cwd_len,
                name_has_ext);
        }
    }

    return result;
}

static int qsort_wcscmp(const void* a, const void* b)
{
    wchar_t* astr = *(wchar_t* const*)a;
    wchar_t* bstr = *(wchar_t* const*)b;
    return env_strncmp(astr, -1, bstr);
}

/*
 * Attempt to find the value of the PATH environment variable in the child's
 * preprocessed environment.
 *
 * If found, a pointer into `env` is returned. If not found, NULL is returned.
 */
static WCHAR* find_path(WCHAR* env)
{
    for (; env != NULL && *env != 0; env += wcslen(env) + 1) {
        if ((env[0] == L'P' || env[0] == L'p') && (env[1] == L'A' || env[1] == L'a') && (env[2] == L'T' || env[2] == L't') && (env[3] == L'H' || env[3] == L'h') && (env[4] == L'=')) {
            return &env[5];
        }
    }

    return NULL;
}

/*
 * Called on Windows thread-pool thread to indicate that
 * a child process has exited.
 */
static void CALLBACK exit_wait_callback(void* data, BOOLEAN didTimeout)
{
    uv_process_t* process = (uv_process_t*)data;
    uv_loop_t* loop = process->loop;

    ex_assert(didTimeout == FALSE);
    ex_assert(process);
    ex_assert(!process->exit_cb_pending);

    process->exit_cb_pending = 1;

    /* Post completed */
    POST_COMPLETION_FOR_REQ(loop, &process->exit_req);
}

int pty_spawn(uv_loop_t* loop,
    uv_process_t* process,
    const uv_process_options_t* options,
    int* stdinfd,
    int* stdoutfd,
    int cols,
    int rows)
{
    int err = 0;
    WCHAR *path = NULL, *alloc_path = NULL;
    BOOL result;
    WCHAR *application_path = NULL, *application = NULL, *arguments = NULL,
          *env = NULL, *cwd = NULL;
    STARTUPINFOEXW startup = { 0 };
    PROCESS_INFORMATION info = { 0 };
    DWORD process_flags;
    conpty_data_t conpty_data = { 0 };
    SIZE_T attrListSize = 0;

    // Initialize ConPTY API
    if (load_conpty_api() < 0) {
        // ConPTY not available, return error
        return UV_ENOTSUP;
    }

    uv__process_init(loop, process);
    process->exit_cb = options->exit_cb;

    if (options->flags & (UV_PROCESS_SETGID | UV_PROCESS_SETUID)) {
        return UV_ENOTSUP;
    }

    if (options->file == NULL || options->args == NULL) {
        return UV_EINVAL;
    }

    ex_assert(options->file != NULL);

    // Create anonymous pipes for ConPTY
    err = create_anonymous_pipes(&conpty_data);
    if (err) {
        goto done;
    }

    // Create ConPTY
    err = create_conpty(&conpty_data, cols, rows);
    if (err) {
        goto done;
    }

    // Prepare process creation first, before connecting pipes
    // This follows the node-pty pattern more closely

    // Prepare process creation parameters
    err = uv__utf8_to_utf16_alloc(options->file, &application);
    if (err)
        goto done;

    err = make_program_args(
        options->args,
        options->flags & UV_PROCESS_WINDOWS_VERBATIM_ARGUMENTS,
        &arguments);
    if (err)
        goto done;

    if (options->env) {
        err = make_program_env(options->env, &env);
        if (err)
            goto done;
    }

    if (options->cwd) {
        err = uv__utf8_to_utf16_alloc(options->cwd, &cwd);
        if (err)
            goto done;
    } else {
        DWORD cwd_len, r;
        cwd_len = GetCurrentDirectoryW(0, NULL);
        if (!cwd_len) {
            err = GetLastError();
            goto done;
        }
        cwd = (WCHAR*)uv__malloc(cwd_len * sizeof(WCHAR));
        if (cwd == NULL) {
            err = ERROR_OUTOFMEMORY;
            goto done;
        }
        r = GetCurrentDirectoryW(cwd_len, cwd);
        if (r == 0 || r >= cwd_len) {
            err = GetLastError();
            goto done;
        }
    }

    // Get PATH and find executable
    path = find_path(env);
    if (path == NULL) {
        DWORD path_len, r;
        path_len = GetEnvironmentVariableW(L"PATH", NULL, 0);
        if (path_len == 0) {
            err = GetLastError();
            goto done;
        }
        alloc_path = (WCHAR*)uv__malloc(path_len * sizeof(WCHAR));
        if (alloc_path == NULL) {
            err = ERROR_OUTOFMEMORY;
            goto done;
        }
        path = alloc_path;
        r = GetEnvironmentVariableW(L"PATH", path, path_len);
        if (r == 0 || r >= path_len) {
            err = GetLastError();
            goto done;
        }
    }

    application_path = search_path(application, cwd, path);
    if (application_path == NULL) {
        err = ERROR_FILE_NOT_FOUND;
        goto done;
    }

    // Setup STARTUPINFOEX with ConPTY
    startup.StartupInfo.cb = sizeof(STARTUPINFOEXW);

    // Initialize process thread attribute list
    InitializeProcThreadAttributeList(NULL, 1, 0, &attrListSize);
    startup.lpAttributeList = (PPROC_THREAD_ATTRIBUTE_LIST)uv__malloc(attrListSize);
    if (!startup.lpAttributeList) {
        err = ERROR_OUTOFMEMORY;
        goto done;
    }

    if (!InitializeProcThreadAttributeList(startup.lpAttributeList, 1, 0, &attrListSize)) {
        err = GetLastError();
        goto done;
    }

    // Associate the ConPTY with this process
    if (!UpdateProcThreadAttribute(
            startup.lpAttributeList,
            0,
            PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
            conpty_data.hpc,
            sizeof(HPCON),
            NULL,
            NULL)) {
        err = GetLastError();
        goto done;
    }

    process_flags = EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT;

    if ((options->flags & UV_PROCESS_WINDOWS_HIDE_CONSOLE) || (options->flags & UV_PROCESS_WINDOWS_HIDE)) {
        process_flags |= CREATE_NO_WINDOW;
    }
    if ((options->flags & UV_PROCESS_WINDOWS_HIDE_GUI) || (options->flags & UV_PROCESS_WINDOWS_HIDE)) {
        startup.StartupInfo.wShowWindow = SW_HIDE;
        startup.StartupInfo.dwFlags |= STARTF_USESHOWWINDOW;
    } else {
        startup.StartupInfo.wShowWindow = SW_SHOWDEFAULT;
        startup.StartupInfo.dwFlags |= STARTF_USESHOWWINDOW;
    }

    if (options->flags & UV_PROCESS_DETACHED) {
        process_flags |= DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP;
    }

    // Anonymous pipes are immediately ready - no connection needed

    // Create the process with ConPTY attached
    if (!CreateProcessW(application_path,
            arguments,
            NULL,
            NULL,
            FALSE, // bInheritHandles: FALSE for ConPTY
            process_flags,
            env,
            cwd,
            &startup.StartupInfo,
            &info)) {
        err = GetLastError();
        goto done;
    }

    // Success! Setup process information
    process->process_handle = info.hProcess;
    process->pid = info.dwProcessId;

    // Now create client handles for the parent process to use
    // Use the parent handles from anonymous pipes
    *stdinfd = _open_osfhandle((intptr_t)conpty_data.hParentOut, _O_WRONLY); // Parent writes to child stdin
    *stdoutfd = _open_osfhandle((intptr_t)conpty_data.hParentIn, _O_RDONLY); // Parent reads from child stdout
    if (*stdinfd == -1 || *stdoutfd == -1) {
        err = ERROR_INVALID_HANDLE;
        goto done;
    }

    // Store ConPTY handle in process for cleanup (we'll use the child_stdio_buffer field)
    process->child_stdio_buffer = (HANDLE*)uv__malloc(sizeof(conpty_data_t));
    if (process->child_stdio_buffer) {
        memcpy(process->child_stdio_buffer, &conpty_data, sizeof(conpty_data_t));
    }

    // Process job assignment
    if (!(options->flags & UV_PROCESS_DETACHED)) {
        uv_once(&uv_global_job_handle_init_guard_, uv__init_global_job_handle);
        if (!AssignProcessToJobObject(uv_global_job_handle_, info.hProcess)) {
            DWORD job_err = GetLastError();
            if (job_err != ERROR_ACCESS_DENIED)
                uv_fatal_error(job_err, "AssignProcessToJobObject");
        }
    }

    // Setup exit callback
    result = RegisterWaitForSingleObject(&process->wait_handle,
        process->process_handle, exit_wait_callback, (void*)process, INFINITE,
        WT_EXECUTEINWAITTHREAD | WT_EXECUTEONLYONCE);
    if (!result) {
        uv_fatal_error(GetLastError(), "RegisterWaitForSingleObject");
    }

    CloseHandle(info.hThread);
    uv__handle_start(process);

done:
    // Cleanup on success - these handles are now managed by the file descriptors
    // On failure, cleanup everything

    if (startup.lpAttributeList) {
        DeleteProcThreadAttributeList(startup.lpAttributeList);
        uv__free(startup.lpAttributeList);
    }

    uv__free(application);
    uv__free(application_path);
    uv__free(arguments);
    uv__free(cwd);
    uv__free(env);
    uv__free(alloc_path);

    if (err != 0) {
        // Cleanup on failure - note: don't close pipe handles if they're now file descriptors
        if (*stdinfd == -1 && conpty_data.hPipeOut != INVALID_HANDLE_VALUE)
            CloseHandle(conpty_data.hPipeOut);
        if (*stdoutfd == -1 && conpty_data.hPipeIn != INVALID_HANDLE_VALUE)
            CloseHandle(conpty_data.hPipeIn);
        if (conpty_data.hpc)
            pfnClosePseudoConsole(conpty_data.hpc);
        if (conpty_data.inPipeName)
            uv__free(conpty_data.inPipeName);
        if (conpty_data.outPipeName)
            uv__free(conpty_data.outPipeName);
    }

    return uv_translate_sys_error(err);
}

int pty_resize(uv_process_t* process, int cols, int rows)
{
    if (!process || cols <= 0 || rows <= 0)
        return -1;

    // Load ConPTY API if needed
    if (load_conpty_api() < 0) {
        // ConPTY not available
        return -1;
    }

    // Get ConPTY data from process->child_stdio_buffer
    if (!process->child_stdio_buffer)
        return -1;

    conpty_data_t* conpty_data = (conpty_data_t*)process->child_stdio_buffer;
    if (!conpty_data->hpc)
        return -1;

    // Resize the ConPTY
    COORD newSize = { (SHORT)cols, (SHORT)rows };
    HRESULT hr = pfnResizePseudoConsole(conpty_data->hpc, newSize);

    return SUCCEEDED(hr) ? 0 : -1;
}

void pty_cleanup(uv_process_t* process)
{
    if (!process || !process->child_stdio_buffer)
        return;

    // Load ConPTY API if needed
    if (load_conpty_api() < 0)
        return;

    conpty_data_t* conpty_data = (conpty_data_t*)process->child_stdio_buffer;

    // Close ConPTY handle
    if (conpty_data->hpc) {
        pfnClosePseudoConsole(conpty_data->hpc);
        conpty_data->hpc = NULL;
    }

    // Clean up pipe names if allocated
    if (conpty_data->inPipeName) {
        uv__free(conpty_data->inPipeName);
        conpty_data->inPipeName = NULL;
    }
    if (conpty_data->outPipeName) {
        uv__free(conpty_data->outPipeName);
        conpty_data->outPipeName = NULL;
    }

    // Free the buffer
    uv__free(process->child_stdio_buffer);
    process->child_stdio_buffer = NULL;
}

#endif // _WIN32
