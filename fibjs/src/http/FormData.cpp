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

result_t FormData_base::_new(Blob_base* init, exlib::string boundary, obj_ptr<FormData_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<FormData> headers = new FormData();
    Blob* initBlob = static_cast<Blob*>(init);
    obj_ptr<Buffer_base> buffer = initBlob->m_impl.getBuffer();

    if (boundary == "") {
        initBlob->get_type(boundary);
    }

    retVal = headers;
    return headers->parseMultipart(buffer, boundary.c_str());
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

result_t FormData::append(exlib::string name, Variant value)
{
    int32_t vt = value.type();

    if (vt != Variant::VT_String) {
        if (vt == Variant::VT_JSValue || vt == Variant::VT_Object) {
            // WHATWG FormData: only a real Blob/File becomes a file entry,
            // every other value is converted to a string.
            v8::Local<v8::Value> v = value;

            obj_ptr<File_base> file = File_base::getInstance(v);
            if (file) {
                value = file;
            } else {
                obj_ptr<Blob_base> blob = Blob_base::getInstance(v);
                if (blob)
                    return append(name, blob.get());

                exlib::string s;
                value.toString(s);
                value = s;
            }
        } else {
            exlib::string s;
            value.toString(s);
            value = s;
        }
    }

    m_sorted = false;
    m_map.emplace_back(name, value);
    return 0;
}

result_t FormData::append(exlib::string name, Blob_base* value)
{
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

    m_sorted = false;
    m_map.emplace_back(name, file);

    return 0;
}

result_t FormData::append(exlib::string name, Variant value, exlib::string filename)
{
    // WHATWG FormData: the filename argument is only allowed for Blob/File values.
    obj_ptr<Blob_base> blob = Blob_base::getInstance((v8::Local<v8::Value>)value);
    if (!blob)
        return Runtime::setError(kTypeError,
            "Failed to execute 'append' on 'FormData': parameter 2 is not of type 'Blob'.");

    return append(name, blob.get(), filename);
}

result_t FormData::append(exlib::string name, Blob_base* value, exlib::string filename)
{
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

    m_sorted = false;
    m_map.emplace_back(name, fileObj);

    return 0;
}

result_t FormData::set(exlib::string name, Blob_base* value)
{
    remove(name);
    append(name, value);

    return 0;
}

result_t FormData::set(exlib::string name, Variant value, exlib::string filename)
{
    obj_ptr<Blob_base> blob = Blob_base::getInstance((v8::Local<v8::Value>)value);
    if (!blob)
        return Runtime::setError(kTypeError,
            "Failed to execute 'set' on 'FormData': parameter 2 is not of type 'Blob'.");

    remove(name);
    append(name, blob.get(), filename);

    return 0;
}

}