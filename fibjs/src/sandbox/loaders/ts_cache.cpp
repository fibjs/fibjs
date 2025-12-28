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
#include "ifs/os.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif

namespace fibjs {

// Compute hash for cache key (using std::hash for speed)
size_t ts_cache_hash(const uint8_t* data, size_t length)
{
    std::hash<std::string_view> hasher;
    return hasher(std::string_view((const char*)data, length));
}

// Get cache directory path: $TMPDIR/fibjs-ts-cache/<version>/
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
        s_cache_dir += fibjs_version;
        s_cache_dir += PATH_SLASH;
        
        s_initialized = true;
    }
    
    return s_cache_dir;
}

// Get cache file path for a given hash
static exlib::string get_cache_path(size_t hash)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%zx.js", hash);
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

bool ts_cache_get(size_t hash, size_t ts_length, obj_ptr<Buffer_base>& cached_js)
{
    // Skip cache for small files
    if (ts_length < TS_CACHE_MIN_SIZE)
        return false;
    
    exlib::string cache_path = get_cache_path(hash);
    
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
    
    size_t file_size = st.st_size;
    if (file_size == 0) {
        close(fd);
        return false;
    }
    
    // Read file content
    obj_ptr<Buffer> buf = new Buffer(nullptr, file_size);
    ssize_t bytes_read = read(fd, buf->data(), file_size);
    close(fd);
    
    if (bytes_read != (ssize_t)file_size)
        return false;
    
    cached_js = buf;
    return true;
}

void ts_cache_set(size_t hash, Buffer_base* js_buf)
{
    size_t js_length = Buffer::Cast(js_buf)->length();
    
    // Skip cache for small files
    if (js_length < TS_CACHE_MIN_SIZE)
        return;
    
    // Check if cache file already exists (avoid duplicate writes)
    exlib::string cache_path = get_cache_path(hash);
    struct stat st;
    if (stat(cache_path.c_str(), &st) == 0)
        return; // Already cached
    
    // Hold reference to buffer (zero copy, reference counting)
    obj_ptr<Buffer_base> buf = js_buf;
    
    // Async write with shared buffer
    async([hash, buf]() {
        if (!ensure_cache_dir())
            return;
        
        exlib::string cache_path = get_cache_path(hash);
        
        // Double check - another thread might have written it
        struct stat st;
        if (stat(cache_path.c_str(), &st) == 0)
            return;
        
        // Write to temp file first, then rename (atomic)
        exlib::string tmp_path = cache_path + ".tmp";
        
        Buffer* data = Buffer::Cast(buf);
        int fd = open(tmp_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0)
            return;
        
        ssize_t written = write(fd, data->data(), data->length());
        close(fd);
        
        if (written == (ssize_t)data->length()) {
            rename(tmp_path.c_str(), cache_path.c_str());
        } else {
            unlink(tmp_path.c_str());
        }
    }, CALL_E_NOSYNC);
}

} // namespace fibjs
