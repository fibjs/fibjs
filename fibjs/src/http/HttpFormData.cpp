/*
 * HttpFormData.cpp
 *
 *  Created on: Jul 11, 2025
 *      Author: lion
 */

#include "object.h"
#include "HttpFormData.h"
#include "Blob.h"

namespace fibjs {

result_t HttpFormData_base::_new(obj_ptr<HttpFormData_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new HttpFormData();
    return 0;
}

result_t HttpFormData_base::_new(v8::Local<v8::Object> init, obj_ptr<HttpFormData_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new HttpFormData();
    return retVal->append(init);
}

result_t HttpFormData_base::_new(HttpFormData_base* init, obj_ptr<HttpFormData_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<HttpFormData> headers = new HttpFormData();
    retVal = headers;
    return headers->init(init);
}

result_t HttpFormData::append(exlib::string name, Blob_base* value)
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

result_t HttpFormData::append(exlib::string name, Blob_base* value, exlib::string filename)
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

result_t HttpFormData::set(exlib::string name, Blob_base* value)
{
    if (name.empty())
        return CALL_E_INVALIDARG;

    remove(name);
    append(name, value);

    return 0;
}

result_t HttpFormData::set(exlib::string name, Blob_base* value, exlib::string filename)
{
    if (name.empty())
        return CALL_E_INVALIDARG;

    remove(name);
    append(name, value, filename);

    return 0;
}

}