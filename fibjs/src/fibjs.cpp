#include <v8/v8.h>

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/LayoutAppender.hh>

#ifdef assert
#undef assert
#endif

#include "ifs/global.h"
#include <exlib/lockfree.h>
#include "AsyncCall.h"

namespace fibjs
{
    v8::Isolate* isolate;

result_t test(int a1, const char* s, double n)
{
    printf("%d, %s, %g\n", a1, s, n);
    return 0;
}

result_t test1(int a1, const char* s, double n, const char* s1)
{
    printf("%d, %s, %g, %s\n", a1, s, n, s1);
    return 0;
}

result_t test2(int a1, const char* s, double n, const char* s1, int& vr)
{
    printf("%d, %s, %g, %s\n", a1, s, n, s1);
    vr = 100;
    return 0;
}

result_t test3(int a1, const char* s, double n, const char* s1, int v)
{
    printf("%d, %s, %g, %s, %d\n", a1, s, n, s1, v);
    return 0;
}

template<typename T1, typename T2, typename T3>
result_t call_stub(result_t (*func)(T1,T2,T3), void** args)
{
    return func(*static_cast<T1*>(args[0]), *static_cast<T2*>(args[1]), *static_cast<T3*>(args[2]));
}

template<typename T1, typename T2, typename T3, typename T4>
result_t call_stub(result_t (*func)(T1,T2,T3,T4), void** args)
{
    return func(*static_cast<T1*>(args[0]), *static_cast<T2*>(args[1]), *static_cast<T3*>(args[2]), *static_cast<T4*>(args[3]));
}

template<typename T1, typename T2, typename T3, typename T4, typename T5>
result_t call_stub(result_t (*func)(T1,T2,T3,T4,T5&), void** args)
{
    return func(*static_cast<T1*>(args[0]), *static_cast<T2*>(args[1]), *static_cast<T3*>(args[2]), *static_cast<T4*>(args[3]), *static_cast<T5*>(args[4]));
}

template<typename T1, typename T2, typename T3, typename T4, typename T5>
result_t call_stub(result_t (*func)(T1,T2,T3,T4,T5), void** args)
{
    return func(*static_cast<T1*>(args[0]), *static_cast<T2*>(args[1]), *static_cast<T3*>(args[2]), *static_cast<T4*>(args[3]), *static_cast<T5*>(args[4]));
}

class MyAppender : public log4cpp::LayoutAppender
{
public:
    MyAppender() : LayoutAppender("console")
    {
    }

    void close()
    {
    }

protected:
    void _append(const log4cpp::LoggingEvent& event)
    {
        if(event.priority > log4cpp::Priority::NOTICE)
            std::cerr << event.message << std::endl;
        else std::cout << event.message << std::endl;
    }
};

void _main(const char* fname)
{
    try
    {
        log4cpp::PropertyConfigurator::configure("log.ini");
    }
    catch (log4cpp::ConfigureFailure e)
    {
        log4cpp::Category& root = log4cpp::Category::getRoot();
        root.addAppender(new MyAppender());
        root.warn(e.what());
    }

/*
    int a = 100;
    double n = 100.123;
    const char *s = "aaaaaaa";
    const char *s1 = "bbbbb";
    int vr = 0;
    void* args[] = {&a, &s, &n, &s1, &vr};

    fibjs::call_stub(fibjs::test, args);
    fibjs::call_stub(fibjs::test1, args);
    fibjs::call_stub(fibjs::test2, args);
    fibjs::call_stub(fibjs::test3, args);

*/
    /*
        v8::ResourceConstraints rc;
        rc.set_max_young_space_size(2048); //KB
        rc.set_max_old_space_size(10); //MB
        rc.set_max_executable_size(10); //MB
        rc.set_stack_limit(reinterpret_cast<uint32_t*>((char*)&rc- 1024 * 400));

        SetResourceConstraints(&rc);
    */

    v8::V8::Initialize();

    fibjs::isolate = v8::Isolate::GetCurrent();
    v8::Locker locker(fibjs::isolate);
    v8::Isolate::Scope isolate_scope(fibjs::isolate);

    fibjs::global_base::run(fname);
}

}

int main(int argc, char* argv[])
{
    if(argc == 2)
        fibjs::_main(argv[1]);
    else fibjs::_main("main.js");

    return 0;
}
