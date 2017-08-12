/*
 * Routing.h
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "ifs/Routing.h"
#include <pcre/pcre.h>
#include <vector>

#ifndef ROUTING_H_
#define ROUTING_H_

namespace fibjs {

class Routing : public Routing_base {
public:
    class rule : public obj_base {
    public:
        rule(exlib::string method, pcre* re, Handler_base* hdlr)
            : m_method(method)
            , m_re(re)
            , m_hdlr(hdlr)
        {
        }

        ~rule()
        {
            pcre_free(m_re);
        }

    public:
        exlib::string m_method;
        pcre* m_re;
        obj_ptr<Handler_base> m_hdlr;
    };

public:
    // object_base
    virtual result_t dispose()
    {
        return CHECK_ERROR(CALL_E_INVALID_CALL);
    }

public:
    // Handler_base
    virtual result_t invoke(object_base* v,
        obj_ptr<Handler_base>& retVal, AsyncEvent* ac);

public:
    // Routing_base
    virtual result_t append(Routing_base* route);
    virtual result_t append(v8::Local<v8::Object> map);
    virtual result_t append(exlib::string pattern, Handler_base* hdlr);
    virtual result_t append(exlib::string method, v8::Local<v8::Object> map);
    virtual result_t append(exlib::string method, exlib::string pattern, Handler_base* hdlr);
    virtual result_t all(v8::Local<v8::Object> map);
    virtual result_t all(exlib::string pattern, Handler_base* hdlr);
    virtual result_t get(v8::Local<v8::Object> map);
    virtual result_t get(exlib::string pattern, Handler_base* hdlr);
    virtual result_t post(v8::Local<v8::Object> map);
    virtual result_t post(exlib::string pattern, Handler_base* hdlr);
    virtual result_t del(v8::Local<v8::Object> map);
    virtual result_t del(exlib::string pattern, Handler_base* hdlr);
    virtual result_t put(v8::Local<v8::Object> map);
    virtual result_t put(exlib::string pattern, Handler_base* hdlr);
    virtual result_t patch(v8::Local<v8::Object> map);
    virtual result_t patch(exlib::string pattern, Handler_base* hdlr);

public:
    result_t append(exlib::string method, exlib::string pattern, v8::Local<v8::Value> hdlr);

private:
    std::vector<obj_ptr<rule>> m_array;
};

} /* namespace fibjs */
#endif /* ROUTING_H_ */
