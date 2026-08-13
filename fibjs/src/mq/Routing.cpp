/*
 * Routing.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "object.h"
#include "Routing.h"
#include "ifs/Message.h"
#include "ifs/HttpRequest.h"
#include "parse.h"
#include "Url.h"

namespace fibjs {

static void load_route_group_names(pcre2_code* re, Routing::route_info& info)
{
    uint32_t name_count = 0;
    uint32_t entry_size = 0;
    PCRE2_SPTR name_table = NULL;

    if (pcre2_pattern_info(re, PCRE2_INFO_NAMECOUNT, &name_count) != 0 || name_count == 0)
        return;

    if (pcre2_pattern_info(re, PCRE2_INFO_NAMEENTRYSIZE, &entry_size) != 0 || entry_size == 0)
        return;

    if (pcre2_pattern_info(re, PCRE2_INFO_NAMETABLE, &name_table) != 0 || name_table == NULL)
        return;

    for (uint32_t i = 0; i < name_count; i++) {
        PCRE2_SPTR entry = name_table + i * entry_size;
        int32_t group_no = (entry[0] << 8) | entry[1];
        exlib::string name((const char*)(entry + 2));

        info.group_names[group_no] = name;
    }
}

static exlib::string add_named_capture(const exlib::string& re, const exlib::string& name)
{
    size_t len = re.length();

    for (size_t i = 0; i < len; i++) {
        if (re[i] != '(')
            continue;

        if (i > 0 && re[i - 1] == '\\')
            continue;

        if (i + 1 < len && re[i + 1] == '?')
            continue;

        return re.substr(0, i + 1) + "?<" + name + ">" + re.substr(i + 1);
    }

    return re;
}

static void clear_named_params(NArray* list)
{
    std::vector<exlib::string> keys;

    keys.reserve(list->m_keys.size());
    for (auto& entry : list->m_keys)
        keys.push_back(entry.first);

    for (auto& key : keys) {
        list->remove(key);
    }
}

static void set_named_param(NArray* list, const exlib::string& name, Variant value)
{
    list->add(name, value);
}

result_t Routing_base::_new(v8::Local<v8::Object> map,
    obj_ptr<Routing_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<Routing> r = new Routing();
    r->wrap(This);

    return r->append(map, retVal);
}

result_t Routing_base::_new(exlib::string method, v8::Local<v8::Object> map,
    obj_ptr<Routing_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<Routing> r = new Routing();
    r->wrap(This);

    return r->_append(method, map, retVal);
}

result_t Routing::isRouting(bool& retVal)
{
    retVal = true;
    return 0;
}

#define RE_SIZE 64
result_t Routing::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncEvent* ac)
{
    int32_t i, j;
    int32_t rc = 0;
    Isolate* isolate = holder();
    obj_ptr<Message_base> msg = Message_base::getInstance(v);
    pcre2_match_data* match_data = pcre2_match_data_create(RE_SIZE, NULL);

    if (msg == NULL) {
        pcre2_match_data_free(match_data);
        return CHECK_ERROR(CALL_E_BADVARTYPE);
    }

    exlib::string value;
    exlib::string method;
    exlib::string host;

    msg->get_value(value);

    obj_ptr<HttpRequest_base> htmsg = HttpRequest_base::getInstance(msg);
    if (htmsg)
        htmsg->get_method(method);

    for (i = (int32_t)m_array.size() - 1; i >= 0; i--) {
        obj_ptr<rule>& r = m_array[i];
        exlib::string& test = value;
        bool isHost = false;

        if (htmsg) {
            if (!qstricmp(r->m_method.c_str(), "HOST")) {
                if (host.empty()) {
                    htmsg->firstHeader("host", host);
                    if (host.empty())
                        host = "*";
                    else {
                        size_t pos = host.find(':');
                        if (pos != exlib::string::npos)
                            host = host.substr(0, pos);
                    }
                }

                test = host;
                isHost = true;
            } else {
                if (r->m_method != "*" && qstricmp(method.c_str(), r->m_method.c_str()))
                    continue;
            }
        }

        rc = pcre2_match(r->m_re, (PCRE2_SPTR)test.c_str(), test.length(),
            0, 0, match_data, NULL);
        if (rc > 0) {
            PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);
            obj_ptr<NArray> list;

            msg->get_params(list);
            list->resize(0);
            clear_named_params(list);

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

                if (r->m_bSub) {
                    i = rc - 1;
                    if (!isHost)
                        msg->set_value(test.substr(ovector[i * 2], ovector[i * 2 + 1] - ovector[i * 2]));
                } else {
                    if (levelCount[1] == 1) {
                        if (!isHost)
                            msg->set_value(test.substr(ovector[2], ovector[3] - ovector[2]));
                        if (levelCount[2] > 0)
                            p = 2;
                    } else if (!isHost)
                        msg->set_value("");

                    if (levelCount[p]) {
                        Variant vUndefined;
                        for (i = 0; i < rc; i++)
                            if (level[i] == p) {
                                const exlib::string* name = NULL;
                                std::unordered_map<int32_t, exlib::string>::const_iterator name_index = r->m_info.group_names.find(i);

                                if (name_index != r->m_info.group_names.end())
                                    name = &name_index->second;

                                if (ovector[i * 2 + 1] - ovector[i * 2] > 0) {
                                    exlib::string param_value;
                                    Url::decodeURI(test.substr(ovector[i * 2], ovector[i * 2 + 1] - ovector[i * 2]), param_value);
                                    list->append(param_value);
                                    if (name)
                                        set_named_param(list, *name, param_value);
                                } else {
                                    list->append(vUndefined);
                                    if (name)
                                        set_named_param(list, *name, vUndefined);
                                }
                            }
                    }
                }
            }

            pcre2_match_data_free(match_data);
            retVal = r->m_hdlr;
            return 0;
        }
    }

    pcre2_match_data_free(match_data);
    return CHECK_ERROR(Runtime::setError("Routing: unknown routing: " + value));
}

result_t Routing::_append(exlib::string method, v8::Local<v8::Object> map,
    obj_ptr<Routing_base>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    JSArray ks = map->GetPropertyNames(context);
    int32_t len = ks->Length();
    int32_t i;
    result_t hr;

    for (i = 0; i < len; i++) {
        JSValue k = ks->Get(context, i);
        v8::Local<v8::Value> v = map->Get(context, k).FromMaybe(v8::Local<v8::Value>());
        if (v.IsEmpty())
            return CALL_E_JAVASCRIPT;

        obj_ptr<Handler_base> hdlr;
        obj_ptr<Routing_base> r;

        hr = GetArgumentValue(isolate, v, hdlr);
        if (hr < 0)
            return hr;

        append(method, isolate->toString(k), hdlr, r);
    }

    retVal = this;

    return 0;
}

exlib::string Routing::path2RegExp(exlib::string pattern, route_info* info)
{
    size_t len = pattern.length();

    if (len > 0 && pattern[len - 1] == '/')
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

            bool hasName = false;
            bool allowNamedCapture = false;

            if (ch == ':') {
                if (p.getKeyWord(str) > 0) {
                    hasName = true;
                    if (info)
                        allowNamedCapture = info->named_groups.insert(str).second;
                    else
                        allowNamedCapture = true;
                } else {
                    res.append(1, ':');
                    continue;
                }

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

            if (info && hasName && allowNamedCapture)
                res.append(add_named_capture(re, str));
            else
                res.append(re);
        }
    }

    res = "^" + res + "(?:/(?=$))?$";
    return res;
}

exlib::string Routing::host2RegExp(exlib::string pattern, route_info* info)
{
    size_t len = pattern.length();

    if (len > 0 && pattern[len - 1] == '/')
        pattern.resize(len - 1);

    _parser p(pattern);
    exlib::string res;
    exlib::string str;
    exlib::string re, re1;
    char ch;

    while (!p.end()) {
        ch = p.getChar();
        if (ch == '\\') {
            res.append(1, '\\');
            res.append(1, p.getChar());
        } else if (ch == '.') {
            res.append("\\.");
        } else if (ch == '*') {
            res.append("((?:[^\\.]+))");
        } else
            res.append(1, ch);
    }

    res = "^" + res + "(?:/(?=$))?$";
    return res;
}

result_t Routing::append(exlib::string method, exlib::string pattern, Handler_base* hdlr,
    obj_ptr<Routing_base>& retVal)
{
    uint32_t opt = PCRE2_UCP | PCRE2_CASELESS;
    int errcode;
    PCRE2_SIZE erroffset;
    pcre2_code* re;
    bool bSub = false;
    route_info info;

    if (pattern.length() > 0 && pattern[0] != '^') {
        if (!qstricmp(method.c_str(), "HOST"))
            pattern = host2RegExp(pattern, &info);
        else {
            bool isRoute = false;
            hdlr->isRouting(isRoute);
            if (isRoute) {
                int32_t len = (int32_t)pattern.length();
                if (len > 0) {
                    if (pattern == "*")
                        pattern = "";
                    else if (pattern[len - 1] == '/')
                        pattern.resize(len - 1);
                }
                pattern += "(.*)";
                bSub = true;
            }

            pattern = path2RegExp(pattern, &info);
        }
    }

    re = pcre2_compile((PCRE2_SPTR)pattern.c_str(), PCRE2_ZERO_TERMINATED, opt, &errcode, &erroffset, NULL);
    if (re == NULL) {
        char buf[1024];
        PCRE2_UCHAR errbuf[256];
        pcre2_get_error_message(errcode, errbuf, sizeof(errbuf));

        snprintf(buf, sizeof(buf), "Routing: Compilation failed at offset %d: %s.", (int)erroffset, errbuf);
        return CHECK_ERROR(Runtime::setError(buf));
    }

    load_route_group_names(re, info);

    int32_t no = (int32_t)m_array.size();

    char strBuf[32];
    snprintf(strBuf, sizeof(strBuf), "handler_%d", no);

    SetPrivate(strBuf, hdlr->wrap());

    obj_ptr<rule> r = new rule(method, re, hdlr, bSub, info);
    m_array.insert(m_array.begin(), r);

    retVal = this;

    return 0;
}

result_t Routing::append(Routing_base* route, obj_ptr<Routing_base>& retVal)
{
    Routing* r_obj = (Routing*)route;

    int32_t i, len = (int32_t)r_obj->m_array.size();
    int32_t no = (int32_t)m_array.size();

    for (i = len - 1; i >= 0; i--) {
        char strBuf[32];
        snprintf(strBuf, sizeof(strBuf), "handler_%d", no++);

        rule* r = r_obj->m_array[i];

        SetPrivate(strBuf, r->m_hdlr->wrap());
        m_array.insert(m_array.begin(), r);
    }

    r_obj->m_array.resize(0);

    retVal = this;

    return 0;
}

result_t Routing::append(v8::Local<v8::Object> map, obj_ptr<Routing_base>& retVal)
{
    return _append("*", map, retVal);
}

result_t Routing::append(exlib::string pattern, Handler_base* hdlr,
    obj_ptr<Routing_base>& retVal)
{
    return append("*", pattern, hdlr, retVal);
}

result_t Routing::_host(v8::Local<v8::Object> map, obj_ptr<Routing_base>& retVal)
{
    return _append("HOST", map, retVal);
}

result_t Routing::_host(exlib::string pattern, Handler_base* hdlr, obj_ptr<Routing_base>& retVal)
{
    return append("HOST", pattern, hdlr, retVal);
}

result_t Routing::all(v8::Local<v8::Object> map, obj_ptr<Routing_base>& retVal)
{
    return _append("*", map, retVal);
}

result_t Routing::all(exlib::string pattern, Handler_base* hdlr,
    obj_ptr<Routing_base>& retVal)
{
    return append("*", pattern, hdlr, retVal);
}

result_t Routing::get(v8::Local<v8::Object> map, obj_ptr<Routing_base>& retVal)
{
    return _append("GET", map, retVal);
}

result_t Routing::get(exlib::string pattern, Handler_base* hdlr,
    obj_ptr<Routing_base>& retVal)
{
    return append("GET", pattern, hdlr, retVal);
}

result_t Routing::post(v8::Local<v8::Object> map, obj_ptr<Routing_base>& retVal)
{
    return _append("POST", map, retVal);
}

result_t Routing::post(exlib::string pattern, Handler_base* hdlr, obj_ptr<Routing_base>& retVal)
{
    return append("POST", pattern, hdlr, retVal);
}

result_t Routing::del(v8::Local<v8::Object> map, obj_ptr<Routing_base>& retVal)
{
    return _append("DELETE", map, retVal);
}

result_t Routing::del(exlib::string pattern, Handler_base* hdlr,
    obj_ptr<Routing_base>& retVal)
{
    return append("DELETE", pattern, hdlr, retVal);
}

result_t Routing::put(v8::Local<v8::Object> map, obj_ptr<Routing_base>& retVal)
{
    return _append("PUT", map, retVal);
}

result_t Routing::put(exlib::string pattern, Handler_base* hdlr,
    obj_ptr<Routing_base>& retVal)
{
    return append("PUT", pattern, hdlr, retVal);
}

result_t Routing::patch(v8::Local<v8::Object> map, obj_ptr<Routing_base>& retVal)
{
    return _append("PATCH", map, retVal);
}

result_t Routing::patch(exlib::string pattern, Handler_base* hdlr,
    obj_ptr<Routing_base>& retVal)
{
    return append("PATCH", pattern, hdlr, retVal);
}

result_t Routing::find(v8::Local<v8::Object> map, obj_ptr<Routing_base>& retVal)
{
    return _append("FIND", map, retVal);
}

result_t Routing::find(exlib::string pattern, Handler_base* hdlr,
    obj_ptr<Routing_base>& retVal)
{
    return append("FIND", pattern, hdlr, retVal);
}

} /* namespace fibjs */
