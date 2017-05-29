/*
 * os_win.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: lion
 */

#include <exlib/include/osconfig.h>

#ifdef _WIN32

#include "object.h"
#include "ifs/os.h"
#include "ifs/path.h"
#include "ifs/process.h"
#include <iphlpapi.h>
#include <psapi.h>
#include "utf8.h"
#include "inetAddr.h"
#include "BufferedStream.h"

typedef struct
    _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER DpcTime;
    LARGE_INTEGER InterruptTime;
    ULONG Reserved2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

typedef LONG(WINAPI* PROCNTQSI)(UINT, PVOID, ULONG, PULONG);
static PROCNTQSI pNtQuerySystemInformation;

#define SystemBasicInformation 0
#define SystemPerformanceInformation 2
#define SystemTimeInformation 3
#define SystemProcessorPerformanceInformation 8

namespace fibjs {

result_t os_base::get_type(exlib::string& retVal)
{
    retVal = "Windows";
    return 0;
}

result_t os_base::platform(exlib::string& retVal)
{
    retVal = "win32";
    return 0;
}

typedef void(WINAPI* RtlGetVersion_FUNC)(OSVERSIONINFOEXW*);

BOOL GetVersion2(OSVERSIONINFOEX* os)
{
    HMODULE hMod;
    RtlGetVersion_FUNC func;
#ifdef UNICODE
    OSVERSIONINFOEXW* osw = os;
#else
    OSVERSIONINFOEXW o;
    OSVERSIONINFOEXW* osw = &o;
#endif

    hMod = LoadLibrary(TEXT("ntdll.dll"));
    if (hMod) {
        func = (RtlGetVersion_FUNC)GetProcAddress(hMod, "RtlGetVersion");
        if (func == 0) {
            FreeLibrary(hMod);
            return FALSE;
        }
        ZeroMemory(osw, sizeof(*osw));
        osw->dwOSVersionInfoSize = sizeof(*osw);
        func(osw);
#ifndef UNICODE
        os->dwBuildNumber = osw->dwBuildNumber;
        os->dwMajorVersion = osw->dwMajorVersion;
        os->dwMinorVersion = osw->dwMinorVersion;
        os->dwPlatformId = osw->dwPlatformId;
        os->dwOSVersionInfoSize = sizeof(*os);
        DWORD sz = sizeof(os->szCSDVersion);
        WCHAR* src = osw->szCSDVersion;
        unsigned char* dtc = (unsigned char*)os->szCSDVersion;
        while (*src)
            *dtc++ = (unsigned char)*src++;
        *dtc = '\0';
#endif

    } else
        return FALSE;
    FreeLibrary(hMod);
    return TRUE;
}

result_t os_base::get_version(exlib::string& retVal)
{
    OSVERSIONINFOEX info = { sizeof(info) };
    char release[256];

    if (GetVersion2(&info) == 0)
        return CHECK_ERROR(LastError());

    sprintf(release, "%d.%d.%d", static_cast<int32_t>(info.dwMajorVersion),
        static_cast<int32_t>(info.dwMinorVersion), static_cast<int32_t>(info.dwBuildNumber));
    retVal = release;

    return 0;
}

result_t os_base::get_EOL(exlib::string& retVal)
{
    retVal.assign("\r\n", 2);
    return 0;
}

result_t os_base::uptime(double& retVal)
{
    BYTE stack_buffer[4096];
    BYTE* malloced_buffer = NULL;
    BYTE* buffer = (BYTE*)stack_buffer;
    size_t buffer_size = sizeof(stack_buffer);
    DWORD data_size;

    PERF_DATA_BLOCK* data_block;
    PERF_OBJECT_TYPE* object_type;
    PERF_COUNTER_DEFINITION* counter_definition;

    DWORD i;

    for (;;) {
        LONG result;

        data_size = (DWORD)buffer_size;
        result = RegQueryValueExW(HKEY_PERFORMANCE_DATA, L"2", NULL, NULL,
            buffer, &data_size);
        if (result == ERROR_SUCCESS) {
            break;
        } else if (result != ERROR_MORE_DATA)
            return CHECK_ERROR(LastError());

        free(malloced_buffer);

        buffer_size *= 2;
        /* Don't let the buffer grow infinitely. */
        if (buffer_size > 1 << 20) {
            goto internalError;
        }

        buffer = malloced_buffer = (BYTE*)malloc(buffer_size);
        if (malloced_buffer == NULL)
            return CHECK_ERROR(LastError());
    }

    if (data_size < sizeof(*data_block))
        goto internalError;

    data_block = (PERF_DATA_BLOCK*)buffer;

    if (wmemcmp(data_block->Signature, L"PERF", 4) != 0)
        goto internalError;

    if (data_size < data_block->HeaderLength + sizeof(*object_type))
        goto internalError;

    object_type = (PERF_OBJECT_TYPE*)(buffer + data_block->HeaderLength);

    if (object_type->NumInstances != PERF_NO_INSTANCES)
        goto internalError;

    counter_definition = (PERF_COUNTER_DEFINITION*)(buffer
        + data_block->HeaderLength + object_type->HeaderLength);
    for (i = 0; i < object_type->NumCounters; i++) {
        if ((BYTE*)counter_definition + sizeof(*counter_definition)
            > buffer + data_size) {
            break;
        }

        if (counter_definition->CounterNameTitleIndex == 674
            && counter_definition->CounterSize == sizeof(uint64_t)) {
            if (counter_definition->CounterOffset + sizeof(uint64_t) > data_size
                || !(counter_definition->CounterType & PERF_OBJECT_TIMER)) {
                goto internalError;
            } else {
                BYTE* address = (BYTE*)object_type
                    + object_type->DefinitionLength
                    + counter_definition->CounterOffset;
                uint64_t value = *((uint64_t*)address);
                retVal = (double)(object_type->PerfTime.QuadPart - value)
                    / (double)object_type->PerfFreq.QuadPart;
                free(malloced_buffer);
                return 0;
            }
        }

        counter_definition = (PERF_COUNTER_DEFINITION*)((BYTE*)counter_definition
            + counter_definition->ByteLength);
    }

    /* If we get here, the uptime value was not found. */
    free(malloced_buffer);
    return CHECK_ERROR(LastError());

internalError:
    free(malloced_buffer);
    return CHECK_ERROR(LastError());
}

result_t os_base::loadavg(v8::Local<v8::Array>& retVal)
{
    double avg[3] = { 0, 0, 0 };

    Isolate* isolate = Isolate::current();
    retVal = v8::Array::New(isolate->m_isolate, 3);
    retVal->Set(0, v8::Number::New(isolate->m_isolate, avg[0]));
    retVal->Set(1, v8::Number::New(isolate->m_isolate, avg[1]));
    retVal->Set(2, v8::Number::New(isolate->m_isolate, avg[2]));

    return 0;
}

result_t os_base::totalmem(int64_t& retVal)
{
    MEMORYSTATUSEX memory_status;
    memory_status.dwLength = sizeof(memory_status);

    if (!GlobalMemoryStatusEx(&memory_status))
        return CHECK_ERROR(LastError());

    retVal = memory_status.ullTotalPhys;
    return 0;
}

result_t os_base::freemem(int64_t& retVal)
{
    MEMORYSTATUSEX memory_status;
    memory_status.dwLength = sizeof(memory_status);

    if (!GlobalMemoryStatusEx(&memory_status))
        return CHECK_ERROR(LastError());

    retVal = memory_status.ullAvailPhys;
    return 0;
}

result_t os_base::CPUs(int32_t& retVal)
{
    static int32_t cpus = 0;

    if (cpus > 0) {
        retVal = cpus;
        return 0;
    }

    SYSTEM_INFO system_info;

    GetSystemInfo(&system_info);
    cpus = system_info.dwNumberOfProcessors;

    retVal = cpus;

    return 0;
}

result_t os_base::CPUInfo(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = v8::Array::New(isolate->m_isolate);

    SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION* sppi;
    DWORD sppi_size;
    SYSTEM_INFO system_info;
    DWORD cpu_count, i, r;
    ULONG result_size;
    result_t hr = 0;

    if (pNtQuerySystemInformation == NULL)
        pNtQuerySystemInformation = (PROCNTQSI)GetProcAddress(
            GetModuleHandle("ntdll"), "NtQuerySystemInformation");

    if (pNtQuerySystemInformation == NULL)
        return CHECK_ERROR(LastError());

    GetSystemInfo(&system_info);
    cpu_count = system_info.dwNumberOfProcessors;

    sppi_size = sizeof(*sppi) * cpu_count;
    sppi = (SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION*)malloc(sppi_size);
    if (!sppi)
        return CHECK_ERROR(LastError());

    r = pNtQuerySystemInformation(SystemProcessorPerformanceInformation, sppi,
        sppi_size, &result_size);
    if (r != ERROR_SUCCESS || result_size != sppi_size) {
        hr = LastError();
        free(sppi);
        return hr;
    }

    for (i = 0; i < cpu_count; i++) {
        exlib::wchar key_name[128];
        HKEY processor_key;
        DWORD cpu_speed;
        DWORD cpu_speed_size = sizeof(cpu_speed);
        char cpu_brand[256];
        DWORD cpu_brand_size = sizeof(cpu_brand);

        _snwprintf(key_name, sizeof(key_name) / sizeof(key_name[0]),
            L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d", i);

        r = RegOpenKeyExW(HKEY_LOCAL_MACHINE, key_name, 0, KEY_QUERY_VALUE,
            &processor_key);
        if (r != ERROR_SUCCESS) {
            hr = LastError();
            free(sppi);
            return hr;
        }

        if (RegQueryValueExW(processor_key, L"~MHz", NULL, NULL,
                (BYTE*)&cpu_speed, &cpu_speed_size)
            != ERROR_SUCCESS) {
            hr = LastError();
            RegCloseKey(processor_key);
            free(sppi);
            return hr;
        }

        if (RegQueryValueExA(processor_key, "ProcessorNameString", NULL, NULL,
                (BYTE*)&cpu_brand, &cpu_brand_size)
            != ERROR_SUCCESS) {
            hr = LastError();
            RegCloseKey(processor_key);
            free(sppi);
            return hr;
        }

        RegCloseKey(processor_key);

        v8::Local<v8::Object> times_info = v8::Object::New(isolate->m_isolate);
        times_info->Set(isolate->NewFromUtf8("user"),
            v8::Integer::New(isolate->m_isolate,
                (int32_t)(sppi[i].UserTime.QuadPart / 10000)));
        times_info->Set(isolate->NewFromUtf8("nice"), v8::Integer::New(isolate->m_isolate, 0));
        times_info->Set(isolate->NewFromUtf8("sys"),
            v8::Integer::New(isolate->m_isolate,
                (int32_t)(sppi[i].KernelTime.QuadPart
                    - sppi[i].IdleTime.QuadPart)
                    / 10000));
        times_info->Set(isolate->NewFromUtf8("idle"),
            v8::Integer::New(isolate->m_isolate,
                (int32_t)(sppi[i].IdleTime.QuadPart / 10000)));
        times_info->Set(isolate->NewFromUtf8("irq"),
            v8::Integer::New(isolate->m_isolate,
                (int32_t)(sppi[i].InterruptTime.QuadPart / 10000)));

        v8::Local<v8::Object> cpu_info = v8::Object::New(isolate->m_isolate);
        cpu_info->Set(isolate->NewFromUtf8("model"), isolate->NewFromUtf8(cpu_brand));
        cpu_info->Set(isolate->NewFromUtf8("speed"), v8::Integer::New(isolate->m_isolate, cpu_speed));
        cpu_info->Set(isolate->NewFromUtf8("times"), times_info);
        retVal->Set(i, cpu_info);
    }

    return hr;
}

result_t os_base::networkInterfaces(v8::Local<v8::Object>& retVal)
{
    unsigned long size = 0;
    IP_ADAPTER_ADDRESSES* adapter_addresses;
    IP_ADAPTER_ADDRESSES* adapter_address;
    IP_ADAPTER_UNICAST_ADDRESS_XP* unicast_address;

    if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, NULL, &size)
        != ERROR_BUFFER_OVERFLOW)
        return CHECK_ERROR(LastError());

    adapter_addresses = (IP_ADAPTER_ADDRESSES*)malloc(size);
    if (!adapter_addresses)
        return CHECK_ERROR(LastError());

    if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, adapter_addresses, &size)
        != ERROR_SUCCESS)
        return CHECK_ERROR(LastError());

    Isolate* isolate = Isolate::current();
    retVal = v8::Object::New(isolate->m_isolate);

    for (adapter_address = adapter_addresses; adapter_address != NULL;
         adapter_address = adapter_address->Next) {
        v8::Local<v8::Array> ret;
        v8::Local<v8::Object> o;
        v8::Local<v8::String> name, ipaddr, family;

        if (adapter_address->OperStatus != IfOperStatusUp)
            continue;

        name = isolate->NewFromUtf8(UTF8_A(adapter_address->FriendlyName));

        ret = v8::Array::New(isolate->m_isolate);
        retVal->Set(name, ret);

        unicast_address = (IP_ADAPTER_UNICAST_ADDRESS_XP*)adapter_address->FirstUnicastAddress;
        while (unicast_address) {
            inetAddr* sock_addr = (inetAddr*)unicast_address->Address.lpSockaddr;

            o = v8::Object::New(isolate->m_isolate);
            o->Set(isolate->NewFromUtf8("address"), isolate->NewFromUtf8(sock_addr->str()));
            o->Set(isolate->NewFromUtf8("family"), sock_addr->family() == net_base::_AF_INET6 ? isolate->NewFromUtf8("IPv6") : isolate->NewFromUtf8("IPv4"));
            o->Set(isolate->NewFromUtf8("internal"),
                adapter_address->IfType == IF_TYPE_SOFTWARE_LOOPBACK ? v8::True(isolate->m_isolate) : v8::False(isolate->m_isolate));

            ret->Set(ret->Length(), o);

            unicast_address = unicast_address->Next;
        }
    }

    free(adapter_addresses);

    return 0;
}

result_t os_base::printerInfo(v8::Local<v8::Array>& retVal)
{
    wchar_t pname[256];
    DWORD dwSize = 256;
    DWORD dwNeeded, dwReturned;
    PRINTER_INFO_5W* pinfo;

    GetDefaultPrinterW(pname, &dwSize);

    EnumPrintersW(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,
        NULL, 5, NULL, 0, &dwNeeded, &dwReturned);

    pinfo = (PRINTER_INFO_5W*)malloc(dwNeeded);

    EnumPrintersW(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,
        NULL, 5, (PBYTE)pinfo, dwNeeded, &dwNeeded, &dwReturned);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Array> ret;
    ret = v8::Array::New(isolate->m_isolate);
    retVal = ret;

    for (DWORD i = 0; i < dwReturned; i++) {
        v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);
        PRINTER_INFO_5W* pItem = &pinfo[i];
        o->Set(isolate->NewFromUtf8("name"), isolate->NewFromUtf8(UTF8_A(pItem->pPrinterName)));
        o->Set(isolate->NewFromUtf8("port"), isolate->NewFromUtf8(UTF8_A(pItem->pPortName)));
        if (PRINTER_ATTRIBUTE_LOCAL & pItem->Attributes)
            o->Set(isolate->NewFromUtf8("type"), isolate->NewFromUtf8("local"));
        else if (PRINTER_ATTRIBUTE_NETWORK & pItem->Attributes)
            o->Set(isolate->NewFromUtf8("type"), isolate->NewFromUtf8("network"));

        if (!qstrcmp(pname, pItem->pPrinterName))
            o->Set(isolate->NewFromUtf8("default"), v8::True(isolate->m_isolate));

        ret->Set(ret->Length(), o);
    }
    free(pinfo);

    return 0;
}

result_t os_base::openPrinter(exlib::string name, obj_ptr<BufferedStream_base>& retVal,
    AsyncEvent* ac)
{
    class PrinterStream : public Stream_base {
    public:
        PrinterStream(HANDLE hPrinter)
            : m_hPrinter(hPrinter)
        {
        }

        ~PrinterStream()
        {
            if (m_hPrinter) {
                ClosePrinter(m_hPrinter);
                m_hPrinter = NULL;
            }
        }

    public:
        // Stream_base
        result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
        {
            return CALL_E_INVALID_CALL;
        }

        result_t write(Buffer_base* data, AsyncEvent* ac)
        {
            if (!ac)
                return CHECK_ERROR(CALL_E_LONGSYNC);

            DOC_INFO_1 DocInfo;

            DocInfo.pDocName = "My Document";
            DocInfo.pOutputFile = NULL;
            DocInfo.pDatatype = "RAW";

            if (!StartDocPrinter(m_hPrinter, 1, (LPBYTE)&DocInfo))
                return CHECK_ERROR(LastError());

            if (!StartPagePrinter(m_hPrinter)) {
                EndDocPrinter(m_hPrinter);
                return CHECK_ERROR(LastError());
            }

            DWORD dwBytesWritten;
            exlib::string strData;

            data->toString(strData);

            if (!WritePrinter(m_hPrinter, (void*)strData.c_str(),
                    (DWORD)strData.length(), &dwBytesWritten)) {
                EndPagePrinter(m_hPrinter);
                EndDocPrinter(m_hPrinter);
                return CHECK_ERROR(LastError());
            }

            EndPagePrinter(m_hPrinter);
            EndDocPrinter(m_hPrinter);

            return 0;
        }

        result_t close(AsyncEvent* ac)
        {
            if (!ac)
                return CHECK_ERROR(CALL_E_LONGSYNC);

            if (m_hPrinter) {
                ClosePrinter(m_hPrinter);
                m_hPrinter = NULL;
            }

            return 0;
        }

        result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac)
        {
            return CALL_E_INVALID_CALL;
        }

    private:
        HANDLE m_hPrinter;
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_LONGSYNC);

    HANDLE hPrinter;
    if (!OpenPrinterW((LPWSTR)UTF8_W(name.c_str()), &hPrinter, NULL))
        return CHECK_ERROR(LastError());

    obj_ptr<Stream_base> stm = new PrinterStream(hPrinter);
    retVal = new BufferedStream(stm);
    return 0;
}

result_t os_base::get_execPath(exlib::string& retVal)
{
    exlib::wchar szFileName[MAX_PATH];

    GetModuleFileNameW(NULL, szFileName, MAX_PATH);
    retVal = utf16to8String(szFileName);
    return 0;
}

result_t os_base::memoryUsage(v8::Local<v8::Object>& retVal)
{
    size_t rss = 0;

    HANDLE current_process;
    PROCESS_MEMORY_COUNTERS pmc;

    current_process = GetCurrentProcess();

    if (!GetProcessMemoryInfo(current_process, &pmc, sizeof(pmc)))
        return CHECK_ERROR(LastError());

    rss = pmc.WorkingSetSize;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> info = v8::Object::New(isolate->m_isolate);

    v8::HeapStatistics v8_heap_stats;
    isolate->m_isolate->GetHeapStatistics(&v8_heap_stats);
    info->Set(isolate->NewFromUtf8("rss"), v8::Number::New(isolate->m_isolate, (double)rss));
    info->Set(isolate->NewFromUtf8("heapTotal"),
        v8::Number::New(isolate->m_isolate, (double)v8_heap_stats.total_heap_size()));
    info->Set(isolate->NewFromUtf8("heapUsed"),
        v8::Number::New(isolate->m_isolate, (double)v8_heap_stats.used_heap_size()));

    v8::Local<v8::Object> objs;
    object_base::class_info().dump(objs);
    info->Set(isolate->NewFromUtf8("nativeObjects"), objs);

    retVal = info;

    return 0;
}

result_t os_base::tmpdir(exlib::string& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> env;
    process_base::get_env(env);

    do {
        GetConfigValue(isolate->m_isolate, env, "TEMP", retVal, true);
        if (!retVal.empty())
            break;

        GetConfigValue(isolate->m_isolate, env, "TMP", retVal, true);
        if (!retVal.empty())
            break;

        GetConfigValue(isolate->m_isolate, env, "SystemRoot", retVal, true);
        if (!retVal.empty()) {
            retVal.append("\\temp", 5);
            break;
        }

        GetConfigValue(isolate->m_isolate, env, "windir", retVal, true);
        if (!retVal.empty()) {
            retVal.append("\\temp", 5);
            break;
        }

        WCHAR buf[MAX_PATH + 1];
        DWORD sz;

        sz = GetTempPathW(MAX_PATH + 1, buf);
        if (sz == 0)
            return CHECK_ERROR(LastError());

        retVal = UTF8_A(buf);
    } while (false);

    path_base::normalize(retVal, retVal);

    if (retVal.length() > 1 && isPathSlash(retVal[retVal.length() - 1]) && retVal[retVal.length() - 2] != ':')
        retVal.resize(retVal.length() - 1);

    return 0;
}

result_t process_base::cwd(exlib::string& retVal)
{
    DWORD utf16_len;
    exlib::wchar utf16_buffer[MAX_PATH];

    utf16_len = GetCurrentDirectoryW(MAX_PATH, utf16_buffer);
    if (utf16_len == 0)
        return CHECK_ERROR(LastError());

    utf16_buffer[utf16_len] = L'\0';

    if (utf16_buffer[utf16_len - 1] == L'\\' && !(utf16_len == 3 && utf16_buffer[1] == L':')) {
        utf16_len--;
        utf16_buffer[utf16_len] = L'\0';
    }

    retVal = utf16to8String(utf16_buffer, (int32_t)utf16_len);

    return 0;
}

result_t process_base::chdir(exlib::string directory)
{
    exlib::wstring str = utf8to16String(directory);
    exlib::wchar utf16_buffer[MAX_PATH];
    DWORD utf16_len;
    exlib::wchar drive_letter;

    if (!SetCurrentDirectoryW(str.c_str()))
        return CHECK_ERROR(LastError());

    utf16_len = GetCurrentDirectoryW(MAX_PATH, utf16_buffer);
    if (utf16_len == 0)
        return CHECK_ERROR(LastError());

    if (utf16_buffer[utf16_len - 1] == L'\\' && !(utf16_len == 3 && utf16_buffer[1] == L':')) {
        utf16_len--;
        utf16_buffer[utf16_len] = L'\0';
    }

    if (utf16_len < 2 || utf16_buffer[1] != L':')
        drive_letter = 0;
    else if (utf16_buffer[0] >= L'A' && utf16_buffer[0] <= L'Z')
        drive_letter = utf16_buffer[0];
    else if (utf16_buffer[0] >= L'a' && utf16_buffer[0] <= L'z')
        drive_letter = utf16_buffer[0] - L'a' + L'A';
    else
        drive_letter = 0;

    if (drive_letter != 0) {
        exlib::wchar env_var[4];

        env_var[0] = L'=';
        env_var[1] = drive_letter;
        env_var[2] = L':';
        env_var[3] = L'\0';

        if (!SetEnvironmentVariableW(env_var, utf16_buffer))
            return CHECK_ERROR(LastError());
    }

    return 0;
}
}

#endif
