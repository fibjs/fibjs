#include "test.h"
#include <fiber.h>
#include <stdio.h>

fiber::Locker lk;

void* t_fiber(void* p)
{
    printf("t_fiber: %p\n", fiber::Service::Current());

    lk.lock();
    printf("enter lock: %p\n", fiber::Service::Current());
    for(int i = 0; i < 3; i ++)
    {
        printf("loop %p: %p\n", fiber::Service::Current(), i);
        fiber::Service::Suspend();
    }
    lk.unlock();
}

void* main_fiber(void* p)
{
    printf("main_fiber: %p\n", fiber::Service::Current());

    fiber::Fiber* f1 = fiber::Service::CreateFiber(t_fiber, (void*)0x01234567, 32768);
    fiber::Fiber* f2 = fiber::Service::CreateFiber(t_fiber, (void*)0x00000011, 32768);
//    fiber::context* f3 = fiber::Service::CreateFiber(t_fiber, (void*)0x87000011, 32768);

    fiber::Service::Join(f1);
    puts("f1 ok");
    fiber::Service::Join(f2);
    puts("f2 ok");
//    fiber::Service::Join(f3);
//    puts("f3 ok");
}

void fiber_main()
{
    fiber::Service::CreateFiber(main_fiber, (void*)0x01234567, 32768);
    while(1)
        fiber::Service::Suspend();
}
