/*
 * WASIHost.cpp
 *
 * WASI preview1 host. The JS/V8 glue is generated from idl/WASIHost.idl by
 * tools/idlc.js; this file implements the syscall semantics.
 *
 * Two-phase discipline (the one the generated ac_ wrappers use):
 *   - the sync phase runs in the JS scope: it validates every guest pointer and
 *     copies the iovec payload into a staging buffer (guest memory may grow
 *     while the fiber is suspended, so no raw pointer is kept);
 *   - the async phase runs after Isolate::LeaveJsScope: it only starts the uv
 *     request and returns pending;
 *   - the uv completion cannot write guest memory (V8 must not be touched from
 *     the uv thread), so it hands the result to Isolate::sync(), which runs on
 *     the JS thread with a V8 scope. That task copies the result back into
 *     guest memory and posts the AsyncHandle, resuming the fiber.
 *
 * Descriptor table:
 *   - 0/1/2 are the host standard streams (not owned);
 *   - preopened directories get 3.. (virtual directory fds: the host path is
 *     stored, no host descriptor is kept);
 *   - files opened by path_open own their host fd and are closed by fd_close.
 */

#include "object.h"
#include "ifs/WASIHost.h"
#include "ifs/wasi.h"
#include "ifs/process.h"
#include "AsyncUV.h"
#include "Fiber.h"
#include "errhelp.h"
#include "wasm_module.h"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <deque>
#include <fcntl.h>
#include <map>
#include <memory>
#include <mutex>
#ifndef _WIN32
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#endif
#include <string>
#include <sys/stat.h>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fibjs {

DECLARE_MODULE(wasi);

// WASI preview1 errno values used by this file.
enum {
    WASI_ERRNO_SUCCESS = 0,
    WASI_ERRNO_ACCES = 2,
    WASI_ERRNO_AGAIN = 6,
    WASI_ERRNO_BADF = 8,
    WASI_ERRNO_EXIST = 20,
    WASI_ERRNO_FAULT = 21,
    WASI_ERRNO_INVAL = 28,
    WASI_ERRNO_IO = 29,
    WASI_ERRNO_ISDIR = 31,
    WASI_ERRNO_LOOP = 32,
    WASI_ERRNO_MFILE = 33,
    WASI_ERRNO_NAMETOOLONG = 37,
    WASI_ERRNO_NOBUFS = 42,
    WASI_ERRNO_NOENT = 44,
    WASI_ERRNO_NOSPC = 51,
    WASI_ERRNO_NOSYS = 52,
    WASI_ERRNO_NOTDIR = 54,
    WASI_ERRNO_NOTEMPTY = 55,
    WASI_ERRNO_NOTSOCK = 57,
    WASI_ERRNO_NOTSUP = 58,
    WASI_ERRNO_OVERFLOW = 61,
    WASI_ERRNO_PERM = 63,
    WASI_ERRNO_PIPE = 64,
    WASI_ERRNO_ROFS = 69,
    WASI_ERRNO_SPIPE = 70,
    WASI_ERRNO_TIMEDOUT = 73,
    WASI_ERRNO_NOTCAPABLE = 76,
};

enum {
    WASI_EVENTTYPE_CLOCK = 0,
};

// File types, open flags, descriptor flags and the rights used by the host.
enum {
    WASI_FILETYPE_UNKNOWN = 0,
    WASI_FILETYPE_BLOCK_DEVICE = 1,
    WASI_FILETYPE_CHARACTER_DEVICE = 2,
    WASI_FILETYPE_DIRECTORY = 3,
    WASI_FILETYPE_REGULAR_FILE = 4,
    WASI_FILETYPE_SOCKET_DGRAM = 5,
    WASI_FILETYPE_SOCKET_STREAM = 6,
    WASI_FILETYPE_SYMBOLIC_LINK = 7,
};

enum {
    WASI_O_CREAT = 1 << 0,
    WASI_O_DIRECTORY = 1 << 1,
    WASI_O_EXCL = 1 << 2,
    WASI_O_TRUNC = 1 << 3,
};

enum {
    WASI_FDFLAG_APPEND = 1 << 0,
    WASI_FDFLAG_DSYNC = 1 << 1,
    WASI_FDFLAG_NONBLOCK = 1 << 2,
    WASI_FDFLAG_RSYNC = 1 << 3,
    WASI_FDFLAG_SYNC = 1 << 4,
};

enum {
    WASI_RIGHT_FD_DATASYNC = 1ull << 0,
    WASI_RIGHT_FD_READ = 1ull << 1,
    WASI_RIGHT_FD_SEEK = 1ull << 2,
    WASI_RIGHT_FD_FDSTAT_SET_FLAGS = 1ull << 3,
    WASI_RIGHT_FD_SYNC = 1ull << 4,
    WASI_RIGHT_FD_TELL = 1ull << 5,
    WASI_RIGHT_FD_WRITE = 1ull << 6,
    WASI_RIGHT_FD_ADVISE = 1ull << 7,
    WASI_RIGHT_FD_ALLOCATE = 1ull << 8,
    WASI_RIGHT_PATH_CREATE_DIRECTORY = 1ull << 9,
    WASI_RIGHT_PATH_CREATE_FILE = 1ull << 10,
    WASI_RIGHT_PATH_LINK_SOURCE = 1ull << 11,
    WASI_RIGHT_PATH_LINK_TARGET = 1ull << 12,
    WASI_RIGHT_PATH_OPEN = 1ull << 13,
    WASI_RIGHT_FD_READDIR = 1ull << 14,
    WASI_RIGHT_PATH_READLINK = 1ull << 15,
    WASI_RIGHT_PATH_RENAME_SOURCE = 1ull << 16,
    WASI_RIGHT_PATH_RENAME_TARGET = 1ull << 17,
    WASI_RIGHT_PATH_FILESTAT_GET = 1ull << 18,
    WASI_RIGHT_PATH_FILESTAT_SET_SIZE = 1ull << 19,
    WASI_RIGHT_PATH_FILESTAT_SET_TIMES = 1ull << 20,
    WASI_RIGHT_FD_FILESTAT_GET = 1ull << 21,
    WASI_RIGHT_FD_FILESTAT_SET_SIZE = 1ull << 22,
    WASI_RIGHT_FD_FILESTAT_SET_TIMES = 1ull << 23,
    WASI_RIGHT_PATH_SYMLINK = 1ull << 24,
    WASI_RIGHT_PATH_REMOVE_DIRECTORY = 1ull << 25,
    WASI_RIGHT_PATH_UNLINK_FILE = 1ull << 26,
    WASI_RIGHT_POLL_FD_READWRITE = 1ull << 27,
    WASI_RIGHT_SOCK_SHUTDOWN = 1ull << 28,
    WASI_RIGHT_SOCK_ACCEPT = 1ull << 29,
};

static const uint64_t RIGHTS_ALL = (1ull << 30) - 1;

static const uint64_t FILE_RIGHTS = WASI_RIGHT_FD_DATASYNC | WASI_RIGHT_FD_READ
    | WASI_RIGHT_FD_SEEK | WASI_RIGHT_FD_FDSTAT_SET_FLAGS | WASI_RIGHT_FD_SYNC
    | WASI_RIGHT_FD_TELL | WASI_RIGHT_FD_WRITE | WASI_RIGHT_FD_ADVISE
    | WASI_RIGHT_FD_ALLOCATE | WASI_RIGHT_FD_FILESTAT_GET
    | WASI_RIGHT_FD_FILESTAT_SET_SIZE | WASI_RIGHT_FD_FILESTAT_SET_TIMES
    | WASI_RIGHT_POLL_FD_READWRITE;

static const uint64_t FILE_INHERITING = 0;

// uvwasi__RIGHTS_DIRECTORY_BASE: no read/write/seek/tell, but fdstat/times
// rights are inherited by directory descriptors.
static const uint64_t DIR_RIGHTS = WASI_RIGHT_FD_FDSTAT_SET_FLAGS
    | WASI_RIGHT_FD_SYNC | WASI_RIGHT_FD_ADVISE
    | WASI_RIGHT_PATH_CREATE_DIRECTORY | WASI_RIGHT_PATH_CREATE_FILE
    | WASI_RIGHT_PATH_LINK_SOURCE | WASI_RIGHT_PATH_LINK_TARGET
    | WASI_RIGHT_PATH_OPEN | WASI_RIGHT_FD_READDIR | WASI_RIGHT_PATH_READLINK
    | WASI_RIGHT_PATH_RENAME_SOURCE | WASI_RIGHT_PATH_RENAME_TARGET
    | WASI_RIGHT_PATH_FILESTAT_GET | WASI_RIGHT_PATH_FILESTAT_SET_SIZE
    | WASI_RIGHT_PATH_FILESTAT_SET_TIMES | WASI_RIGHT_FD_FILESTAT_GET
    | WASI_RIGHT_FD_FILESTAT_SET_TIMES | WASI_RIGHT_PATH_SYMLINK
    | WASI_RIGHT_PATH_UNLINK_FILE | WASI_RIGHT_PATH_REMOVE_DIRECTORY
    | WASI_RIGHT_POLL_FD_READWRITE;

static const uint64_t DIR_INHERITING = DIR_RIGHTS | FILE_RIGHTS;

static const uint64_t TTY_RIGHTS = WASI_RIGHT_FD_READ
    | WASI_RIGHT_FD_FDSTAT_SET_FLAGS | WASI_RIGHT_FD_WRITE
    | WASI_RIGHT_FD_FILESTAT_GET | WASI_RIGHT_POLL_FD_READWRITE;

static const uint64_t SOCKET_RIGHTS = WASI_RIGHT_FD_READ
    | WASI_RIGHT_FD_FDSTAT_SET_FLAGS | WASI_RIGHT_FD_WRITE
    | WASI_RIGHT_FD_FILESTAT_GET | WASI_RIGHT_POLL_FD_READWRITE
    | WASI_RIGHT_SOCK_SHUTDOWN | WASI_RIGHT_SOCK_ACCEPT;

static int32_t wasi_errno(int32_t uv_err)
{
    switch (uv_err) {
    case UV_EACCES:
        return WASI_ERRNO_ACCES;
    case UV_EAGAIN:
        return WASI_ERRNO_AGAIN;
    case UV_EBADF:
        return WASI_ERRNO_BADF;
    case UV_EEXIST:
        return WASI_ERRNO_EXIST;
    case UV_EFAULT:
        return WASI_ERRNO_FAULT;
    case UV_EINVAL:
        return WASI_ERRNO_INVAL;
    case UV_EISDIR:
        return WASI_ERRNO_ISDIR;
    case UV_ELOOP:
        return WASI_ERRNO_LOOP;
    case UV_EMFILE:
        return WASI_ERRNO_MFILE;
    case UV_ENAMETOOLONG:
        return WASI_ERRNO_NAMETOOLONG;
    case UV_ENOENT:
        return WASI_ERRNO_NOENT;
    case UV_ENOSPC:
        return WASI_ERRNO_NOSPC;
    case UV_ENOSYS:
        return WASI_ERRNO_NOSYS;
    case UV_ENOTDIR:
        return WASI_ERRNO_NOTDIR;
    case UV_ENOTEMPTY:
        return WASI_ERRNO_NOTEMPTY;
    case UV_ENOTSUP:
        return WASI_ERRNO_NOTSUP;
    case UV_EPERM:
        return WASI_ERRNO_PERM;
    case UV_EPIPE:
        return WASI_ERRNO_PIPE;
    case UV_EROFS:
        return WASI_ERRNO_ROFS;
    case UV_ESPIPE:
        return WASI_ERRNO_SPIPE;
    case UV_ETIMEDOUT:
        return WASI_ERRNO_TIMEDOUT;
    default:
        return WASI_ERRNO_IO;
    }
}

static inline uint32_t read_u32(const uint8_t* p)
{
    uint32_t v;
    memcpy(&v, p, sizeof(v));
    return v;
}

static inline uint64_t read_u64(const uint8_t* p)
{
    uint64_t v;
    memcpy(&v, p, sizeof(v));
    return v;
}

static inline uint16_t read_u16(const uint8_t* p)
{
    uint16_t v;
    memcpy(&v, p, sizeof(v));
    return v;
}

static inline void write_u32(uint8_t* p, uint32_t v)
{
    memcpy(p, &v, sizeof(v));
}

static inline void write_u16(uint8_t* p, uint16_t v)
{
    memcpy(p, &v, sizeof(v));
}

static inline void write_u64(uint8_t* p, uint64_t v)
{
    memcpy(p, &v, sizeof(v));
}

static inline uint64_t timespec_ns(const uv_timespec_t& ts)
{
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static uint8_t filetype_from_mode(uint64_t mode)
{
#ifdef _WIN32
    switch (mode & _S_IFMT) {
    case _S_IFDIR:
        return WASI_FILETYPE_DIRECTORY;
    case _S_IFREG:
        return WASI_FILETYPE_REGULAR_FILE;
    case _S_IFCHR:
        return WASI_FILETYPE_CHARACTER_DEVICE;
    default:
        return WASI_FILETYPE_UNKNOWN;
    }
#else
    if (S_ISDIR(mode))
        return WASI_FILETYPE_DIRECTORY;
    if (S_ISREG(mode))
        return WASI_FILETYPE_REGULAR_FILE;
    if (S_ISLNK(mode))
        return WASI_FILETYPE_SYMBOLIC_LINK;
    if (S_ISCHR(mode))
        return WASI_FILETYPE_CHARACTER_DEVICE;
    if (S_ISSOCK(mode))
        return WASI_FILETYPE_SOCKET_STREAM;
#ifdef S_ISFIFO
    if (S_ISFIFO(mode))
        return WASI_FILETYPE_SOCKET_STREAM;
#endif
    if (S_ISBLK(mode))
        return WASI_FILETYPE_BLOCK_DEVICE;
    return WASI_FILETYPE_UNKNOWN;
#endif
}

// filestat: dev 0, ino 8, filetype 16, nlink 24, size 32, atim 40, mtim 48,
// ctim 56; all little-endian.
static uint8_t dirent_type(uv_dirent_type_t type)
{
    switch (type) {
    case UV_DIRENT_FILE:
        return WASI_FILETYPE_REGULAR_FILE;
    case UV_DIRENT_DIR:
        return WASI_FILETYPE_DIRECTORY;
    case UV_DIRENT_LINK:
        return WASI_FILETYPE_SYMBOLIC_LINK;
    case UV_DIRENT_CHAR:
        return WASI_FILETYPE_CHARACTER_DEVICE;
    case UV_DIRENT_BLOCK:
        return 1;
    default:
        return WASI_FILETYPE_UNKNOWN;
    }
}

static void fill_filestat(const uv_stat_t& st, uint8_t* out)
{
    memset(out, 0, 64);
    write_u64(out + 0, st.st_dev);
    write_u64(out + 8, st.st_ino);
    out[16] = filetype_from_mode(st.st_mode);
    write_u64(out + 24, st.st_nlink);
    write_u64(out + 32, st.st_size);
    write_u64(out + 40, timespec_ns(st.st_atim));
    write_u64(out + 48, timespec_ns(st.st_mtim));
    write_u64(out + 56, timespec_ns(st.st_ctim));
}

struct FdEntry {
    enum Kind {
        STDIO,
        FILE,
        DIR,
        SOCKET,
    };

    // Identifies this descriptor instance. A syscall records it in its call
    // and uses it to find the entry again on completion: the guest fd number
    // may have been renumbered away while the request was in flight, and the
    // host is lock free, so another fiber can edit the table in between.
    uint64_t id = 0;
    Kind kind = STDIO;
    int32_t host_fd = -1;
    exlib::string host_path; // DIR: the host directory path
    exlib::string guest_name; // preopen: the guest path
    bool preopen = false;
    bool owned = false;
    uint16_t fdflags = 0;
    uint64_t rights_base = 0;
    uint64_t rights_inheriting = 0;
    int64_t offset = 0;
    struct SocketState* sock = nullptr; // SOCKET: listener or connection
};

class WasiOp;

// One host socket handle plus the connections accepted but not yet claimed by a
// sock_accept call. The uv thread owns the handle; only the fields under the
// mutex are touched from the JS thread.
struct SocketState {
    uv_tcp_t* handle = nullptr;
    bool listener = false;
    bool closed = false;
    std::mutex mutex;
    std::deque<uv_tcp_t*> backlog;
    std::deque<WasiOp*> waiters;
};

static void delete_tcp(uv_handle_t* handle)
{
    delete (uv_tcp_t*)handle;
}

struct BindParam {
    SocketState* state = nullptr;
    exlib::string address;
    int32_t port = 0;
    int32_t status = UV_EINVAL;
    exlib::Event done;
};

// Closes a socket handle from the uv thread; the handle deletes itself.
static void close_tcp(uv_tcp_t* handle)
{
    if (handle)
        uv_close((uv_handle_t*)handle, delete_tcp);
}

// Mirrors uvwasi_fd_table_get: a descriptor may only use the rights it was
// granted, and path_* calls additionally need the directory's inheriting bits.
static bool capable(const FdEntry* entry, uint64_t rights_base,
    uint64_t rights_inheriting = 0)
{
    return (rights_base & ~entry->rights_base) == 0
        && (rights_inheriting & ~entry->rights_inheriting) == 0;
}

// uvwasi validates fstflags before stat'ing: unknown bits and the ATIM/ATIM_NOW
// (or MTIM/MTIM_NOW) conflict are EINVAL.
static bool valid_fstflags(uint32_t flags)
{
    if (flags & ~0x0fu)
        return false;
    if ((flags & 3) == 3 || (flags & 12) == 12)
        return false;
    return true;
}

// uvwasi__get_rights for a host descriptor: the masks depend on the file type,
// and read/write are masked by the open mode.
static void host_rights(int32_t fd, int32_t open_flags, uint64_t& rights_base,
    uint64_t& rights_inheriting)
{
    uv_fs_t req;
    uint8_t type = WASI_FILETYPE_UNKNOWN;

    if (uv_fs_fstat(nullptr, &req, fd, nullptr) == 0) {
        type = filetype_from_mode(req.statbuf.st_mode);
        uv_fs_req_cleanup(&req);
    }

    switch (type) {
    case WASI_FILETYPE_REGULAR_FILE:
        rights_base = FILE_RIGHTS;
        rights_inheriting = FILE_INHERITING;
        break;
    case WASI_FILETYPE_DIRECTORY:
        rights_base = DIR_RIGHTS;
        rights_inheriting = DIR_INHERITING;
        break;
    case WASI_FILETYPE_CHARACTER_DEVICE:
        if (uv_guess_handle(fd) == UV_TTY) {
            rights_base = TTY_RIGHTS;
            rights_inheriting = 0;
        } else {
            rights_base = RIGHTS_ALL;
            rights_inheriting = RIGHTS_ALL;
        }
        break;
    case WASI_FILETYPE_SOCKET_STREAM:
    case WASI_FILETYPE_SOCKET_DGRAM:
        rights_base = SOCKET_RIGHTS;
        rights_inheriting = RIGHTS_ALL;
        break;
    case WASI_FILETYPE_BLOCK_DEVICE:
        rights_base = RIGHTS_ALL;
        rights_inheriting = RIGHTS_ALL;
        break;
    default:
        rights_base = 0;
        rights_inheriting = 0;
        break;
    }

    int access_mode = open_flags & (O_RDONLY | O_WRONLY | O_RDWR);
    if (access_mode == O_RDONLY)
        rights_base &= ~WASI_RIGHT_FD_WRITE;
    else if (access_mode == O_WRONLY)
        rights_base &= ~WASI_RIGHT_FD_READ;
}

// One completed poll subscription, written to the guest event array.
struct PollEvent {
    uint64_t userdata = 0;
    uint8_t type = 0;
    uint16_t error = 0;
    uint16_t flags = 0;
};

// One host descriptor a poll call waits on. POSIX watches the descriptor, while
// Windows only accepts sockets (uv_poll_init and select() reject everything
// else), so there the socket handle is carried instead.
struct PollFd {
    int32_t fd = -1;
    uv_os_sock_t sock = (uv_os_sock_t)-1;
    int32_t mask = 0;
};

// Resolves the handle the host can wait on. Sockets go through their uv handle
// (which owns the descriptor on POSIX and the socket handle on Windows) because
// a listener never stores a host descriptor; everything else uses the
// descriptor opened at construction. A virtual preopened directory has neither.
static bool poll_handle(const FdEntry* entry, PollFd& out)
{
    if (entry->sock && entry->sock->handle) {
        uv_os_fd_t osfd;
        if (uv_fileno((uv_handle_t*)entry->sock->handle, &osfd) == 0) {
#ifdef _WIN32
            out.sock = (uv_os_sock_t)osfd;
#else
            out.fd = (int32_t)osfd;
#endif
            return true;
        }
    }

    if (entry->host_fd >= 0) {
        out.fd = entry->host_fd;
        return true;
    }

    return false;
}

// Event flags: bit 0 is WASI_EVENT_FD_READWRITE_HANGUP.
static bool poll_fd_ready(const FdEntry* entry, uint8_t type, uint16_t& flags,
    uint16_t& error)
{
    flags = 0;
    error = 0;

    PollFd pfd;
    if (!poll_handle(entry, pfd))
        return true; // virtual preopened directory: nothing to wait on

#ifdef _WIN32
    // Only sockets can be watched on Windows; files, pipes and the console have
    // no readiness the host could ask for, so report them ready instead of
    // blocking a guest forever.
    if (pfd.sock == (uv_os_sock_t)-1)
        return true;

    fd_set rfds;
    fd_set wfds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    if (type == 1)
        FD_SET(pfd.sock, &rfds);
    else
        FD_SET(pfd.sock, &wfds);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    int32_t r = ::select(0, type == 1 ? &rfds : nullptr,
        type == 1 ? nullptr : &wfds, nullptr, &tv);
    if (r == SOCKET_ERROR) {
        error = WASI_ERRNO_IO;
        return true;
    }
    if (r == 0)
        return false;

    // A closed peer shows up as readable; peek at the stream so the guest gets
    // the HANGUP flag instead of a read that returns 0 bytes. A listener is
    // readable because a connection is pending, not because the peer closed.
    if (type == 1 && entry->sock && !entry->sock->listener) {
        char peek;
        int32_t n = ::recv(pfd.sock, &peek, 1, MSG_PEEK);
        if (n == 0)
            flags = 1;
        else if (n == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
            error = WASI_ERRNO_IO;
    }

    return true;
#else
    struct pollfd ready;
    ready.fd = pfd.fd;
    ready.events = type == 1 ? POLLIN : POLLOUT;
    ready.revents = 0;

    int32_t r = ::poll(&ready, 1, 0);
    if (r < 0) {
        error = WASI_ERRNO_IO;
        return true;
    }
    if (r == 0)
        return false;
    if (ready.revents & POLLNVAL) {
        error = WASI_ERRNO_BADF;
        return true;
    }
    if (ready.revents & POLLERR) {
        error = WASI_ERRNO_IO;
        return true;
    }
    if (ready.revents & POLLHUP) {
        flags = 1;
        return true;
    }
    return (ready.revents & (POLLIN | POLLOUT)) != 0;
#endif
}

static void write_events(uint8_t* data, size_t size, uint32_t out,
    const std::vector<PollEvent>& events)
{
    for (size_t i = 0; i < events.size(); i++) {
        uint64_t offset = (uint64_t)out + i * 32;
        if (offset + 32 > size)
            break;

        uint8_t* event = data + offset;
        memset(event, 0, 32);
        write_u64(event, events[i].userdata);
        event[8] = (uint8_t)(events[i].error & 0xff);
        event[9] = (uint8_t)(events[i].error >> 8);
        event[10] = events[i].type;
        event[24] = (uint8_t)(events[i].flags & 0xff);
        event[25] = (uint8_t)(events[i].flags >> 8);
    }
}

// One in-flight syscall. The memory handle is a Global because it can only be
// read and destroyed on the JS thread; WasiOp always deletes this object from
// finish(), which Isolate::sync() runs there.
struct WasiCall {
    int32_t op = 0; // 0 write, 1 read, 2 poll, 3 close, 4 fdstat, 5 filestat,
        // 6 path_open, 7 path_filestat, 8 seek_end, 11 readdir,
        // 12 mkdir, 13 rmdir, 14 unlink, 15 rename, 16 link, 17 readlink,
        // 18 symlink, 19 ftruncate, 20 fd_utime, 21 path_utime, 22 renumber,
        // 23 fsync/fdatasync, 30 sock_accept, 31 sock_recv, 32 sock_send,
        // 33 sock_shutdown
    struct PollSub {
        uint64_t userdata = 0;
        uint8_t type = 0;
        int32_t guest_fd = -1;
    };

    int32_t fd = -1; // host fd
    int32_t guest_fd = -1;
    uint64_t fd_id = 0;
    uint64_t rename_from_id = 0;
    int64_t offset = -1;
    bool no_advance = false;
    uint32_t out_addr = 0;
    uint32_t nevents_addr = 0;
    uint32_t buf_len = 0;
    uint32_t bufused_addr = 0;
    uint64_t cookie = 0;
    std::string data;
    std::vector<std::pair<uint32_t, uint32_t>> iovs;
    std::vector<PollEvent> events;
    v8::Global<v8::Object> memory;

    // poll_oneoff
    std::vector<std::pair<uint64_t, uint64_t>> clocks;
    std::vector<PollSub> poll_subs;
    std::vector<PollFd> poll_host_fds;
    uint64_t userdata = 0;
    uint64_t timeout_ms = 0;
    bool timer_fired = false;

    // sockets
    struct SocketState* sock_state = nullptr;
    uv_tcp_t* conn = nullptr;
    int32_t ro_flags_addr = 0;

    // path operations
    exlib::string host_path;
    exlib::string target_path;
    int32_t open_flags = 0;
    int32_t open_mode = 0;
    uint64_t open_rights_base = 0;
    uint64_t open_rights_inheriting = 0;
    bool follow = true;

    // timestamp / fsync / renumber state
    uint64_t atim = 0;
    uint64_t mtim = 0;
    int32_t time_flags = 0;
    bool stage = false;
    bool data_only = false;
    int32_t rename_to = -1;

    // results
    int32_t err = WASI_ERRNO_SUCCESS;
    uint32_t nbytes = 0;
    int32_t host_fd = -1;
    uv_stat_t stat;
};

class WASIHost : public WASIHost_base {
    // The guest calls the imports from one fiber at a time (a wasm instance is
    // single threaded), so the per-object reentrancy lock that the generated
    // async glue takes would only serialize host code that deliberately shares
    // one host between fibers: a fiber suspended in sock_accept/poll_oneoff
    // would then block every other syscall on the same instance. Disable it
    // like the other fd-style fibjs objects do (Socket, TcpServer, ...).
    //
    // What keeps the shared state safe without that lock:
    //   - the descriptor table (m_fds/m_sockets/m_calls) is only touched from
    //     the JS thread, and every mutation is a single non-suspending step;
    //   - descriptors carry an id (FdEntry::id) and late completions look them
    //     up by id, so a close or a renumber during a suspended call cannot be
    //     mistaken for the descriptor that inherited the guest fd number;
    //   - fd_close/fd_renumber remove the entry before suspending, so a second
    //     close reports BADF instead of closing the host file twice;
    //   - SocketState has its own mutex and a one-shot close, and WasiOp
    //     completes exactly once (atomic CAS), because the uv thread and a
    //     closing fiber can both try to settle the same accept;
    //   - guest memory is never held as a raw pointer: the sync phase copies
    //     iovec payloads into WasiCall and completions re-derive the backing
    //     store, which also survives memory.grow.
    FIBER_FREE();

public:
    WASIHost() = default;
    virtual ~WASIHost();

    void set_version(const exlib::string& version)
    {
        m_version = version;
    }

    void set_return_on_exit(bool value)
    {
        m_return_on_exit = value;
    }

    result_t configure(std::vector<exlib::string>&& args, std::vector<exlib::string>&& env,
        std::vector<std::pair<exlib::string, exlib::string>>&& preopens,
        std::vector<std::pair<exlib::string, int32_t>>&& sockets,
        int32_t stdin_fd, int32_t stdout_fd, int32_t stderr_fd);

    // Called from the uv thread when a listener accepts a connection.
    static void on_new_connection(uv_stream_t* server, int status);

public:
    virtual result_t getImportObject(v8::Local<v8::Object>& retVal);
    virtual result_t start(v8::Local<v8::Value> instance, int32_t& retVal);
    virtual result_t initialize(v8::Local<v8::Value> instance);
    virtual result_t finalizeBindings(v8::Local<v8::Value> instance, v8::Local<v8::Object> options);
    virtual result_t registerAsImport();
    virtual result_t unregisterAsImport();
    virtual result_t get_wasiImport(v8::Local<v8::Object>& retVal);
    virtual result_t fd_read(int32_t fd, int32_t iovs, int32_t iovs_len, int32_t nread,
        int32_t& retVal, AsyncHandle ac);
    virtual result_t fd_write(int32_t fd, int32_t iovs, int32_t iovs_len, int32_t nwritten,
        int32_t& retVal, AsyncHandle ac);
    virtual result_t poll_oneoff(int32_t in_ptr, int32_t out_ptr, int32_t nsubscriptions,
        int32_t nevents, int32_t& retVal, AsyncHandle ac);
    virtual result_t args_sizes_get(int32_t argc, int32_t argv_buf_size, int32_t& retVal);
    virtual result_t args_get(int32_t argv, int32_t argv_buf, int32_t& retVal);
    virtual result_t environ_sizes_get(int32_t environ_count, int32_t environ_buf_size,
        int32_t& retVal);
    virtual result_t environ_get(int32_t environ, int32_t environ_buf, int32_t& retVal);
    virtual result_t clock_res_get(int32_t clock_id, int32_t resolution, int32_t& retVal);
    virtual result_t clock_time_get(int32_t clock_id, int64_t precision, int32_t time,
        int32_t& retVal);
    virtual result_t random_get(int32_t buf, int32_t buf_len, int32_t& retVal);
    virtual result_t sched_yield(int32_t& retVal);
    virtual result_t proc_exit(int32_t code);
    virtual result_t proc_raise(int32_t sig, int32_t& retVal);
    virtual result_t fd_close(int32_t fd, int32_t& retVal, AsyncHandle ac);
    virtual result_t fd_fdstat_get(int32_t fd, int32_t buf, int32_t& retVal, AsyncHandle ac);
    virtual result_t fd_filestat_get(int32_t fd, int32_t buf, int32_t& retVal, AsyncHandle ac);
    virtual result_t fd_prestat_get(int32_t fd, int32_t buf, int32_t& retVal);
    virtual result_t fd_prestat_dir_name(int32_t fd, int32_t path, int32_t path_len,
        int32_t& retVal);
    virtual result_t path_open(int32_t fd, int32_t dirflags, int32_t path, int32_t path_len,
        int32_t oflags, int64_t rights_base, int64_t rights_inheriting, int32_t fdflags,
        int32_t opened_fd, int32_t& retVal, AsyncHandle ac);
    virtual result_t path_filestat_get(int32_t fd, int32_t flags, int32_t path,
        int32_t path_len, int32_t buf, int32_t& retVal, AsyncHandle ac);
    virtual result_t fd_seek(int32_t fd, int64_t offset, int32_t whence, int32_t newoffset,
        int32_t& retVal, AsyncHandle ac);
    virtual result_t fd_tell(int32_t fd, int32_t offset, int32_t& retVal);
    virtual result_t fd_pread(int32_t fd, int32_t iovs, int32_t iovs_len, int64_t offset,
        int32_t nread, int32_t& retVal, AsyncHandle ac);
    virtual result_t fd_pwrite(int32_t fd, int32_t iovs, int32_t iovs_len, int64_t offset,
        int32_t nwritten, int32_t& retVal, AsyncHandle ac);
    virtual result_t fd_readdir(int32_t fd, int32_t buf, int32_t buf_len, int64_t cookie,
        int32_t bufused, int32_t& retVal, AsyncHandle ac);
    virtual result_t path_create_directory(int32_t fd, int32_t path, int32_t path_len,
        int32_t& retVal, AsyncHandle ac);
    virtual result_t path_remove_directory(int32_t fd, int32_t path, int32_t path_len,
        int32_t& retVal, AsyncHandle ac);
    virtual result_t path_unlink_file(int32_t fd, int32_t path, int32_t path_len,
        int32_t& retVal, AsyncHandle ac);
    virtual result_t path_rename(int32_t fd, int32_t old_path, int32_t old_path_len,
        int32_t new_fd, int32_t new_path, int32_t new_path_len, int32_t& retVal,
        AsyncHandle ac);
    virtual result_t path_link(int32_t old_fd, int32_t old_flags, int32_t old_path,
        int32_t old_path_len, int32_t new_fd, int32_t new_path, int32_t new_path_len,
        int32_t& retVal, AsyncHandle ac);
    virtual result_t path_readlink(int32_t fd, int32_t path, int32_t path_len, int32_t buf,
        int32_t buf_len, int32_t bufused, int32_t& retVal, AsyncHandle ac);
    virtual result_t path_symlink(int32_t old_path, int32_t old_path_len, int32_t fd,
        int32_t new_path, int32_t new_path_len, int32_t& retVal, AsyncHandle ac);
    virtual result_t fd_filestat_set_size(int32_t fd, int64_t size, int32_t& retVal,
        AsyncHandle ac);
    virtual result_t fd_sync(int32_t fd, int32_t& retVal, AsyncHandle ac);
    virtual result_t fd_datasync(int32_t fd, int32_t& retVal, AsyncHandle ac);
    virtual result_t fd_advise(int32_t fd, int64_t offset, int64_t len, int32_t advice,
        int32_t& retVal);
    virtual result_t fd_allocate(int32_t fd, int64_t offset, int64_t len, int32_t& retVal);
    virtual result_t fd_fdstat_set_flags(int32_t fd, int32_t flags, int32_t& retVal);
    virtual result_t fd_fdstat_set_rights(int32_t fd, int64_t rights_base,
        int64_t rights_inheriting, int32_t& retVal);
    virtual result_t fd_filestat_set_times(int32_t fd, int64_t atim, int64_t mtim,
        int32_t flags, int32_t& retVal, AsyncHandle ac);
    virtual result_t path_filestat_set_times(int32_t fd, int32_t flags, int32_t path,
        int32_t path_len, int64_t atim, int64_t mtim, int32_t fst_flags, int32_t& retVal,
        AsyncHandle ac);
    virtual result_t fd_renumber(int32_t from, int32_t to, int32_t& retVal, AsyncHandle ac);
    virtual result_t sock_accept(int32_t fd, int32_t flags, int32_t fd_out, int32_t& retVal,
        AsyncHandle ac);
    virtual result_t sock_recv(int32_t fd, int32_t ri_data, int32_t ri_data_len, int32_t ri_flags,
        int32_t ro_datalen, int32_t ro_flags, int32_t& retVal, AsyncHandle ac);
    virtual result_t sock_send(int32_t fd, int32_t si_data, int32_t si_data_len, int32_t si_flags,
        int32_t so_datalen, int32_t& retVal, AsyncHandle ac);
    virtual result_t sock_shutdown(int32_t fd, int32_t how, int32_t& retVal, AsyncHandle ac);

public:
    // Called by WasiOp::finish() on the JS thread. The id argument is the
    // FdEntry::id captured by the sync phase; the entry is looked up by id
    // because the fd number may have been renumbered or closed meanwhile.
    void on_io_done(uint64_t fd_id, uint32_t nbytes);
    bool on_open_done(uint64_t fd_id, int32_t host_fd, int32_t open_flags,
        uint64_t rights_base, uint64_t rights_inheriting, int32_t& guest_fd);
    void on_open_failed(uint64_t fd_id);
    void on_seek_done(uint64_t fd_id, int64_t offset);
    void on_renumber_done(uint64_t from_id, int32_t to);
    int32_t adopt_socket(uv_tcp_t* conn);
    void close_socket_state(SocketState* state);
    bool probe_poll(int32_t guest_fd, uint8_t type, uint16_t& flags, uint16_t& error);
    void fill_fdstat(uint64_t fd_id, const uv_stat_t& st, uint8_t* out);

private:
    result_t call_export(v8::Local<v8::Value> instance, const char* name, bool required,
        int32_t& retVal);
    // Node throws ERR_WASI_NOT_STARTED when a syscall runs before the instance
    // memory is bound; the guest cannot make progress without memory, so report
    // the programming error instead of an errno from a call that cannot work.
    result_t not_started() const
    {
        return result_t::make(CALL_E_INVALID_CALL)
            .with_type_name(errtype::kError)
            .with_code("ERR_WASI_NOT_STARTED")
            .with_message("wasi.start() has not been called");
    }
    bool memory(uint8_t*& data, size_t& size);
    FdEntry* find_fd(int32_t fd);
    FdEntry* find_fd_by_id(uint64_t fd_id, int32_t* guest_fd = nullptr);
    int32_t insert_fd(FdEntry& entry);
    int32_t resolve_path(FdEntry& dir, int32_t path, int32_t path_len, exlib::string& host);
    result_t prepare_io(WasiCall& call, bool write, int32_t iovs, int32_t iovs_len,
        int32_t& err);
    result_t sync_fd(int32_t fd, bool data_only, int32_t& retVal, AsyncHandle& ac);
    int64_t store_call(std::unique_ptr<WasiCall> call);
    std::unique_ptr<WasiCall> take_call(AsyncHandle& ac);

private:
    v8::Global<v8::Object> m_memory;
    exlib::string m_version = "preview1";
    std::vector<exlib::string> m_args;
    std::vector<exlib::string> m_env;
    std::map<int32_t, FdEntry> m_fds;
    int32_t m_next_fd = 3;
    uint64_t m_next_fd_id = 1;
    std::vector<SocketState*> m_sockets;
    bool m_bound = false;
    bool m_return_on_exit = true;
    bool m_exited = false;
    int32_t m_exit_code = 0;
    int64_t m_next_id = 1;
    std::unordered_map<int64_t, std::unique_ptr<WasiCall>> m_calls;
};

// The uv request; it owns the WasiCall (and therefore the V8 Global) until
// finish() deletes it on the JS thread.
class WasiOp : public uv_fs_t {
public:
    WasiOp(WASIHost* host, Isolate* isolate, std::unique_ptr<WasiCall> call,
        int32_t* retVal, AsyncHandle ac)
        : m_host(host)
        , m_isolate(isolate)
        , m_call(std::move(call))
        , m_retVal(retVal)
        , m_ac(std::move(ac))
        , m_timer(nullptr)
        , m_done(false)
    {
    }

    ~WasiOp()
    {
    }

public:
    // Called on the uv thread when a listener hands over an accepted stream.
    void accept_ready(uv_tcp_t* conn)
    {
        if (!try_complete()) {
            close_tcp(conn);
            return;
        }

        m_call->conn = conn;
        complete(WASI_ERRNO_SUCCESS, 0);
    }

    // Wakes a fiber whose syscall can no longer be satisfied (e.g. the socket
    // was closed while another fiber waited in sock_accept).
    void abort(int32_t err)
    {
        if (!try_complete())
            return;

        complete(err, 0);
    }

    void start()
    {
        switch (m_call->op) {
        case 2: {
            if (!m_call->clocks.empty()) {
                m_timer = new uv_timer_t;
                uv_timer_init(s_uv_loop, m_timer);
                m_timer->data = this;
                uv_timer_start(m_timer, on_timer, m_call->timeout_ms, 0);
            }

            for (auto& item : m_call->poll_host_fds) {
                uv_poll_t* handle = new uv_poll_t;
#ifdef _WIN32
                int32_t r = uv_poll_init_socket(s_uv_loop, handle, item.sock);
#else
                int32_t r = uv_poll_init(s_uv_loop, handle, item.fd);
#endif
                if (r != 0) {
                    delete handle;
                    continue;
                }

                handle->data = this;
                if (uv_poll_start(handle, item.mask, on_poll) != 0) {
                    uv_close((uv_handle_t*)handle, [](uv_handle_t* h) {
                        delete (uv_poll_t*)h;
                    });
                    continue;
                }

                m_polls.push_back(handle);
            }

            if (m_polls.empty() && !m_timer)
                complete(WASI_ERRNO_SUCCESS, 0);
            return;
        }
        case 30: {
            SocketState* state = m_call->sock_state;
            std::lock_guard<std::mutex> lock(state->mutex);

            if (!state->backlog.empty() && try_complete()) {
                m_call->conn = state->backlog.front();
                state->backlog.pop_front();
                complete(WASI_ERRNO_SUCCESS, 0);
                return;
            }

            state->waiters.push_back(this);
            return;
        }
        case 31: {
            uv_tcp_t* conn = m_call->sock_state->handle;
            conn->data = this;
            int32_t r = uv_read_start((uv_stream_t*)conn, on_alloc, on_read);
            if (r != 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 32: {
            uv_tcp_t* conn = m_call->sock_state->handle;
            uv_buf_t buf = uv_buf_init(m_call->data.empty() ? nullptr : &m_call->data[0],
                (unsigned int)m_call->data.size());
            int32_t r = uv_try_write((uv_stream_t*)conn, &buf, 1);
            if (r < 0)
                complete(wasi_errno(r), 0);
            else
                complete(WASI_ERRNO_SUCCESS, (uint32_t)r);
            return;
        }
        case 33: {
            uv_tcp_t* conn = m_call->sock_state->handle;
            m_shutdown_req.data = this;
            int32_t r = uv_shutdown(&m_shutdown_req, (uv_stream_t*)conn, on_shutdown);
            if (r != 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 3: {
            int32_t r = uv_fs_close(s_uv_loop, this, m_call->fd, on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 4:
        case 5: {
            int32_t r = (m_call->fd < 0 && !m_call->host_path.empty())
                ? uv_fs_stat(s_uv_loop, this, m_call->host_path.c_str(), on_fs)
                : uv_fs_fstat(s_uv_loop, this, m_call->fd, on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 6: {
            int32_t r = uv_fs_open(s_uv_loop, this, m_call->host_path.c_str(),
                m_call->open_flags, m_call->open_mode, on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 7: {
            int32_t r = m_call->follow
                ? uv_fs_stat(s_uv_loop, this, m_call->host_path.c_str(), on_fs)
                : uv_fs_lstat(s_uv_loop, this, m_call->host_path.c_str(), on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 8: {
            int32_t r = uv_fs_fstat(s_uv_loop, this, m_call->fd, on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 11: {
            int32_t r = uv_fs_scandir(s_uv_loop, this, m_call->host_path.c_str(), 0, on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 12: {
            int32_t r = uv_fs_mkdir(s_uv_loop, this, m_call->host_path.c_str(), 0777, on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 13: {
            int32_t r = uv_fs_rmdir(s_uv_loop, this, m_call->host_path.c_str(), on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 14: {
            int32_t r = uv_fs_unlink(s_uv_loop, this, m_call->host_path.c_str(), on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 15: {
            int32_t r = uv_fs_rename(s_uv_loop, this, m_call->host_path.c_str(),
                m_call->target_path.c_str(), on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 16: {
            int32_t r = uv_fs_link(s_uv_loop, this, m_call->host_path.c_str(),
                m_call->target_path.c_str(), on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 17: {
            int32_t r = uv_fs_readlink(s_uv_loop, this, m_call->host_path.c_str(), on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 18: {
            int32_t r = uv_fs_symlink(s_uv_loop, this, m_call->target_path.c_str(),
                m_call->host_path.c_str(), 0, on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 19: {
            int32_t r = uv_fs_ftruncate(s_uv_loop, this, m_call->fd, m_call->offset, on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 20: {
            int32_t r = uv_fs_fstat(s_uv_loop, this, m_call->fd, on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 21: {
            int32_t r = m_call->follow
                ? uv_fs_stat(s_uv_loop, this, m_call->host_path.c_str(), on_fs)
                : uv_fs_lstat(s_uv_loop, this, m_call->host_path.c_str(), on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 22: {
            int32_t r = uv_fs_close(s_uv_loop, this, m_call->fd, on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        case 23: {
            int32_t r = m_call->data_only
                ? uv_fs_fdatasync(s_uv_loop, this, m_call->fd, on_fs)
                : uv_fs_fsync(s_uv_loop, this, m_call->fd, on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        default: {
            if (m_call->data.empty()) {
                complete(WASI_ERRNO_SUCCESS, 0);
                return;
            }

            uv_buf_t buf = uv_buf_init(&m_call->data[0], (unsigned int)m_call->data.size());
            int32_t r = m_call->op == 0
                ? uv_fs_write(s_uv_loop, this, m_call->fd, &buf, 1, m_call->offset, on_fs)
                : uv_fs_read(s_uv_loop, this, m_call->fd, &buf, 1, m_call->offset, on_fs);
            if (r < 0)
                complete(wasi_errno(r), 0);
            return;
        }
        }
    }

private:
    static void on_fs(uv_fs_t* req)
    {
        WasiOp* op = (WasiOp*)req;
        int32_t r = (int32_t)uv_fs_get_result(req);

        if (op->m_call->op == 20 || op->m_call->op == 21) {
            op->handle_time_set(req, r);
            return;
        }

        if (op->m_call->op == 4 || op->m_call->op == 5 || op->m_call->op == 7
            || op->m_call->op == 8)
            op->m_call->stat = *uv_fs_get_statbuf(req);

        if (op->m_call->op == 17 && r >= 0 && req->ptr)
            op->m_call->data = (const char*)req->ptr;

        // A successful scandir is consumed in finish() on the JS thread, so the
        // request memory must survive until then; every other op is done here.
        if (!(op->m_call->op == 11 && r >= 0))
            uv_fs_req_cleanup(req);

        if (op->m_call->op == 6) {
            if (r < 0)
                op->complete(wasi_errno(r), 0);
            else {
                op->m_call->host_fd = r;
                op->complete(WASI_ERRNO_SUCCESS, 0);
            }
            return;
        }

        if ((op->m_call->op == 0 || op->m_call->op == 1) && r >= 0)
            op->complete(WASI_ERRNO_SUCCESS, (uint32_t)r);
        else
            op->complete(r < 0 ? wasi_errno(r) : WASI_ERRNO_SUCCESS, 0);
    }

    void handle_time_set(uv_fs_t* req, int32_t r)
    {
        if (r < 0) {
            uv_fs_req_cleanup(req);
            complete(wasi_errno(r), 0);
            return;
        }

        if (m_call->stage) {
            uv_fs_req_cleanup(req);
            complete(WASI_ERRNO_SUCCESS, 0);
            return;
        }

        uv_stat_t st = *uv_fs_get_statbuf(req);
        uv_fs_req_cleanup(req);

        uv_timeval64_t tv;
        double now = uv_gettimeofday(&tv) == 0
            ? (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0
            : 0.0;

        int32_t flags = m_call->time_flags;
        double atim = (flags & 2) ? now
            : (flags & 1) ? (double)m_call->atim / 1000000000.0
                          : (double)st.st_atim.tv_sec + (double)st.st_atim.tv_nsec / 1000000000.0;
        double mtim = (flags & 8) ? now
            : (flags & 4) ? (double)m_call->mtim / 1000000000.0
                          : (double)st.st_mtim.tv_sec + (double)st.st_mtim.tv_nsec / 1000000000.0;

        m_call->stage = true;

        int32_t rr;
        if (m_call->op == 20)
            rr = uv_fs_futime(s_uv_loop, req, m_call->fd, atim, mtim, on_fs);
        else if (m_call->follow)
            rr = uv_fs_utime(s_uv_loop, req, m_call->host_path.c_str(), atim, mtim, on_fs);
        else
            rr = uv_fs_lutime(s_uv_loop, req, m_call->host_path.c_str(), atim, mtim, on_fs);
        if (rr < 0)
            complete(wasi_errno(rr), 0);
    }

    // A uv callback and a close from another fiber can reach the same op, so
    // only the thread that flips m_done may report the result (finish() posts
    // to the JS thread and deletes the op).
    bool try_complete()
    {
        bool expected = false;
        return m_done.compare_exchange_strong(expected, true);
    }

    static void on_timer(uv_timer_t* handle)
    {
        WasiOp* op = (WasiOp*)handle->data;
        if (!op->try_complete())
            return;

        op->m_call->timer_fired = true;
        op->stop_handles();
        op->complete(WASI_ERRNO_SUCCESS, 0);
    }

    static void on_poll(uv_poll_t* handle, int status, int events)
    {
        WasiOp* op = (WasiOp*)handle->data;
        if (!op->try_complete())
            return;

        op->stop_handles();
        op->complete(status == 0 ? WASI_ERRNO_SUCCESS : wasi_errno(status), 0);
    }

    static void on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
    {
        (void)suggested_size;
        WasiOp* op = (WasiOp*)handle->data;
        buf->base = op->m_call->data.empty() ? nullptr : &op->m_call->data[0];
        buf->len = (unsigned int)op->m_call->data.size();
    }

    static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
    {
        (void)buf;
        WasiOp* op = (WasiOp*)stream->data;
        if (!op->try_complete())
            return;

        uv_read_stop(stream);

        if (nread < 0)
            op->complete(wasi_errno((int32_t)nread), 0);
        else if (nread == 0)
            op->complete(WASI_ERRNO_AGAIN, 0);
        else
            op->complete(WASI_ERRNO_SUCCESS, (uint32_t)nread);
    }

    static void on_shutdown(uv_shutdown_t* req, int status)
    {
        WasiOp* op = (WasiOp*)req->data;
        if (!op->try_complete())
            return;

        op->complete(status == 0 ? WASI_ERRNO_SUCCESS : wasi_errno(status), 0);
    }

    void stop_handles()
    {
        for (uv_poll_t* handle : m_polls) {
            uv_poll_stop(handle);
            uv_close((uv_handle_t*)handle, [](uv_handle_t* h) {
                delete (uv_poll_t*)h;
            });
        }
        m_polls.clear();

        if (m_timer) {
            uv_timer_stop(m_timer);
            uv_close((uv_handle_t*)m_timer, [](uv_handle_t* h) {
                delete (uv_timer_t*)h;
            });
            m_timer = nullptr;
        }
    }

    void complete(int32_t err, uint32_t nbytes)
    {
        m_call->err = err;
        m_call->nbytes = nbytes;

        m_isolate->sync([this]() {
            finish();
        });
    }

    void finish()
    {
        JSFiber::EnterJsScope scope;

        if (m_call->err == WASI_ERRNO_SUCCESS)
            write_result();
        else if (m_call->op == 6)
            m_host->on_open_failed(m_call->fd_id);

        if (m_call->op == 11)
            uv_fs_req_cleanup(this);

        *m_retVal = m_call->err;
        m_ac.post(result_t());
        delete this;
    }

    bool get_memory(uint8_t*& data, size_t& size)
    {
        if (m_call->memory.IsEmpty())
            return false;

        Isolate* isolate = m_isolate;
        v8::Local<v8::Context> context = isolate->context();
        v8::Local<v8::Object> memory = m_call->memory.Get(isolate->m_isolate);
        v8::Local<v8::Value> buffer;
        if (!memory->Get(context, isolate->NewString("buffer")).ToLocal(&buffer))
            return false;

        std::shared_ptr<v8::BackingStore> store;
        if (buffer->IsArrayBuffer())
            store = buffer.As<v8::ArrayBuffer>()->GetBackingStore();
        else if (buffer->IsSharedArrayBuffer())
            store = buffer.As<v8::SharedArrayBuffer>()->GetBackingStore();
        else
            return false;

        data = (uint8_t*)store->Data();
        size = store->ByteLength();
        return data != nullptr;
    }

    void write_result()
    {
        uint8_t* data;
        size_t size;

        switch (m_call->op) {
        case 0:
        case 1: {
            if (!get_memory(data, size))
                return;

            if ((uint64_t)m_call->out_addr + 4 <= size)
                write_u32(data + m_call->out_addr, m_call->nbytes);

            if (m_call->op == 1) {
                // Copy only the bytes that were actually read. The staging
                // buffer is sized to the whole iovec list, so copying it in
                // full would write the unread tail (zeros) over guest memory.
                size_t off = 0;
                size_t remaining = m_call->nbytes;
                for (auto& iov : m_call->iovs) {
                    if (remaining == 0 || off >= m_call->data.size())
                        break;
                    size_t len = iov.second;
                    if (len > remaining)
                        len = remaining;
                    if (len > m_call->data.size() - off)
                        len = m_call->data.size() - off;
                    memcpy(data + iov.first, m_call->data.data() + off, len);
                    off += len;
                    remaining -= len;
                }
            }
            if (!m_call->no_advance)
                m_host->on_io_done(m_call->fd_id, m_call->nbytes);
            return;
        }
        case 2: {
            if (!get_memory(data, size))
                return;

            std::vector<PollEvent> events;

            // Re-probe every descriptor subscription: any fd that is ready now
            // wins over a concurrent clock timeout, like uvwasi.
            for (auto& sub : m_call->poll_subs) {
                uint16_t flags = 0;
                uint16_t error = 0;
                if (!m_host->probe_poll(sub.guest_fd, sub.type, flags, error))
                    continue;

                PollEvent event;
                event.userdata = sub.userdata;
                event.type = sub.type;
                event.error = error;
                event.flags = flags;
                events.push_back(event);
            }

            if (events.empty() && !m_call->clocks.empty() && m_call->timer_fired)
                events.push_back(PollEvent { m_call->userdata, 0, 0, 0 });

            // If the wait woke up but nothing is ready now (a drained pipe or
            // a spurious wakeup), report the subscriptions as ready instead of
            // blocking the guest forever.
            if (events.empty() && !m_call->poll_subs.empty()) {
                for (auto& sub : m_call->poll_subs)
                    events.push_back(PollEvent { sub.userdata, sub.type, 0, 0 });
            }

            if (events.empty() && !m_call->clocks.empty())
                events.push_back(PollEvent { m_call->userdata, 0, 0, 0 });

            write_events(data, size, m_call->out_addr, events);
            if ((uint64_t)m_call->nevents_addr + 4 <= size)
                write_u32(data + m_call->nevents_addr, (uint32_t)events.size());
            return;
        }
        case 30: {
            if (!get_memory(data, size) || !m_call->conn)
                return;

            int32_t fd = m_host->adopt_socket(m_call->conn);
            m_call->conn = nullptr;
            if ((uint64_t)m_call->out_addr + 4 <= size)
                write_u32(data + m_call->out_addr, (uint32_t)fd);
            return;
        }
        case 31: {
            if (!get_memory(data, size))
                return;

            if ((uint64_t)m_call->out_addr + 4 <= size)
                write_u32(data + m_call->out_addr, m_call->nbytes);
            if ((uint64_t)m_call->ro_flags_addr + 2 <= size)
                write_u16(data + m_call->ro_flags_addr, 0);

            if (!m_call->iovs.empty()) {
                size_t len = m_call->iovs[0].second;
                if (len > m_call->data.size())
                    len = m_call->data.size();
                memcpy(data + m_call->iovs[0].first, m_call->data.data(), len);
            }
            return;
        }
        case 32: {
            if (!get_memory(data, size))
                return;

            if ((uint64_t)m_call->out_addr + 4 <= size)
                write_u32(data + m_call->out_addr, m_call->nbytes);
            return;
        }
        case 33:
            return;
        case 3:
            // fd_close removed the entry before suspending, so there is no
            // late bookkeeping left.
            return;
        case 4: {
            if (!get_memory(data, size))
                return;
            if ((uint64_t)m_call->out_addr + 24 <= size)
                m_host->fill_fdstat(m_call->fd_id, m_call->stat, data + m_call->out_addr);
            return;
        }
        case 5:
        case 7: {
            if (!get_memory(data, size))
                return;
            if ((uint64_t)m_call->out_addr + 64 <= size)
                fill_filestat(m_call->stat, data + m_call->out_addr);
            return;
        }
        case 6: {
            if (!get_memory(data, size))
                return;

            int32_t guest_fd = m_call->guest_fd;
            if (!m_host->on_open_done(m_call->fd_id, m_call->host_fd, m_call->open_flags,
                    m_call->open_rights_base, m_call->open_rights_inheriting, guest_fd)) {
                // The reservation was closed meanwhile; the host descriptor has
                // been closed and the guest must not see a success.
                m_call->err = WASI_ERRNO_BADF;
                return;
            }

            if ((uint64_t)m_call->out_addr + 4 <= size)
                write_u32(data + m_call->out_addr, (uint32_t)guest_fd);
            return;
        }
        case 8: {
            if (!get_memory(data, size))
                return;

            int64_t position = (int64_t)m_call->stat.st_size + m_call->offset;
            if (position < 0) {
                m_call->err = WASI_ERRNO_INVAL;
                return;
            }

            m_host->on_seek_done(m_call->fd_id, position);
            if ((uint64_t)m_call->out_addr + 8 <= size)
                write_u64(data + m_call->out_addr, (uint64_t)position);
            return;
        }
        case 11:
            write_readdir();
            return;
        case 22:
            m_host->on_renumber_done(m_call->rename_from_id, m_call->rename_to);
            return;
        case 17: {
            if (!get_memory(data, size))
                return;

            if (m_call->data.size() >= m_call->buf_len) {
                m_call->err = WASI_ERRNO_NOBUFS;
                return;
            }

            uint32_t used = (uint32_t)m_call->data.size();
            if ((uint64_t)m_call->out_addr + used + 1 <= size) {
                memcpy(data + m_call->out_addr, m_call->data.data(), used);
                data[m_call->out_addr + used] = 0;
            }

            if ((uint64_t)m_call->bufused_addr + 4 <= size)
                write_u32(data + m_call->bufused_addr, used);
            return;
        }
        default:
            return;
        }
    }

    void write_readdir()
    {
        uint8_t* data;
        size_t size;
        if (!get_memory(data, size))
            return;

        uint32_t used = 0;
        uint64_t index = 0;
        uv_dirent_t entry;

        while (uv_fs_scandir_next(this, &entry) != UV_EOF) {
            if (index++ < m_call->cookie)
                continue;

            size_t name_len = strlen(entry.name);
            if ((uint64_t)m_call->out_addr + used + 24 + name_len > size
                || used + 24 + name_len > m_call->buf_len)
                break;

            uint8_t* out = data + m_call->out_addr + used;
            memset(out, 0, 24);
            write_u64(out, index);
            write_u32(out + 16, (uint32_t)name_len);
            out[20] = dirent_type(entry.type);
            memcpy(out + 24, entry.name, name_len);
            used += (uint32_t)(24 + name_len);
        }

        if ((uint64_t)m_call->bufused_addr + 4 <= size)
            write_u32(data + m_call->bufused_addr, used);
    }

private:
    WASIHost* m_host;
    Isolate* m_isolate;
    std::unique_ptr<WasiCall> m_call;
    int32_t* m_retVal;
    AsyncHandle m_ac;
    uv_timer_t* m_timer;
    std::vector<uv_poll_t*> m_polls;
    uv_shutdown_t m_shutdown_req;
    // Touched by the uv thread (callbacks) and by the JS thread (a concurrent
    // close aborting a waiting accept), so make it atomic.
    std::atomic<bool> m_done;
};

static bool read_js_string(v8::Local<v8::Context> context, v8::Local<v8::Value> value,
    exlib::string& out)
{
    v8::Local<v8::String> string;
    if (!value->ToString(context).ToLocal(&string))
        return false;

    v8::String::Utf8Value text(Isolate::current()->m_isolate, string);
    if (*text)
        out = *text;
    return true;
}

static result_t read_options(v8::Local<v8::Object> options, std::vector<exlib::string>& args,
    std::vector<exlib::string>& env,
    std::vector<std::pair<exlib::string, exlib::string>>& preopens)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Value> argv;
    if (options->Get(context, isolate->NewString("args")).ToLocal(&argv)
        && !argv->IsUndefined() && !argv->IsNull()) {
        if (!argv->IsArray())
            return result_t::make(CALL_E_TYPEMISMATCH)
                .with_type_name(errtype::kTypeError)
                .with_code("ERR_INVALID_ARG_TYPE")
                .with_message("The \"options.args\" property must be an instance of Array.");

        v8::Local<v8::Array> array = argv.As<v8::Array>();
        for (uint32_t i = 0; i < array->Length(); i++) {
            v8::Local<v8::Value> value;
            exlib::string text;
            if (array->Get(context, i).ToLocal(&value) && read_js_string(context, value, text))
                args.push_back(text);
        }
    }

    v8::Local<v8::Value> envv;
    if (options->Get(context, isolate->NewString("env")).ToLocal(&envv)
        && !envv->IsUndefined() && !envv->IsNull()) {
        if (!envv->IsObject())
            return result_t::make(CALL_E_TYPEMISMATCH)
                .with_type_name(errtype::kTypeError)
                .with_code("ERR_INVALID_ARG_TYPE")
                .with_message("The \"options.env\" property must be of type object.");

        v8::Local<v8::Object> object = envv.As<v8::Object>();
        v8::Local<v8::Array> names;
        if (object->GetPropertyNames(context).ToLocal(&names)) {
            for (uint32_t i = 0; i < names->Length(); i++) {
                v8::Local<v8::Value> key_value;
                v8::Local<v8::Value> value;
                exlib::string key;
                exlib::string text;
                if (!names->Get(context, i).ToLocal(&key_value)
                    || !object->Get(context, key_value).ToLocal(&value)
                    || !read_js_string(context, key_value, key))
                    continue;
                // Node drops entries whose value is undefined.
                if (value->IsUndefined())
                    continue;
                if (!read_js_string(context, value, text))
                    continue;
                env.push_back(key + "=" + text);
            }
        }
    }

    v8::Local<v8::Value> preopens_value;
    if (options->Get(context, isolate->NewString("preopens")).ToLocal(&preopens_value)
        && !preopens_value->IsUndefined() && !preopens_value->IsNull()) {
        if (!preopens_value->IsObject())
            return result_t::make(CALL_E_TYPEMISMATCH)
                .with_type_name(errtype::kTypeError)
                .with_code("ERR_INVALID_ARG_TYPE")
                .with_message("The \"options.preopens\" property must be of type object.");

        v8::Local<v8::Object> object = preopens_value.As<v8::Object>();
        v8::Local<v8::Array> names;
        if (object->GetPropertyNames(context).ToLocal(&names)) {
            for (uint32_t i = 0; i < names->Length(); i++) {
                v8::Local<v8::Value> key_value;
                v8::Local<v8::Value> value;
                exlib::string guest;
                exlib::string host;
                if (!names->Get(context, i).ToLocal(&key_value)
                    || !object->Get(context, key_value).ToLocal(&value)
                    || !read_js_string(context, key_value, guest)
                    || !read_js_string(context, value, host))
                    continue;
                preopens.emplace_back(guest, host);
            }
        }
    }

    return result_t();
}

result_t WASIHost::configure(std::vector<exlib::string>&& args, std::vector<exlib::string>&& env,
    std::vector<std::pair<exlib::string, exlib::string>>&& preopens,
    std::vector<std::pair<exlib::string, int32_t>>&& sockets,
    int32_t stdin_fd, int32_t stdout_fd, int32_t stderr_fd)
{
    m_args = std::move(args);
    m_env = std::move(env);

    int32_t stdio[3] = { stdin_fd, stdout_fd, stderr_fd };
    for (int32_t i = 0; i < 3; i++) {
        FdEntry fd;
        fd.id = m_next_fd_id++;
        fd.kind = FdEntry::STDIO;
        fd.host_fd = stdio[i];
        host_rights(stdio[i], UV_FS_O_RDWR, fd.rights_base, fd.rights_inheriting);
        m_fds[i] = fd;
    }

    for (auto& preopen : preopens) {
        // uvwasi_init() canonicalises and opens every preopen directory, so a
        // bad host path fails the constructor instead of the first path_open.
        uv_fs_t req;
        int32_t r = uv_fs_realpath(NULL, &req, preopen.second.c_str(), NULL);
        if (r < 0) {
            exlib::string message = "wasi: preopen '";
            message += preopen.first;
            message += "' -> '";
            message += preopen.second;
            message += "': ";
            message += uv_strerror(r);
            uv_fs_req_cleanup(&req);
            return result_t::make(CALL_E_INVALID_DATA)
                .with_type_name(errtype::kError)
                .with_code("ERR_WASI_PREOPEN")
                .with_message(message);
        }

        exlib::string real_path((const char*)req.ptr);
        uv_fs_req_cleanup(&req);

        uv_fs_t stat_req;
        r = uv_fs_stat(NULL, &stat_req, real_path.c_str(), NULL);
        bool is_dir = r == 0 && filetype_from_mode(stat_req.statbuf.st_mode) == WASI_FILETYPE_DIRECTORY;
        uv_fs_req_cleanup(&stat_req);

        if (!is_dir) {
            exlib::string message = "wasi: preopen '";
            message += preopen.first;
            message += "' -> '";
            message += real_path;
            message += "' is not a directory";
            return result_t::make(CALL_E_INVALID_DATA)
                .with_type_name(errtype::kError)
                .with_code("ERR_WASI_PREOPEN")
                .with_message(message);
        }

        FdEntry dir;
        dir.kind = FdEntry::DIR;
        dir.host_path = real_path;
        dir.guest_name = preopen.first;
        dir.preopen = true;
        dir.rights_base = DIR_RIGHTS;
        dir.rights_inheriting = DIR_INHERITING;
        insert_fd(dir);
    }

    for (auto& item : sockets) {
        SocketState* state = new SocketState;
        state->listener = true;

        BindParam param;
        param.state = state;
        param.address = item.first;
        param.port = item.second;

        // Handles belong to the uv thread; bind and listen there and wait for
        // the result so constructor errors are reported synchronously.
        uv_post([&param]() {
            uv_tcp_t* server = new uv_tcp_t;
            uv_tcp_init(s_uv_loop, server);

            struct sockaddr_in addr;
            int32_t rc = uv_ip4_addr(param.address.c_str(), param.port, &addr);
            if (rc == 0)
                rc = uv_tcp_bind(server, (const struct sockaddr*)&addr, 0);
            if (rc == 0)
                rc = uv_listen((uv_stream_t*)server, 128, WASIHost::on_new_connection);

            if (rc != 0)
                close_tcp(server);
            else {
                server->data = param.state;
                param.state->handle = server;
            }

            param.status = rc;
            param.done.set();
        });
        param.done.wait();

        if (param.status != 0) {
            delete state;
            exlib::string message = "wasi: cannot listen on ";
            message += item.first;
            message += ":";
            message += std::to_string(item.second);
            message += ": ";
            message += uv_strerror(param.status);
            return result_t::make(CALL_E_INVALID_DATA)
                .with_type_name(errtype::kError)
                .with_code("ERR_WASI_SOCKET")
                .with_message(message);
        }

        FdEntry socket;
        socket.kind = FdEntry::SOCKET;
        socket.host_fd = -1;
        socket.owned = true;
        socket.sock = state;
        socket.rights_base = SOCKET_RIGHTS;
        socket.rights_inheriting = RIGHTS_ALL;
        insert_fd(socket);
        m_sockets.push_back(state);
    }

    return result_t();
}

WASIHost::~WASIHost()
{
    // Wake fibers still parked in sock_accept before dropping the handles: the
    // instance can be collected while another fiber waits on it (the host is
    // lock free, so nothing prevents it). close_socket_state() also schedules
    // the handle close on the uv thread, which owns them.
    std::vector<SocketState*> sockets = std::move(m_sockets);
    for (SocketState* state : sockets)
        close_socket_state(state);
    m_sockets.clear();
}

void WASIHost::on_new_connection(uv_stream_t* server, int status)
{
    if (status != 0)
        return;

    SocketState* state = (SocketState*)server->data;
    if (!state || !state->listener)
        return;

    uv_tcp_t* conn = new uv_tcp_t;
    uv_tcp_init(server->loop, conn);
    if (uv_accept(server, (uv_stream_t*)conn) != 0) {
        close_tcp(conn);
        return;
    }

    std::lock_guard<std::mutex> lock(state->mutex);
    if (!state->waiters.empty()) {
        WasiOp* op = state->waiters.front();
        state->waiters.pop_front();
        op->accept_ready(conn);
    } else
        state->backlog.push_back(conn);
}

int32_t WASIHost::adopt_socket(uv_tcp_t* conn)
{
    SocketState* state = new SocketState;
    state->handle = conn;
    state->listener = false;

    FdEntry entry;
    entry.kind = FdEntry::SOCKET;
    entry.host_fd = -1;
    entry.owned = true;
    entry.sock = state;
    entry.rights_base = SOCKET_RIGHTS;
    entry.rights_inheriting = RIGHTS_ALL;

#ifndef _WIN32
    uv_os_fd_t osfd;
    if (uv_fileno((uv_handle_t*)conn, &osfd) == 0)
        entry.host_fd = (int32_t)osfd;
#endif

    int32_t fd = insert_fd(entry);
    m_sockets.push_back(state);
    return fd;
}

void WASIHost::close_socket_state(SocketState* state)
{
    std::deque<WasiOp*> waiters;
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        if (state->closed)
            return;
        state->closed = true;
        waiters.swap(state->waiters);
    }

    // Wake fibers blocked in sock_accept; they observe BADF.
    for (WasiOp* op : waiters) {
        op->abort(WASI_ERRNO_BADF);
    }

    uv_post([state]() {
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            while (!state->backlog.empty()) {
                close_tcp(state->backlog.front());
                state->backlog.pop_front();
            }
        }
        close_tcp(state->handle);
        delete state;
    });
}

bool WASIHost::probe_poll(int32_t guest_fd, uint8_t type, uint16_t& flags, uint16_t& error)
{
    FdEntry* entry = find_fd(guest_fd);
    if (!entry) {
        flags = 0;
        error = WASI_ERRNO_BADF;
        return true;
    }

    return poll_fd_ready(entry, type, flags, error);
}

static bool normalize_path(const std::string& input, std::string& output)
{
    if (input.empty()) {
        output.clear();
        return true;
    }
    if (input[0] == '/')
        return false;

    std::vector<std::string> parts;
    size_t start = 0;

    while (start <= input.size()) {
        size_t end = input.find('/', start);
        std::string part = end == std::string::npos ? input.substr(start) : input.substr(start, end - start);

        if (part.empty() || part == ".")
            ;
        else if (part == "..") {
            if (parts.empty())
                return false;
            parts.pop_back();
        } else
            parts.push_back(part);

        if (end == std::string::npos)
            break;
        start = end + 1;
    }

    output.clear();
    for (size_t i = 0; i < parts.size(); i++) {
        if (i)
            output += '/';
        output += parts[i];
    }
    return true;
}

FdEntry* WASIHost::find_fd(int32_t fd)
{
    auto it = m_fds.find(fd);
    return it == m_fds.end() ? nullptr : &it->second;
}

// Resolve a descriptor by its instance id: the guest fd number may have been
// renumbered while the request was in flight, and a closed descriptor must not
// be confused with a later one that happens to reuse the number.
FdEntry* WASIHost::find_fd_by_id(uint64_t fd_id, int32_t* guest_fd)
{
    for (auto& item : m_fds) {
        if (item.second.id == fd_id) {
            if (guest_fd)
                *guest_fd = item.first;
            return &item.second;
        }
    }
    return nullptr;
}

int32_t WASIHost::insert_fd(FdEntry& entry)
{
    int32_t fd = m_next_fd++;
    entry.id = m_next_fd_id++;
    m_fds[fd] = entry;
    return fd;
}

bool WASIHost::memory(uint8_t*& data, size_t& size)
{
    if (m_memory.IsEmpty())
        return false;

    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> memory_object = m_memory.Get(isolate->m_isolate);
    v8::Local<v8::Value> buffer;
    if (!memory_object->Get(context, isolate->NewString("buffer")).ToLocal(&buffer))
        return false;

    std::shared_ptr<v8::BackingStore> store;
    if (buffer->IsArrayBuffer())
        store = buffer.As<v8::ArrayBuffer>()->GetBackingStore();
    else if (buffer->IsSharedArrayBuffer())
        store = buffer.As<v8::SharedArrayBuffer>()->GetBackingStore();
    else
        return false;

    data = (uint8_t*)store->Data();
    size = store->ByteLength();
    return data != nullptr;
}

int32_t WASIHost::resolve_path(FdEntry& dir, int32_t path, int32_t path_len,
    exlib::string& host)
{
    if (dir.kind != FdEntry::DIR)
        return WASI_ERRNO_NOTDIR;

    uint8_t* data;
    size_t size;
    if (!memory(data, size))
        return WASI_ERRNO_INVAL;

    if (path < 0 || path_len < 0
        || (uint64_t)(uint32_t)path + (uint64_t)path_len > size)
        return WASI_ERRNO_OVERFLOW;

    std::string raw((const char*)data + path, path_len);
    if (raw.find('\0') != std::string::npos)
        return WASI_ERRNO_INVAL;

    std::string normalized;
    if (!normalize_path(raw, normalized))
        return WASI_ERRNO_NOTCAPABLE;

    host = dir.host_path;
    if (!normalized.empty()) {
        if (!host.empty() && host[host.size() - 1] != '/')
            host += '/';
        host += normalized;
    }

    return WASI_ERRNO_SUCCESS;
}

void WASIHost::on_io_done(uint64_t fd_id, uint32_t nbytes)
{
    FdEntry* entry = find_fd_by_id(fd_id);
    if (!entry || entry->kind != FdEntry::FILE)
        return;

    if (entry->fdflags & WASI_FDFLAG_APPEND) {
        // O_APPEND writes move the host position past the written bytes; the
        // tracked offset must follow it (uvwasi reads the position with lseek).
#ifndef _WIN32
        if (entry->host_fd >= 0) {
            off_t pos = ::lseek(entry->host_fd, 0, SEEK_CUR);
            if (pos >= 0)
                entry->offset = (int64_t)pos;
        }
#endif
    } else
        entry->offset += nbytes;
}

void WASIHost::on_open_failed(uint64_t fd_id)
{
    int32_t fd;
    if (find_fd_by_id(fd_id, &fd))
        m_fds.erase(fd);
}

bool WASIHost::on_open_done(uint64_t fd_id, int32_t host_fd, int32_t open_flags,
    uint64_t rights_base, uint64_t rights_inheriting, int32_t& guest_fd)
{
    FdEntry* entry = find_fd_by_id(fd_id, &guest_fd);
    if (!entry) {
        // The placeholder was closed while the open was in flight; the host
        // descriptor has no owner left, so close it on the uv thread that
        // created it.
        uv_post([host_fd]() {
            uv_fs_t* req = new uv_fs_t;
            uv_fs_close(s_uv_loop, req, host_fd, [](uv_fs_t* req) {
                uv_fs_req_cleanup(req);
                delete req;
            });
        });
        return false;
    }

    entry->host_fd = host_fd;
    entry->owned = true;

    // rights are intersected with the maximum the opened file type supports,
    // exactly like uvwasi_path_open().
    uint64_t max_base = 0;
    uint64_t max_inheriting = 0;
    host_rights(host_fd, open_flags, max_base, max_inheriting);
    entry->rights_base = rights_base & max_base;
    entry->rights_inheriting = rights_inheriting & max_inheriting;
    entry->kind = max_base == DIR_RIGHTS ? FdEntry::DIR : FdEntry::FILE;
    return true;
}

void WASIHost::on_seek_done(uint64_t fd_id, int64_t offset)
{
    FdEntry* entry = find_fd_by_id(fd_id);
    if (entry)
        entry->offset = offset;
}

void WASIHost::on_renumber_done(uint64_t from_id, int32_t to)
{
    int32_t from = -1;
    if (!find_fd_by_id(from_id, &from))
        return;

    auto it = m_fds.find(from);
    if (it == m_fds.end())
        return;

    m_fds.erase(to);
    m_fds[to] = std::move(it->second);
    m_fds.erase(it);
}

void WASIHost::fill_fdstat(uint64_t fd_id, const uv_stat_t& st, uint8_t* out)
{
    FdEntry* entry = find_fd_by_id(fd_id);

    memset(out, 0, 24);
    out[0] = filetype_from_mode(st.st_mode);
    if (entry)
        memcpy(out + 2, &entry->fdflags, sizeof(entry->fdflags));
    if (entry) {
        write_u64(out + 8, entry->rights_base);
        write_u64(out + 16, entry->rights_inheriting);
    }
}

int64_t WASIHost::store_call(std::unique_ptr<WasiCall> call)
{
    int64_t id = m_next_id++;
    m_calls[id] = std::move(call);
    return id;
}

std::unique_ptr<WasiCall> WASIHost::take_call(AsyncHandle& ac)
{
    if (ac.ctx(0).failed())
        return nullptr;

    int64_t id = ac.ctxv()[0].longVal();
    auto it = m_calls.find(id);
    if (it == m_calls.end())
        return nullptr;

    std::unique_ptr<WasiCall> call = std::move(it->second);
    m_calls.erase(it);
    return call;
}

static result_t bind_syscalls(v8::Local<v8::Object> self);

result_t WASIHost_base::_new(v8::Local<v8::Object> options, obj_ptr<WASIHost_base>& retVal,
    v8::Local<v8::Object> This)
{
    // Node requires an explicit version: the option selects the import
    // namespace ('wasi_snapshot_preview1' vs 'wasi_unstable') and cannot be
    // guessed from the guest.
    exlib::string version;
    result_t hr = GetConfigValue(options, "version", version, true);
    if (hr.failed()) {
        if (hr.is(CALL_E_PARAMNOTOPTIONAL))
            return result_t::make(CALL_E_INVALID_DATA)
                .with_type_name(errtype::kTypeError)
                .with_code("ERR_INVALID_ARG_TYPE")
                .with_message("The \"options.version\" property must be of type string.");
        return hr;
    }

    // Node accepts 'unstable' (namespace wasi_unstable) and 'preview1'.
    if (version == "unstable")
        version = "wasi_unstable";
    else if (version != "preview1") {
        exlib::string message = "wasi: unsupported version: '";
        message += version;
        message += "'";
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_VALUE")
            .with_message(message);
    }

    std::vector<exlib::string> args;
    std::vector<exlib::string> env;
    std::vector<std::pair<exlib::string, exlib::string>> preopens;
    hr = read_options(options, args, env, preopens);
    if (hr.failed())
        return hr;

    std::vector<std::pair<exlib::string, int32_t>> sockets;
    v8::Local<v8::Context> context = Isolate::current()->context();
    v8::Local<v8::Value> sockets_value;
    if (options->Get(context, Isolate::current()->NewString("sockets")).ToLocal(&sockets_value)
        && !sockets_value->IsUndefined() && !sockets_value->IsNull()) {
        if (!sockets_value->IsArray())
            return result_t::make(CALL_E_TYPEMISMATCH)
                .with_type_name(errtype::kTypeError)
                .with_code("ERR_INVALID_ARG_TYPE")
                .with_message("The \"options.sockets\" property must be an instance of Array.");

        v8::Local<v8::Array> array = sockets_value.As<v8::Array>();
        for (uint32_t i = 0; i < array->Length(); i++) {
            v8::Local<v8::Value> item;
            if (!array->Get(context, i).ToLocal(&item) || !item->IsObject())
                return result_t::make(CALL_E_TYPEMISMATCH)
                    .with_type_name(errtype::kTypeError)
                    .with_code("ERR_INVALID_ARG_TYPE")
                    .with_message("Each entry of \"options.sockets\" must be an object.");

            v8::Local<v8::Object> object = item.As<v8::Object>();
            exlib::string address = "127.0.0.1";
            int32_t port = 0;

            hr = GetConfigValue(object, "address", address, true);
            if (hr.failed() && !hr.is(CALL_E_PARAMNOTOPTIONAL))
                return hr;
            hr = GetConfigValue(object, "port", port, true);
            if (hr.failed()) {
                if (hr.is(CALL_E_PARAMNOTOPTIONAL))
                    return result_t::make(CALL_E_INVALID_DATA)
                        .with_type_name(errtype::kTypeError)
                        .with_code("ERR_INVALID_ARG_TYPE")
                        .with_message("Each entry of \"options.sockets\" must have a numeric port.");
                return hr;
            }
            if (port < 0 || port > 65535)
                return result_t::make(CALL_E_INVALID_DATA)
                    .with_type_name(errtype::kRangeError)
                    .with_code("ERR_OUT_OF_RANGE")
                    .with_message("The \"port\" option must be between 0 and 65535.");

            sockets.emplace_back(address, port);
        }
    }

    int32_t stdin_fd = 0;
    int32_t stdout_fd = 1;
    int32_t stderr_fd = 2;
    hr = GetConfigValue(options, "stdin", stdin_fd, true);
    if (hr.failed() && !hr.is(CALL_E_PARAMNOTOPTIONAL))
        return hr;
    hr = GetConfigValue(options, "stdout", stdout_fd, true);
    if (hr.failed() && !hr.is(CALL_E_PARAMNOTOPTIONAL))
        return hr;
    hr = GetConfigValue(options, "stderr", stderr_fd, true);
    if (hr.failed() && !hr.is(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    // Node validates the stdio descriptors as non-negative int32 values.
    const char* stdio_names[3] = { "stdin", "stdout", "stderr" };
    int32_t stdio_values[3] = { stdin_fd, stdout_fd, stderr_fd };
    for (int32_t i = 0; i < 3; i++) {
        if (stdio_values[i] < 0) {
            exlib::string message = "The value of \"options.";
            message += stdio_names[i];
            message += "\" is out of range. It must be >= 0 && <= 2147483647. Received ";
            message += std::to_string(stdio_values[i]);
            return result_t::make(CALL_E_INVALID_DATA)
                .with_type_name(errtype::kRangeError)
                .with_code("ERR_OUT_OF_RANGE")
                .with_message(message);
        }
    }

    bool return_on_exit = true;
    v8::Local<v8::Value> return_on_exit_value;
    if (options->Get(Isolate::current()->context(), Isolate::current()->NewString("returnOnExit"))
            .ToLocal(&return_on_exit_value)
        && !return_on_exit_value->IsUndefined()
        && !return_on_exit_value->IsNull()) {
        if (!return_on_exit_value->IsBoolean())
            return result_t::make(CALL_E_TYPEMISMATCH)
                .with_type_name(errtype::kTypeError)
                .with_code("ERR_INVALID_ARG_TYPE")
                .with_message("The \"options.returnOnExit\" property must be of type boolean.");
        return_on_exit = return_on_exit_value->BooleanValue(Isolate::current()->m_isolate);
    }

    obj_ptr<WASIHost> host = new WASIHost();
    host->set_version(version);
    host->set_return_on_exit(return_on_exit);
    hr = host->configure(std::move(args), std::move(env), std::move(preopens),
        std::move(sockets), stdin_fd, stdout_fd, stderr_fd);
    if (hr.failed())
        return hr;

    v8::Local<v8::Object> self = host->wrap(This);
    hr = bind_syscalls(self);
    if (hr.failed())
        return hr;

    retVal = host;
    return result_t();
}

// The import namespace a wasm module asks for, selected by the WASI version.
static const char* wasi_abi_name(const exlib::string& version)
{
    return version == "wasi_unstable" ? "wasi_unstable" : "wasi_snapshot_preview1";
}

result_t WASIHost::getImportObject(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> imports = v8::Object::New(isolate->m_isolate);
    const char* key = wasi_abi_name(m_version);
    imports->Set(context, isolate->NewString(key), wrap()).Check();

    retVal = imports;
    return result_t();
}

result_t WASIHost::registerAsImport()
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> registry = wasm_import_registry(isolate, true);
    registry->Set(context, isolate->NewString(wasi_abi_name(m_version)), wrap()).Check();

    return result_t();
}

result_t WASIHost::unregisterAsImport()
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> registry = wasm_import_registry(isolate, false);
    if (registry.IsEmpty())
        return result_t();

    exlib::string key = wasi_abi_name(m_version);
    v8::Local<v8::Value> current;
    if (!registry->Get(context, isolate->NewString(key)).ToLocal(&current)
        || current.IsEmpty() || current->IsUndefined())
        return result_t();

    // Only drop the entry while it still points at this host.
    if (!current->StrictEquals(wrap()))
        return result_t();

    registry->Delete(context, isolate->NewString(key)).Check();
    return result_t();
}

result_t WASIHost::get_wasiImport(v8::Local<v8::Object>& retVal)
{
    retVal = wrap();
    return result_t();
}

// Reads instance.exports[name] as a value (undefined when the instance or the
// export is missing).
static void get_export(Isolate* isolate, v8::Local<v8::Value> instance,
    const char* name, v8::Local<v8::Value>& fn)
{
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Value> exports;

    if (!instance.As<v8::Object>()
            ->Get(context, isolate->NewString("exports"))
            .ToLocal(&exports)
        || !exports->IsObject()
        || !exports.As<v8::Object>()
                ->Get(context, isolate->NewString(name))
                .ToLocal(&fn))
        fn = v8::Undefined(isolate->m_isolate);
}

// Node's wording for an export whose shape does not match the entry point.
static result_t require_export(Isolate* isolate, v8::Local<v8::Value> instance,
    const char* name, bool want_function)
{
    v8::Local<v8::Value> fn;
    get_export(isolate, instance, name, fn);

    if (want_function ? fn->IsFunction() : fn->IsUndefined())
        return result_t();

    exlib::string message = "The \"instance.exports.";
    message += name;
    message += want_function ? "\" property must be of type function."
                             : "\" property must be undefined.";
    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kTypeError)
        .with_code("ERR_INVALID_ARG_TYPE")
        .with_message(message);
}

result_t WASIHost::call_export(v8::Local<v8::Value> instance, const char* name, bool required,
    int32_t& retVal)
{
    if (instance.IsEmpty() || !instance->IsObject())
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_TYPE")
            .with_message("WASI: instance must be a WebAssembly.Instance.");

    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Value> fn;
    get_export(isolate, instance, name, fn);
    if (!fn->IsFunction()) {
        if (!required)
            return result_t();
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_TYPE")
            .with_message(exlib::string("WASI: the instance must export '") + name + "'.");
    }

    v8::TryCatch try_catch(isolate->m_isolate);
    v8::Local<v8::Value> result;
    if (!fn.As<v8::Function>()->Call(context, instance, 0, nullptr).ToLocal(&result)) {
        if (m_exited) {
            m_exited = false;
            retVal = m_exit_code;
            return result_t();
        }

        if (try_catch.HasCaught()) {
            v8::String::Utf8Value text(isolate->m_isolate, try_catch.Exception());
            exlib::string message = "WASI: the guest trapped";
            if (*text) {
                message += ": ";
                message += *text;
            }
            return result_t::make(CALL_E_INVALID_CALL)
                .with_type_name(errtype::kError)
                .with_code("ERR_WASI_TRAPPED")
                .with_message(message);
        }
        return result_t(CALL_E_INVALID_CALL);
    }

    retVal = 0;
    return result_t();
}

// `require('./app.wasm')` hands back the instance exports, which carry the
// linked WebAssembly.Instance under a symbol. Accept that shape as well as a
// real instance, so `start(require('./app.wasm'))` needs no unwrapping.
static void resolve_instance(Isolate* isolate, v8::Local<v8::Value>& instance)
{
    if (instance.IsEmpty() || !instance->IsObject())
        return;

    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> obj = instance.As<v8::Object>();

    v8::Local<v8::Value> exports;
    if (obj->Get(context, isolate->NewString("exports")).ToLocal(&exports)
        && exports->IsObject())
        return;

    v8::Local<v8::Value> linked;
    if (obj->Get(context, wasm_instance_symbol(isolate)).ToLocal(&linked)
        && linked->IsObject())
        instance = linked;
}

result_t WASIHost::start(v8::Local<v8::Value> instance, int32_t& retVal)
{
    Isolate* isolate = holder();

    resolve_instance(isolate, instance);

    result_t hr = finalizeBindings(instance, v8::Local<v8::Object>());
    if (hr.failed())
        return hr;

    hr = require_export(isolate, instance, "_start", true);
    if (hr.failed())
        return hr;
    hr = require_export(isolate, instance, "_initialize", false);
    if (hr.failed())
        return hr;

    return call_export(instance, "_start", true, retVal);
}

result_t WASIHost::initialize(v8::Local<v8::Value> instance)
{
    Isolate* isolate = holder();

    resolve_instance(isolate, instance);

    result_t hr = finalizeBindings(instance, v8::Local<v8::Object>());
    if (hr.failed())
        return hr;

    hr = require_export(isolate, instance, "_start", false);
    if (hr.failed())
        return hr;

    v8::Local<v8::Value> fn;
    get_export(isolate, instance, "_initialize", fn);
    if (fn->IsUndefined())
        return result_t();

    hr = require_export(isolate, instance, "_initialize", true);
    if (hr.failed())
        return hr;

    int32_t code = 0;
    return call_export(instance, "_initialize", false, code);
}

result_t WASIHost::finalizeBindings(v8::Local<v8::Value> instance, v8::Local<v8::Object> options)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    if (m_bound)
        return result_t::make(CALL_E_INVALID_CALL)
            .with_type_name(errtype::kError)
            .with_code("ERR_WASI_ALREADY_STARTED")
            .with_message("WASI instance has already started");

    resolve_instance(isolate, instance);

    if (instance.IsEmpty() || !instance->IsObject())
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_TYPE")
            .with_message("The \"instance\" argument must be of type object.");

    v8::Local<v8::Value> exports;
    if (!instance.As<v8::Object>()
            ->Get(context, isolate->NewString("exports"))
            .ToLocal(&exports)
        || !exports->IsObject())
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_TYPE")
            .with_message("The \"instance.exports\" property must be of type object.");

    v8::Local<v8::Value> memory;
    if (!options.IsEmpty()) {
        v8::Local<v8::Value> override;
        if (options->Get(context, isolate->NewString("memory")).ToLocal(&override)
            && !override->IsUndefined())
            memory = override;
    }

    if (memory.IsEmpty()
        && (!exports.As<v8::Object>()
                    ->Get(context, isolate->NewString("memory"))
                    .ToLocal(&memory)
            || !memory->IsObject()))
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_TYPE")
            .with_message("The \"instance.exports.memory\" property must be a WebAssembly.Memory object.");

    v8::Local<v8::Value> buffer;
    if (!memory->IsObject()
        || !memory.As<v8::Object>()->Get(context, isolate->NewString("buffer")).ToLocal(&buffer)
        || (!buffer->IsArrayBuffer() && !buffer->IsSharedArrayBuffer()))
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_TYPE")
            .with_message("WASI: memory must be a WebAssembly.Memory object.");

    m_memory.Reset(isolate->m_isolate, memory.As<v8::Object>());
    m_bound = true;
    return result_t();
}

// The sync phase: read guest memory, bound-check every pointer and copy the
// iovec payload. It runs in the JS scope, so the staging buffer is plain host
// memory before the fiber can be suspended.
result_t WASIHost::prepare_io(WasiCall& call, bool write, int32_t iovs, int32_t iovs_len,
    int32_t& err)
{
    Isolate* isolate = holder();

    uint8_t* data;
    size_t size;
    if (!memory(data, size)) {
        err = WASI_ERRNO_INVAL;
        return result_t();
    }

    if (iovs < 0 || iovs_len < 0 || iovs_len > 1024
        || (uint64_t)(uint32_t)iovs + (uint64_t)iovs_len * 8 > size) {
        err = WASI_ERRNO_OVERFLOW;
        return result_t();
    }

    uint64_t total = 0;
    for (int32_t i = 0; i < iovs_len; i++) {
        uint32_t addr = read_u32(data + iovs + i * 8);
        uint32_t len = read_u32(data + iovs + i * 8 + 4);

        if ((uint64_t)addr + len > size) {
            err = WASI_ERRNO_OVERFLOW;
            return result_t();
        }

        total += len;
        if (total > size || total > UINT32_MAX) {
            err = WASI_ERRNO_INVAL;
            return result_t();
        }

        call.iovs.push_back(std::make_pair(addr, len));
    }

    if ((uint64_t)call.out_addr + 4 > size) {
        err = WASI_ERRNO_OVERFLOW;
        return result_t();
    }

    if (write) {
        call.data.reserve((size_t)total);
        for (auto& iov : call.iovs)
            call.data.append((const char*)data + iov.first, iov.second);
    } else
        call.data.resize((size_t)total);

    call.memory.Reset(isolate->m_isolate, m_memory.Get(isolate->m_isolate));
    return result_t();
}

static result_t start_op(WASIHost* host, AsyncHandle& ac, int32_t& retVal,
    std::unique_ptr<WasiCall> call)
{
    if (!call) {
        retVal = WASI_ERRNO_INVAL;
        return result_t();
    }

    retVal = WASI_ERRNO_SUCCESS;
    WasiOp* op = new WasiOp(host, Isolate::current(), std::move(call), &retVal, std::move(ac));
    uv_post([op]() {
        op->start();
    });
    return result_t::pending();
}

result_t WASIHost::fd_write(int32_t fd, int32_t iovs, int32_t iovs_len, int32_t nwritten,
    int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 0;
        call->guest_fd = fd;
        call->out_addr = (uint32_t)nwritten;

        FdEntry* entry = find_fd(fd);
        int32_t err = WASI_ERRNO_SUCCESS;

        if (!entry)
            err = WASI_ERRNO_BADF;
        else if (!capable(entry, WASI_RIGHT_FD_WRITE))
            err = WASI_ERRNO_NOTCAPABLE;
        else if (entry->kind == FdEntry::DIR)
            err = WASI_ERRNO_ISDIR;
        else {
            call->fd = entry->host_fd;
            call->fd_id = entry->id;
            call->offset = entry->kind == FdEntry::FILE && !(entry->fdflags & WASI_FDFLAG_APPEND)
                ? entry->offset
                : -1;

            result_t hr = prepare_io(*call, true, iovs, iovs_len, err);
            if (hr.failed())
                return hr;
        }

        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::fd_read(int32_t fd, int32_t iovs, int32_t iovs_len, int32_t nread,
    int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 1;
        call->guest_fd = fd;
        call->out_addr = (uint32_t)nread;

        FdEntry* entry = find_fd(fd);
        int32_t err = WASI_ERRNO_SUCCESS;

        if (!entry)
            err = WASI_ERRNO_BADF;
        else if (!capable(entry, WASI_RIGHT_FD_READ))
            err = WASI_ERRNO_NOTCAPABLE;
        else if (entry->kind == FdEntry::DIR)
            err = WASI_ERRNO_ISDIR;
        else {
            call->fd = entry->host_fd;
            call->fd_id = entry->id;
            // reads are not affected by FDFLAG_APPEND; only writes are.
            call->offset = entry->kind == FdEntry::FILE ? entry->offset : -1;

            result_t hr = prepare_io(*call, false, iovs, iovs_len, err);
            if (hr.failed())
                return hr;
        }

        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::poll_oneoff(int32_t in_ptr, int32_t out_ptr, int32_t nsubscriptions,
    int32_t nevents, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 2;
        call->out_addr = (uint32_t)out_ptr;
        call->nevents_addr = (uint32_t)nevents;

        int32_t err = WASI_ERRNO_SUCCESS;
        Isolate* isolate = holder();
        uint8_t* data;
        size_t size;

        if (!memory(data, size)) {
            err = WASI_ERRNO_INVAL;
        } else {
            if (nsubscriptions <= 0)
                err = WASI_ERRNO_INVAL;
            else if (nsubscriptions > 1024
                || (uint64_t)(uint32_t)in_ptr + (uint64_t)nsubscriptions * 48 > size
                || (uint64_t)(uint32_t)out_ptr + (uint64_t)nsubscriptions * 32 > size
                || (uint64_t)(uint32_t)nevents + 4 > size)
                err = WASI_ERRNO_OVERFLOW;
            else {
                std::vector<PollEvent> ready;

                for (int32_t i = 0; i < nsubscriptions; i++) {
                    const uint8_t* sub = data + in_ptr + i * 48;
                    uint8_t type = sub[8];
                    uint64_t userdata = read_u64(sub);

                    if (type == WASI_EVENTTYPE_CLOCK) {
                        uint64_t timeout = read_u64(sub + 24);

                        // SUBSCRIPTION_CLOCK_ABSTIME converts an absolute
                        // realtime deadline into a relative timeout.
                        if (read_u16(sub + 40) & 1) {
                            uv_timeval64_t tv;
                            uint64_t now = 0;
                            if (uv_gettimeofday(&tv) == 0)
                                now = (uint64_t)tv.tv_sec * 1000000000ull
                                    + (uint64_t)tv.tv_usec * 1000ull;
                            timeout = timeout > now ? timeout - now : 0;
                        }

                        call->clocks.push_back(std::make_pair(userdata, timeout));
                    } else if (type == 1 || type == 2) {
                        int32_t subscribed_fd = (int32_t)read_u32(sub + 16);
                        PollEvent event;
                        event.userdata = userdata;
                        event.type = type;

                        FdEntry* entry = find_fd(subscribed_fd);
                        if (!entry) {
                            event.error = WASI_ERRNO_BADF;
                            ready.push_back(event);
                        } else if (!capable(entry,
                                       (type == 1 ? WASI_RIGHT_FD_READ : WASI_RIGHT_FD_WRITE)
                                           | WASI_RIGHT_POLL_FD_READWRITE))
                            err = WASI_ERRNO_NOTCAPABLE;
                        else if (poll_fd_ready(entry, type, event.flags, event.error))
                            ready.push_back(event);
                        else {
                            call->poll_subs.push_back(WasiCall::PollSub {
                                userdata, type, subscribed_fd });

                            PollFd pfd;
                            if (poll_handle(entry, pfd)) {
                                // One uv_poll per host handle; only the
                                // subscribed direction is watched, so a
                                // writable socket does not wake a read. A
                                // listener shares its descriptor with the
                                // uv_tcp_t that accepts the connections, and
                                // libuv keeps one watcher per fd, so the wait on
                                // a listener may fail with EEXIST; the probe
                                // still answers and finish() re-probes.
                                int32_t mask = (type == 1 ? UV_READABLE : UV_WRITABLE)
                                    | UV_DISCONNECT;
                                bool seen = false;
                                for (auto& item : call->poll_host_fds) {
                                    if (item.fd == pfd.fd && item.sock == pfd.sock) {
                                        item.mask |= mask;
                                        seen = true;
                                    }
                                }
                                if (!seen) {
                                    pfd.mask = mask;
                                    call->poll_host_fds.push_back(pfd);
                                }
                            }
                        }

                        if (err != WASI_ERRNO_SUCCESS)
                            break;
                    } else {
                        err = WASI_ERRNO_INVAL;
                        break;
                    }
                }

                if (err == WASI_ERRNO_SUCCESS) {
                    if (!ready.empty()) {
                        write_events((uint8_t*)data, size, (uint32_t)out_ptr, ready);
                        write_u32((uint8_t*)data + nevents, (uint32_t)ready.size());
                        retVal = WASI_ERRNO_SUCCESS;
                        return result_t();
                    }

                    if (call->clocks.empty() && call->poll_subs.empty()) {
                        err = WASI_ERRNO_INVAL;
                    } else {
                        if (!call->clocks.empty()) {
                            uint64_t timeout = UINT64_MAX;
                            for (auto& clock : call->clocks) {
                                if (clock.second < timeout) {
                                    timeout = clock.second;
                                    call->userdata = clock.first;
                                }
                            }

                            if (timeout > UINT64_MAX - 999999)
                                timeout = UINT64_MAX - 999999;
                            call->timeout_ms = (timeout + 999999) / 1000000;
                        }

                        call->memory.Reset(isolate->m_isolate, m_memory.Get(isolate->m_isolate));
                    }
                }
            }
        }

        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

static int32_t write_string_list(uint8_t* data, size_t size, int32_t pointer_addr,
    int32_t buffer_addr, const std::vector<exlib::string>& list)
{
    uint64_t total = 0;
    for (auto& item : list)
        total += item.size() + 1;

    if (pointer_addr < 0 || buffer_addr < 0
        || (uint64_t)(uint32_t)pointer_addr + (uint64_t)list.size() * 4 > size
        || (uint64_t)(uint32_t)buffer_addr + total > size)
        return WASI_ERRNO_OVERFLOW;

    uint32_t offset = (uint32_t)buffer_addr;
    for (size_t i = 0; i < list.size(); i++) {
        write_u32(data + pointer_addr + i * 4, offset);
        memcpy(data + offset, list[i].c_str(), list[i].size());
        data[offset + list[i].size()] = 0;
        offset += (uint32_t)list[i].size() + 1;
    }

    return WASI_ERRNO_SUCCESS;
}

result_t WASIHost::args_sizes_get(int32_t argc, int32_t argv_buf_size, int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    uint8_t* data;
    size_t size;
    if (!memory(data, size)) {
        retVal = WASI_ERRNO_INVAL;
        return result_t();
    }

    uint64_t total = 0;
    for (auto& arg : m_args)
        total += arg.size() + 1;

    if (argc < 0 || argv_buf_size < 0
        || (uint64_t)(uint32_t)argc + 4 > size
        || (uint64_t)(uint32_t)argv_buf_size + 4 > size) {
        retVal = WASI_ERRNO_OVERFLOW;
        return result_t();
    }

    write_u32(data + argc, (uint32_t)m_args.size());
    write_u32(data + argv_buf_size, (uint32_t)total);
    retVal = WASI_ERRNO_SUCCESS;
    return result_t();
}

result_t WASIHost::args_get(int32_t argv, int32_t argv_buf, int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    uint8_t* data;
    size_t size;
    if (!memory(data, size)) {
        retVal = WASI_ERRNO_INVAL;
        return result_t();
    }

    retVal = write_string_list(data, size, argv, argv_buf, m_args);
    return result_t();
}

result_t WASIHost::environ_sizes_get(int32_t environ_count, int32_t environ_buf_size,
    int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    uint8_t* data;
    size_t size;
    if (!memory(data, size)) {
        retVal = WASI_ERRNO_INVAL;
        return result_t();
    }

    uint64_t total = 0;
    for (auto& item : m_env)
        total += item.size() + 1;

    if (environ_count < 0 || environ_buf_size < 0
        || (uint64_t)(uint32_t)environ_count + 4 > size
        || (uint64_t)(uint32_t)environ_buf_size + 4 > size) {
        retVal = WASI_ERRNO_OVERFLOW;
        return result_t();
    }

    write_u32(data + environ_count, (uint32_t)m_env.size());
    write_u32(data + environ_buf_size, (uint32_t)total);
    retVal = WASI_ERRNO_SUCCESS;
    return result_t();
}

result_t WASIHost::environ_get(int32_t environ, int32_t environ_buf, int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    uint8_t* data;
    size_t size;
    if (!memory(data, size)) {
        retVal = WASI_ERRNO_INVAL;
        return result_t();
    }

    retVal = write_string_list(data, size, environ, environ_buf, m_env);
    return result_t();
}

result_t WASIHost::clock_res_get(int32_t clock_id, int32_t resolution, int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    if (clock_id < 0 || clock_id > 3) {
        retVal = WASI_ERRNO_INVAL;
        return result_t();
    }

    uint8_t* data;
    size_t size;
    if (!memory(data, size) || (uint64_t)(uint32_t)resolution + 8 > size) {
        retVal = WASI_ERRNO_OVERFLOW;
        return result_t();
    }

    write_u64(data + resolution, 1);
    retVal = WASI_ERRNO_SUCCESS;
    return result_t();
}

result_t WASIHost::clock_time_get(int32_t clock_id, int64_t precision, int32_t time,
    int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    (void)precision;

    uint8_t* data;
    size_t size;
    if (!memory(data, size) || (uint64_t)(uint32_t)time + 8 > size) {
        retVal = WASI_ERRNO_OVERFLOW;
        return result_t();
    }

    uint64_t value;
    if (clock_id == 0) {
        uv_timeval64_t tv;
        if (uv_gettimeofday(&tv) < 0) {
            retVal = WASI_ERRNO_IO;
            return result_t();
        }
        value = (uint64_t)tv.tv_sec * 1000000000ull + (uint64_t)tv.tv_usec * 1000ull;
    } else if (clock_id >= 1 && clock_id <= 3)
        value = uv_hrtime();
    else {
        retVal = WASI_ERRNO_INVAL;
        return result_t();
    }

    write_u64(data + time, value);
    retVal = WASI_ERRNO_SUCCESS;
    return result_t();
}

result_t WASIHost::random_get(int32_t buf, int32_t buf_len, int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    uint8_t* data;
    size_t size;
    if (!memory(data, size)) {
        retVal = WASI_ERRNO_INVAL;
        return result_t();
    }

    if (buf < 0 || buf_len < 0
        || (uint64_t)(uint32_t)buf + (uint64_t)buf_len > size) {
        retVal = WASI_ERRNO_OVERFLOW;
        return result_t();
    }

    int32_t r = uv_random(NULL, NULL, data + buf, (size_t)buf_len, 0, NULL);
    retVal = r < 0 ? wasi_errno(r) : WASI_ERRNO_SUCCESS;
    return result_t();
}

result_t WASIHost::sched_yield(int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    exlib::Fiber::sleep(0);
    retVal = WASI_ERRNO_SUCCESS;
    return result_t();
}

result_t WASIHost::proc_exit(int32_t code)
{
    if (!m_return_on_exit) {
        // Node installs wasiReturnOnProcExit only when returnOnExit is true;
        // otherwise uvwasi_proc_exit() terminates the process.
        process_base::exit(code);
    }

    m_exit_code = code;
    m_exited = true;

    return result_t::make(CALL_E_INVALID_CALL)
        .with_type_name(errtype::kError)
        .with_code("ERR_WASI_EXIT")
        .with_message("WASI: proc_exit");
}

// WASI preview1 signal numbers to host signals, mirroring
// uvwasi__translate_to_uv_signal(). Unsupported signals report NOSYS.
static int32_t wasi_signal(int32_t sig)
{
#ifdef SIGHUP
    if (sig == 1)
        return SIGHUP;
#endif
#ifdef SIGINT
    if (sig == 2)
        return SIGINT;
#endif
#ifdef SIGQUIT
    if (sig == 3)
        return SIGQUIT;
#endif
#ifdef SIGILL
    if (sig == 4)
        return SIGILL;
#endif
#ifdef SIGTRAP
    if (sig == 5)
        return SIGTRAP;
#endif
#ifdef SIGABRT
    if (sig == 6)
        return SIGABRT;
#endif
#ifdef SIGBUS
    if (sig == 7)
        return SIGBUS;
#endif
#ifdef SIGFPE
    if (sig == 8)
        return SIGFPE;
#endif
#ifdef SIGKILL
    if (sig == 9)
        return SIGKILL;
#endif
#ifdef SIGUSR1
    if (sig == 10)
        return SIGUSR1;
#endif
#ifdef SIGSEGV
    if (sig == 11)
        return SIGSEGV;
#endif
#ifdef SIGUSR2
    if (sig == 12)
        return SIGUSR2;
#endif
#ifdef SIGPIPE
    if (sig == 13)
        return SIGPIPE;
#endif
#ifdef SIGALRM
    if (sig == 14)
        return SIGALRM;
#endif
#ifdef SIGTERM
    if (sig == 15)
        return SIGTERM;
#endif
#ifdef SIGCHLD
    if (sig == 16)
        return SIGCHLD;
#endif
#ifdef SIGCONT
    if (sig == 17)
        return SIGCONT;
#endif
#ifdef SIGSTOP
    if (sig == 18)
        return SIGSTOP;
#endif
#ifdef SIGTSTP
    if (sig == 19)
        return SIGTSTP;
#endif
#ifdef SIGTTIN
    if (sig == 20)
        return SIGTTIN;
#endif
#ifdef SIGTTOU
    if (sig == 21)
        return SIGTTOU;
#endif
#ifdef SIGURG
    if (sig == 22)
        return SIGURG;
#endif
#ifdef SIGXCPU
    if (sig == 23)
        return SIGXCPU;
#endif
#ifdef SIGXFSZ
    if (sig == 24)
        return SIGXFSZ;
#endif
#ifdef SIGVTALRM
    if (sig == 25)
        return SIGVTALRM;
#endif
#ifdef SIGPROF
    if (sig == 26)
        return SIGPROF;
#endif
#ifdef SIGWINCH
    if (sig == 27)
        return SIGWINCH;
#endif
#ifdef SIGPOLL
    if (sig == 28)
        return SIGPOLL;
#endif
#ifdef SIGPWR
    if (sig == 29)
        return SIGPWR;
#endif
#ifdef SIGSYS
    if (sig == 30)
        return SIGSYS;
#endif
    return -1;
}

result_t WASIHost::proc_raise(int32_t sig, int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    int32_t host_sig = wasi_signal(sig);
    if (host_sig < 0) {
        retVal = WASI_ERRNO_NOSYS;
        return result_t();
    }

    int32_t r = uv_kill(uv_os_getpid(), host_sig);
    retVal = r == 0 ? WASI_ERRNO_SUCCESS : wasi_errno(r);
    return result_t();
}

result_t WASIHost::fd_close(int32_t fd, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* entry = find_fd(fd);
        if (!entry) {
            retVal = WASI_ERRNO_BADF;
            return result_t();
        }

        if (entry->kind == FdEntry::SOCKET) {
            SocketState* state = entry->sock;
            m_fds.erase(fd);
            auto it = std::find(m_sockets.begin(), m_sockets.end(), state);
            if (it != m_sockets.end())
                m_sockets.erase(it);
            close_socket_state(state);
            retVal = WASI_ERRNO_SUCCESS;
            return result_t();
        }

        if (entry->kind != FdEntry::FILE && !entry->owned) {
            m_fds.erase(fd);
            retVal = WASI_ERRNO_SUCCESS;
            return result_t();
        }

        // The host went lock free, so another fiber may run between this
        // descriptor lookup and the completion of the uv close request.
        // Remove the entry now: a second fd_close must report BADF instead of
        // closing the same host descriptor twice.
        int32_t host_fd = entry->host_fd;
        m_fds.erase(fd);

        if (host_fd < 0) {
            // A descriptor reserved by an in-flight path_open; the open
            // completion notices the missing entry and closes the host file.
            retVal = WASI_ERRNO_SUCCESS;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 3;
        call->fd = host_fd;
        call->guest_fd = fd;

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::fd_fdstat_get(int32_t fd, int32_t buf, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* entry = find_fd(fd);
        if (!entry) {
            retVal = WASI_ERRNO_BADF;
            return result_t();
        }

        uint8_t* data;
        size_t size;
        if (!memory(data, size) || (uint64_t)(uint32_t)buf + 24 > size) {
            retVal = WASI_ERRNO_OVERFLOW;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 4;
        call->fd = entry->kind == FdEntry::FILE ? entry->host_fd : (entry->kind == FdEntry::STDIO ? entry->host_fd : -1);
        call->fd_id = entry->id;
        call->guest_fd = fd;
        call->out_addr = (uint32_t)buf;
        call->memory.Reset(holder()->m_isolate, m_memory.Get(holder()->m_isolate));

        if (call->fd < 0) {
            // Virtual preopened directories and sockets have no host stat to
            // perform for fdstat.
            memset(data + buf, 0, 24);
            data[buf] = entry->kind == FdEntry::SOCKET ? WASI_FILETYPE_SOCKET_STREAM
                                                       : WASI_FILETYPE_DIRECTORY;
            memcpy(data + buf + 2, &entry->fdflags, sizeof(entry->fdflags));
            write_u64(data + buf + 8, entry->rights_base);
            write_u64(data + buf + 16, entry->rights_inheriting);
            retVal = WASI_ERRNO_SUCCESS;
            return result_t();
        }

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::fd_filestat_get(int32_t fd, int32_t buf, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* entry = find_fd(fd);
        if (!entry) {
            retVal = WASI_ERRNO_BADF;
            return result_t();
        }

        uint8_t* data;
        size_t size;
        if (!memory(data, size) || (uint64_t)(uint32_t)buf + 64 > size) {
            retVal = WASI_ERRNO_OVERFLOW;
            return result_t();
        }

        if (!capable(entry, WASI_RIGHT_FD_FILESTAT_GET)) {
            retVal = WASI_ERRNO_NOTCAPABLE;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 5;
        call->fd = entry->host_fd;
        call->guest_fd = fd;
        call->out_addr = (uint32_t)buf;
        if (entry->host_fd < 0)
            call->host_path = entry->host_path;
        call->memory.Reset(holder()->m_isolate, m_memory.Get(holder()->m_isolate));

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::fd_prestat_get(int32_t fd, int32_t buf, int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    FdEntry* entry = find_fd(fd);
    if (!entry) {
        retVal = WASI_ERRNO_BADF;
        return result_t();
    }
    // uvwasi reports INVAL for an existing descriptor that is not a preopen
    // (fd_prestat_dir_name uses EBADF for the same case).
    if (!entry->preopen) {
        retVal = WASI_ERRNO_INVAL;
        return result_t();
    }

    uint8_t* data;
    size_t size;
    if (!memory(data, size) || (uint64_t)(uint32_t)buf + 8 > size) {
        retVal = WASI_ERRNO_OVERFLOW;
        return result_t();
    }

    memset(data + buf, 0, 8);
    data[buf] = 0; // dir
    write_u32(data + buf + 4, (uint32_t)entry->guest_name.size());
    retVal = WASI_ERRNO_SUCCESS;
    return result_t();
}

result_t WASIHost::fd_prestat_dir_name(int32_t fd, int32_t path, int32_t path_len,
    int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    FdEntry* entry = find_fd(fd);
    if (!entry || !entry->preopen) {
        retVal = WASI_ERRNO_BADF;
        return result_t();
    }

    uint8_t* data;
    size_t size;
    if (!memory(data, size)) {
        retVal = WASI_ERRNO_INVAL;
        return result_t();
    }

    if (path < 0 || path_len < 0
        || (uint64_t)(uint32_t)path + (uint64_t)path_len > size) {
        retVal = WASI_ERRNO_OVERFLOW;
        return result_t();
    }

    if ((size_t)path_len < entry->guest_name.size()) {
        retVal = WASI_ERRNO_NOBUFS;
        return result_t();
    }

    memcpy(data + path, entry->guest_name.data(), entry->guest_name.size());
    retVal = WASI_ERRNO_SUCCESS;
    return result_t();
}

result_t WASIHost::path_open(int32_t fd, int32_t dirflags, int32_t path, int32_t path_len,
    int32_t oflags, int64_t rights_base, int64_t rights_inheriting, int32_t fdflags,
    int32_t opened_fd, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* dir = find_fd(fd);
        int32_t err = dir ? WASI_ERRNO_SUCCESS : WASI_ERRNO_BADF;
        exlib::string host_path;

        uint64_t requested_base = (uint64_t)rights_base;
        uint64_t requested_inheriting = (uint64_t)rights_inheriting;

        // uvwasi_path_open() computes the required rights before opening.
        bool want_read = (requested_base & (WASI_RIGHT_FD_READ | WASI_RIGHT_FD_READDIR)) != 0;
        bool want_write = (requested_base
                              & (WASI_RIGHT_FD_DATASYNC | WASI_RIGHT_FD_WRITE
                                  | WASI_RIGHT_FD_ALLOCATE | WASI_RIGHT_FD_FILESTAT_SET_SIZE))
            != 0;

        uint64_t needed_base = WASI_RIGHT_PATH_OPEN;
        uint64_t needed_inheriting = requested_base | requested_inheriting;

        if (oflags & WASI_O_CREAT)
            needed_base |= WASI_RIGHT_PATH_CREATE_FILE;
        if (oflags & WASI_O_TRUNC)
            needed_base |= WASI_RIGHT_PATH_FILESTAT_SET_SIZE;
        if (fdflags & WASI_FDFLAG_DSYNC)
            needed_inheriting |= WASI_RIGHT_FD_DATASYNC;
        if (fdflags & (WASI_FDFLAG_RSYNC | WASI_FDFLAG_SYNC))
            needed_inheriting |= WASI_RIGHT_FD_SYNC;
        if (want_write && !(fdflags & (WASI_FDFLAG_APPEND | WASI_FDFLAG_DSYNC | WASI_FDFLAG_SYNC)))
            needed_inheriting |= WASI_RIGHT_FD_SEEK;

        if (err == WASI_ERRNO_SUCCESS && !capable(dir, needed_base, needed_inheriting))
            err = WASI_ERRNO_NOTCAPABLE;

        if (err == WASI_ERRNO_SUCCESS)
            err = resolve_path(*dir, path, path_len, host_path);

        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        uint8_t* data;
        size_t size;
        if (!memory(data, size) || (uint64_t)(uint32_t)opened_fd + 4 > size) {
            retVal = WASI_ERRNO_OVERFLOW;
            return result_t();
        }

        int32_t flags = want_write ? (want_read ? O_RDWR : O_WRONLY) : O_RDONLY;

        if (oflags & WASI_O_CREAT)
            flags |= O_CREAT;
        if (oflags & WASI_O_EXCL)
            flags |= O_EXCL;
        if (oflags & WASI_O_TRUNC)
            flags |= O_TRUNC;
        if (fdflags & WASI_FDFLAG_APPEND)
            flags |= O_APPEND;
#ifdef O_DSYNC
        if (fdflags & WASI_FDFLAG_DSYNC)
            flags |= O_DSYNC;
#endif
#ifdef O_NONBLOCK
        if (fdflags & WASI_FDFLAG_NONBLOCK)
            flags |= O_NONBLOCK;
#endif
#ifdef O_RSYNC
        if (fdflags & WASI_FDFLAG_RSYNC)
            flags |= O_RSYNC;
#endif
#ifdef O_SYNC
        if (fdflags & WASI_FDFLAG_SYNC)
            flags |= O_SYNC;
#endif
#ifdef O_DIRECTORY
        if (oflags & WASI_O_DIRECTORY)
            flags |= O_DIRECTORY;
#endif

        // Reserve the descriptor now so the guest fd number cannot be handed
        // out twice; it is not owned until the open completes, so a concurrent
        // close only drops the reservation.
        FdEntry entry;
        entry.kind = FdEntry::FILE;
        entry.host_path = host_path;
        entry.host_fd = -1;
        entry.owned = false;
        entry.fdflags = (uint16_t)fdflags;
        entry.rights_base = requested_base;
        entry.rights_inheriting = requested_inheriting;
        int32_t guest_fd = insert_fd(entry);

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 6;
        call->host_path = host_path;
        call->open_flags = flags;
        call->open_mode = 0666;
        call->open_rights_base = requested_base;
        call->open_rights_inheriting = requested_inheriting;
        call->guest_fd = guest_fd;
        call->fd_id = entry.id;
        call->out_addr = (uint32_t)opened_fd;
        call->memory.Reset(holder()->m_isolate, m_memory.Get(holder()->m_isolate));

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::path_filestat_get(int32_t fd, int32_t flags, int32_t path,
    int32_t path_len, int32_t buf, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* dir = find_fd(fd);
        int32_t err = dir ? WASI_ERRNO_SUCCESS : WASI_ERRNO_BADF;
        exlib::string host_path;

        if (err == WASI_ERRNO_SUCCESS && !capable(dir, WASI_RIGHT_PATH_FILESTAT_GET))
            err = WASI_ERRNO_NOTCAPABLE;

        if (err == WASI_ERRNO_SUCCESS)
            err = resolve_path(*dir, path, path_len, host_path);

        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        uint8_t* data;
        size_t size;
        if (!memory(data, size) || (uint64_t)(uint32_t)buf + 64 > size) {
            retVal = WASI_ERRNO_OVERFLOW;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 7;
        call->host_path = host_path;
        call->follow = (flags & 1) != 0;
        call->guest_fd = fd;
        call->out_addr = (uint32_t)buf;
        call->memory.Reset(holder()->m_isolate, m_memory.Get(holder()->m_isolate));

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::fd_seek(int32_t fd, int64_t offset, int32_t whence, int32_t newoffset,
    int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* entry = find_fd(fd);
        if (!entry) {
            retVal = WASI_ERRNO_BADF;
            return result_t();
        }
        if (!capable(entry, WASI_RIGHT_FD_SEEK)) {
            retVal = WASI_ERRNO_NOTCAPABLE;
            return result_t();
        }
        if (entry->kind != FdEntry::FILE) {
            retVal = WASI_ERRNO_SPIPE;
            return result_t();
        }
        if (whence < 0 || whence > 2) {
            retVal = WASI_ERRNO_INVAL;
            return result_t();
        }

        if (whence != 2) {
            int64_t position = (whence == 0 ? 0 : entry->offset) + offset;
            if (position < 0) {
                retVal = WASI_ERRNO_INVAL;
                return result_t();
            }

            uint8_t* data;
            size_t size;
            if (!memory(data, size) || (uint64_t)(uint32_t)newoffset + 8 > size) {
                retVal = WASI_ERRNO_OVERFLOW;
                return result_t();
            }

            write_u64(data + newoffset, (uint64_t)position);
            entry->offset = position;
            retVal = WASI_ERRNO_SUCCESS;
            return result_t();
        }

        uint8_t* data;
        size_t size;
        if (!memory(data, size) || (uint64_t)(uint32_t)newoffset + 8 > size) {
            retVal = WASI_ERRNO_OVERFLOW;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 8;
        call->fd = entry->host_fd;
        call->fd_id = entry->id;
        call->guest_fd = fd;
        call->offset = offset;
        call->out_addr = (uint32_t)newoffset;
        call->memory.Reset(holder()->m_isolate, m_memory.Get(holder()->m_isolate));

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::fd_tell(int32_t fd, int32_t offset, int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    FdEntry* entry = find_fd(fd);
    if (!entry) {
        retVal = WASI_ERRNO_BADF;
        return result_t();
    }
    if (!capable(entry, WASI_RIGHT_FD_TELL)) {
        retVal = WASI_ERRNO_NOTCAPABLE;
        return result_t();
    }
    if (entry->kind != FdEntry::FILE) {
        retVal = WASI_ERRNO_SPIPE;
        return result_t();
    }

    uint8_t* data;
    size_t size;
    if (!memory(data, size) || (uint64_t)(uint32_t)offset + 8 > size) {
        retVal = WASI_ERRNO_OVERFLOW;
        return result_t();
    }

    write_u64(data + offset, (uint64_t)entry->offset);
    retVal = WASI_ERRNO_SUCCESS;
    return result_t();
}

result_t WASIHost::fd_pread(int32_t fd, int32_t iovs, int32_t iovs_len, int64_t offset,
    int32_t nread, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 1;
        call->guest_fd = fd;
        call->no_advance = true;
        call->offset = offset;
        call->out_addr = (uint32_t)nread;

        FdEntry* entry = find_fd(fd);
        int32_t err = WASI_ERRNO_SUCCESS;

        if (!entry)
            err = WASI_ERRNO_BADF;
        else if (!capable(entry, WASI_RIGHT_FD_READ | WASI_RIGHT_FD_SEEK))
            err = WASI_ERRNO_NOTCAPABLE;
        else if (entry->kind == FdEntry::DIR)
            err = WASI_ERRNO_ISDIR;
        else {
            call->fd = entry->host_fd;
            result_t hr = prepare_io(*call, false, iovs, iovs_len, err);
            if (hr.failed())
                return hr;
        }

        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::fd_pwrite(int32_t fd, int32_t iovs, int32_t iovs_len, int64_t offset,
    int32_t nwritten, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 0;
        call->guest_fd = fd;
        call->no_advance = true;
        call->offset = offset;
        call->out_addr = (uint32_t)nwritten;

        FdEntry* entry = find_fd(fd);
        int32_t err = WASI_ERRNO_SUCCESS;

        if (!entry)
            err = WASI_ERRNO_BADF;
        else if (!capable(entry, WASI_RIGHT_FD_WRITE | WASI_RIGHT_FD_SEEK))
            err = WASI_ERRNO_NOTCAPABLE;
        else if (entry->kind == FdEntry::DIR)
            err = WASI_ERRNO_ISDIR;
        else {
            call->fd = entry->host_fd;
            result_t hr = prepare_io(*call, true, iovs, iovs_len, err);
            if (hr.failed())
                return hr;
        }

        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::fd_readdir(int32_t fd, int32_t buf, int32_t buf_len, int64_t cookie,
    int32_t bufused, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* entry = find_fd(fd);
        if (!entry) {
            retVal = WASI_ERRNO_BADF;
            return result_t();
        }
        if (!capable(entry, WASI_RIGHT_FD_READDIR)) {
            retVal = WASI_ERRNO_NOTCAPABLE;
            return result_t();
        }
        if (entry->kind != FdEntry::DIR) {
            retVal = WASI_ERRNO_NOTDIR;
            return result_t();
        }

        uint8_t* data;
        size_t size;
        if (!memory(data, size) || buf < 0 || buf_len < 0
            || (uint64_t)(uint32_t)buf + (uint64_t)buf_len > size
            || (uint64_t)(uint32_t)bufused + 4 > size) {
            retVal = WASI_ERRNO_OVERFLOW;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 11;
        call->host_path = entry->host_path;
        call->out_addr = (uint32_t)buf;
        call->buf_len = (uint32_t)buf_len;
        call->bufused_addr = (uint32_t)bufused;
        call->cookie = (uint64_t)cookie;
        call->memory.Reset(holder()->m_isolate, m_memory.Get(holder()->m_isolate));

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::path_create_directory(int32_t fd, int32_t path, int32_t path_len,
    int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* dir = find_fd(fd);
        int32_t err = dir ? WASI_ERRNO_SUCCESS : WASI_ERRNO_BADF;
        exlib::string host;

        if (err == WASI_ERRNO_SUCCESS && !capable(dir, WASI_RIGHT_PATH_CREATE_DIRECTORY))
            err = WASI_ERRNO_NOTCAPABLE;

        if (err == WASI_ERRNO_SUCCESS)
            err = resolve_path(*dir, path, path_len, host);

        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 12;
        call->host_path = host;

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::path_remove_directory(int32_t fd, int32_t path, int32_t path_len,
    int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* dir = find_fd(fd);
        int32_t err = dir ? WASI_ERRNO_SUCCESS : WASI_ERRNO_BADF;
        exlib::string host;

        if (err == WASI_ERRNO_SUCCESS && !capable(dir, WASI_RIGHT_PATH_REMOVE_DIRECTORY))
            err = WASI_ERRNO_NOTCAPABLE;

        if (err == WASI_ERRNO_SUCCESS)
            err = resolve_path(*dir, path, path_len, host);

        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 13;
        call->host_path = host;

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::path_unlink_file(int32_t fd, int32_t path, int32_t path_len,
    int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* dir = find_fd(fd);
        int32_t err = dir ? WASI_ERRNO_SUCCESS : WASI_ERRNO_BADF;
        exlib::string host;

        if (err == WASI_ERRNO_SUCCESS && !capable(dir, WASI_RIGHT_PATH_UNLINK_FILE))
            err = WASI_ERRNO_NOTCAPABLE;

        if (err == WASI_ERRNO_SUCCESS)
            err = resolve_path(*dir, path, path_len, host);

        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 14;
        call->host_path = host;

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::path_rename(int32_t fd, int32_t old_path, int32_t old_path_len,
    int32_t new_fd, int32_t new_path, int32_t new_path_len, int32_t& retVal,
    AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* old_dir = find_fd(fd);
        FdEntry* new_dir = find_fd(new_fd);
        int32_t err = (old_dir && new_dir) ? WASI_ERRNO_SUCCESS : WASI_ERRNO_BADF;
        exlib::string old_host;
        exlib::string new_host;

        if (err == WASI_ERRNO_SUCCESS) {
            uint64_t old_rights = WASI_RIGHT_PATH_RENAME_SOURCE
                | (fd == new_fd ? WASI_RIGHT_PATH_RENAME_TARGET : 0);
            if (!capable(old_dir, old_rights))
                err = WASI_ERRNO_NOTCAPABLE;
            else if (fd != new_fd && !capable(new_dir, WASI_RIGHT_PATH_RENAME_TARGET))
                err = WASI_ERRNO_NOTCAPABLE;
        }

        if (err == WASI_ERRNO_SUCCESS)
            err = resolve_path(*old_dir, old_path, old_path_len, old_host);
        if (err == WASI_ERRNO_SUCCESS)
            err = resolve_path(*new_dir, new_path, new_path_len, new_host);

        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 15;
        call->host_path = old_host;
        call->target_path = new_host;

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::path_link(int32_t old_fd, int32_t old_flags, int32_t old_path,
    int32_t old_path_len, int32_t new_fd, int32_t new_path, int32_t new_path_len,
    int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    (void)old_flags;

    if (ac.isSync()) {
        FdEntry* old_dir = find_fd(old_fd);
        FdEntry* new_dir = find_fd(new_fd);
        int32_t err = (old_dir && new_dir) ? WASI_ERRNO_SUCCESS : WASI_ERRNO_BADF;
        exlib::string old_host;
        exlib::string new_host;

        if (err == WASI_ERRNO_SUCCESS) {
            uint64_t old_rights = WASI_RIGHT_PATH_LINK_SOURCE
                | (old_fd == new_fd ? WASI_RIGHT_PATH_LINK_TARGET : 0);
            if (!capable(old_dir, old_rights))
                err = WASI_ERRNO_NOTCAPABLE;
            else if (old_fd != new_fd && !capable(new_dir, WASI_RIGHT_PATH_LINK_TARGET))
                err = WASI_ERRNO_NOTCAPABLE;
        }

        if (err == WASI_ERRNO_SUCCESS)
            err = resolve_path(*old_dir, old_path, old_path_len, old_host);
        if (err == WASI_ERRNO_SUCCESS)
            err = resolve_path(*new_dir, new_path, new_path_len, new_host);

        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 16;
        call->host_path = old_host;
        call->target_path = new_host;

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::path_readlink(int32_t fd, int32_t path, int32_t path_len, int32_t buf,
    int32_t buf_len, int32_t bufused, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* dir = find_fd(fd);
        int32_t err = dir ? WASI_ERRNO_SUCCESS : WASI_ERRNO_BADF;
        exlib::string host;

        if (err == WASI_ERRNO_SUCCESS && !capable(dir, WASI_RIGHT_PATH_READLINK))
            err = WASI_ERRNO_NOTCAPABLE;

        if (err == WASI_ERRNO_SUCCESS)
            err = resolve_path(*dir, path, path_len, host);
        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        uint8_t* data;
        size_t size;
        if (!memory(data, size) || buf < 0 || buf_len < 0
            || (uint64_t)(uint32_t)buf + (uint64_t)buf_len > size
            || (uint64_t)(uint32_t)bufused + 4 > size) {
            retVal = WASI_ERRNO_OVERFLOW;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 17;
        call->host_path = host;
        call->out_addr = (uint32_t)buf;
        call->buf_len = (uint32_t)buf_len;
        call->bufused_addr = (uint32_t)bufused;
        call->memory.Reset(holder()->m_isolate, m_memory.Get(holder()->m_isolate));

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::path_symlink(int32_t old_path, int32_t old_path_len, int32_t fd,
    int32_t new_path, int32_t new_path_len, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* dir = find_fd(fd);
        int32_t err = dir ? WASI_ERRNO_SUCCESS : WASI_ERRNO_BADF;
        exlib::string new_host;

        if (err == WASI_ERRNO_SUCCESS && !capable(dir, WASI_RIGHT_PATH_SYMLINK))
            err = WASI_ERRNO_NOTCAPABLE;

        if (err == WASI_ERRNO_SUCCESS)
            err = resolve_path(*dir, new_path, new_path_len, new_host);
        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        uint8_t* data;
        size_t size;
        if (!memory(data, size) || old_path < 0 || old_path_len < 0
            || (uint64_t)(uint32_t)old_path + (uint64_t)old_path_len > size) {
            retVal = WASI_ERRNO_OVERFLOW;
            return result_t();
        }

        std::string target((const char*)data + old_path, old_path_len);
        if (target.find('\0') != std::string::npos) {
            retVal = WASI_ERRNO_INVAL;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 18;
        call->host_path = new_host;
        call->target_path = target;

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::fd_filestat_set_size(int32_t fd, int64_t size, int32_t& retVal,
    AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* entry = find_fd(fd);
        if (!entry) {
            retVal = WASI_ERRNO_BADF;
            return result_t();
        }
        if (!capable(entry, WASI_RIGHT_FD_FILESTAT_SET_SIZE)) {
            retVal = WASI_ERRNO_NOTCAPABLE;
            return result_t();
        }
        if (entry->kind != FdEntry::FILE) {
            retVal = WASI_ERRNO_ISDIR;
            return result_t();
        }
        if (size < 0) {
            retVal = WASI_ERRNO_INVAL;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 19;
        call->fd = entry->host_fd;
        call->guest_fd = fd;
        call->offset = size;

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::sync_fd(int32_t fd, bool data_only, int32_t& retVal, AsyncHandle& ac)
{
    if (ac.isSync()) {
        FdEntry* entry = find_fd(fd);
        if (!entry) {
            retVal = WASI_ERRNO_BADF;
            return result_t();
        }
        if (!capable(entry, data_only ? WASI_RIGHT_FD_DATASYNC : WASI_RIGHT_FD_SYNC)) {
            retVal = WASI_ERRNO_NOTCAPABLE;
            return result_t();
        }
        if (!entry->owned) {
            retVal = WASI_ERRNO_SUCCESS;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 23;
        call->fd = entry->host_fd;
        call->data_only = data_only;

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::fd_sync(int32_t fd, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    return sync_fd(fd, false, retVal, ac);
}

result_t WASIHost::fd_datasync(int32_t fd, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    return sync_fd(fd, true, retVal, ac);
}

result_t WASIHost::fd_advise(int32_t fd, int64_t offset, int64_t len, int32_t advice,
    int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    (void)offset;
    (void)len;

    FdEntry* entry = find_fd(fd);
    if (!entry) {
        retVal = WASI_ERRNO_BADF;
        return result_t();
    }
    if (advice < 0 || advice > 5) {
        retVal = WASI_ERRNO_INVAL;
        return result_t();
    }
    if (!capable(entry, WASI_RIGHT_FD_ADVISE)) {
        retVal = WASI_ERRNO_NOTCAPABLE;
        return result_t();
    }
    if (entry->kind == FdEntry::DIR) {
        retVal = WASI_ERRNO_BADF;
        return result_t();
    }

    retVal = WASI_ERRNO_SUCCESS;
    return result_t();
}

result_t WASIHost::fd_allocate(int32_t fd, int64_t offset, int64_t len, int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    FdEntry* entry = find_fd(fd);
    if (!entry) {
        retVal = WASI_ERRNO_BADF;
        return result_t();
    }
    if (!capable(entry, WASI_RIGHT_FD_ALLOCATE)) {
        retVal = WASI_ERRNO_NOTCAPABLE;
        return result_t();
    }
    if (entry->kind == FdEntry::DIR) {
        retVal = WASI_ERRNO_ISDIR;
        return result_t();
    }
    if (offset < 0 || len < 0 || (uint64_t)offset + (uint64_t)len > INT64_MAX) {
        retVal = WASI_ERRNO_INVAL;
        return result_t();
    }

#ifndef _WIN32
    // macOS has no posix_fallocate(); uvwasi's fallback is fstat + ftruncate,
    // which grows the file to the requested end offset.
    if (entry->host_fd >= 0 && len > 0) {
        uint64_t end = (uint64_t)offset + (uint64_t)len;
        struct stat st;
        if (::fstat(entry->host_fd, &st) == 0 && (uint64_t)st.st_size < end) {
            if (::ftruncate(entry->host_fd, (off_t)end) != 0) {
                retVal = wasi_errno(uv_translate_sys_error(errno));
                return result_t();
            }
        }
    }
#endif

    retVal = WASI_ERRNO_SUCCESS;
    return result_t();
}

result_t WASIHost::fd_fdstat_set_flags(int32_t fd, int32_t flags, int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    FdEntry* entry = find_fd(fd);
    if (!entry) {
        retVal = WASI_ERRNO_BADF;
        return result_t();
    }
    if (!capable(entry, WASI_RIGHT_FD_FDSTAT_SET_FLAGS)) {
        retVal = WASI_ERRNO_NOTCAPABLE;
        return result_t();
    }

#ifdef _WIN32
    // uvwasi does not implement descriptor flag updates on Windows.
    (void)flags;
    retVal = WASI_ERRNO_NOSYS;
    return result_t();
#else
    // uvwasi maps the WASI flags onto the host descriptor with fcntl().
    if (entry->host_fd >= 0) {
        int32_t mapped = 0;
        if (flags & WASI_FDFLAG_APPEND)
            mapped |= O_APPEND;
        if (flags & WASI_FDFLAG_DSYNC)
#ifdef O_DSYNC
            mapped |= O_DSYNC;
#else
            mapped |= O_SYNC;
#endif
        if (flags & WASI_FDFLAG_NONBLOCK)
            mapped |= O_NONBLOCK;
        if (flags & WASI_FDFLAG_RSYNC)
#ifdef O_RSYNC
            mapped |= O_RSYNC;
#else
            mapped |= O_SYNC;
#endif
        if (flags & WASI_FDFLAG_SYNC)
            mapped |= O_SYNC;

        if (::fcntl(entry->host_fd, F_SETFL, mapped) < 0) {
            retVal = wasi_errno(UV_EINVAL);
            return result_t();
        }
    }
#endif

    entry->fdflags = (uint16_t)flags;
    retVal = WASI_ERRNO_SUCCESS;
    return result_t();
}

result_t WASIHost::fd_fdstat_set_rights(int32_t fd, int64_t rights_base,
    int64_t rights_inheriting, int32_t& retVal)
{
    if (!m_bound)
        return not_started();

    FdEntry* entry = find_fd(fd);
    if (!entry) {
        retVal = WASI_ERRNO_BADF;
        return result_t();
    }

    uint64_t base = (uint64_t)rights_base;
    uint64_t inheriting = (uint64_t)rights_inheriting;

    if ((base & ~entry->rights_base) != 0 || (inheriting & ~entry->rights_inheriting) != 0) {
        retVal = WASI_ERRNO_NOTCAPABLE;
        return result_t();
    }

    entry->rights_base = base;
    entry->rights_inheriting = inheriting;
    retVal = WASI_ERRNO_SUCCESS;
    return result_t();
}

result_t WASIHost::fd_filestat_set_times(int32_t fd, int64_t atim, int64_t mtim,
    int32_t flags, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* entry = find_fd(fd);
        if (!entry) {
            retVal = WASI_ERRNO_BADF;
            return result_t();
        }
        // uvwasi validates the flags before checking the rights.
        if (!valid_fstflags((uint32_t)flags)) {
            retVal = WASI_ERRNO_INVAL;
            return result_t();
        }
        if (!capable(entry, WASI_RIGHT_FD_FILESTAT_SET_TIMES)) {
            retVal = WASI_ERRNO_NOTCAPABLE;
            return result_t();
        }
        if (entry->kind == FdEntry::DIR) {
            retVal = WASI_ERRNO_ISDIR;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 20;
        call->fd = entry->host_fd;
        call->guest_fd = fd;
        call->atim = (uint64_t)atim;
        call->mtim = (uint64_t)mtim;
        call->time_flags = flags;

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::path_filestat_set_times(int32_t fd, int32_t flags, int32_t path,
    int32_t path_len, int64_t atim, int64_t mtim, int32_t fst_flags, int32_t& retVal,
    AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* dir = find_fd(fd);
        int32_t err = dir ? WASI_ERRNO_SUCCESS : WASI_ERRNO_BADF;
        exlib::string host;

        if (err == WASI_ERRNO_SUCCESS && !capable(dir, WASI_RIGHT_PATH_FILESTAT_SET_TIMES))
            err = WASI_ERRNO_NOTCAPABLE;
        if (err == WASI_ERRNO_SUCCESS && !valid_fstflags((uint32_t)fst_flags))
            err = WASI_ERRNO_INVAL;

        if (err == WASI_ERRNO_SUCCESS)
            err = resolve_path(*dir, path, path_len, host);
        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 21;
        call->host_path = host;
        call->follow = (flags & 1) != 0;
        call->atim = (uint64_t)atim;
        call->mtim = (uint64_t)mtim;
        call->time_flags = fst_flags;

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::fd_renumber(int32_t from, int32_t to, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        FdEntry* entry = find_fd(from);
        if (!entry) {
            retVal = WASI_ERRNO_BADF;
            return result_t();
        }
        if (from == to) {
            retVal = WASI_ERRNO_SUCCESS;
            return result_t();
        }

        uint64_t from_id = entry->id;

        auto target = m_fds.find(to);
        if (target == m_fds.end() || !target->second.owned) {
            // A free number, or a descriptor reserved by an in-flight
            // path_open: the move itself needs no host call.
            on_renumber_done(from_id, to);
            retVal = WASI_ERRNO_SUCCESS;
            return result_t();
        }

        if (target->second.kind == FdEntry::SOCKET) {
            // The socket handle is owned by the uv thread and is not a plain
            // host descriptor: close it through its state instead of uv_fs_close
            // (which would pull the fd out from under libuv).
            SocketState* state = target->second.sock;
            m_fds.erase(target);
            auto it = std::find(m_sockets.begin(), m_sockets.end(), state);
            if (it != m_sockets.end())
                m_sockets.erase(it);
            close_socket_state(state);
            on_renumber_done(from_id, to);
            retVal = WASI_ERRNO_SUCCESS;
            return result_t();
        }

        // Drop the target entry now so a concurrent close of `to` reports BADF
        // instead of closing the same host descriptor twice.
        int32_t target_fd = target->second.host_fd;
        m_fds.erase(target);

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 22;
        call->fd = target_fd;
        call->rename_from_id = from_id;
        call->rename_to = to;

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::sock_accept(int32_t fd, int32_t flags, int32_t fd_out, int32_t& retVal,
    AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        // uvwasi validates the accept flags before looking at the descriptor,
        // so a bad flag wins over BADF/NOTCAPABLE/NOTSOCK.
        if (flags & ~WASI_FDFLAG_NONBLOCK) {
            retVal = WASI_ERRNO_NOTSUP;
            return result_t();
        }

        FdEntry* entry = find_fd(fd);
        if (!entry) {
            retVal = WASI_ERRNO_BADF;
            return result_t();
        }
        if (!capable(entry, SOCKET_RIGHTS)) {
            retVal = WASI_ERRNO_NOTCAPABLE;
            return result_t();
        }
        if (entry->kind != FdEntry::SOCKET || !entry->sock || !entry->sock->listener) {
            retVal = WASI_ERRNO_NOTSOCK;
            return result_t();
        }

        uint8_t* data;
        size_t size;
        if (!memory(data, size) || (uint64_t)(uint32_t)fd_out + 4 > size) {
            retVal = WASI_ERRNO_OVERFLOW;
            return result_t();
        }

        SocketState* state = entry->sock;
        uv_tcp_t* conn = nullptr;
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            if (!state->backlog.empty()) {
                conn = state->backlog.front();
                state->backlog.pop_front();
            } else if (flags & WASI_FDFLAG_NONBLOCK) {
                retVal = WASI_ERRNO_AGAIN;
                return result_t();
            }
        }

        if (conn) {
            write_u32(data + fd_out, (uint32_t)adopt_socket(conn));
            retVal = WASI_ERRNO_SUCCESS;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 30;
        call->sock_state = state;
        call->out_addr = (uint32_t)fd_out;
        call->memory.Reset(holder()->m_isolate, m_memory.Get(holder()->m_isolate));

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::sock_recv(int32_t fd, int32_t ri_data, int32_t ri_data_len,
    int32_t ri_flags, int32_t ro_datalen, int32_t ro_flags, int32_t& retVal,
    AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        // uvwasi checks ri_flags before the descriptor (NOTSUP wins).
        if (ri_flags != 0) {
            retVal = WASI_ERRNO_NOTSUP;
            return result_t();
        }

        FdEntry* entry = find_fd(fd);
        if (!entry) {
            retVal = WASI_ERRNO_BADF;
            return result_t();
        }
        if (!capable(entry, SOCKET_RIGHTS)) {
            retVal = WASI_ERRNO_NOTCAPABLE;
            return result_t();
        }
        if (entry->kind != FdEntry::SOCKET || !entry->sock || entry->sock->listener) {
            retVal = WASI_ERRNO_NOTSOCK;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 31;
        call->guest_fd = fd;
        call->sock_state = entry->sock;
        call->out_addr = (uint32_t)ro_datalen;
        call->ro_flags_addr = (uint32_t)ro_flags;

        int32_t err = WASI_ERRNO_SUCCESS;
        result_t hr = prepare_io(*call, false, ri_data, ri_data_len, err);
        if (hr.failed())
            return hr;
        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        if (call->iovs.empty()) {
            retVal = WASI_ERRNO_INVAL;
            return result_t();
        }
        if (call->iovs[0].second == 0) {
            retVal = WASI_ERRNO_AGAIN;
            return result_t();
        }

        // uvwasi only reads into the first iovec.
        call->data.resize(call->iovs[0].second);
        call->iovs.resize(1);

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::sock_send(int32_t fd, int32_t si_data, int32_t si_data_len,
    int32_t si_flags, int32_t so_datalen, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        // uvwasi rejects non-zero si_flags before resolving the descriptor.
        if (si_flags != 0) {
            retVal = WASI_ERRNO_INVAL;
            return result_t();
        }

        FdEntry* entry = find_fd(fd);
        if (!entry) {
            retVal = WASI_ERRNO_BADF;
            return result_t();
        }
        if (!capable(entry, SOCKET_RIGHTS)) {
            retVal = WASI_ERRNO_NOTCAPABLE;
            return result_t();
        }
        if (entry->kind != FdEntry::SOCKET || !entry->sock || entry->sock->listener) {
            retVal = WASI_ERRNO_NOTSOCK;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 32;
        call->guest_fd = fd;
        call->sock_state = entry->sock;
        call->out_addr = (uint32_t)so_datalen;

        int32_t err = WASI_ERRNO_SUCCESS;
        result_t hr = prepare_io(*call, true, si_data, si_data_len, err);
        if (hr.failed())
            return hr;
        if (err != WASI_ERRNO_SUCCESS) {
            retVal = err;
            return result_t();
        }

        if (call->data.empty()) {
            uint8_t* data;
            size_t size;
            if (!memory(data, size) || (uint64_t)(uint32_t)so_datalen + 4 > size) {
                retVal = WASI_ERRNO_OVERFLOW;
                return result_t();
            }
            write_u32(data + so_datalen, 0);
            retVal = WASI_ERRNO_SUCCESS;
            return result_t();
        }

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

result_t WASIHost::sock_shutdown(int32_t fd, int32_t how, int32_t& retVal, AsyncHandle ac)
{
    if (!m_bound)
        return not_started();

    if (ac.isSync()) {
        // uvwasi only implements SHUT_WR: any other bit is NOTSUP before the
        // descriptor is resolved.
        if (how & ~2) {
            retVal = WASI_ERRNO_NOTSUP;
            return result_t();
        }

        FdEntry* entry = find_fd(fd);
        if (!entry) {
            retVal = WASI_ERRNO_BADF;
            return result_t();
        }
        if (!capable(entry, SOCKET_RIGHTS)) {
            retVal = WASI_ERRNO_NOTCAPABLE;
            return result_t();
        }
        if (entry->kind != FdEntry::SOCKET || !entry->sock || entry->sock->listener) {
            retVal = WASI_ERRNO_NOTSOCK;
            return result_t();
        }
        if (!(how & 2)) {
            retVal = WASI_ERRNO_SUCCESS;
            return result_t();
        }

        std::unique_ptr<WasiCall> call = std::make_unique<WasiCall>();
        call->op = 33;
        call->sock_state = entry->sock;

        ac.ctxv().resize(1);
        ac.ctxv()[0] = store_call(std::move(call));
        return result_t::nosync();
    }

    return start_op(this, ac, retVal, take_call(ac));
}

// wasm imports are called as plain functions, so `this` is undefined. Bind
// each syscall method to This and store the bound function as an own property
// of This; the import object is then This itself (wasiImport === wasi), and
// the generated METHOD_INSTANCE dispatch still finds the native host.
static result_t bind_syscalls(v8::Local<v8::Object> self)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    static const char* names[] = {
        "fd_read", "fd_write", "poll_oneoff",
        "args_sizes_get", "args_get", "environ_sizes_get", "environ_get",
        "clock_res_get", "clock_time_get", "random_get", "sched_yield", "proc_exit",
        "proc_raise",
        "fd_close", "fd_fdstat_get", "fd_filestat_get", "fd_prestat_get",
        "fd_prestat_dir_name", "path_open", "path_filestat_get",
        "fd_seek", "fd_tell", "fd_pread", "fd_pwrite", "fd_readdir",
        "path_create_directory", "path_remove_directory", "path_unlink_file",
        "path_rename", "path_link", "path_readlink", "path_symlink",
        "fd_filestat_set_size", "fd_sync", "fd_datasync", "fd_advise",
        "fd_allocate", "fd_fdstat_set_flags", "fd_fdstat_set_rights",
        "fd_filestat_set_times", "path_filestat_set_times", "fd_renumber",
        "sock_accept", "sock_recv", "sock_send", "sock_shutdown"
    };

    for (const char* name : names) {
        v8::Local<v8::Value> fn_value;
        if (!self->Get(context, isolate->NewString(name)).ToLocal(&fn_value)
            || !fn_value->IsFunction())
            continue;

        v8::Local<v8::Function> fn = fn_value.As<v8::Function>();
        v8::Local<v8::Value> bind_value;
        if (!fn->Get(context, isolate->NewString("bind")).ToLocal(&bind_value)
            || !bind_value->IsFunction())
            return result_t::make(CALL_E_INTERNAL)
                .with_message("WASI: Function.prototype.bind is unavailable.");

        v8::Local<v8::Value> argv[] = { self };
        v8::Local<v8::Value> bound;
        if (!bind_value.As<v8::Function>()
                 ->Call(context, fn, 1, argv)
                 .ToLocal(&bound))
            return result_t(CALL_E_JAVASCRIPT);

        if (!self->Set(context, isolate->NewString(name), bound).IsJust())
            return result_t(CALL_E_JAVASCRIPT);
    }

    return result_t();
}

} // namespace fibjs
