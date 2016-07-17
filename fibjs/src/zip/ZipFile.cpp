/*
 * zip.cpp
 *
 *  Created on: Mar 26, 2016
 *      Author: lion
 */

#include "object.h"
#include "ifs/zip.h"
#include "ifs/fs.h"
#include "ZipFile.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include "List.h"

namespace fibjs
{

DECLARE_MODULE(zip);

class StreamIO : public zlib_filefunc64_def
{
public:
	StreamIO(SeekableStream_base* strm)
	{
		zopen64_file = fopen64_file_func;
		zread_file = fread_file_func;
		zwrite_file = fwrite_file_func;
		ztell64_file = ftell64_file_func;
		zseek64_file = fseek64_file_func;
		zclose_file = fclose_file_func;
		zerror_file = ferror_file_func;
		opaque = strm;
	}

private:
	static voidpf ZCALLBACK fopen64_file_func (voidpf opaque, const void* filename, int mode)
	{
		return opaque;
	}

	static uLong ZCALLBACK fread_file_func (voidpf opaque, voidpf stream, void* buf, uLong size)
	{
		result_t hr;
		SeekableStream_base* strm = (SeekableStream_base*)stream;
		obj_ptr<Buffer_base> data;

		hr = strm->cc_read((int32_t)size, data);
		if (hr < 0)
			return 0;

		exlib::string strData;
		data->toString(strData);
		memcpy(buf, strData.c_str(), strData.length());

		return (uLong)strData.length();
	}

	static uLong ZCALLBACK fwrite_file_func (voidpf opaque, voidpf stream, const void* buf, uLong size)
	{
		result_t hr;
		SeekableStream_base* strm = (SeekableStream_base*)stream;
		obj_ptr<Buffer_base> data = new Buffer(buf, size);

		hr = strm->cc_write(data);
		if (hr < 0)
			return 0;

		return size;
	}

	static ZPOS64_T ZCALLBACK ftell64_file_func (voidpf opaque, voidpf stream)
	{
		result_t hr;
		SeekableStream_base* strm = (SeekableStream_base*)stream;
		int64_t pos;

		hr = strm->tell(pos);
		if (hr < 0)
			return -1;

		return pos;
	}

	static long ZCALLBACK fseek64_file_func (voidpf  opaque, voidpf stream, ZPOS64_T offset, int origin)
	{
		result_t hr;
		SeekableStream_base* strm = (SeekableStream_base*)stream;
		int32_t fseek_origin = 0;

		switch (origin)
		{
		case ZLIB_FILEFUNC_SEEK_CUR :
			fseek_origin = fs_base::_SEEK_CUR;
			break;
		case ZLIB_FILEFUNC_SEEK_END :
			fseek_origin = fs_base::_SEEK_END;
			break;
		case ZLIB_FILEFUNC_SEEK_SET :
			fseek_origin = fs_base::_SEEK_SET;
			break;
		default: return -1;
		}

		hr = strm->seek(offset, fseek_origin);
		if (hr < 0)
			return -1;

		return 0;
	}

	static int ZCALLBACK fclose_file_func (voidpf opaque, voidpf stream)
	{
		result_t hr;
		SeekableStream_base* strm = (SeekableStream_base*)stream;

		hr = strm->cc_close();
		if (hr < 0)
			return -1;

		return 0;
	}

	static int ZCALLBACK ferror_file_func (voidpf opaque, voidpf stream)
	{
		return 0;
	}
};

const char* zip_error(int32_t err)
{
	switch (err)
	{
	case UNZ_BADZIPFILE:
		return "File is not a zip file.";
	case UNZ_CRCERROR:
		return "Bad CRC-32.";
	case UNZ_PARAMERROR:
		return "Invalid argument.";
	case UNZ_INTERNALERROR:
		return "Internal error.";
	case UNZ_END_OF_LIST_OF_FILE:
		return "End of file.";
	}

	return zError(err);
}

result_t zip_base::isZipFile(const char* filename, bool& retVal, AsyncEvent* ac)
{
	return 0;
}

result_t zip_base::open(const char* path, obj_ptr<ZipFile_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	obj_ptr<File_base> file;
	result_t hr;

	hr = fs_base::cc_open(path, "r", file);
	if (hr < 0)
		return hr;

	return open(file, retVal, ac);
}

result_t zip_base::open(Buffer_base* data, obj_ptr<ZipFile_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	exlib::string strData;

	data->toString(strData);
	obj_ptr<SeekableStream_base> strm = new MemoryStream::CloneStream(strData, 0);

	return open(strm, retVal, ac);
}

result_t zip_base::open(SeekableStream_base* strm, obj_ptr<ZipFile_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	retVal = new ZipFile(strm);
	return 0;
}

result_t zip_base::create(const char* path, int32_t compress_type, obj_ptr<ZipFile_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	//retVal = new ZipFile();
	return 0;
}

ZipFile::Info::Info(const char* name, unz_file_info64& info)
{
	m_name = name;
	m_date.fromDosTime(info.dosDate);

	if (info.compression_method == 0)
		m_compress_type = "Stored";
	else if (info.compression_method == Z_DEFLATED)
		m_compress_type = "Deflate";
	else if (info.compression_method == Z_BZIP2ED)
		m_compress_type = "BZip2";
	else
		m_compress_type = "Unknown";

	m_file_size = info.uncompressed_size;
	m_compress_size = info.compressed_size;
	m_password = info.flag & 1;
}

result_t ZipFile::Info::get_filename(exlib::string& retVal)
{
	retVal = m_name;
	return 0;
}

result_t ZipFile::Info::get_date(date_t& retVal)
{
	retVal = m_date;
	return 0;
}

result_t ZipFile::Info::get_compress_type(exlib::string& retVal)
{
	retVal = m_compress_type;
	return 0;
}

result_t ZipFile::Info::get_compress_size(int64_t& retVal)
{
	retVal = m_compress_size;
	return 0;
}

result_t ZipFile::Info::get_file_size(int64_t& retVal)
{
	retVal = m_file_size;
	return 0;
}

result_t ZipFile::Info::get_password(bool& retVal)
{
	retVal = m_password;
	return 0;
}

result_t ZipFile::Info::get_data(obj_ptr<Buffer_base>& retVal)
{
	if (!m_data)
		return CALL_RETURN_NULL;

	retVal = m_data;
	return 0;
}

ZipFile::ZipFile(SeekableStream_base* strm) : m_strm(strm)
{
	StreamIO sio(strm);
	m_unz = unzOpen2_64("", &sio);
}

result_t ZipFile::get_info(obj_ptr<Info>& retVal)
{
	int32_t err;
	char filename_inzip[256];
	unz_file_info64 file_info;

	err = unzGetCurrentFileInfo64(m_unz, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
	if (err != UNZ_OK)
		return CHECK_ERROR(Runtime::setError(zip_error(err)));

	retVal = new Info(filename_inzip, file_info);
	return 0;
}

result_t ZipFile::namelist(obj_ptr<List_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	result_t hr;
	unz_global_info64 gi;
	int32_t err;
	int32_t i;
	obj_ptr<List> names = new List();

	err = unzGetGlobalInfo64(m_unz, &gi);
	if (err != UNZ_OK)
		return CHECK_ERROR(Runtime::setError(zip_error(err)));

	for (i = 0; i < gi.number_entry; i++)
	{
		obj_ptr<Info> info;

		hr = get_info(info);
		if (hr < 0)
			return hr;

		names->append(info->m_name);

		if ((i + 1) < gi.number_entry)
		{
			err = unzGoToNextFile(m_unz);
			if (err != UNZ_OK)
				return CHECK_ERROR(Runtime::setError(zip_error(err)));
		}
	}

	retVal = names;
	return 0;
}

result_t ZipFile::infolist(obj_ptr<List_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	result_t hr;
	unz_global_info64 gi;
	int32_t err;
	int32_t i;
	obj_ptr<List> names = new List();

	err = unzGetGlobalInfo64(m_unz, &gi);
	if (err != UNZ_OK)
		return CHECK_ERROR(Runtime::setError(zip_error(err)));

	for (i = 0; i < gi.number_entry; i++)
	{
		obj_ptr<Info> info;

		hr = get_info(info);
		if (hr < 0)
			return hr;

		names->append(info);

		if ((i + 1) < gi.number_entry)
		{
			err = unzGoToNextFile(m_unz);
			if (err != UNZ_OK)
				return CHECK_ERROR(Runtime::setError(zip_error(err)));
		}
	}

	retVal = names;
	return 0;
}

result_t ZipFile::getinfo(const char* member, obj_ptr<ZipInfo_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	int32_t err;

	err = unzLocateFile(m_unz, member, 0);
	if (err != UNZ_OK)
		return CHECK_ERROR(Runtime::setError(zip_error(err)));

	obj_ptr<Info> info;
	result_t hr;

	hr = get_info(info);
	if (hr < 0)
		return hr;

	retVal = info;
	return 0;
}

#define BUF_SIZE	8192

result_t ZipFile::extract(SeekableStream_base* strm, const char* password)
{
	int32_t err;
	result_t hr;
	obj_ptr<Buffer> buf = new Buffer();

	buf->resize(BUF_SIZE);
	if (*password)
		err = unzOpenCurrentFilePassword(m_unz, password);
	else
		err = unzOpenCurrentFile(m_unz);
	if (err != UNZ_OK)
		return CHECK_ERROR(Runtime::setError(zip_error(err)));

	do
	{
		err = unzReadCurrentFile(m_unz, buf->data(), BUF_SIZE);
		if (err < 0)
		{
			unzCloseCurrentFile(m_unz);
			return CHECK_ERROR(Runtime::setError(zip_error(err)));
		}

		if (err > 0)
		{
			hr = strm->cc_write(buf);
			if (hr < 0)
				return hr;
		}
	} while (err > 0);

	err = unzCloseCurrentFile(m_unz);
	if (err != UNZ_OK)
		return CHECK_ERROR(Runtime::setError(zip_error(err)));

	return 0;
}

result_t ZipFile::read(const char* password, obj_ptr<Buffer_base>& retVal)
{
	result_t hr;
	obj_ptr<MemoryStream> strm;

	strm = new MemoryStream();

	hr = extract(strm, password);
	if (hr < 0)
		return hr;

	hr = strm->rewind();
	if (hr < 0)
		return hr;

	return strm->cc_readAll(retVal);
}

result_t ZipFile::read(const char* member, const char* password, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	int32_t err;

	err = unzLocateFile(m_unz, member, 0);
	if (err != UNZ_OK)
		return CHECK_ERROR(Runtime::setError(zip_error(err)));

	return read(password, retVal);
}

result_t ZipFile::extract(const char* member, const char* path, const char* password, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	int32_t err;
	result_t hr;
	obj_ptr<File_base> file;

	err = unzLocateFile(m_unz, member, 0);
	if (err != UNZ_OK)
		return CHECK_ERROR(Runtime::setError(zip_error(err)));

	hr = fs_base::cc_open(path, "w", file);
	if (hr < 0)
		return hr;

	return extract(file, password);
}

result_t ZipFile::extract(const char* member, SeekableStream_base* strm, const char* password, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	int32_t err;

	err = unzLocateFile(m_unz, member, 0);
	if (err != UNZ_OK)
		return CHECK_ERROR(Runtime::setError(zip_error(err)));

	return extract(strm, password);
}

result_t ZipFile::extractAll(const char* path, const char* password, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	result_t hr;
	unz_global_info64 gi;
	int32_t err;
	int32_t i;
	obj_ptr<List> datas = new List();

	err = unzGetGlobalInfo64(m_unz, &gi);
	if (err != UNZ_OK)
		return CHECK_ERROR(Runtime::setError(zip_error(err)));

	for (i = 0; i < gi.number_entry; i++)
	{
		obj_ptr<Info> info;

		hr = get_info(info);
		if (hr < 0)
			return hr;

		hr = read(password, info->m_data);
		if (hr < 0)
			return hr;

		// datas->append(info);

		if ((i + 1) < gi.number_entry)
		{
			err = unzGoToNextFile(m_unz);
			if (err != UNZ_OK)
				return CHECK_ERROR(Runtime::setError(zip_error(err)));
		}
	}

	return 0;
}

result_t ZipFile::readAll(const char* password, obj_ptr<List_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	result_t hr;
	unz_global_info64 gi;
	int32_t err;
	int32_t i;
	obj_ptr<List> datas = new List();

	err = unzGetGlobalInfo64(m_unz, &gi);
	if (err != UNZ_OK)
		return CHECK_ERROR(Runtime::setError(zip_error(err)));

	for (i = 0; i < gi.number_entry; i++)
	{
		obj_ptr<Info> info;

		hr = get_info(info);
		if (hr < 0)
			return hr;

		hr = read(password, info->m_data);
		if (hr < 0)
			return hr;

		datas->append(info);

		if ((i + 1) < gi.number_entry)
		{
			err = unzGoToNextFile(m_unz);
			if (err != UNZ_OK)
				return CHECK_ERROR(Runtime::setError(zip_error(err)));
		}
	}

	retVal = datas;

	return 0;
}

result_t ZipFile::setpasswd(const char* password)
{
	return 0;
}

result_t ZipFile::write(const char* filename, AsyncEvent* ac)
{
	return 0;
}

result_t ZipFile::write(Buffer_base* data, AsyncEvent* ac)
{
	return 0;
}

result_t ZipFile::write(SeekableStream_base* strm, AsyncEvent* ac)
{
	return 0;
}

result_t ZipFile::close(AsyncEvent* ac)
{
	return 0;
}


}