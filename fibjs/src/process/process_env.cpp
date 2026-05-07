/*
 * process_env.cpp
 *
 *  Created on: Jan 24, 2023
 *      Author: lion
 */

#include "object.h"
#include "ifs/process.h"
#include "ifs/fs.h"
#include "file_path.h"
#include <uv/include/uv.h>
#include "unicode/locid.h"
#include "unicode/timezone.h"
#include "../util/dotenv_parser.h"

extern "C" char** environ;

namespace fibjs {

inline void on_env_update(Isolate* isolate, exlib::string key, exlib::string val)
{
    if (key == "TZ") {
        icu::TimeZone* zone = icu::TimeZone::createTimeZone(val.c_str());
        icu::TimeZone::setDefault(*zone);

        isolate->m_isolate->DateTimeConfigurationChangeNotification(v8::Isolate::TimeZoneDetection::kRedetect);
    } else if (key == "LANG") {
        icu::Locale locale(val.c_str());
        UErrorCode error_code = U_ZERO_ERROR;
        icu::Locale::setDefault(locale, error_code);

        isolate->m_isolate->LocaleConfigurationChangeNotification();
    }
}

static v8::Intercepted SetEnv(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    Isolate* isolate = Isolate::current(info);
    exlib::string key = isolate->toString(property);

    if (value->IsUndefined()) {
        uv_os_unsetenv(key.c_str());
        on_env_update(isolate, key, "");
    } else {
        exlib::string val = isolate->toString(value);
        uv_os_setenv(key.c_str(), val.c_str());
        on_env_update(isolate, key, val);
    }

    return v8::Intercepted::kYes;
}

static v8::Intercepted DelEnv(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& info)
{
    Isolate* isolate = Isolate::current(info);
    exlib::string key = isolate->toString(property);

    uv_os_unsetenv(key.c_str());
    on_env_update(isolate, key, "");

    return v8::Intercepted::kYes;
}

static void EnumEnv(const v8::PropertyCallbackInfo<v8::Array>& info)
{
    Isolate* isolate = Isolate::current(info);
    v8::Local<v8::Context> context = isolate->context();

    uv_env_item_t* env_items;
    int env_count;
    int r = uv_os_environ(&env_items, &env_count);

    v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate);
    int32_t idx = 0;

    for (int i = 0; i < env_count; ++i) {
        uv_env_item_t* item = &env_items[i];
        arr->Set(context, idx++, isolate->NewString(item->name)).IsJust();
    }

    info.GetReturnValue().Set(arr);
}

static v8::Intercepted GetEnv(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    Isolate* isolate = Isolate::current(info);
    exlib::string key = isolate->toString(property);

    char buf[4096];
    size_t sz = sizeof(buf);
    if (uv_os_getenv(key.c_str(), buf, &sz) == 0) {
        info.GetReturnValue().Set(isolate->NewString(buf, sz));
        return v8::Intercepted::kYes;
    }

    return v8::Intercepted::kNo;
}

v8::Intercepted QueryEnv(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Integer>& info)
{
    Isolate* isolate = Isolate::current(info);
    exlib::string key = isolate->toString(property);

    char buf[4096];
    size_t sz = sizeof(buf);
    if (uv_os_getenv(key.c_str(), buf, &sz) == 0)
        return v8::Intercepted::kYes;

    return v8::Intercepted::kNo;
}

result_t process_base::get_env(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    if (isolate->m_env.IsEmpty()) {
        v8::Local<v8::FunctionTemplate> templ = v8::FunctionTemplate::New(isolate->m_isolate);
        templ->InstanceTemplate()->SetHandler(v8::NamedPropertyHandlerConfiguration(GetEnv, SetEnv, QueryEnv, DelEnv, EnumEnv));
        v8::Local<v8::Object> o = templ->GetFunction(context).ToLocalChecked()->NewInstance(context).ToLocalChecked();
        isolate->m_env.Reset(isolate->m_isolate, o);
        retVal = o;
    } else
        retVal = isolate->m_env.Get(isolate->m_isolate);

    return 0;
}

result_t process_base::loadEnvFile(exlib::string path)
{
    if (path.empty())
        path = ".env";

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    exlib::string content;
    hr = fs_base::ac_readTextFile(path, content);
    if (hr < 0)
        return hr;

    dotenv_parser::store_t store;
    dotenv_parser::parse_content(std::string_view(content.c_str(), content.length()), store);

    Isolate* isolate = Isolate::current();
    for (const auto& entry : store) {
        char exists_buf[1];
        size_t exists_sz = sizeof(exists_buf);
        int32_t uv_ret = uv_os_getenv(entry.first.c_str(), exists_buf, &exists_sz);
        if (uv_ret == 0 || uv_ret == UV_ENOBUFS)
            continue;

        uv_os_setenv(entry.first.c_str(), entry.second.c_str());
        on_env_update(isolate,
            exlib::string(entry.first.c_str(), entry.first.length()),
            exlib::string(entry.second.c_str(), entry.second.length()));
    }

    return 0;
}

}
