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

	puts("Persistent<Context> context = Context::New();");
	Persistent<Context> context = Context::New();

	puts("Context::Scope context_scope(context);");
	Context::Scope context_scope(context);

	{
		puts("Persistent<Context> context1 = Context::New();");
		Persistent<Context> context1 = Context::New();

		puts("Context::Scope context_scope(context);");
		Context::Scope context_scope1(context1);

		Handle<String> source = String::New("'Hello' + ', World'");

		Handle<Script> script = Script::Compile(source);

		Handle<Value> result = script->Run();

		context1.Dispose();

		String::AsciiValue ascii(result);
		printf("%s\n", *ascii);
	}

	context.Dispose();



	return 0;
}
