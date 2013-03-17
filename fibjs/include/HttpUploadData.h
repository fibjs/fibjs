/*
 * HttpUploadData.h
 *
 *  Created on: Aug 28, 2012
 *      Author: lion
 */

#include "ifs/HttpUploadData.h"

#ifndef HTTPUPLOADDATA_H_
#define HTTPUPLOADDATA_H_

namespace fibjs
{

class HttpUploadData: public HttpUploadData_base
{
public:
	// HttpUploadData_base
	virtual result_t get_fileName(std::string& retVal);
	virtual result_t get_contentType(std::string& retVal);
	virtual result_t get_contentTransferEncoding(std::string& retVal);
	virtual result_t get_body(obj_ptr<SeekableStream_base>& retVal);

public:
	std::string m_name;
	std::string m_type;
	std::string m_encoding;
	obj_ptr<SeekableStream_base> m_body;
};

} /* namespace fibjs */
#endif /* HTTPUPLOADDATA_H_ */
