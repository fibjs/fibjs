/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-28 16:15:41
 * @modify date 2020-06-28 16:15:41
 * @desc FSWatcher - Fixed version with proper lifecycle management
 */

#pragma once

#include "ifs/FSWatcher.h"
#include "ifs/fs.h"
#include "path.h"
#include "EventEmitter.h"
#include "EventInfo.h"
#include "Buffer.h"
#include <uv/include/uv.h>
#include "Timer.h"
#include "utf8.h"
#include "Stat.h"
#include "AsyncUV.h"

#include <map>
#include <vector>

namespace fibjs {

class FSWatcher : public FSWatcher_base {

public:
    FSWatcher(
        exlib::string target,
        v8::Local<v8::Function> callback,
        bool persistent = true,
        bool recursive = false,
        exlib::string encoding = "utf8")
        : m_closed(false)
        , m_started(false)
        , m_filename(target)
        , m_persistent(persistent)
        , m_recursive(recursive)
        , m_encoding(encoding)
        , m_self_recursive(false)
    {
        memset(&m_fs_handle, 0, sizeof(m_fs_handle));
        m_fs_handle.data = this;

        if (!callback.IsEmpty()) {
            exlib::string _chname = "change";
            JSTrigger self = JSTrigger(this);
            self.putFunction(self.GetHiddenList(_chname, true), callback, _chname);
        }
    }

    ~FSWatcher()
    {
        // Ensure handle is closed in destructor
        if (m_started && !m_closed) {
            m_closed = true;
            if (m_self_recursive) {
                closeRecursive();
            } else if (!uv_is_closing((uv_handle_t*)&m_fs_handle)) {
                uv_fs_event_stop(&m_fs_handle);
            }
        }
    }

private:
    void on_watched(const char* relname, int events, int status)
    {
        if (m_closed)
            return;

        Variant v[2];
        exlib::string only_evtType;
        bool hasChange = events & UV_CHANGE;
        bool hasRename = events & UV_RENAME;

        if (status) {
            v[0] = "";
        } else if (hasRename && hasChange) {
            v[0] = "change";
        } else if (hasChange) {
            only_evtType = "changeonly";
            v[0] = "change";
        } else if (hasRename) {
            only_evtType = "renameonly";
            v[0] = "rename";
        } else {
            onError(CALL_E_INVALID_CALL, "Unknown change event type");
            return;
        }

        // Handle encoding
        if (relname) {
            if (m_encoding == "buffer") {
                obj_ptr<Buffer_base> buf = new Buffer(relname, strlen(relname));
                v[1] = buf;
            } else {
                v[1] = relname;
            }
        } else {
            v[1] = "";
        }

        _emit("change", v, 2);
        if (only_evtType.length() > 0)
            _emit(only_evtType, v, 2);
    }

    static void on_uv_close(uv_handle_t* handle)
    {
        FSWatcher* watcher = (FSWatcher*)handle->data;
        if (watcher) {
            watcher->Unref();
        }
    }

    static void fs_event_cb(uv_fs_event_t* fs_event, const char* filename, int events, int status)
    {
        FSWatcher* watcher = (FSWatcher*)fs_event->data;
        if (watcher && !watcher->m_closed) {
            watcher->on_watched(filename, events, status);
        }
    }

    void onError(result_t hr, const char* msg)
    {
        if (!m_closed) {
            (new EventInfo(this, "error", hr, msg))->emit();
        }
    }

public:
    result_t start()
    {
        if (m_closed)
            return CHECK_ERROR(Runtime::setError("watch: cannot re-start one closed FSWatcher."));

        if (m_started)
            return 0;

        // Keep object alive
        m_holder = new ValueHolder(wrap());

        if (m_persistent)
            isolate_ref();

        return uv_call([&] {
            // Ref the object for UV callback
            Ref();

            int32_t uv_err_no;

            if (m_recursive && needSelfRecursive()) {
                // libuv's inotify backend does not support UV_FS_EVENT_RECURSIVE.
                // Implement recursive watching ourselves (see FSWatcher.cpp).
                uv_err_no = startRecursive();
            } else {
                uv_err_no = uv_fs_event_init(s_uv_loop, &m_fs_handle);
                if (uv_err_no == 0) {
                    m_fs_handle.data = this;

                    uv_err_no = uv_fs_event_start(&m_fs_handle, fs_event_cb, m_filename.c_str(),
                        m_recursive ? UV_FS_EVENT_RECURSIVE : 0);

                    if (uv_err_no != 0)
                        uv_close((uv_handle_t*)&m_fs_handle, on_uv_close);
                }
            }

            if (uv_err_no != 0) {
                Unref();
                if (m_persistent)
                    isolate_unref();
                m_holder.Release();
                onError(CALL_E_INVALID_CALL, uv_strerror(uv_err_no));
                return uv_err_no;
            }

            m_started = true;
            return 0;
        });
    }

public:
    // FSWatcher_base
    virtual result_t close()
    {
        if (m_closed)
            return 0;

        m_closed = true;

        if (m_persistent)
            isolate_unref();

        if (m_holder)
            m_holder.Release();

        if (m_started) {
            uv_call([&] {
                if (m_self_recursive) {
                    closeRecursive();
                } else if (!uv_is_closing((uv_handle_t*)&m_fs_handle)) {
                    uv_fs_event_stop(&m_fs_handle);
                    uv_close((uv_handle_t*)&m_fs_handle, on_uv_close);
                }
                return 0;
            });
        } else {
            // If never started, still need to unref
            Unref();
        }

        _emit("close");

        return 0;
    }

    virtual result_t ref(obj_ptr<FSWatcher_base>& retVal)
    {
        isolate_ref();
        retVal = this;
        return 0;
    }

    virtual result_t unref(obj_ptr<FSWatcher_base>& retVal)
    {
        isolate_unref();
        retVal = this;
        return 0;
    }

public:
    const char* get_target() { return m_filename.c_str(); }
    bool isPersistent() { return m_persistent; }
    bool isRecursiveForDir() { return m_recursive; }

private:
    struct WatchItem {
        FSWatcher* owner;
        uv_fs_event_t handle;
        exlib::string abs_path; /* absolute path of the watched entry */
        exlib::string rel_path; /* path relative to the watch root ("" for root) */
        bool is_dir;
        bool is_root;
    };

    std::map<exlib::string, WatchItem*> m_items; /* recursive mode: abs_path -> item */
    bool m_self_recursive;                       /* recursive mode implemented in FSWatcher.cpp */

    /* true on platforms where libuv does not support UV_FS_EVENT_RECURSIVE */
    static bool needSelfRecursive();

    /* self-implemented recursive watching (Linux): one inotify watcher per entry,
     * new directories are registered by rescanning on directory change events,
     * mirroring how Node.js implements recursive watch where libuv cannot. */
    int32_t startRecursive();
    void closeRecursive();
    static void item_cb(uv_fs_event_t* handle, const char* filename, int events, int status);
    static void item_close_cb(uv_handle_t* handle);
    void onItemEvent(WatchItem* item, const char* filename, int events, int status);
    void rescanDir(WatchItem* item, bool emit_new);
    WatchItem* addItem(const exlib::string& abs_path, const exlib::string& rel_path, bool is_dir);
    bool removeItem(WatchItem* item);
    void emitChange(const char* type, const exlib::string& rel_path, bool also_only);
    static bool isDirectory(const exlib::string& path);
    static bool pathExists(const exlib::string& path);

private:
    uv_fs_event_t m_fs_handle;
    exlib::atomic m_closed;
    bool m_started;
    exlib::string m_filename;
    bool m_persistent;
    bool m_recursive;
    exlib::string m_encoding;
    obj_ptr<ValueHolder> m_holder;
};

}
