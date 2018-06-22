/*
 * unixAddr.h
 *
 *  Created on: June 22, 2018
 *      Author: Asion
 */

#include "ifs/net.h"
#include "path.h"
#include <string.h>

#ifndef UNIXADDR_H_
#define UNIXADDR_H_

namespace fibjs {

struct unixAddr {
    struct sockaddr_un addrun;

    int32_t size()
    {
        return (int32_t)sizeof(addrun);
    }

    int32_t family()
    {
        return net_base::_AF_LOCAL;
    }

    void init()
    {
        memset(&addrun, 0, sizeof(addrun));
        addrun.sun_family = PF_LOCAL;
    }

    void init(sockaddr* addr)
    {
        memmove(&addrun, addr, sizeof(addrun));
    }

    int32_t addr(exlib::string s)
    {
        bool bfull;
        path_base::isAbsolute(s, bfull);
        if (!bfull)
            return -1;
        memcpy(addrun.sun_path, s.c_str(), sizeof(addrun.sun_path) - 1);
        return 0;
    }
    exlib::string str()
    {
        return addrun.sun_path;
    }
};
}
#endif