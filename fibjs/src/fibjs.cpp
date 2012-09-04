#include <v8/v8.h>

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/LayoutAppender.hh>

#ifdef assert
#undef assert
#endif

#include "ifs/global.h"
#include <exlib/lockfree.h>
#include "Fiber.h"
#include "utf8.h"

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
exlib::Service* g_pService;

class MyAppender: public log4cpp::LayoutAppender
{
public:
	MyAppender() :
			LayoutAppender("console")
	{
#ifdef _WIN32
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

		m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		m_hError = GetStdHandle(STD_ERROR_HANDLE);
		GetConsoleScreenBufferInfo(m_hError, &csbiInfo);
		m_wAttributes = csbiInfo.wAttributes;
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
			SetConsoleTextAttribute(m_hError, FOREGROUND_RED);
			DWORD dwWrite;
			std::wstring s = UTF8_W(event.message);
			s.append(L"\r\n", 2);
			WriteConsoleW(m_hConsole, s.c_str(), (DWORD)s.length(), &dwWrite, NULL);
			SetConsoleTextAttribute(m_hError, m_wAttributes);
#endif
		}
		else
		{
#ifndef _WIN32
			std::cout << event.message << std::endl;
#else
			DWORD dwWrite;
			std::wstring s = UTF8_W(event.message);
			s.append(L"\r\n", 2);
			WriteConsoleW(m_hConsole, s.c_str(), (DWORD)s.length(), &dwWrite, NULL);
#endif
		}
	}

#ifdef _WIN32
private:
	HANDLE m_hConsole;
	HANDLE m_hError;
	WORD m_wAttributes;
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

	exlib::Service::init();
	g_pService = exlib::Service::getFiberService();

	v8::V8::Initialize();

	fibjs::isolate = v8::Isolate::GetCurrent();
	v8::Locker locker(fibjs::isolate);
	v8::Isolate::Scope isolate_scope(fibjs::isolate);

	v8::HandleScope handle_scope;

	s_context = v8::Context::New();
	v8::Context::Scope context_scope(s_context);

	initModule();

	Fiber_base* fb = new JSFiber();
	g_pService->tlsPut(g_tlsCurrent, fb);
	fb->Ref();

	v8::TryCatch try_catch;
	fibjs::global_base::run(fname);
	if (try_catch.HasCaught())
		ReportException(&try_catch, true);

	clearTimer();
	flushLog();
	s_context.Dispose();
}

}

#ifdef _WIN32
#include <DbgHelp.h>

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
		CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
		CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

static MINIDUMPWRITEDUMP s_pDump;

HANDLE CreateUniqueDumpFile()
{
	char fname[MAX_PATH];
	int l, i;
	HANDLE hFile;

	puts("core dump....");
	l = GetCurrentDirectory(MAX_PATH, fname);
	memcpy(fname + l, "\\core.", 14);
	l += 10;

	for (i = 0; i < 104; i++)
	{
		_itoa_s(i, fname + l, 10, 10);
		memcpy(fname + l + (i > 999 ? 4 : (i > 99 ? 3 : (i > 9 ? 2 : 1))),
				L".dmp", 10);

		hFile = CreateFile(fname, GENERIC_READ | GENERIC_WRITE, 0, NULL,
				CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
			return hFile;

		if (GetLastError() != ERROR_FILE_EXISTS)
			return INVALID_HANDLE_VALUE;
	};

	return INVALID_HANDLE_VALUE;
}

static void CreateMiniDump(LPEXCEPTION_POINTERS lpExceptionInfo)
{
	HANDLE hFile = CreateUniqueDumpFile();

	if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;

		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = lpExceptionInfo;
		mdei.ClientPointers = FALSE;

		MINIDUMP_TYPE mdt = MiniDumpNormal;
		BOOL retv = s_pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,
				mdt, (lpExceptionInfo != 0) ? &mdei : 0, 0, 0);

		CloseHandle(hFile);
	}
}

static LONG WINAPI GPTUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	CreateMiniDump(pExceptionInfo);
	exit(pExceptionInfo->ExceptionRecord->ExceptionCode);
	return EXCEPTION_EXECUTE_HANDLER;
}

void enableDump()
{
	HMODULE hDll;
	if (hDll = ::LoadLibrary("DBGHELP.DLL"))
	{
		s_pDump = (MINIDUMPWRITEDUMP) ::GetProcAddress(hDll,
				"MiniDumpWriteDump");
		if (s_pDump)
			SetUnhandledExceptionFilter (GPTUnhandledExceptionFilter);
	}
}

#else

#include <pwd.h>
#include <sys/resource.h>

void enableDump()
{
	struct rlimit corelimit =
	{ RLIM_INFINITY, RLIM_INFINITY };

	setrlimit(RLIMIT_CORE, &corelimit);
}

#endif

int main(int argc, char* argv[])
{
	enableDump();

	if (argc == 2)
		fibjs::_main(argv[1]);
	else
		fibjs::_main("main.js");

	return 0;
}
