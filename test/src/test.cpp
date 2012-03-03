//============================================================================
// Name        : test.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <v8/v8.h>

using namespace v8;

using namespace std;

int main() {
	HandleScope handle_scope;

	Persistent<Context> context = Context::New();

	Context::Scope context_scope(context);

	Handle<String> source = String::New("'Hello' + ', World'");

	Handle<Script> script = Script::Compile(source);

	Handle<Value> result = script->Run();

	context.Dispose();

	String::AsciiValue ascii(result);
	printf("%s\n", *ascii);

	return 0;
}
