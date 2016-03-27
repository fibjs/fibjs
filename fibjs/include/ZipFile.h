/*
 * ZipFile.h
 *
 *  Created on: Mar 26, 2016
 *      Author: lion
 */

#include "ifs/ZipFile.h"
#include "ifs/ZipInfo.h"
#include "ifs/SeekableStream.h"
#include "unzip/include/unzip.h"

#ifndef ZIPFILE_H_
#define ZIPFILE_H_

namespace fibjs
{

class ZipFile: public ZipFile_base
{
public:
	class Info: public ZipInfo_base
	{
	public:
		Info(const char* name, unz_file_info64& info);

	public:
		// ZipInfo_base
		virtual result_t get_filename(std::string& retVal);
		virtual result_t get_date(date_t& retVal);
		virtual result_t get_compress_type(std::string& retVal);
		virtual result_t get_compress_size(int64_t& retVal);
		virtual result_t get_file_size(int64_t& retVal);
		virtual result_t get_password(bool& retVal);

	private:
		std::string m_name;
		date_t m_date;
		std::string m_compress_type;
		int64_t m_compress_size;
		int64_t m_file_size;
		bool m_password;
	};

public:
	ZipFile(SeekableStream_base* strm);

public:
	// ZipFile_base
	virtual result_t list(obj_ptr<List_base>& retVal, AsyncEvent* ac);
	virtual result_t get(const char* member, obj_ptr<ZipInfo_base>& retVal, AsyncEvent* ac);
	virtual result_t read(const char* member, const char* password, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
	virtual result_t extract(const char* member, const char* path, const char* password, AsyncEvent* ac);
	virtual result_t extract(const char* member, SeekableStream_base* strm, const char* password, AsyncEvent* ac);

private:
	result_t get_info(obj_ptr<ZipInfo_base>& retVal);

private:
	unzFile m_unz;
	obj_ptr<SeekableStream_base> m_strm;
};

} /* namespace fibjs */
#endif /* ZIPFILE_H_ */
