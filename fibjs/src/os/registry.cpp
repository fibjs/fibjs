/*
 * registry.cpp
 *
 *  Created on: Mar 2, 2017
 *      Author: lion
 */


#include <exlib/include/osconfig.h>

#ifdef _WIN32

#include "object.h"
#include "ifs/registry.h"
#include "ifs/path.h"
#include "utf8.h"
#include "Buffer.h"
#include "List.h"

namespace fibjs
{

DECLARE_MODULE(registry);

class Registry
{
public:
	Registry() :  hKey(0)
	{}

	~Registry()
	{
		if (hKey)
			RegCloseKey(hKey);
	}

	result_t open(int32_t root, exlib::string key, bool bWrite = false, bool bCreate = false)
	{
		static HKEY s_hkeys[] = {
			HKEY_CLASSES_ROOT,
			HKEY_CURRENT_USER,
			HKEY_LOCAL_MACHINE,
			HKEY_USERS,
			HKEY_PERFORMANCE_DATA,
			HKEY_CURRENT_CONFIG
		};

		if (root < registry_base::_CLASSES_ROOT ||
		        root > registry_base::_CURRENT_CONFIG)
			return CHECK_ERROR(CALL_E_INVALIDARG);

		exlib::string path;
		exlib::string sk;

		if ((key.length() > 0) && (key[key.length() - 1] == '\\'))
			path = key.substr(0, key.length() - 1);
		else
		{
			path_base::dirname(key, path);
			path_base::basename(key, "", sk);
			skey = utf8to16String(sk);
		}

		if (path == "\\")
		{
			hKey = s_hkeys[root];
			return 0;
		}

		int32_t type = bWrite ? KEY_WRITE | KEY_SET_VALUE : KEY_READ;
		exlib::wstring wpath = utf8to16String(path);

		LONG lResult = RegOpenKeyExW(s_hkeys[root], wpath.c_str(), 0, type, &hKey);
		if (lResult != ERROR_SUCCESS)
		{
			if (bCreate && lResult == ERROR_FILE_NOT_FOUND)
			{
				SECURITY_DESCRIPTOR SD;
				SECURITY_ATTRIBUTES SA;
				DWORD dwFunc;

				if (!InitializeSecurityDescriptor(&SD, SECURITY_DESCRIPTOR_REVISION))
					return CHECK_ERROR(LastError());

				if (!SetSecurityDescriptorDacl(&SD, true, 0, false))
					return CHECK_ERROR(LastError());

				SA.nLength = sizeof(SA);
				SA.lpSecurityDescriptor = &SD;
				SA.bInheritHandle = false;

				lResult = RegCreateKeyExW(s_hkeys[root], wpath.c_str(), 0, (LPWSTR)NULL,
				                          REG_OPTION_NON_VOLATILE, KEY_WRITE, &SA, &hKey,
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
	HKEY hKey;
	exlib::wstring skey;
};

#define MAX_KEY_LENGTH 255

result_t registry_base::listSubKey(int32_t root, exlib::string key, obj_ptr<List_base>& retVal)
{
	Registry r;
	result_t hr = r.open(root, key + "\\*");
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

	obj_ptr<List> l = new List();

	if (cSubKeys)
		for (i = 0; i < cSubKeys; i++)
		{
			cchValue = MAX_KEY_LENGTH;
			retCode = RegEnumKeyExW(r.hKey, i, achValue, &cchValue, NULL,
			                        NULL, NULL, &ftLastWriteTime);
			if (retCode == ERROR_SUCCESS)
				l->append(utf16to8String(achValue, cchValue));
		}

	retVal = l;

	return 0;
}

result_t registry_base::listValue(int32_t root, exlib::string key, obj_ptr<List_base>& retVal)
{
	Registry r;
	result_t hr = r.open(root, key + "\\*");
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

	obj_ptr<List> l = new List();

	if (cValues)
		for (i = 0; i < cValues; i++)
		{
			cchValue = MAX_KEY_LENGTH;
			retCode = RegEnumValueW(r.hKey, i, achValue, &cchValue, NULL,
			                        NULL, NULL, NULL);
			if (retCode == ERROR_SUCCESS)
				l->append(utf16to8String(achValue, cchValue));
		}

	retVal = l;

	return 0;
}

result_t registry_base::get(int32_t root, exlib::string key, v8::Local<v8::Value>& retVal)
{
	Registry r;
	result_t hr = r.open(root, key);
	if (hr < 0)
		return hr;

	DWORD dwType = 0, dwSize =  0;

	LONG lResult = RegQueryValueExW(r.hKey, r.skey.c_str(), NULL, &dwType, NULL, &dwSize);
	if (lResult != ERROR_SUCCESS)
		return CHECK_ERROR(-lResult);

	switch (dwType)
	{
	case REG_DWORD:
	{
		Isolate* isolate = Isolate::current();
		int32_t value = 0;
		dwSize = sizeof(value);

		RegQueryValueExW(r.hKey, r.skey.c_str(), NULL, &dwType, (LPBYTE)&value, &dwSize);
		retVal = GetReturnValue(isolate->m_isolate, value);
		break;
	}
	case REG_QWORD:
	{
		Isolate* isolate = Isolate::current();
		int64_t value = 0;
		dwSize = sizeof(value);

		RegQueryValueExW(r.hKey, r.skey.c_str(), NULL, &dwType, (LPBYTE)&value, &dwSize);
		retVal = GetReturnValue(isolate->m_isolate, value);
		break;
	}
	case REG_SZ:
	case REG_EXPAND_SZ:
	{
		Isolate* isolate = Isolate::current();
		exlib::wstring buf;
		exlib::string sbuf;

		buf.resize(dwSize / 2 - 1);
		RegQueryValueExW(r.hKey, r.skey.c_str(), NULL, &dwType, (LPBYTE)buf.c_buffer(), &dwSize);

		sbuf = utf16to8String(buf);
		retVal = GetReturnValue(isolate->m_isolate, sbuf);
		break;
	}
	case REG_MULTI_SZ:
	{
		Isolate* isolate = Isolate::current();
		exlib::wstring buf;
		exlib::string sbuf;

		buf.resize(dwSize / 2);
		RegQueryValueExW(r.hKey, r.skey.c_str(), NULL, &dwType, (LPBYTE)buf.c_buffer(), &dwSize);

		v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate);
		int32_t n = 0;

		const wchar_t* p1 = buf.c_str();
		const wchar_t* pn = p1 + dwSize / 2;
		while ((p1 < pn) && *p1)
		{
			const wchar_t* p2 = p1;
			while (*p2)
				p2 ++;

			sbuf = utf16to8String(p1, (int32_t)(p2 - p1));
			arr->Set(n ++, GetReturnValue(isolate->m_isolate, sbuf));
			p1 = p2 + 1;
		}

		retVal = arr;
		break;
	}
	case REG_BINARY:
	case REG_NONE:
	{
		exlib::string sbuf;
		obj_ptr<Buffer_base> buf;

		sbuf.resize(dwSize);
		RegQueryValueExW(r.hKey, r.skey.c_str(), NULL, &dwType, (LPBYTE)sbuf.c_buffer(), &dwSize);

		buf = new Buffer(sbuf);
		retVal = buf->wrap();

		break;
	}
	default:
		return CHECK_ERROR(CALL_E_RETURN_TYPE);
	}

	return 0;
}

result_t set_reg(int32_t root, exlib::string key, int32_t type, const void* data,
                 int32_t size)
{
	Registry r;
	result_t hr = r.open(root, key, true, true);
	if (hr < 0)
		return hr;

	LONG lResult = RegSetValueExW(r.hKey, r.skey.c_str(), 0, type, (const BYTE*)data, size);
	if (lResult != ERROR_SUCCESS)
		return CHECK_ERROR(-lResult);

	return 0;
}

result_t registry_base::set(int32_t root, exlib::string key, double value, int32_t type)
{
	if (type != REG_DWORD && type != REG_QWORD)
		return CHECK_ERROR(CALL_E_INVALIDARG);

	int64_t v = (int64_t)value;
	return set_reg(root, key, type, &v,
	               type == REG_DWORD ? sizeof(int32_t) : sizeof(int64_t));
}

result_t registry_base::set(int32_t root, exlib::string key, exlib::string value, int32_t type)
{
	if (type != REG_SZ && type != REG_EXPAND_SZ)
		return CHECK_ERROR(CALL_E_INVALIDARG);

	exlib::wstring wvalue = utf8to16String(value);
	return set_reg(root, key, type, wvalue.c_str(),
	               (int32_t)(sizeof(wchar_t) * (wvalue.length() + 1)));
}

result_t registry_base::set(int32_t root, exlib::string key, v8::Local<v8::Array> value)
{
	int32_t len = value->Length();
	int32_t i;
	result_t hr;
	exlib::wstring data;

	for (i = 0; i < len; i ++)
	{
		exlib::string v;
		exlib::wstring wv;

		hr = GetArgumentValue(value->Get(i), v, false);
		if (hr < 0)
			return hr;

		wv = utf8to16String(v);
		data.append(wv.c_str(), wv.length() + 1);
	}

	return set_reg(root, key, REG_MULTI_SZ, data.c_str(),
	               (int32_t)(sizeof(wchar_t) * (data.length() + 1)));
}

result_t registry_base::set(int32_t root, exlib::string key, Buffer_base* value)
{
	exlib::string data;

	value->toString(data);
	return set_reg(root, key, REG_BINARY, data.c_str(), (int32_t)data.length());
}

result_t registry_base::del(int32_t root, exlib::string key)
{
	Registry r;
	result_t hr = r.open(root, key, true);
	if (hr < 0)
		return hr;

	LONG lResult = RegDeleteValueW(r.hKey, r.skey.c_str());
	if (lResult == ERROR_FILE_NOT_FOUND)
		lResult = RegDeleteKeyW(r.hKey, r.skey.c_str());

	if (lResult != ERROR_SUCCESS)
		return CHECK_ERROR(-lResult);

	return 0;
}

}

#endif
