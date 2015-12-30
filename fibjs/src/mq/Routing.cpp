/*
 * Routing.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "Routing.h"
#include "JSHandler.h"
#include "ifs/Message.h"
#include "List.h"

namespace fibjs
{

result_t Routing_base::_new(v8::Local<v8::Object> map,
                            obj_ptr<Routing_base> &retVal,
                            v8::Local<v8::Object> This)
{
    obj_ptr<Routing_base> r = new Routing();
    r->wrap(This);

    result_t hr = r->append(map);
    if (hr < 0)
        return hr;

    retVal = r;
    return 0;
}

#define RE_SIZE 64
result_t Routing::invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                         AsyncEvent *ac)
{
    int32_t i, j;
    int32_t rc = 0;
    obj_ptr<Message_base> msg = Message_base::getInstance(v);
    int32_t ovector[RE_SIZE];

    if (msg == NULL)
        return CHECK_ERROR(CALL_E_BADVARTYPE);

    std::string value;

    msg->get_value(value);

    for (i = (int32_t) m_array.size() - 1; i >= 0; i--)
    {
        obj_ptr<rule> &r = m_array[i];

        if ((rc = pcre_exec(r->m_re, r->m_extra, value.c_str(),
                            (int32_t) value.length(), 0, 0, ovector, RE_SIZE)) > 0)
        {
            obj_ptr<List> list = new List();

            if (rc == 1)
                msg->set_value("");
            else
            {
                int32_t levelCount[RE_SIZE] =
                { 0 };
                int32_t level[RE_SIZE] =
                { 0 };
                int32_t p = 1;

                levelCount[0] = 1;

                for (i = 1; i < rc; i++)
                {
                    for (j = i - 1; j >= 0; j--)
                        if (ovector[i * 2] < ovector[j * 2 + 1])
                        {
                            level[i] = level[j] + 1;
                            break;
                        }
                    levelCount[level[i]]++;
                }

                if (levelCount[1] == 1)
                {
                    msg->set_value(
                        value.substr(ovector[2], ovector[3] - ovector[2]).c_str());

                    if (levelCount[2] > 0)
                        p = 2;
                }
                else
                    msg->set_value("");

                if (levelCount[p])
                {
                    for (i = 0; i < rc; i++)
                        if (level[i] == p)
                            list->push(
                                value.substr(ovector[i * 2],
                                             ovector[i * 2 + 1]
                                             - ovector[i * 2]));
                }
            }

            msg->set_params(list);

            retVal = r->m_hdlr;
            return 0;
        }
    }

    return CHECK_ERROR(Runtime::setError("Routing: unknown routing: " + value));
}

result_t Routing::append(const char *pattern, Handler_base *hdlr)
{
    int32_t opt = PCRE_JAVASCRIPT_COMPAT | PCRE_EXTRA | PCRE_NEWLINE_ANYCRLF
                  | PCRE_UCP | PCRE_CASELESS;
    const char *error;
    int32_t erroffset;
    pcre *re;
    pcre_extra *extra;

    re = pcre_compile(pattern, opt, &error, &erroffset, NULL);
    if (re == NULL)
    {
        char buf[1024];

        sprintf(buf, "Routing: Compilation failed at offset %d: %s.", erroffset, error);
        return CHECK_ERROR(Runtime::setError(buf));
    }

    extra = pcre_study(re, PCRE_STUDY_JIT_COMPILE, &error);
    if (extra == NULL)
    {
        pcre_free(re);
        return CHECK_ERROR(Runtime::setError(error));
    }

    Isolate* isolate = holder();
    v8::Local<v8::String> k = v8::String::NewFromUtf8(isolate->m_isolate, "handler");
    v8::Local<v8::Value> v = wrap()->GetHiddenValue(k);
    v8::Local<v8::Array> a;

    if (IsEmpty(v))
    {
        a = v8::Array::New(isolate->m_isolate);
        wrap()->SetHiddenValue(k, a);
    }
    else
        a = v8::Local<v8::Array>::Cast(v);

    a->Set((int32_t)m_array.size(), hdlr->wrap());

    obj_ptr<rule> r = new rule(re, extra, hdlr);
    m_array.append(r);

    return 0;
}

result_t Routing::append(const char *pattern, v8::Local<v8::Value> hdlr)
{
    obj_ptr<Handler_base> hdlr1;
    result_t hr = JSHandler::New(hdlr, hdlr1);
    if (hr < 0)
        return hr;
    return append(pattern, hdlr1);
}

result_t Routing::append(v8::Local<v8::Object> map)
{
    v8::Local<v8::Array> ks = map->GetPropertyNames();
    int32_t len = ks->Length();
    int32_t i;
    result_t hr;

    for (i = len - 1; i >= 0; i--)
    {
        v8::Local<v8::Value> k = ks->Get(i);
        v8::Local<v8::Value> v = map->Get(k);

        obj_ptr<Handler_base> hdlr = Handler_base::getInstance(v);

        if (hdlr)
        {
            append(*v8::String::Utf8Value(k), hdlr);
            continue;
        }

        hr = append(*v8::String::Utf8Value(k), v);
        if (hr < 0)
            return hr;
    }

    return 0;
}

} /* namespace fibjs */
