#ifndef _fj_ASYNCCALL_H
#define _fj_ASYNCCALL_H

#include <exlib/fiber.h>
#include "utils.h"

namespace fibjs
{

class AsyncCall : public exlib::AsyncEvent
{
public:
    AsyncCall(void ** a, void (*f)(AsyncCall*) = NULL) :
        func(f), args(a), hr(0)
    {
    }

public:
    void (*func)(AsyncCall*);
    void ** args;
    result_t hr;
};

}

#endif
