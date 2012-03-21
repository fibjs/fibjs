/*
 *  v8.cpp
 *  Created on: Mar 4, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <v8/v8.h>
#include <fiber.h>
#include "test.h"
//#include <pthread.h>

using namespace v8;

Isolate* isolate;

Handle<Value> Print(const Arguments& args)
{
    HandleScope handle_scope;
    {
        Unlocker unlocker(isolate);
        fiber::Service::Suspend();
        //pthread_yield();
    }

    bool first = true;
    for (int i = 0; i < args.Length(); i++)
    {
        //HandleScope handle_scope;

        if (first)
        {
            first = false;
        }
        else
        {
            printf(" ");
        }
        //convert the args[i] type to normal char* string
        String::AsciiValue str(args[i]);
        printf("%s",  *str);
        //printf("%x: %s",  fiber::Service::Current(), *str);
    }
    printf("\n");
    //returning Undefined is the same as returning void...
    return Undefined();
}

Persistent<ObjectTemplate> g_global;

void* t(void* p)
{
    Locker locker(isolate);
    Isolate::Scope isolate_scope(isolate);

    HandleScope handle_scope;

    Handle<Context> context = Context::New(NULL, g_global);
    Context::Scope context_scope(context);

    Handle<String> source = String::New("var n = 0;while(1){print('Hello, World? ' + n);n = n + 1;}//console.log('aaaaa')");
    Handle<Script> script = Script::Compile(source);
    script->Run();

    return NULL;
}

#define COUNT 1000

void v8_main()
{
    log4cpp::Category& v8log = log4cpp::Category::getInstance(std::string("v8"));

    v8log.debug("--------- v8 sample --------");

    isolate = Isolate::New();

    Locker lock(isolate);
    Isolate::Scope isolate_scope(isolate);

    HandleScope handle_scope;

    {
        Handle<ObjectTemplate> global = ObjectTemplate::New();
        global->Set(String::New("print"), FunctionTemplate::New(Print));
        g_global = Persistent<ObjectTemplate>(global);
    }

    Unlocker unlocker(isolate);

    for(int i = 0; i < COUNT; i ++)
    {
        fiber::Service::CreateFiber(t, (void*)i, 200000);
        //pthread_t ptid;
        //pthread_create(&ptid, NULL, t, (void*)i);
        printf("new fiber: %d\n", i);
    }

    fiber::Service::Run();
    //usleep(30000);

    v8log.debug("--------- v8 sample --------");
}

