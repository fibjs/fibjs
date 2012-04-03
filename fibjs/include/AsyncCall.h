#ifndef _fj_ASYNCCALL_H
#define _fj_ASYNCCALL_H

#include <exlib/fiber.h>
#include "utils.h"

namespace fibjs
{

class AsyncCall
{
public:
    AsyncCall(void ** a, result_t (*f)(void**) = NULL) :
        func(f), args(a), hr(0), m_next(NULL)
    {
    }

public:
    result_t (*func)(void**);
    void ** args;
    result_t hr;
    exlib::Event weak;

    AsyncCall* m_next;
};

}

#endif
