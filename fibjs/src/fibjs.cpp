#include <v8/v8.h>

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/LayoutAppender.hh>

#include <locale.h>

#ifdef assert
#undef assert
#endif

#include "ifs/global.h"
#include "ifs/process.h"
#include "SandBox.h"
#include <exlib/lockfree.h>
#include "Fiber.h"
#include "utf8.h"
#include <exlib/thread.h>

#define COLOR_RESET "\x1b[0m"
#define COLOR_BLACK "\x1b[30m" /* Black */
#define COLOR_RED "\x1b[31m" /* Red */
#define COLOR_GREEN "\x1b[32m" /* Green */
#define COLOR_YELLOW "\x1b[33m" /* Yellow */
#define COLOR_BLUE "\x1b[34m" /* Blue */
#define COLOR_MAGENTA "\x1b[35m" /* Magenta */
#define COLOR_CYAN "\x1b[36m" /* Cyan */
#define COLOR_WHITE "\x1b[37m" /* White */
#define COLOR_NORMAL "\x1b[39m" /* White */

namespace fibjs
{
v8::Isolate* isolate;
v8::Persistent<v8::Context> s_context;
exlib::Service* g_pService;

class MyAppender: public log4cpp::LayoutAppender
{
#ifdef _WIN32
protected:
	class color_out
	{
	public:
		void init(DWORD type)
		{
			CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

			m_handle = GetStdHandle(type);
			GetConsoleScreenBufferInfo(m_handle, &csbiInfo);
			m_Now = m_wAttr = csbiInfo.wAttributes;

			if (type == STD_ERROR_HANDLE)
				m_stream = stderr;
			else if (type == STD_OUTPUT_HANDLE)
				m_stream = stdout;
		}

		void out(const std::string& message)
		{
			std::wstring s = UTF8_W(message);
			s.append(L"\r\n", 2);
			LPWSTR ptr = (LPWSTR) s.c_str();
			LPWSTR ptr1, ptr2;

			ptr1 = ptr;
			while (ptr2 = (LPWSTR) qstrchr(ptr1, L'\x1b'))
			{
				ptr1 = ptr2 + 1;

				if (ptr2[1] == '[')
				{
					WORD mask, val;

					if ( qisdigit(ptr2[2]) && ptr2[3] == 'm')
					{
						if (ptr2 > ptr)
						{
							ptr2[0] = 0;
							fputws(ptr, m_stream);
							fflush(m_stream);
						}

						ptr = ptr1 = ptr2 + 4;
					}
					else if ( ptr2[2] == '2' && qisdigit(ptr2[3]) && ptr2[4] == 'm')
					{
						if (ptr2 > ptr)
						{
							ptr2[0] = 0;
							fputws(ptr, m_stream);
							fflush(m_stream);
						}

						ptr = ptr1 = ptr2 + 5;
					}
					else if (ptr2[2] == '9' && ptr2[3] == '0' && ptr2[4] == 'm')
					{
						mask = 0xf0;
						val = FOREGROUND_BLUE | FOREGROUND_GREEN
								| FOREGROUND_RED;

						if (ptr2 > ptr)
						{
							ptr2[0] = 0;
							fputws(ptr, m_stream);
							fflush(m_stream);
						}

						m_Now = (m_Now & mask) | val;
						SetConsoleTextAttribute(m_handle, m_Now);

						ptr = ptr1 = ptr2 + 5;
					}
					else if ((ptr2[2] == '3' || ptr2[2] == '4')
							&& qisdigit(ptr2[3])
							&& ptr2[4] == 'm')
					{
						if (ptr2[2] == '3')
							mask = 0xf0;
						else
							mask = 0x0f;

						val = ptr2[3] - '0';

						if (val == 9)
							val = (mask ^ 0xff) & m_wAttr;
						else
						{
							val = (val & 2) | ((val & 1) ? 4 : 0)
									| ((val & 4) ? 1 : 0)
									| FOREGROUND_INTENSITY;

							if (mask == 0x0f)
								val <<= 4;
						}

						if (ptr2 > ptr)
						{
							ptr2[0] = 0;
							fputws(ptr, m_stream);
							fflush(m_stream);
						}

						m_Now = (m_Now & mask) | val;
						SetConsoleTextAttribute(m_handle, m_Now);

						ptr = ptr1 = ptr2 + 5;
					}
				}
			}

			fputws(ptr, m_stream);
			fflush(m_stream);
		}

	private:
		HANDLE m_handle;
		FILE *m_stream;
		WORD m_wAttr, m_Now;
	};
#endif

public:
	MyAppender() :
			LayoutAppender("console")
	{
#ifdef _WIN32
		err.init(STD_ERROR_HANDLE);
		out.init(STD_OUTPUT_HANDLE);
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
			std::cerr << COLOR_RED << event.message << COLOR_NORMAL << std::endl;
#else
			out.out(COLOR_RED + event.message + COLOR_NORMAL);
#endif
		}
		else
		{
#ifndef _WIN32
			std::cout << event.message << std::endl;
#else
			out.out(event.message);
#endif
		}
	}

#ifdef _WIN32
private:
	color_out err, out;
#endif
};

void init_argv(int argc, char** argv);

void _main(const char* fname)
{
	try
	{
		log4cpp::PropertyConfigurator::configure("log.ini");
	} catch (log4cpp::ConfigureFailure e)
	{
		log4cpp::Category& root = log4cpp::Category::getRoot();
		root.addAppender(new MyAppender());
//		root.warn(e.what());
	}

	exlib::Service::init();
	g_pService = exlib::Service::getFiberService();

	v8::V8::Initialize();

	fibjs::isolate = v8::Isolate::GetCurrent();
	v8::Locker locker(fibjs::isolate);
	v8::Isolate::Scope isolate_scope(fibjs::isolate);

	v8::HandleScope handle_scope(isolate);

	v8::Handle<v8::Context> _context = v8::Context::New(isolate);
	s_context.Reset(isolate, _context);
	v8::Context::Scope context_scope(_context);

	Fiber_base* fb = new JSFiber();
	g_pService->tlsPut(g_tlsCurrent, fb);
	fb->Ref();

	{
		JSFiber::scope s;
		obj_ptr<SandBox> sbox = new SandBox();

		sbox->initRoot();
		result_t hr = sbox->run(fname);
		if (hr != -2 || qstrcmp(fname, "main.js"))
			s.m_hr = hr;
	}

	process_base::exit(0);

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
	static const char s_opts[] = "--stack_size=110 --harmony";
	enableDump();

	exlib::OSThread::Sleep(1);

	v8::V8::SetFlagsFromString(s_opts, sizeof(s_opts) - 1);
	v8::V8::SetFlagsFromCommandLine(&argc, argv, true);

	fibjs::init_argv(argc, argv);

	::setlocale(LC_ALL,"");

	if (argc >= 2 && argv[1][0] != '-')
		fibjs::_main(argv[1]);
	else
		fibjs::_main("main.js");

	return 0;
}
