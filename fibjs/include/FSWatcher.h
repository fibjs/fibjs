/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-28 16:15:41
 * @modify date 2020-06-28 16:15:41
 * @desc 
 */

#pragma once

#include "ifs/FSWatcher.h"
#include "ifs/fs.h"
#include "path.h"
#include "EventEmitter.h"
#include "EventInfo.h"
#include <uv/include/uv.h>
#include "Timer.h"
#include "utf8.h"
#include "Stat.h"
#include "AsyncUV.h"

namespace fibjs {

class FSWatcher : public FSWatcher_base {

public:
    FSWatcher(
        exlib::string target,
        v8::Local<v8::Function> callback,
        bool persistent = true,
        bool recursive = false)
        : m_closed(false)
        , m_filename(target)
        , m_Persistent(persistent)
        , m_recursiveForDir(recursive)
        , m_vholder(NULL)
    {
        if (!callback.IsEmpty()) {
            exlib::string _chname = "change";
            JSTrigger self = JSTrigger(this);
            self.putFunction(self.GetHiddenList(_chname, true), callback, _chname);
        }
    }

    ~FSWatcher() {};

public:
    EVENT_SUPPORT();

public:
    EVENT_FUNC(change);
    EVENT_FUNC(close);
    EVENT_FUNC(error);

public:
    void on_watched(const char* relname, int events, int status)
    {
        Variant v[2];
        exlib::string only_evtType;
        bool hasChange = events & UV_CHANGE;
        bool hasRename = events & UV_RENAME;

        if (status)
            v[0] = "";
        else if (hasRename && hasChange) {
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

        v[1] = relname;

        _emit("change", v, 2);
        if (only_evtType.length() > 0)
            _emit(only_evtType, v, 2);
    }

    static void on_close(uv_handle_t* handle)
    {
        FSWatcher* proc = container_of(handle, FSWatcher, m_fs_handle);
        proc->Unref();
    }

public:
    uv_fs_event_t m_fs_handle;

private:
    static void fs_event_cb(uv_fs_event_t* fs_event, const char* filename, int events, int status)
    {
        FSWatcher* p = container_of(fs_event, FSWatcher, m_fs_handle);

        uv_handle_t* handle = (uv_handle_t*)fs_event;
        if (p->m_closed) {
            uv_close(handle, on_close);
        } else {
            p->on_watched(filename, events, status);
        }
    }

public:
    void onError(result_t hr, const char* msg)
    {
        _emit("error", new EventInfo(this, "error", hr, msg));
    }

public:
    void start()
    {
        if (m_closed) {
            onError(CALL_E_INVALID_CALL, "cannot re-start one closed FSWatcher");
            return;
        }

        m_vholder = new ValueHolder(wrap());

        if (m_Persistent)
            isolate_ref();

        uv_call([&] {
            Ref();

            uv_fs_event_init(s_uv_loop, &m_fs_handle);

            int32_t uv_err_no = uv_fs_event_start(&m_fs_handle, fs_event_cb, get_target(), isRecursiveForDir() ? UV_FS_EVENT_RECURSIVE : 0);
            if (uv_err_no != 0) {
                onError(CALL_E_INVALID_CALL, uv_strerror(uv_err_no));
                close();
            }
            return uv_err_no;
        });
    };

public:
    // FSWatcher_base
    virtual result_t close()
    {
        if (m_closed)
            return 0;

        m_closed = true;

        if (isPersistent())
            isolate_unref();

        if (m_vholder)
            m_vholder.Release();

        _emit("close");

        return 0;
    };

public:
    const char* get_target() { return m_filename.c_str(); }
    bool isPersistent() { return m_Persistent; }
    bool isRecursiveForDir() { return m_recursiveForDir; }

    exlib::atomic m_closed;

protected:
    exlib::string m_filename;

    bool m_Persistent;
    bool m_recursiveForDir;
    obj_ptr<ValueHolder> m_vholder;
};
}
