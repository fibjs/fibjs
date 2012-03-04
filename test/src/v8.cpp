/*
 *  v8.cpp
 *  Created on: Mar 4, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <v8/v8.h>
using namespace v8;

void v8_main() {
	puts("--------- v8 sample --------");
	// Create a stack-allocated handle scope.
	HandleScope handle_scope;
	// Create a new context.
	Handle < Context > context = Context::New();
	// Enter the created context for compiling and
	// running the hello world script.
	Context::Scope context_scope(context);
	// Create a string containing the JavaScript source code.
	Handle < String > source = String::New("'Hello' + ', World!'");
	// Compile the source code.
	Handle < Script > script = Script::Compile(source);
	// Run the script to get the result.
	Handle < Value > result = script->Run();
	// Convert the result to an ASCII string and print it.
	String::AsciiValue ascii(result);
	printf("%s\n", *ascii);
	puts("--------- v8 sample --------");
}
