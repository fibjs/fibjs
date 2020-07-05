/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-28 16:15:41
 * @modify date 2020-06-28 16:15:41
 * @desc 
 */

#include "ifs/FSWatcher.h"
#include "ifs/fs.h"
#include "EventEmitter.h"
#include "EventInfo.h"
#include <uv/include/uv.h>
#include "Timer.h"
#include "utf8.h"
#include "Stat.h"
#include "AsyncUV.h"

#ifndef _fj_FSWATCHER_H
#define _fj_FSWATCHER_H

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
        , m_proc(NULL)
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
    class AsyncWatchFSProc : public AsyncUVTask {
    public:
        AsyncWatchFSProc(FSWatcher* watcher)
            : m_watcher(watcher)
        {
            watcher->setProc(this);
        }
        ~AsyncWatchFSProc()
        {
        }

    public:
        virtual int32_t post(int32_t v);

        virtual void invoke();

        void on_watched(const char* fullfname, int events, int status)
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
                m_watcher->onError(CALL_E_INVALID_CALL, "Unknown change event type");
                return;
            }

            v[1] = fullfname;

            m_watcher->_emit("change", v, 2);
            if (only_evtType.length() > 0)
                m_watcher->_emit(only_evtType, v, 2);
        }

        static void freeSelfAfterUVHandleStop(uv_handle_t* handle)
        {
            AsyncWatchFSProc* proc = (AsyncWatchFSProc*)(handle->data);
            delete proc;
        }

    public:
        FSWatcher* m_watcher;
        uv_fs_event_t m_fs_handle;

    private:
        static void fs_event_cb(uv_fs_event_t* fs_event, const char* filename, int events, int status)
        {
            AsyncWatchFSProc* p = NULL;
            p = (AsyncWatchFSProc*)((intptr_t)fs_event - (intptr_t)&p->m_fs_handle);

            assert(fs_event == &p->m_fs_handle);

            if (!p)
                return;

            uv_handle_t* handle = (uv_handle_t*)fs_event;
            if (p->m_watcher->m_closed) {
                handle->data = (void*)p;
                uv_close(handle, AsyncWatchFSProc::freeSelfAfterUVHandleStop);
            } else {
                size_t size = 2074;
                char fullname[size + 1];

                uv_fs_event_getpath(fs_event, fullname, &size);
                fullname[size] = '\0';

                p->on_watched(fullname, events, status);
            }
        }
    };

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

        asyncCall(startWatchInNativeThread, this);
    };

    static result_t startWatchInNativeThread(FSWatcher* watcher)
    {
        (new AsyncWatchFSProc(watcher))->post(0);

        return 0;
    }

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
    void setProc(AsyncWatchFSProc* proc) { m_proc = proc; }

    exlib::atomic m_closed;

protected:
    exlib::string m_filename;

    bool m_Persistent;
    bool m_recursiveForDir;
    AsyncWatchFSProc* m_proc;
    obj_ptr<ValueHolder> m_vholder;
};
}

#endif // _fj_FSWATCHER_H
