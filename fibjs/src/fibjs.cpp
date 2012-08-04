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

#define RESET "\033[0m"
#define BLACK "\033[30m" /* Black */
#define RED "\033[31m" /* Red */
#define GREEN "\033[32m" /* Green */
#define YELLOW "\033[33m" /* Yellow */
#define BLUE "\033[34m" /* Blue */
#define MAGENTA "\033[35m" /* Magenta */
#define CYAN "\033[36m" /* Cyan */
#define WHITE "\033[37m" /* White */
#define BOLDBLACK "\033[1m\033[30m" /* Bold Black */
#define BOLDRED "\033[1m\033[31m" /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m" /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m" /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m" /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m" /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m" /* Bold White */

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
#ifdef _WIN32
		m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	}

	void close()
	{
	}

protected:
	void _append(const log4cpp::LoggingEvent& event)
	{
		if (event.priority < log4cpp::Priority::NOTICE)
		{
#ifndef _WIN32
			std::cerr << RED << event.message << RESET << std::endl;
#else
			SetConsoleTextAttribute(m_hConsole, FOREGROUND_RED);
			std::cerr << event.message << std::endl;
			SetConsoleTextAttribute(m_hConsole, FOREGROUND_INTENSITY);
#endif
		}
		else
			std::cout << event.message << std::endl;
	}

#ifdef _WIN32
private:
	HANDLE m_hConsole;
#endif
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
