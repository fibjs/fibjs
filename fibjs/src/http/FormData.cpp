/*
 * FormData.cpp
 *
 *  Created on: Jul 11, 2025
 *      Author: lion
 */

#include "object.h"
#include "FormData.h"
#include "Blob.h"

namespace fibjs {

result_t FormData_base::_new(obj_ptr<FormData_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new FormData();
    return 0;
}

result_t FormData_base::_new(exlib::string init, obj_ptr<FormData_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<FormData> headers = new FormData();
    retVal = headers;
    return headers->parse(init);
}

result_t FormData_base::_new(Buffer_base* init, exlib::string boundary, obj_ptr<FormData_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<FormData> headers = new FormData();
    retVal = headers;
    return headers->parseMultipart(init, boundary.c_str());
}

result_t FormData_base::_new(v8::Local<v8::Object> init, obj_ptr<FormData_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new FormData();
    return retVal->append(init);
}

result_t FormData_base::_new(FormData_base* init, obj_ptr<FormData_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<FormData> headers = new FormData();
    retVal = headers;
    return headers->init(init);
}

result_t FormData::append(exlib::string name, Blob_base* value)
{
    if (name.empty())
        return CALL_E_INVALIDARG;

    obj_ptr<File_base> file = File_base::getInstance(value);
    if (!file) {
        obj_ptr<File> fileObj = new File();

        ((Blob*)value)->m_impl.clone(fileObj->m_impl);
        fileObj->m_name = "blob";
        date_t d;
        d.now();
        fileObj->m_lastModified = d.date();

        file = fileObj;
    }

    m_map.emplace_back(name, file);

    return 0;
}

result_t FormData::append(exlib::string name, Blob_base* value, exlib::string filename)
{
    if (name.empty())
        return CALL_E_INVALIDARG;

    obj_ptr<File> fileObj = new File();

    obj_ptr<File_base> file = File_base::getInstance(value);
    if (!file) {
        ((Blob*)value)->m_impl.clone(fileObj->m_impl);
    } else {
        file.As<File>()->m_impl.clone(fileObj->m_impl);
    }

    fileObj->m_name = filename;
    date_t d;
    d.now();
    fileObj->m_lastModified = d.date();

    m_map.emplace_back(name, fileObj);

    return 0;
}

result_t FormData::set(exlib::string name, Blob_base* value)
{
    if (name.empty())
        return CALL_E_INVALIDARG;

    remove(name);
    append(name, value);

    return 0;
}

result_t FormData::set(exlib::string name, Blob_base* value, exlib::string filename)
{
    if (name.empty())
        return CALL_E_INVALIDARG;

    remove(name);
    append(name, value, filename);

    return 0;
}

}