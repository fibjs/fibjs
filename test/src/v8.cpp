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

using namespace v8;

Isolate* isolate;

Handle<Value> Print(const Arguments& args)
{
    HandleScope handle_scope;
    {
        Unlocker unlocker(isolate);
        fiber::Service::Suspend();
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
        printf("%x: %s",  fiber::Service::Current(), *str);
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

    Handle<String> source = String::New("var n = 0;{print('Hello, World? ' + n);n = n + 1;}//console.log('aaaaa')");
    Handle<Script> script = Script::Compile(source);
    script->Run();

    return NULL;
}

#define COUNT 10

void v8_main()
{
    log4cpp::Category& v8log = log4cpp::Category::getInstance(std::string("v8"));

    v8log.debug("--------- v8 sample --------");

    int i;
    isolate = Isolate::New();
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope;

    Handle<ObjectTemplate> global = ObjectTemplate::New();
    global->Set(String::New("print"), FunctionTemplate::New(Print));

    g_global = Persistent<ObjectTemplate>(global);

    while(1)
    {
        for(int i = 0; i < COUNT; i ++)
        {
            fiber::Service::CreateFiber(t, (void*)i, 80000);
            printf("new fiber: %d\n", i);
        }

        fiber::Service::Run();
    }

    v8log.debug("--------- v8 sample --------");
}

