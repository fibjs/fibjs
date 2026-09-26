/*
 * ts_cache.cpp
 *
 * TypeScript to JavaScript cache layer
 * Caches stripped JS to avoid re-parsing TypeScript on subsequent loads
 *
 *  Created on: Dec 28, 2024
 *      Author: lion
 */

#include "object.h"
#include "ts_cache.h"
#include "Buffer.h"
#include "version.h"
#include "gitinfo.h"
#include "ifs/os.h"
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#else
#include <unistd.h>
#endif

namespace fibjs {

// Cache file layout: this header followed by the stripped JS. The file name is the
// content hash of the source, so the payload never has to be validated against a
// path or a timestamp.
struct TsCacheHeader {
    uint32_t magic;
    uint32_t header_size; // sizeof(TsCacheHeader)
    int64_t js_size;      // payload length, to catch a truncated cache file
};

static constexpr uint32_t TS_CACHE_MAGIC = 0x54534332; // 'TSC2'

// Content hash of the source. std::hash<std::string_view> already runs at 12-16 GB/s
// on libc++ (about 70 us per MB, i.e. 1% of the ~7 ms/MB a strip costs), so there is
// nothing to win by hand-rolling a hash here - a byte-at-a-time FNV variant measured
// 3-4x slower.
size_t ts_cache_key(const uint8_t* data, size_t length)
{
    std::hash<std::string_view> hasher;
    return hasher(std::string_view((const char*)data, length));
}

// Get cache directory path: $TMPDIR/fibjs-ts-cache/<git describe>/
static exlib::string get_cache_dir()
{
    static exlib::string s_cache_dir;
    static bool s_initialized = false;
    
    if (!s_initialized) {
        exlib::string tmpdir;
        os_base::tmpdir(tmpdir);
        
        s_cache_dir = tmpdir;
        if (!s_cache_dir.empty() && s_cache_dir[s_cache_dir.length() - 1] != PATH_SLASH)
            s_cache_dir += PATH_SLASH;
        s_cache_dir += "fibjs-ts-cache";
        s_cache_dir += PATH_SLASH;
        // The cached code was produced by this build's stripper, so the directory is
        // stamped with the commit the binary was built from (GIT_INFO is
        // `git describe --tags --always`, e.g. "v0.37.0-1279-g3c23a873f", so it
        // already carries the version): upgrading fibjs switches to a fresh
        // directory instead of reusing entries written by an older build.
        s_cache_dir += GIT_INFO;
        s_cache_dir += PATH_SLASH;
        
        s_initialized = true;
    }
    
    return s_cache_dir;
}

// Get cache file path for a given key
static exlib::string get_cache_path(size_t key)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%zx.js", key);
    return get_cache_dir() + buf;
}

// Ensure cache directory exists
static bool ensure_cache_dir()
{
    static bool s_dir_created = false;
    
    if (s_dir_created)
        return true;
    
    exlib::string cache_dir = get_cache_dir();
    
    // Create parent directories
    exlib::string tmpdir;
    os_base::tmpdir(tmpdir);
    if (!tmpdir.empty() && tmpdir[tmpdir.length() - 1] != PATH_SLASH)
        tmpdir += PATH_SLASH;
    
    exlib::string ts_cache_dir = tmpdir + "fibjs-ts-cache";
    mkdir(ts_cache_dir.c_str(), 0755);
    
    mkdir(cache_dir.c_str(), 0755);
    
    s_dir_created = true;
    return true;
}

// Read the header of a cache file and check the payload is complete
static bool read_cache_header(int fd, size_t& header_size, size_t& js_size)
{
    TsCacheHeader header;
    if (read(fd, &header, sizeof(header)) != (ssize_t)sizeof(header))
        return false;
    if (header.magic != TS_CACHE_MAGIC || header.header_size != sizeof(TsCacheHeader))
        return false;
    if (header.js_size < 0)
        return false;

    header_size = header.header_size;
    js_size = (size_t)header.js_size;
    return true;
}

bool ts_cache_get(size_t key, size_t ts_length, obj_ptr<Buffer_base>& cached_js)
{
    // Skip cache for small files
    if (ts_length < TS_CACHE_MIN_SIZE)
        return false;

    exlib::string cache_path = get_cache_path(key);

    // Try to open cache file
    int fd = open(cache_path.c_str(), O_RDONLY);
    if (fd < 0)
        return false;

    // Get file size
    struct stat st;
    if (fstat(fd, &st) < 0) {
        close(fd);
        return false;
    }

    size_t header_size = 0, js_size = 0;
    if (!read_cache_header(fd, header_size, js_size)
        || (size_t)st.st_size != header_size + js_size) {
        close(fd);
        return false;
    }

    // Read the JS payload
    obj_ptr<Buffer> buf = new Buffer(nullptr, js_size);
    ssize_t bytes_read = read(fd, buf->data(), js_size);
    close(fd);

    if (bytes_read != (ssize_t)js_size)
        return false;

    cached_js = buf;
    return true;
}

void ts_cache_set(size_t key, Buffer_base* js_buf)
{
    Buffer* data = Buffer::Cast(js_buf);
    size_t js_length = data->length();
    
    // Skip cache for small files
    if (js_length < TS_CACHE_MIN_SIZE)
        return;

    exlib::string cache_path = get_cache_path(key);

    // Hold reference to buffer (zero copy, reference counting)
    obj_ptr<Buffer_base> buf = js_buf;

    // Async write with shared buffer
    async([cache_path, buf]() {
        if (!ensure_cache_dir())
            return;

        Buffer* data = Buffer::Cast(buf);

        // Write to temp file first, then rename (atomic)
        exlib::string tmp_path = cache_path + ".tmp";

        int fd = open(tmp_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0)
            return;

        TsCacheHeader header;
        header.magic = TS_CACHE_MAGIC;
        header.header_size = sizeof(TsCacheHeader);
        header.js_size = (int64_t)data->length();

        bool ok = write(fd, &header, sizeof(header)) == (ssize_t)sizeof(header)
            && write(fd, data->data(), data->length()) == (ssize_t)data->length();
        close(fd);

        if (ok) {
            rename(tmp_path.c_str(), cache_path.c_str());
        } else {
            unlink(tmp_path.c_str());
        }
    }, CALL_E_NOSYNC);
}

} // namespace fibjs
