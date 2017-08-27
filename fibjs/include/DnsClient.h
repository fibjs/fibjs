/*
 * DnsClient.h
 *
 *  Created on: Aug 16, 2017
 *      Author: Rube
 */

#ifndef DNSCLIENT_H_
#define DNSCLIENT_H_

#include "ifs/DnsClient.h"

namespace fibjs {

    class DnsClient : public DnsClient_base {
        FIBER_FREE();

    public:
        virtual result_t resolve(exlib::string host, exlib::string type, obj_ptr<object_base>& retVal, AsyncEvent* ac);
        virtual result_t resolve4(exlib::string host, Map_base* options, obj_ptr<List_base>& retVal, AsyncEvent* ac);
        virtual result_t resolve4(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac);
        virtual result_t resolve6(exlib::string host, Map_base* options, obj_ptr<List_base>& retVal, AsyncEvent* ac);
        virtual result_t resolve6(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac);
        virtual result_t resolveMx(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac);
        virtual result_t resolveTxt(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac);
        virtual result_t resolveSrv(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac);
        virtual result_t resolveSoa(exlib::string host, obj_ptr<object_base>& retVal, AsyncEvent* ac);
        virtual result_t resolveNs(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac);
        virtual result_t resolveCname(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac);
        virtual result_t resolveNaptr(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac);
        virtual result_t resolvePtr(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac);
    };
}

#endif //DNSCLIENT_H_
