#include "ifs/FSWatcher.h"
#include "ifs/fs.h"
#include "EventEmitter.h"
#include "EventInfo.h"
#include <uv/include/uv.h>
#include "Timer.h"
#include "utf8.h"
#include "Stat.h"

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
        : m_filename(target)
        , m_Persistent(persistent)
        , m_recursiveForDir(recursive)
        , m_closed(false)
        , m_proc(NULL)
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
    class AsyncWatchFSProc : public AsyncEvent {
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

        void on_watched(int events, int status)
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
                m_watcher->close();
                return;
            }

#ifdef Darwin
            if (m_fs_handle.realpath)
                v[1] = m_fs_handle.realpath;
            else if (m_fs_handle.path)
                v[1] = m_fs_handle.path;
#else
            v[1] = m_fs_handle.path;
#endif

            m_watcher->_emit("change", v, 2);
            if (only_evtType.length() > 0)
                m_watcher->_emit(only_evtType, v, 2);
        }

        void finished()
        {
            uv_fs_event_stop(&m_fs_handle);

            delete this;
        }

    public:
        FSWatcher* m_watcher;
        uv_fs_event_t m_fs_handle;

    private:
        static void fs_event_cb(uv_fs_event_t* handle, const char* filename, int events, int status)
        {
            AsyncWatchFSProc* p = NULL;
            p = ((AsyncWatchFSProc*)((intptr_t)handle - (intptr_t)&p->m_fs_handle));

            assert(&p->m_fs_handle == handle);
            p->on_watched(events, status);
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

        if (m_Persistent)
            isolate_ref();

        asyncCall(asyncStart, this);
    };
    // FSWatcher_base
    virtual result_t close()
    {
        if (m_closed)
            return 0;

        if (m_Persistent)
            isolate_unref();

        m_closed = true;

        _emit("close");

        if (m_proc)
            m_proc->finished();

        return 0;
    };

public:
    static result_t asyncStart(FSWatcher* watcher)
    {
        (new AsyncWatchFSProc(watcher))->post(0);

        return 0;
    }

public:
    const char* get_target() { return m_filename.c_str(); }
    bool isPersistent() { return m_Persistent; }
    bool isRecursiveForDir() { return m_recursiveForDir; }
    void setProc(AsyncWatchFSProc* proc) { m_proc = proc; }

protected:
    exlib::string m_filename;

    bool m_Persistent;
    bool m_recursiveForDir;
    bool m_closed;
    AsyncWatchFSProc* m_proc;
};
}

#endif // _fj_FSWATCHER_H
