/*
 * Routing.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "object.h"
#include "Routing.h"
#include "JSHandler.h"
#include "ifs/Message.h"
#include "ifs/HttpRequest.h"
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

result_t Routing_base::_new(exlib::string method, v8::Local<v8::Object> map,
                            obj_ptr<Routing_base>& retVal,
                            v8::Local<v8::Object> This)
{
    obj_ptr<Routing_base> r = new Routing();
    r->wrap(This);

    result_t hr = r->append(method, map);
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

    exlib::string value;
    exlib::string method;

    msg->get_value(value);

    obj_ptr<HttpRequest_base> htmsg = HttpRequest_base::getInstance(msg);
    if (htmsg)
        htmsg->get_method(method);

    for (i = (int32_t) m_array.size() - 1; i >= 0; i--)
    {
        obj_ptr<rule> &r = m_array[i];

        if (htmsg && r->m_method != "*" &&
                qstricmp(method.c_str(), r->m_method.c_str()))
            continue;

        if ((rc = pcre_exec(r->m_re, NULL, value.c_str(),
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

result_t Routing::append(exlib::string method, exlib::string pattern, Handler_base *hdlr)
{
    int32_t opt = PCRE_JAVASCRIPT_COMPAT | PCRE_NEWLINE_ANYCRLF |
                  PCRE_UCP | PCRE_CASELESS;
    const char *error;
    int32_t erroffset;
    pcre *re;

    re = pcre_compile(pattern.c_str(), opt, &error, &erroffset, NULL);
    if (re == NULL)
    {
        char buf[1024];

        sprintf(buf, "Routing: Compilation failed at offset %d: %s.", erroffset, error);
        return CHECK_ERROR(Runtime::setError(buf));
    }

    int32_t no = (int32_t)m_array.size();

    char strBuf[32];
    sprintf(strBuf, "handler_%d", no);

    SetPrivate(strBuf, hdlr->wrap());

    obj_ptr<rule> r = new rule(method, re, hdlr);
    m_array.insert(m_array.begin(), r);

    return 0;
}

result_t Routing::append(exlib::string method, v8::Local<v8::Object> map)
{
    v8::Local<v8::Array> ks = map->GetPropertyNames();
    int32_t len = ks->Length();
    int32_t i;
    result_t hr;

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> k = ks->Get(i);
        v8::Local<v8::Value> v = map->Get(k);

        obj_ptr<Handler_base> hdlr = Handler_base::getInstance(v);

        if (hdlr)
        {
            append(method, *v8::String::Utf8Value(k), hdlr);
            continue;
        }

        hr = append(method, *v8::String::Utf8Value(k), v);
        if (hr < 0)
            return hr;
    }

    return 0;
}

result_t Routing::append(exlib::string method, exlib::string pattern, v8::Local<v8::Value> hdlr)
{
    obj_ptr<Handler_base> hdlr1;
    result_t hr = JSHandler::New(hdlr, hdlr1);
    if (hr < 0)
        return hr;
    return append(method, pattern, hdlr1);
}

result_t Routing::append(Routing_base* route)
{
    Routing* r_obj = (Routing*)route;

    int32_t i, len = (int32_t)r_obj->m_array.size();
    int32_t no = (int32_t)m_array.size();

    for (i = len - 1; i >= 0; i --)
    {
        char strBuf[32];
        sprintf(strBuf, "handler_%d", no ++);

        rule *r = r_obj->m_array[i];

        SetPrivate(strBuf, r->m_hdlr->wrap());
        m_array.insert(m_array.begin(), r);
    }

    r_obj->m_array.resize(0);

    return 0;
}

result_t Routing::append(v8::Local<v8::Object> map)
{
    return append("*", map);
}

result_t Routing::append(exlib::string pattern, v8::Local<v8::Value> hdlr)
{
    return append("*", pattern, hdlr);
}

result_t Routing::all(v8::Local<v8::Object> map)
{
    return append("*", map);
}

result_t Routing::all(exlib::string pattern, v8::Local<v8::Value> hdlr)
{
    return append("*", pattern, hdlr);
}

result_t Routing::get(v8::Local<v8::Object> map)
{
    return append("GET", map);
}

result_t Routing::get(exlib::string pattern, v8::Local<v8::Value> hdlr)
{
    return append("GET", pattern, hdlr);
}

result_t Routing::post(v8::Local<v8::Object> map)
{
    return append("POST", map);
}

result_t Routing::post(exlib::string pattern, v8::Local<v8::Value> hdlr)
{
    return append("POST", pattern, hdlr);
}

result_t Routing::del(v8::Local<v8::Object> map)
{
    return append("DELETE", map);
}

result_t Routing::del(exlib::string pattern, v8::Local<v8::Value> hdlr)
{
    return append("DELETE", pattern, hdlr);
}

result_t Routing::put(v8::Local<v8::Object> map)
{
    return append("PUT", map);
}

result_t Routing::put(exlib::string pattern, v8::Local<v8::Value> hdlr)
{
    return append("PUT", pattern, hdlr);
}

result_t Routing::patch(v8::Local<v8::Object> map)
{
    return append("PATCH", map);
}

result_t Routing::patch(exlib::string pattern, v8::Local<v8::Value> hdlr)
{
    return append("PATCH", pattern, hdlr);
}

} /* namespace fibjs */
