/*
 * process_env.cpp
 *
 *  Created on: Jan 24, 2023
 *      Author: lion
 */

#include "object.h"
#include "ifs/process.h"
#include <uv/include/uv.h>
#include "unicode/locid.h"
#include "unicode/timezone.h"

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

static void SetEnv(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    Isolate* isolate = Isolate::current(info);

    if (!isolate->m_env.IsEmpty()) {
        exlib::string key = isolate->toString(property);
        exlib::string val = isolate->toString(value);

        uv_os_setenv(key.c_str(), val.c_str());
        on_env_update(isolate, key, val);
    }
}

static void DelEnv(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& info)
{
    Isolate* isolate = Isolate::current(info);

    exlib::string key = isolate->toString(property);

    uv_os_unsetenv(key.c_str());
    on_env_update(isolate, key, "");
}

static void EnumEnv(const v8::PropertyCallbackInfo<v8::Array>& info)
{
    Isolate* isolate = Isolate::current(info);
    v8::Local<v8::Context> context = isolate->context();
    char** env = environ;
    const char *p, *p1;

    v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate);
    int32_t idx = 0;
    while ((p = *env++) != NULL) {
        p1 = qstrchr(p, '=');
        if (p1)
            arr->Set(context, idx++, isolate->NewString(p, (int32_t)(p1 - p))).IsJust();
    }

    info.GetReturnValue().Set(arr);
}

static void GetEnv(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    Isolate* isolate = Isolate::current(info);
    exlib::string key = isolate->toString(property);

    char buf[4096];
    size_t sz = sizeof(buf);
    if (uv_os_getenv(key.c_str(), buf, &sz) == 0)
        info.GetReturnValue().Set(isolate->NewString(buf, sz));
}

result_t process_base::get_env(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    if (isolate->m_env.IsEmpty()) {
        v8::Local<v8::FunctionTemplate> templ = v8::FunctionTemplate::New(isolate->m_isolate);
        templ->InstanceTemplate()->SetHandler(v8::NamedPropertyHandlerConfiguration(GetEnv, SetEnv, nullptr, DelEnv, EnumEnv));
        v8::Local<v8::Object> o = templ->GetFunction(context).ToLocalChecked()->NewInstance(context).ToLocalChecked();
        isolate->m_env.Reset(isolate->m_isolate, o);
        retVal = o;
    } else
        retVal = isolate->m_env.Get(isolate->m_isolate);

    return 0;
}

}
