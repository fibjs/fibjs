/*
 * winreg.cpp
 *
 *  Created on: 2020-05-05
 *      Author: myy
 */

#include <exlib/include/osconfig.h>

#ifdef _WIN32

#include "object.h"
#include "ifs/winreg.h"
#include "utf8.h"
#include "Buffer.h"

namespace fibjs {

DECLARE_MODULE(winreg);

class Registry {
public:
    Registry() : hKey(0) 
	{
    }

    ~Registry()
    {
       if (hKey) Windows::RegCloseKey(hKey);
	   hKey = 0;
    }

	result_t parse(exlib::string key, HKEY& root, exlib::string& path) 
	{
		root = 0;
		path = "";

		static HKEY s_root_hkey[] = {
			HKEY_CLASSES_ROOT,
			HKEY_CURRENT_USER,
			HKEY_LOCAL_MACHINE,
			HKEY_USERS,
			HKEY_PERFORMANCE_DATA,
			HKEY_CURRENT_CONFIG
		};

		static char* s_root_name1[] = {
			"HKEY_CLASSES_ROOT\\",
			"HKEY_CURRENT_USER\\",
			"HKEY_LOCAL_MACHINE\\",
			"HKEY_USERS\\",
			"HKEY_PERFORMANCE_DATA\\",
			"HKEY_CURRENT_CONFIG\\"
		};

		static char* s_root_name2[] = {
			"HKCR\\",
			"HKCU\\",
			"HKLM\\",
			"HKU\\",
			"HKPD\\",
			"HKCC\\"
		};

		exlib::string k(key);
		for (int i = 0; i < k.length(); i++) {
			if (k[i] == '/') k[i] = '\\';
		}

		exlib::string u(k); u.toupper();

		for (int i = 0; i < 6; i++) {
			if (0 == u.find(s_root_name1[i], 0)) {
				root = s_root_hkey[i];
				path = k.substr(strlen(s_root_name1[i]));
				break;
			}
			if (0 == u.find(s_root_name2[i], 0)) {
				root = s_root_hkey[i];
				path = k.substr(strlen(s_root_name2[i]));
				break;
			}
		}
		return 0;
	}
	
	result_t open(exlib::string key, bool bWrite = false, bool bCreate = false)
    {
		Windows::HKEY root = 0;
		exlib::string path;

		parse(key, root, path);
		
		if (0 == root)
            return CHECK_ERROR(CALL_E_INVALIDARG);

	    sKey = utf8to16String(path);

        DWORD type = bWrite ? KEY_WRITE : KEY_READ;

        LONG lResult = RegOpenKeyExW(root, sKey.c_str(), 0, type, &hKey);
        if (lResult != ERROR_SUCCESS) {
            if (bCreate && lResult == ERROR_FILE_NOT_FOUND) {
                DWORD dwFunc;
                lResult = RegCreateKeyExW(root, sKey.c_str(), 0, (LPWSTR)NULL,
                    REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey,
                    &dwFunc);

                if (lResult != ERROR_SUCCESS)
                    return CHECK_ERROR(-lResult);

                return 0;
            }

            return CHECK_ERROR(-lResult);
        }

        return 0;
    }

public:
    Windows::HKEY   hKey;  // 打开的 key 句柄
	exlib::wstring  sKey;  // 打开的 key 路径（不包括 Root）
};

#define MAX_KEY_LENGTH 255

result_t winreg_base::listSubKey(exlib::string key, obj_ptr<NArray>& retVal)
{
    Registry r;
    result_t hr = r.open(key);
    if (hr < 0)
        return hr;

    wchar_t achClass[MAX_PATH] = L"";
    DWORD cchClassName = MAX_PATH, cSubKeys = 0;
    DWORD cbMaxSubKey, cchMaxClass, cValues;
    DWORD cchMaxValue, cbMaxValueData, cbSecurityDescriptor;
    FILETIME ftLastWriteTime;
    DWORD i, retCode;

    retCode = RegQueryInfoKeyW(r.hKey, achClass, &cchClassName, NULL, &cSubKeys,
        &cbMaxSubKey, &cchMaxClass, &cValues, &cchMaxValue,
        &cbMaxValueData, &cbSecurityDescriptor,
        &ftLastWriteTime);

    wchar_t achValue[MAX_KEY_LENGTH];
    DWORD cchValue;

    obj_ptr<NArray> l = new NArray();

    if (cSubKeys)
        for (i = 0; i < cSubKeys; i++) {
            cchValue = MAX_KEY_LENGTH;
            retCode = RegEnumKeyExW(r.hKey, i, achValue, &cchValue, NULL,
                NULL, NULL, &ftLastWriteTime);
            if (retCode == ERROR_SUCCESS)
                l->append(utf16to8String(achValue, cchValue));
        }

    retVal = l;

    return 0;
}

result_t winreg_base::listValueName(exlib::string key, obj_ptr<NArray>& retVal)
{
    Registry r;
    result_t hr = r.open(key);
    if (hr < 0)
        return hr;

    wchar_t achClass[MAX_PATH] = L"";
    DWORD cchClassName = MAX_PATH, cSubKeys = 0;
    DWORD cbMaxSubKey, cchMaxClass, cValues;
    DWORD cchMaxValue, cbMaxValueData, cbSecurityDescriptor;
    FILETIME ftLastWriteTime;
    DWORD i, retCode;

    retCode = RegQueryInfoKeyW(r.hKey, achClass, &cchClassName, NULL, &cSubKeys,
        &cbMaxSubKey, &cchMaxClass, &cValues, &cchMaxValue,
        &cbMaxValueData, &cbSecurityDescriptor,
        &ftLastWriteTime);

    wchar_t achValue[MAX_KEY_LENGTH];
    DWORD cchValue;

    obj_ptr<NArray> l = new NArray();

    if (cValues)
        for (i = 0; i < cValues; i++) {
            cchValue = MAX_KEY_LENGTH;
            retCode = RegEnumValueW(r.hKey, i, achValue, &cchValue, NULL,
                NULL, NULL, NULL);
            if (retCode == ERROR_SUCCESS)
                l->append(utf16to8String(achValue, cchValue));
        }

    retVal = l;

    return 0;
}

result_t winreg_base::get(exlib::string key, exlib::string name, v8::Local<v8::Value>& retVal)
{
    Registry r;
    result_t hr = r.open(key);
    if (hr < 0)
        return hr;

    DWORD dwType = 0, dwSize = 0;

	exlib::wstring  wname = utf8to16String(name);

    LONG lResult = RegQueryValueExW(r.hKey, wname.c_str(), NULL, &dwType, NULL, &dwSize);
    if (lResult != ERROR_SUCCESS)
        return CHECK_ERROR(-lResult);

    switch (dwType) {
    case REG_DWORD: {
        Isolate* isolate = Isolate::current();
        int32_t value = 0;
        dwSize = sizeof(value);

        RegQueryValueExW(r.hKey, wname.c_str(), NULL, &dwType, (LPBYTE)&value, &dwSize);
        retVal = GetReturnValue(isolate->m_isolate, value);
        break;
    }
    case REG_QWORD: {
        Isolate* isolate = Isolate::current();
        int64_t value = 0;
        dwSize = sizeof(value);

        RegQueryValueExW(r.hKey, wname.c_str(), NULL, &dwType, (LPBYTE)&value, &dwSize);
        retVal = GetReturnValue(isolate->m_isolate, value);
        break;
    }
    case REG_SZ:
    case REG_EXPAND_SZ: {
        Isolate* isolate = Isolate::current();
        exlib::wstring buf;
        exlib::string sbuf;

        buf.resize(dwSize / 2 - 1);
        RegQueryValueExW(r.hKey, wname.c_str(), NULL, &dwType, (LPBYTE)buf.c_buffer(), &dwSize);

        sbuf = utf16to8String(buf);
        retVal = GetReturnValue(isolate->m_isolate, sbuf);
        break;
    }
    case REG_MULTI_SZ: {
        Isolate* isolate = Isolate::current();
        exlib::wstring buf;
        exlib::string sbuf;

        buf.resize(dwSize / 2);
        RegQueryValueExW(r.hKey, wname.c_str(), NULL, &dwType, (LPBYTE)buf.c_buffer(), &dwSize);

        v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate);
        int32_t n = 0;

        const wchar_t* p1 = buf.c_str();
        const wchar_t* pn = p1 + dwSize / 2;
        while ((p1 < pn) && *p1) {
            const wchar_t* p2 = p1;
            while (*p2)
                p2++;

            sbuf = utf16to8String(p1, (int32_t)(p2 - p1));
            arr->Set(n++, GetReturnValue(isolate->m_isolate, sbuf));
            p1 = p2 + 1;
        }

        retVal = arr;
        break;
    }
    case REG_BINARY:
    case REG_NONE: {
        exlib::string sbuf;
        obj_ptr<Buffer_base> buf;

        sbuf.resize(dwSize);
        RegQueryValueExW(r.hKey, wname.c_str(), NULL, &dwType, (LPBYTE)sbuf.c_buffer(), &dwSize);

        buf = new Buffer(sbuf);
        retVal = buf->wrap();

        break;
    }
    default:
        return CHECK_ERROR(CALL_E_RETURN_TYPE);
    }

    return 0;
}

result_t set_reg(exlib::string key, exlib::string name, int32_t type, const void* data, int32_t size)
{
    Registry r;
    result_t hr = r.open(key, true, true);
    if (hr < 0)
        return hr;

	exlib::wstring  wname = utf8to16String(name);

    LONG lResult = RegSetValueExW(r.hKey, wname.c_str(), 0, type, (const BYTE*)data, size);
    if (lResult != ERROR_SUCCESS)
        return CHECK_ERROR(-lResult);

    return 0;
}

result_t winreg_base::set_dword(exlib::string key, exlib::string name, int32_t value)
{
	return set_reg(key, name, REG_DWORD, &value, sizeof(int32_t));
}

result_t winreg_base::set_qword(exlib::string key, exlib::string name, int64_t value)
{
	return set_reg(key, name, REG_QWORD, &value, sizeof(int64_t));
}

result_t winreg_base::set_sz(exlib::string key, exlib::string name, exlib::string value)
{
	exlib::wstring  wv = utf8to16String(value);
	//wv.append((wchar_t)'\x0');
	return set_reg(key, name, REG_SZ, wv.c_str(), wv.length() * sizeof(wchar_t));
}

result_t winreg_base::set_expand_sz(exlib::string key, exlib::string name, exlib::string value)
{
	exlib::wstring  wv = utf8to16String(value);
	//wv.append((wchar_t)'\x0');
	return set_reg(key, name, REG_EXPAND_SZ, wv.c_str(), wv.length() * sizeof(wchar_t));
}

result_t winreg_base::set_multi_sz(exlib::string key, exlib::string name, v8::Local<v8::Array> value)
{
	exlib::wstring  wv;
	
	int32_t len = value->Length();
	int32_t i;

	for(i = 0; i < len; i++) {
		exlib::string v;
		result_t hr = GetArgumentValue(value->Get(i), v, false);
		if (hr < 0)
			return hr;

		exlib::wstring w = utf8to16String(v);
		wv.append(w.c_str(), w.length() + 1);
		//wv.append((wchar_t)'\x0');
	}

	return set_reg(key, name, REG_MULTI_SZ, wv.c_str(), wv.length() * sizeof(wchar_t));
}

result_t winreg_base::set_binary(exlib::string key, exlib::string name, Buffer_base* value)
{
	exlib::string v;
	value->toString(v);
	return set_reg(key, name, REG_BINARY, v.c_str(), v.length());
}

result_t winreg_base::delKey(exlib::string key)
{
	Registry r;

	Windows::HKEY root = 0;
	exlib::string path;

	r.parse(key, root, path);
	if (0 == root)
		return 0;

	exlib::wstring wpath = utf8to16String(path);
	
	LONG lResult = RegDeleteKeyExW(root, wpath.c_str(), 0, 0);
	if (lResult == ERROR_FILE_NOT_FOUND)
		return 0;

	if (lResult != ERROR_SUCCESS)
		return CHECK_ERROR(-lResult);

	return 0;
}

result_t winreg_base::delValue(exlib::string key, exlib::string name)
{
	Registry r;
	result_t hr = r.open(key, true, false);
	if (hr < 0)
		return hr;

	exlib::wstring  wname = utf8to16String(name);

	LONG lResult = RegDeleteValueW(r.hKey, wname.c_str());
	if (lResult == ERROR_FILE_NOT_FOUND)
		return 0;

	if (lResult != ERROR_SUCCESS)
		return CHECK_ERROR(-lResult);

	return 0;
}
}

#endif
