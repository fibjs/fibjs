/*
 * zip.cpp
 *
 *  Created on: Mar 26, 2016
 *      Author: lion
 */

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

		std::string strData;
		data->toString(strData);
		memcpy(buf, strData.data(), strData.length());

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

result_t zip_base::open(const char* path, obj_ptr<ZipFile_base>& retVal, AsyncEvent* ac)
{
	class asyncOpenFile: public AsyncState
	{
	public:
		asyncOpenFile(const char* path, obj_ptr<ZipFile_base>& retVal, AsyncEvent *ac) :
			AsyncState(ac), m_path(path), m_retVal(retVal)
		{
			set(openfile);
		}

		static int32_t openfile(AsyncState *pState, int32_t n)
		{
			asyncOpenFile *pThis = (asyncOpenFile *) pState;

			pThis->set(openzip);
			return fs_base::open(pThis->m_path.c_str(), "r", pThis->m_file, pThis);
		}

		static int32_t openzip(AsyncState *pState, int32_t n)
		{
			asyncOpenFile *pThis = (asyncOpenFile *) pState;

			pThis->done();
			return zip_base::open(pThis->m_file, pThis->m_retVal, pThis);
		}

	private:
		std::string m_path;
		obj_ptr<File_base> m_file;
		obj_ptr<ZipFile_base>& m_retVal;
	};

	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	return (new asyncOpenFile(path, retVal, ac))->post(0);
}

result_t zip_base::open(Buffer_base* data, obj_ptr<ZipFile_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	std::string strData;

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

result_t zip_base::create(const char* path, obj_ptr<ZipFile_base>& retVal, AsyncEvent* ac)
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

result_t ZipFile::Info::get_filename(std::string& retVal)
{
	retVal = m_name;
	return 0;
}

result_t ZipFile::Info::get_date(date_t& retVal)
{
	retVal = m_date;
	return 0;
}

result_t ZipFile::Info::get_compress_type(std::string& retVal)
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

ZipFile::ZipFile(SeekableStream_base* strm) : m_strm(strm)
{
	StreamIO sio(strm);
	m_unz = unzOpen2_64("", &sio);
}

result_t ZipFile::get_info(obj_ptr<ZipInfo_base>& retVal)
{
	int32_t err;
	char filename_inzip[256];
	unz_file_info64 file_info;

	err = unzGetCurrentFileInfo64(m_unz, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
	if (err != UNZ_OK)
	{
		printf("error %d with zipfile in unzGetCurrentFileInfo\n", err);
		return -1;
	}

	retVal = new Info(filename_inzip, file_info);
	return 0;
}

result_t ZipFile::list(obj_ptr<List_base>& retVal, AsyncEvent* ac)
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
	{
		printf("error %d with zipfile in unzGetGlobalInfo \n", err);
		return -1;
	}

	for (i = 0; i < gi.number_entry; i++)
	{
		obj_ptr<ZipInfo_base> info;

		hr = get_info(info);
		if (hr < 0)
			return hr;

		names->append(info);

		if ((i + 1) < gi.number_entry)
		{
			err = unzGoToNextFile(m_unz);
			if (err != UNZ_OK)
			{
				printf("error %d with zipfile in unzGoToNextFile\n", err);
				return -1;
			}
		}
	}

	retVal = names;
	return 0;
}

result_t ZipFile::get(const char* member, obj_ptr<ZipInfo_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	if (unzLocateFile(m_unz, member, 0) != UNZ_OK)
	{
		printf("file %s not found in the zipfile\n", member);
		return -1;
	}

	return get_info(retVal);
}

result_t ZipFile::read(const char* member, const char* password, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	return 0;
}

result_t ZipFile::extract(const char* member, const char* path, const char* password, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);
	return 0;

}

result_t ZipFile::extract(const char* member, SeekableStream_base* strm, const char* password, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	return 0;
}



}