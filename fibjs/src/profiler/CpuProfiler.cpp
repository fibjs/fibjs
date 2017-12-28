/*
 * CpuProfiler.cpp
 *
 *  Created on: Oct 11, 2017
 *      Author: lion
 */

#include "object.h"
#include "ifs/profiler.h"
#include "ifs/timers.h"
#include "ifs/fs.h"
#include "ifs/coroutine.h"
#include "ifs/json.h"
#include "Buffer.h"

namespace fibjs {

static void cpu_profiler(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();

    v8::Local<v8::Array> fibers;
    coroutine_base::get_fibers(fibers);
    int32_t i, cnt = 0, len = fibers->Length();

    v8::Local<v8::Array> stacks = v8::Array::New(isolate);
    for (i = 0; i < len; i++) {
        exlib::string stack;
        obj_ptr<Fiber_base> fb = Fiber_base::getInstance(fibers->Get(i));

        fb->get_stack(stack);
        if (stack != "")
            stacks->Set(cnt++, NewString(isolate, stack));
    }

    v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());
    if (cnt > 0) {
        exlib::string str;
        json_base::encode(stacks, str);

        obj_ptr<Buffer_base> buf = new Buffer(str + '\n');
        SeekableStream_base::getInstance(_data)->cc_write(buf);
    }

    date_t d;
    d.now();
    if (d.date() > _data->Get(NewString(isolate, "_time"))->NumberValue())
        Timer_base::getInstance(args.This())->clear();
}

result_t profiler_base::start(exlib::string fname, int32_t time, int32_t interval, obj_ptr<Timer_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    obj_ptr<SeekableStream_base> f;
    OptArgs args;
    result_t hr;

    hr = fs_base::ac_openFile(fname, "a", f);
    if (hr < 0)
        return hr;

    v8::Local<v8::Object> _data = f->wrap();

    if (time > 0) {
        date_t d;
        d.now();
        d.add(time, date_t::_MICROSECOND);
        _data->Set(isolate->NewString("_time"), d.value(isolate->m_isolate));
    } else
        _data->Set(isolate->NewString("_time"), v8::Number::New(isolate->m_isolate, INFINITY));

    obj_ptr<Timer_base> t;
    timers_base::setHrInterval(isolate->NewFunction("_cpu_profiler", cpu_profiler, _data),
        interval, args, t);

    return t->unref(retVal);
}
}