/*
 * HttpUploadCollection.h
 *
 *  Created on: Aug 28, 2012
 *      Author: lion
 */

#include "ifs/HttpCollection.h"
#include "QuickArray.h"

#ifndef HTTPUPLOADCOLLECTION_H_
#define HTTPUPLOADCOLLECTION_H_

namespace fibjs
{

class HttpUploadCollection: public HttpCollection_base
{
public:
	HttpUploadCollection() :
			m_count(0)
	{
	}

public:
	// HttpCollection_base
	virtual result_t clear();
	virtual result_t has(const char* name, bool& retVal);
	virtual result_t first(const char* name, Variant& retVal);
	virtual result_t all(const char* name, v8::Handle<v8::Array>& retVal);
	virtual result_t add(v8::Handle<v8::Object> map);
	virtual result_t add(const char* name, Variant value);
	virtual result_t set(v8::Handle<v8::Object> map);
	virtual result_t set(const char* name, Variant value);
	virtual result_t remove(const char* name);
	virtual result_t _named_getter(const char* property, Variant& retVal);
	virtual result_t _named_setter(const char* property, Variant newVal);

	void parse(std::string& str, const char* boundary);

private:
	QuickArray<std::string> m_names;
	QuickArray<VariantEx> m_values;
	int32_t m_count;
};

} /* namespace fibjs */
#endif /* HTTPUPLOADCOLLECTION_H_ */
