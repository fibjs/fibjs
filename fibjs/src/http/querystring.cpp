/*
 * querystring.cpp
 *
 *  Created on: Feb 20, 2017
 *      Author: lion
 */

#include "object.h"
#include "ifs/querystring.h"
#include "ifs/encoding.h"
#include "HttpCollection.h"
#include "StringBuffer.h"

namespace fibjs {

DECLARE_MODULE(querystring);

result_t querystring_base::escape(exlib::string str, exlib::string& retVal)
{
    return encoding_base::encodeURIComponent(str, true, retVal);
}

result_t querystring_base::unescape(exlib::string str, exlib::string& retVal)
{
    return encoding_base::decodeURI(str, retVal);
}

result_t querystring_base::parse(exlib::string str, exlib::string sep,
    exlib::string eq, v8::Local<v8::Object> opt,
    obj_ptr<HttpCollection_base>& retVal)
{
    result_t hr;

    // maxKeys caps the number of pairs parsed; Node uses 1000 as the default and
    // treats 0 (like every non-positive value) as "no limit" (Node: `Infinity`).
    int32_t max_keys = 1000;
    hr = GetConfigValue(opt, "maxKeys", max_keys, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;
    if (max_keys < 0)
        max_keys = 0;

    obj_ptr<HttpCollection> c = new HttpCollection();
    hr = c->parse(str, sep.c_str(), eq.c_str(), max_keys);
    if (hr.failed())
        return hr;

    retVal = c;

    return result_t();
}

result_t querystring_base::stringify(ValueMap& obj, exlib::string sep,
    exlib::string eq, v8::Local<v8::Object> opt,
    exlib::string& retVal)
{
    StringBuffer bufs;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    for (auto& item : obj) {
        exlib::string strKey = item.first, strValue;
        v8::Local<v8::Value> v = item.second;

        encoding_base::encodeURIComponent(strKey, true, strKey);

        if (v->IsArray()) {
            v8::Local<v8::Array> vs = v.As<v8::Array>();
            int32_t len = vs->Length();

            for (int32_t i = 0; i < len; i++) {
                // node renders every value through its string form
                strValue = isolate->toString(JSValue(vs->Get(context, i)));
                encoding_base::encodeURIComponent(strValue, true, strValue);

                if (bufs.size() > 0)
                    bufs.append(sep);

                bufs.append(strKey);
                bufs.append(eq);
                bufs.append(strValue);
            }
        } else {
            strValue = isolate->toString(v);
            encoding_base::encodeURIComponent(strValue, true, strValue);

            if (bufs.size() > 0)
                bufs.append(sep);

            bufs.append(strKey);
            bufs.append(eq);
            bufs.append(strValue);
        }
    }

    retVal = bufs.str();

    return result_t();
}
}