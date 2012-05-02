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
v8::Persistent<v8::Context> s_context;

class MyAppender: public log4cpp::LayoutAppender
{
public:
	MyAppender() :
			LayoutAppender("console")
	{
	}

	void close()
	{
	}

protected:
	void _append(const log4cpp::LoggingEvent& event)
	{
		if (event.priority < log4cpp::Priority::NOTICE)
			std::cerr << event.message << std::endl;
		else
			std::cout << event.message << std::endl;
	}
};

void initModule();
void flushLog();
void clearTimer();

void _main(const char* fname)
{
	try
	{
		log4cpp::PropertyConfigurator::configure("log.ini");
	} catch (log4cpp::ConfigureFailure e)
	{
		log4cpp::Category& root = log4cpp::Category::getRoot();
		root.addAppender(new MyAppender());
		root.warn(e.what());
	}

	v8::V8::Initialize();

	fibjs::isolate = v8::Isolate::GetCurrent();
	v8::Locker locker(fibjs::isolate);
	v8::Isolate::Scope isolate_scope(fibjs::isolate);

	v8::HandleScope handle_scope;

	s_context = v8::Context::New();
	v8::Context::Scope context_scope(s_context);

	initModule();

	v8::TryCatch try_catch;
	fibjs::global_base::run(fname);
	if (try_catch.HasCaught())
		ReportException(&try_catch, true);

	clearTimer();
	flushLog();
	s_context.Dispose();
}

}

int main(int argc, char* argv[])
{
	if (argc == 2)
		fibjs::_main(argv[1]);
	else
		fibjs::_main("main.js");

	return 0;
}
