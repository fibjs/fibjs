/*
 *  v8.cpp
 *  Created on: Mar 4, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <v8/v8.h>
#include "test.h"

using namespace v8;


char* p, *p1;

void putStack()
{
    char n = 0;

    if(p == 0)
        p = &n;

    log4cpp::Category::getInstance(std::string("v8")).debug("--- %d, %d", (p - &n), (p1 - &n));
    p1 = &n;
}

// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called. Prints its arguments on stdout separated by
// spaces and ending with a newline.
Handle<Value> Print(const Arguments& args) {
    putStack();
    bool first = true;
    for (int i = 0; i < args.Length(); i++)
    {
        HandleScope handle_scope;
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
        printf("%s", *str);
    }
    printf("\n");
    //returning Undefined is the same as returning void...
    return Undefined();
}

void v8_main() {
    log4cpp::Category& v8log = log4cpp::Category::getInstance(std::string("v8"));

	v8log.debug("--------- v8 sample --------");
	// Create a stack-allocated handle scope.
	HandleScope handle_scope;

	Handle<ObjectTemplate> global = ObjectTemplate::New();

    global->Set(String::New("print"), FunctionTemplate::New(Print));

	// Create a new context.
	Handle < Context > context = Context::New(NULL, global);
	// Enter the created context for compiling and
	// running the hello world script.
	Context::Scope context_scope(context);
	// Create a string containing the JavaScript source code.
	Handle < String > source = String::New("function t(n){var n1 = n + 1;if(n %1 == 1)print('Hello, World! ' + n);else print('Hello, World? ' + n);t(n1);};t(0)");
	// Compile the source code.
	Handle < Script > script = Script::Compile(source);
	// Run the script to get the result.

	putStack();
	script->Run();
	putStack();

	v8log.debug("--------- v8 sample --------");
}
