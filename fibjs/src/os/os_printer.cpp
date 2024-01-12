/*
 * os_win.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/os.h"

#ifdef _WIN32

#include "BufferedStream.h"

namespace fibjs {

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
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Array> ret;
    ret = v8::Array::New(isolate->m_isolate);
    retVal = ret;

    for (DWORD i = 0; i < dwReturned; i++) {
        v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);
        PRINTER_INFO_5W* pItem = &pinfo[i];
        o->Set(context, isolate->NewString("name"), isolate->NewString(utf16to8String((const char16_t*)pItem->pPrinterName)));
        o->Set(context, isolate->NewString("port"), isolate->NewString(utf16to8String((const char16_t*)pItem->pPortName)));
        if (PRINTER_ATTRIBUTE_LOCAL & pItem->Attributes)
            o->Set(context, isolate->NewString("type"), isolate->NewString("local"));
        else if (PRINTER_ATTRIBUTE_NETWORK & pItem->Attributes)
            o->Set(context, isolate->NewString("type"), isolate->NewString("network"));

        if (!qstrcmp(pname, pItem->pPrinterName))
            o->Set(context, isolate->NewString("default"), v8::True(isolate->m_isolate));

        ret->Set(context, ret->Length(), o);
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
        result_t get_fd(int32_t& retVal)
        {
            retVal = *(int32_t*)&m_hPrinter;

            return 0;
        }

        result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
        {
            return CALL_E_INVALID_CALL;
        }

        result_t write(Buffer_base* data, AsyncEvent* ac)
        {
            if (ac->isSync())
                return CHECK_ERROR(CALL_E_LONGSYNC);

            DOC_INFO_1 DocInfo;

            DocInfo.pDocName = (LPSTR)"My Document";
            DocInfo.pOutputFile = NULL;
            DocInfo.pDatatype = (LPSTR)"RAW";

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

        result_t flush(AsyncEvent* ac)
        {
            return 0;
        }

        result_t close(AsyncEvent* ac)
        {
            if (ac->isSync())
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

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    HANDLE hPrinter;
    if (!OpenPrinterW((LPWSTR)UTF8_W(name.c_str()), &hPrinter, NULL))
        return CHECK_ERROR(LastError());

    obj_ptr<Stream_base> stm = new PrinterStream(hPrinter);
    retVal = new BufferedStream(stm);
    return 0;
}
}

#else

namespace fibjs {

result_t os_base::printerInfo(v8::Local<v8::Array>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t os_base::openPrinter(exlib::string name, obj_ptr<BufferedStream_base>& retVal,
    AsyncEvent* ac)
{
    return CALL_E_INVALID_CALL;
}
}

#endif
