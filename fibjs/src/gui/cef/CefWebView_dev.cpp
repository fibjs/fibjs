/*
 * CefWebView.cpp
 *
 *  Created on: Feb 28, 2021
 *      Author: lion
 */

#include "CefWebView.h"
#include "CefWebView_dev.h"
#include "EventEmitter.h"

namespace fibjs {

static void call_method(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());

    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(_data->Get(NewString(isolate, "obj")));
    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(o->Get(NewString(isolate, "executeDevToolsMethod")));

    std::vector<v8::Local<v8::Value>> _args;

    _args.resize(args.Length() + 1);
    _args[0] = _data->Get(NewString(isolate, "name"));
    for (int32_t i = 0; i < args.Length(); i++)
        _args[i + 1] = args[i];

    v8::Local<v8::Value> result;

    func->Call(func->CreationContext(), o, (int32_t)_args.size(), _args.data()).ToLocal(&result);
    args.GetReturnValue().Set(result);
}

result_t CefWebView::get_dev(v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Value> v = GetPrivate("dev");
    v8::Local<v8::Object> dev;

    if (m_domain.size() == 0) {
        v8::Local<v8::Object> o = wrap();
        v = dev = v8::Object::New(isolate->m_isolate);
        SetPrivate("dev", dev);

        int32_t dom_id = 0;
        while (s_domains[dom_id][0]) {
            exlib::string dom_name = (const char*)s_domains[dom_id][0];
            obj_ptr<EventEmitter_base> dom = new EventEmitter();
            v8::Local<v8::Object> dom_obj = dom->wrap();

            m_domain.insert(std::pair<exlib::string, obj_ptr<EventEmitter_base>>(dom_name, dom));
            dev->Set(isolate->NewString(dom_name), dom_obj);

            int32_t api_id = 0;
            const char** apis = (const char**)s_domains[dom_id][1];
            while (apis[api_id]) {
                exlib::string api_name = apis[api_id];
                v8::Local<v8::Object> _data = v8::Object::New(isolate->m_isolate);

                _data->Set(isolate->NewString("name"), isolate->NewString(dom_name + '.' + api_name));
                _data->Set(isolate->NewString("obj"), o);

                v8::Local<v8::Value> func = isolate->NewFunction(api_name.c_str(), call_method, _data);
                dom_obj->Set(isolate->NewString(api_name), func);

                api_id++;
            }

            dom_id++;
        }
    }

    retVal = v;
    return 0;
}
}
