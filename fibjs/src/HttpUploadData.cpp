/*
 * HttpUploadData.cpp
 *
 *  Created on: Aug 28, 2012
 *      Author: lion
 */

#include "HttpUploadData.h"

namespace fibjs
{

result_t HttpUploadData::get_fileName(std::string& retVal)
{
	retVal = m_name;
	return 0;
}

result_t HttpUploadData::get_contentType(std::string& retVal)
{
	retVal = m_type;
	return 0;
}

result_t HttpUploadData::get_body(obj_ptr<SeekableStream_base>& retVal)
{
	retVal = m_body;
	return 0;
}

} /* namespace fibjs */
