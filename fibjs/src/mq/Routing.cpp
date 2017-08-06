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
#include "parse.h"

namespace fibjs {

result_t Routing_base::_new(v8::Local<v8::Object> map,
    obj_ptr<Routing_base>& retVal,
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
result_t Routing::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncEvent* ac)
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

    for (i = (int32_t)m_array.size() - 1; i >= 0; i--) {
        obj_ptr<rule>& r = m_array[i];

        if (htmsg && r->m_method != "*" && qstricmp(method.c_str(), r->m_method.c_str()))
            continue;

        rc = pcre_exec(r->m_re, NULL, value.c_str(), (int32_t)value.length(),
            0, 0, ovector, RE_SIZE);
        if (rc > 0) {
            obj_ptr<List> list = new List();

            if (rc > 1) {
                int32_t levelCount[RE_SIZE] = { 0 };
                int32_t level[RE_SIZE] = { 0 };
                int32_t p = 1;

                levelCount[0] = 1;

                for (i = 1; i < rc; i++) {
                    for (j = i - 1; j >= 0; j--)
                        if (ovector[i * 2] < ovector[j * 2 + 1]) {
                            level[i] = level[j] + 1;
                            break;
                        }
                    levelCount[level[i]]++;
                }

                if (levelCount[1] == 1) {
                    msg->set_value(value.substr(ovector[2], ovector[3] - ovector[2]));
                    if (levelCount[2] > 0)
                        p = 2;
                } else
                    msg->set_value("");
                int32_t toDrop;
                if (levelCount[p]) {
                    Variant vUndefined;
                    for (i = 0; i < rc; i++)
                        if (level[i] == p) {
                            if (ovector[i * 2 + 1] - ovector[i * 2] > 0)
                                list->push(value.substr(ovector[i * 2],
                                               ovector[i * 2 + 1] - ovector[i * 2]),
                                    toDrop);
                            else
                                list->push(vUndefined, toDrop);
                        }
                }
            }

            msg->set_params(list);

            retVal = r->m_hdlr;
            return 0;
        }
    }

    return CHECK_ERROR(Runtime::setError("Routing: unknown routing: " + value));
}

result_t Routing::append(exlib::string method, v8::Local<v8::Object> map)
{
    v8::Local<v8::Array> ks = map->GetPropertyNames();
    int32_t len = ks->Length();
    int32_t i;
    result_t hr;

    for (i = 0; i < len; i++) {
        v8::Local<v8::Value> k = ks->Get(i);
        v8::Local<v8::Value> v = map->Get(k);

        obj_ptr<Handler_base> hdlr = Handler_base::getInstance(v);

        if (hdlr) {
            append(method, *v8::String::Utf8Value(k), hdlr);
            continue;
        }

        hr = append(method, *v8::String::Utf8Value(k), v);
        if (hr < 0)
            return hr;
    }

    return 0;
}

exlib::string path2RegExp(exlib::string pattern)
{
    size_t len = pattern.length();

    if (pattern[len - 1] == '/')
        pattern.resize(len - 1);

    _parser p(pattern);
    exlib::string res;
    exlib::string str;
    exlib::string re, re1;
    char ch, last_ch;

    while (!p.end()) {
        p.getString(str, ":(.*\\");
        res.append(str);

        ch = p.getChar();
        if (ch == '\\') {
            res.append(1, '\\');
            res.append(1, p.getChar());
        } else if (ch == '.') {
            res.append("\\.");
        } else if (ch == '*') {
            res.append("((?:.*))");
        } else if ((ch == ':') || (ch == '(')) {
            if (res.length() > 0)
                last_ch = res[res.length() - 1];
            else
                last_ch = 0;

            if (ch == ':') {
                p.getKeyWord(str);
                re = (last_ch == '.') ? "[^\\.]+" : "[^/]+";
                re1 = re + "?";
                ch = p.get();
                if (ch == '(')
                    p.skip();
            }

            if (ch == '(') {
                p.getString(re, ')');
                re1 = re;
                if (p.get() == ')')
                    p.skip();
                ch = p.get();
            }

            if (ch == '?') {
                p.skip();
                ch = p.get();
                if ((ch == 0 || ch == '/') && last_ch == '/') {
                    res.resize(res.length() - 1);
                    re = "(?:/((?:" + re1 + ")))?";
                } else if (last_ch == '.') {
                    res.resize(res.length() - 2);
                    re = "(?:\\.((?:" + re1 + ")))?";
                } else
                    re = "((?:" + re1 + "))?";
            } else if (ch == '+') {
                p.skip();
                if (last_ch == '/')
                    re = "((?:" + re + ")(?:/(?:" + re + "))*)";
                else if (last_ch == '.')
                    re = "((?:" + re + ")(?:\\.(?:" + re + "))*)";
                else
                    re = "((?:" + re + ")((?:" + re + "))*)";
            } else if (ch == '*') {
                p.skip();
                ch = p.get();
                if ((ch == 0 || ch == '/') && last_ch == '/') {
                    res.resize(res.length() - 1);
                    re = "(?:/((?:" + re + ")(?:/(?:" + re + "))*))?";
                } else if (last_ch == '.') {
                    res.resize(res.length() - 2);
                    re = "(?:\\.((?:" + re + ")(?:\\.(?:" + re + "))*))?";
                } else if (last_ch == '/')
                    re = "(((?:" + re + ")(?:/(?:" + re + "))*))?";
                else
                    re = "(((?:" + re + ")((?:" + re + "))*))?";
            } else
                re = "((?:" + re1 + "))";

            res.append(re);
        }
    }

    res = "^" + res + "(?:/(?=$))?$";
    return res;
}

result_t Routing::append(exlib::string method, exlib::string pattern, Handler_base* hdlr)
{
    int32_t opt = PCRE_JAVASCRIPT_COMPAT | PCRE_NEWLINE_ANYCRLF | PCRE_UCP | PCRE_CASELESS;
    const char* error;
    int32_t erroffset;
    pcre* re;

    if (pattern.length() > 0 && pattern[0] != '^')
        pattern = path2RegExp(pattern);

    re = pcre_compile(pattern.c_str(), opt, &error, &erroffset, NULL);
    if (re == NULL) {
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

    for (i = len - 1; i >= 0; i--) {
        char strBuf[32];
        sprintf(strBuf, "handler_%d", no++);

        rule* r = r_obj->m_array[i];

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

result_t Routing::append(exlib::string pattern, Handler_base* hdlr)
{
    return append("*", pattern, hdlr);
}

result_t Routing::all(v8::Local<v8::Object> map)
{
    return append("*", map);
}

result_t Routing::all(exlib::string pattern, Handler_base* hdlr)
{
    return append("*", pattern, hdlr);
}

result_t Routing::get(v8::Local<v8::Object> map)
{
    return append("GET", map);
}

result_t Routing::get(exlib::string pattern, Handler_base* hdlr)
{
    return append("GET", pattern, hdlr);
}

result_t Routing::post(v8::Local<v8::Object> map)
{
    return append("POST", map);
}

result_t Routing::post(exlib::string pattern, Handler_base* hdlr)
{
    return append("POST", pattern, hdlr);
}

result_t Routing::del(v8::Local<v8::Object> map)
{
    return append("DELETE", map);
}

result_t Routing::del(exlib::string pattern, Handler_base* hdlr)
{
    return append("DELETE", pattern, hdlr);
}

result_t Routing::put(v8::Local<v8::Object> map)
{
    return append("PUT", map);
}

result_t Routing::put(exlib::string pattern, Handler_base* hdlr)
{
    return append("PUT", pattern, hdlr);
}

result_t Routing::patch(v8::Local<v8::Object> map)
{
    return append("PATCH", map);
}

result_t Routing::patch(exlib::string pattern, Handler_base* hdlr)
{
    return append("PATCH", pattern, hdlr);
}

} /* namespace fibjs */
