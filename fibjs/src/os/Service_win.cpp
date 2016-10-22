/*
 * Service_win.cpp
 *
 *  Created on: Oct 15, 2016
 *      Author: lion
 */

#ifdef _WIN32

#include "object.h"
#include "Service.h"
#include "Fiber.h"
#include "utf8.h"

namespace fibjs
{

result_t Service_base::_new(exlib::string name, v8::Local<v8::Function> worker,
                            v8::Local<v8::Object> event, obj_ptr<Service_base>& retVal,
                            v8::Local<v8::Object> This)
{
	int32_t n;
	obj_ptr<Service> srv = new Service(name);
	srv->wrap(This);

	srv->SetPrivate("worker", worker);
	srv->on(event, n);

	retVal = srv;

	return 0;
}

result_t Service::install(exlib::string cmd, exlib::string displayName, exlib::string description)
{
	SC_HANDLE   schService;
	SC_HANDLE   schSCManager;

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schSCManager)
		return CHECK_ERROR(LastError());

	schService = CreateServiceW(schSCManager, UTF8_W(m_name), UTF8_W(displayName),
	                            SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
	                            SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
	                            UTF8_W(cmd), NULL, NULL, NULL, NULL, NULL);
	if (!schService)
	{
		CloseServiceHandle(schSCManager);
		return CHECK_ERROR(LastError());
	}

	exlib::wstring wdescription = utf8to16String(description);
	SERVICE_DESCRIPTIONW sd = {0};
	sd.lpDescription = (LPWSTR)wdescription.c_str();

	ChangeServiceConfig2W(schService, SERVICE_CONFIG_DESCRIPTION, &sd);

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return 0;
}

result_t Service::remove()
{
	SC_HANDLE   schService;
	SC_HANDLE   schSCManager;

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schSCManager)
		return CHECK_ERROR(LastError());

	schService = OpenServiceW(schSCManager, UTF8_W(m_name), SERVICE_ALL_ACCESS);
	if (!schService)
	{
		CloseServiceHandle(schSCManager);
		return CHECK_ERROR(LastError());
	}

	DeleteService(schService);

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return 0;
}

result_t Service::start()
{
	SC_HANDLE   schService;
	SC_HANDLE   schSCManager;

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schSCManager)
		return CHECK_ERROR(LastError());

	schService = OpenServiceW(schSCManager, UTF8_W(m_name), SERVICE_ALL_ACCESS);
	if (!schService)
	{
		CloseServiceHandle(schSCManager);
		return CHECK_ERROR(LastError());
	}

	StartService(schService, 0, NULL);

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return 0;
}

result_t Service::stop()
{
	SC_HANDLE   schService;
	SC_HANDLE   schSCManager;
	SERVICE_STATUS ssStatus;

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schSCManager)
		return CHECK_ERROR(LastError());

	schService = OpenServiceW(schSCManager, UTF8_W(m_name), SERVICE_ALL_ACCESS);
	if (!schService)
	{
		CloseServiceHandle(schSCManager);
		return CHECK_ERROR(LastError());
	}

	if (ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus))
	{
		Sleep(1);

		while (QueryServiceStatus(schService, &ssStatus))
		{
			if (ssStatus.dwCurrentState != SERVICE_STOPPED)
				Sleep(1);
			else break;
		}
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return 0;
}

result_t Service::restart()
{
	result_t hr;

	hr = stop();
	if (hr < 0)
		return hr;

	return start();
}

void WINAPI service_main(DWORD dwArgc, LPWSTR *lpszArgv);
static exlib::atomic_ptr<Service> s_srv;
static Isolate* s_isolate;
static SERVICE_STATUS_HANDLE s_hSStat;
static SERVICE_STATUS s_ssStatus;
static SERVICE_TABLE_ENTRYW s_dispatchTable[] =
{
	{ NULL, service_main},
	{ NULL, NULL }
};

void ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;
	BOOL fResult = TRUE;

	if (dwCurrentState == SERVICE_START_PENDING)
		s_ssStatus.dwControlsAccepted = 0;
	else
		s_ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;

	s_ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	s_ssStatus.dwCurrentState = dwCurrentState;
	s_ssStatus.dwWin32ExitCode = dwWin32ExitCode;
	s_ssStatus.dwWaitHint = dwWaitHint;

	if ((dwCurrentState == SERVICE_RUNNING) ||
	        (dwCurrentState == SERVICE_STOPPED))
		s_ssStatus.dwCheckPoint = 0;
	else
		s_ssStatus.dwCheckPoint = dwCheckPoint++;

	SetServiceStatus(s_hSStat, &s_ssStatus);
}

static void WINAPI service_ctrl(DWORD dwCtrlCode)
{
	Service* srv = s_srv;

	switch (dwCtrlCode)
	{
	case SERVICE_CONTROL_STOP:
		srv->_trigger("stop", (Variant*)NULL, 0);
		break;
	case SERVICE_CONTROL_CONTINUE:
		ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0);
		srv->_trigger("continue", (Variant*)NULL, 0);
		break;
	case SERVICE_CONTROL_PAUSE:
		ReportStatusToSCMgr(SERVICE_PAUSED, NO_ERROR, 0);
		srv->_trigger("pause", (Variant*)NULL, 0);
		break;
	}
}

static void service_worker(Service* srv)
{
	JSFiber::scope s;
	v8::Local<v8::Value> v;
	v8::Local<v8::Function> func;

	v = srv->GetPrivate("worker");
	if (!v.IsEmpty())
	{
		func = v8::Local<v8::Function>::Cast(v);
		func->Call(srv->wrap(), 0, &v);
	}

	ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);
}

static void WINAPI service_main(DWORD dwArgc, LPWSTR *lpszArgv)
{
	s_hSStat = RegisterServiceCtrlHandlerW(s_dispatchTable[0].lpServiceName, service_ctrl);
	if (s_hSStat)
	{
		Service* srv = s_srv;
		syncCall(s_isolate, service_worker, srv);
		ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0);
	}
}

result_t Service::run(AsyncEvent* ac)
{
	if (!ac)
	{
		if (s_srv.CompareAndSwap(NULL, this) != NULL)
			return CHECK_ERROR(CALL_E_INVALID_CALL);

		s_isolate = holder();

		return CHECK_ERROR(CALL_E_LONGSYNC);
	}

	exlib::wstring wname = utf8to16String(m_name);
	s_dispatchTable[0].lpServiceName = (LPWSTR)wname.c_str();

	if (!StartServiceCtrlDispatcherW(s_dispatchTable))
	{
		s_srv = NULL;
		return CHECK_ERROR(LastError());
	}

	s_srv = NULL;

	return 0;
}

result_t Service::isInstalled(bool& retVal)
{
	SC_HANDLE   schService;
	SC_HANDLE   schSCManager;

	retVal = false;

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schSCManager)
		return CHECK_ERROR(LastError());

	schService = OpenServiceW(schSCManager, UTF8_W(m_name), SERVICE_ALL_ACCESS);
	if (schService)
	{
		retVal = true;
		CloseServiceHandle(schService);
	}
	CloseServiceHandle(schSCManager);

	return 0;
}

result_t Service::isRunning(bool& retVal)
{
	SC_HANDLE   schService;
	SC_HANDLE   schSCManager;
	SERVICE_STATUS ssStatus;

	retVal = false;

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schSCManager)
		return CHECK_ERROR(LastError());

	schService = OpenServiceW(schSCManager, UTF8_W(m_name), SERVICE_ALL_ACCESS);
	if (schService)
	{
		if (QueryServiceStatus(schService, &ssStatus) && ssStatus.dwCurrentState == SERVICE_RUNNING)
			retVal = TRUE;

		CloseServiceHandle(schService);
	}
	CloseServiceHandle(schSCManager);

	return 0;
}

}

#endif