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
#include "unzip/include/zip.h"

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
		Info(exlib::string name, unz_file_info64& info);

	public:
		// ZipInfo_base
		virtual result_t get_filename(exlib::string& retVal);
		virtual result_t get_date(date_t& retVal);
		virtual result_t get_compress_type(exlib::string& retVal);
		virtual result_t get_compress_size(int64_t& retVal);
		virtual result_t get_file_size(int64_t& retVal);
		virtual result_t get_password(bool& retVal);
		virtual result_t get_data(obj_ptr<Buffer_base>& retVal);

	public:
		exlib::string m_name;
		date_t m_date;
		exlib::string m_compress_type;
		int64_t m_compress_size;
		int64_t m_file_size;
		bool m_password;
		obj_ptr<Buffer_base> m_data;
	};

public:
	ZipFile(SeekableStream_base* strm, exlib::string mod, int32_t compress_type);

public:
	// ZipFile_base
	virtual result_t namelist(obj_ptr<List_base>& retVal, AsyncEvent* ac);
	virtual result_t infolist(obj_ptr<List_base>& retVal, AsyncEvent* ac);
	virtual result_t getinfo(exlib::string member, obj_ptr<ZipInfo_base>& retVal, AsyncEvent* ac);
	virtual result_t read(exlib::string member, exlib::string password, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
	virtual result_t readAll(exlib::string password, obj_ptr<List_base>& retVal, AsyncEvent* ac);
	virtual result_t extract(exlib::string member, exlib::string path, exlib::string password, AsyncEvent* ac);
	virtual result_t extract(exlib::string member, SeekableStream_base* strm, exlib::string password, AsyncEvent* ac);
	virtual result_t extractAll(exlib::string path, exlib::string password, AsyncEvent* ac);
	virtual result_t write(exlib::string filename, exlib::string inZipName, exlib::string password, AsyncEvent* ac);
	virtual result_t write(Buffer_base* data, exlib::string inZipName, exlib::string password, AsyncEvent* ac);
	virtual result_t write(SeekableStream_base* strm, exlib::string inZipName, exlib::string password, AsyncEvent* ac);
	virtual result_t close(AsyncEvent* ac);

private:
	result_t get_info(obj_ptr<Info>& retVal);
	result_t extract(SeekableStream_base* strm, exlib::string password);
	result_t read(exlib::string password, obj_ptr<Buffer_base>& retVal);
	result_t write(exlib::string filename, exlib::string password, SeekableStream_base* strm);
	result_t getFileCrc(SeekableStream_base* strm, uint32_t& crc);
	result_t checkGuard(exlib::string path);

private:
	unzFile m_unz;
	zipFile m_zip;
	int32_t m_compress_type;
	exlib::string m_mod;
	obj_ptr<SeekableStream_base> m_strm;
};

} /* namespace fibjs */
#endif /* ZIPFILE_H_ */
