#include "test.h"
#include <exlib/fiber.h>
#include <stdio.h>

exlib::Locker lk;

void* t_fiber(void* p)
{
    printf("t_fiber: %p\n", exlib::Fiber::Current());

    lk.lock();
    printf("enter lock: %p\n", exlib::Fiber::Current());
    for(int i = 0; i < 3; i ++)
    {
        printf("loop %p: %p\n", exlib::Fiber::Current(), i);
        exlib::Fiber::yield();
    }
    lk.unlock();

	return NULL;
}

void* main_fiber(void* p)
{
    printf("main_fiber: %p\n", exlib::Fiber::Current());

    exlib::Fiber* f1 = exlib::Service::CreateFiber(t_fiber, (void*)0x01234567, 32768);
    exlib::Fiber* f2 = exlib::Service::CreateFiber(t_fiber, (void*)0x00000011, 32768);
//    exlib::context* f3 = exlib::Service::CreateFiber(t_fiber, (void*)0x87000011, 32768);

    f1->join();
    puts("f1 ok");
    f2->join();
    puts("f2 ok");
//    exlib::Service::Join(f3);
//    puts("f3 ok");

	return NULL;
}

void fiber_main()
{
    exlib::Service::CreateFiber(main_fiber, (void*)0x01234567, 32768);
    while(1)
        exlib::Fiber::yield();
}
