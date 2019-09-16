/*
 * Repeater.h
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "ifs/HttpRepeater.h"
#include <vector>
#include "HttpClient.h"
#include "Url.h"

#ifndef REPEATER_H_
#define REPEATER_H_

namespace fibjs {

class HttpRepeater : public HttpRepeater_base {

public:
    HttpRepeater();

public:
    // HttpRepeater_base
    virtual result_t load(v8::Local<v8::Array> urls);
    virtual result_t get_urls(obj_ptr<NArray>& retVal);
    virtual result_t get_client(obj_ptr<HttpClient_base>& retVal);

public:
    // Handler_base
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
        AsyncEvent* ac);

public:
    obj_ptr<HttpClient> m_client;
    std::vector<obj_ptr<Url>> m_urls;
    int32_t m_idx;
    exlib::spinlock m_lock;
};

} /* namespace fibjs */
#endif /* REPEATER_H_ */
