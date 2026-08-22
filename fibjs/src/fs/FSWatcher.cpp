/*
 * FSWatcher.cpp
 *
 * Self-implemented recursive fs.watch for platforms where libuv does not
 * support UV_FS_EVENT_RECURSIVE (Linux inotify backend).
 *
 * Strategy (mirrors Node.js lib/internal/fs/recursive_watch.js):
 *  - one non-recursive inotify watcher is registered per entry (files and
 *    directories) below the watch root;
 *  - when a directory watcher fires, the directory is rescanned; newly
 *    discovered entries get a watcher and a "rename" event, entries that
 *    disappeared are unwatched and reported as "rename";
 *  - a file watcher fires on content modification ("change"); when the file
 *    disappears its watcher is removed and "rename" is reported.
 *
 * Because discovery is scan based (readdir), events are never lost for
 * entries created between the mkdir and the scan, closing the gap that
 * plagues a pure event-driven approach.
 *
 *  Created on: Aug 22, 2026
 */

#include "FSWatcher.h"

#if defined(__linux__)

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <set>

namespace fibjs {

static exlib::string joinPath(const exlib::string& base, const char* name)
{
    if (base.empty() || base.back() == '/')
        return base + name;

    return base + "/" + name;
}

bool FSWatcher::needSelfRecursive()
{
    return true;
}

bool FSWatcher::pathExists(const exlib::string& path)
{
    struct stat st;

    return lstat(path.c_str(), &st) == 0;
}

bool FSWatcher::isDirectory(const exlib::string& path)
{
    struct stat st;

    if (lstat(path.c_str(), &st) != 0)
        return false;

    return S_ISDIR(st.st_mode);
}

void FSWatcher::emitChange(const char* type, const exlib::string& rel_path, bool also_only)
{
    if (m_closed)
        return;

    Variant v[2];
    v[0] = type;

    if (m_encoding == "buffer") {
        obj_ptr<Buffer_base> buf = new Buffer(rel_path.c_str(), (int32_t)rel_path.length());
        v[1] = buf;
    } else {
        v[1] = rel_path;
    }

    _emit("change", v, 2);

    if (also_only) {
        exlib::string only(type);
        only += "only";
        _emit(only, v, 2);
    }
}

FSWatcher::WatchItem* FSWatcher::addItem(const exlib::string& abs_path, const exlib::string& rel_path, bool is_dir)
{
    WatchItem* item = new WatchItem();
    item->owner = this;
    item->abs_path = abs_path;
    item->rel_path = rel_path;
    item->is_dir = is_dir;
    item->is_root = false;
    memset(&item->handle, 0, sizeof(item->handle));
    item->handle.data = item;

    if (uv_fs_event_init(s_uv_loop, &item->handle) != 0) {
        delete item;
        return NULL;
    }

    if (uv_fs_event_start(&item->handle, item_cb, abs_path.c_str(), 0) != 0) {
        /* Entry vanished concurrently (e.g. deleted between readdir and watch). */
        uv_close((uv_handle_t*)&item->handle, item_close_cb);
        return NULL;
    }

    m_items[abs_path] = item;

    return item;
}

bool FSWatcher::removeItem(WatchItem* item)
{
    auto it = m_items.find(item->abs_path);
    if (it == m_items.end() || it->second != item)
        return false;

    m_items.erase(it);

    /* Recursively remove descendants. Their events are not emitted
     * separately: the removal of the ancestor already represents the whole
     * subtree going away (same as Node.js). */
    std::vector<WatchItem*> children;
    exlib::string prefix = item->abs_path + "/";
    for (auto& kv : m_items)
        if (kv.second->abs_path.find(prefix) == 0)
            children.push_back(kv.second);

    for (WatchItem* c : children)
        removeItem(c);

    if (!uv_is_closing((uv_handle_t*)&item->handle)) {
        uv_fs_event_stop(&item->handle);
        uv_close((uv_handle_t*)&item->handle, item_close_cb);
    }

    return true;
}

void FSWatcher::rescanDir(WatchItem* item, bool emit_new)
{
    DIR* dir = opendir(item->abs_path.c_str());
    if (!dir) {
        /* The directory is gone (deleted or renamed away). */
        if (emit_new) {
            exlib::string rel = item->rel_path;
            if (removeItem(item))
                emitChange("rename", rel, false);
        }
        return;
    }

    std::set<exlib::string> seen;
    struct dirent* ent;

    while ((ent = readdir(dir)) != NULL) {
        const char* name = ent->d_name;

        if (name[0] == '.' && (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')))
            continue;

        exlib::string abs = joinPath(item->abs_path, name);
        seen.insert(abs);

        if (m_items.count(abs) > 0)
            continue;

        bool is_dir = isDirectory(abs);
        exlib::string rel = item->rel_path.empty() ? exlib::string(name) : item->rel_path + "/" + name;

        WatchItem* nit = addItem(abs, rel, is_dir);
        if (nit == NULL)
            continue; /* entry vanished concurrently */

        if (emit_new)
            emitChange("rename", rel, false);

        if (is_dir)
            rescanDir(nit, emit_new);
    }

    closedir(dir);

    /* Unwatch direct children that are no longer on disk. This is the
     * fallback for events that were never delivered to the child watcher
     * itself (e.g. a directory moved out of the tree: inotify reports it to
     * the parent, not to the moved directory's watchers). */
    std::vector<WatchItem*> gone;
    exlib::string child_prefix = item->rel_path.empty() ? "" : item->rel_path + "/";

    for (auto& kv : m_items) {
        WatchItem* it = kv.second;
        if (it == item)
            continue;

        const exlib::string& rel = it->rel_path;
        bool is_child;

        if (child_prefix.empty())
            is_child = rel.find('/') == exlib::string::npos;
        else {
            is_child = rel.find(child_prefix) == 0 &&
                       rel.find('/', child_prefix.length()) == exlib::string::npos;
        }

        if (is_child && seen.count(it->abs_path) == 0)
            gone.push_back(it);
    }

    for (WatchItem* it : gone) {
        exlib::string rel = it->rel_path;
        if (removeItem(it))
            emitChange("rename", rel, false);
    }
}

void FSWatcher::onItemEvent(WatchItem* item, const char* filename, int events, int status)
{
    if (m_closed)
        return;

    if (status) {
        /* The watched path is no longer accessible. */
        exlib::string rel = item->rel_path;
        if (removeItem(item))
            emitChange("rename", rel, false);
        return;
    }

    if (item->is_dir) {
        rescanDir(item, true);
    } else {
        if (!pathExists(item->abs_path)) {
            /* File deleted (IN_DELETE_SELF). */
            exlib::string rel = item->rel_path;
            if (removeItem(item))
                emitChange("rename", rel, false);
        } else {
            emitChange("change", item->rel_path, true);
        }
    }
}

void FSWatcher::item_cb(uv_fs_event_t* handle, const char* filename, int events, int status)
{
    WatchItem* item = (WatchItem*)handle->data;

    if (item && item->owner && !item->owner->m_closed)
        item->owner->onItemEvent(item, filename, events, status);
}

void FSWatcher::item_close_cb(uv_handle_t* handle)
{
    WatchItem* item = (WatchItem*)handle->data;

    if (item->is_root && item->owner)
        item->owner->Unref();

    delete item;
}

int32_t FSWatcher::startRecursive()
{
    m_self_recursive = true;

    if (!pathExists(m_filename))
        return UV_ENOENT;

    bool root_is_dir = isDirectory(m_filename);

    WatchItem* root = new WatchItem();
    root->owner = this;
    root->abs_path = m_filename;
    root->rel_path = "";
    root->is_dir = root_is_dir;
    root->is_root = false;
    memset(&root->handle, 0, sizeof(root->handle));
    root->handle.data = root;

    int32_t err = uv_fs_event_init(s_uv_loop, &root->handle);
    if (err != 0) {
        delete root;
        return err;
    }

    err = uv_fs_event_start(&root->handle, item_cb, m_filename.c_str(), 0);
    if (err != 0) {
        /* Close callback frees the item; not marked as root so that the
         * caller's failure path performs the matching Unref(). */
        uv_close((uv_handle_t*)&root->handle, item_close_cb);
        return err;
    }

    root->is_root = true;
    m_items[m_filename] = root;

    if (root_is_dir)
        rescanDir(root, false); /* initial scan: register watchers, do not emit */

    return 0;
}

void FSWatcher::closeRecursive()
{
    std::vector<WatchItem*> items;

    for (auto& kv : m_items)
        items.push_back(kv.second);

    m_items.clear();

    for (WatchItem* item : items) {
        if (!uv_is_closing((uv_handle_t*)&item->handle)) {
            uv_fs_event_stop(&item->handle);
            uv_close((uv_handle_t*)&item->handle, item_close_cb);
        }
    }
}

} // namespace fibjs

#else /* !defined(__linux__) */

namespace fibjs {

bool FSWatcher::needSelfRecursive()
{
    return false;
}

bool FSWatcher::pathExists(const exlib::string& path)
{
    return false;
}

bool FSWatcher::isDirectory(const exlib::string& path)
{
    return false;
}

void FSWatcher::emitChange(const char* type, const exlib::string& rel_path, bool also_only)
{
}

FSWatcher::WatchItem* FSWatcher::addItem(const exlib::string& abs_path, const exlib::string& rel_path, bool is_dir)
{
    return NULL;
}

bool FSWatcher::removeItem(WatchItem* item)
{
    return false;
}

void FSWatcher::rescanDir(WatchItem* item, bool emit_new)
{
}

void FSWatcher::onItemEvent(WatchItem* item, const char* filename, int events, int status)
{
}

void FSWatcher::item_cb(uv_fs_event_t* handle, const char* filename, int events, int status)
{
}

void FSWatcher::item_close_cb(uv_handle_t* handle)
{
}

int32_t FSWatcher::startRecursive()
{
    return 0;
}

void FSWatcher::closeRecursive()
{
}

} // namespace fibjs

#endif /* defined(__linux__) */
