/*
 * zip.cpp
 *
 *  Created on: Mar 26, 2016
 *      Author: lion
 */

#include "object.h"
#include "errhelp.h"
#include "ifs/zip.h"
#include "ifs/fs.h"
#include "file_path.h"
#include "path.h"
#include "ZipFile.h"
#include "utf8.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include "encoding_conv.h"
#include <algorithm>

namespace fibjs {

DECLARE_MODULE(zip);

// minizip issues a ZREAD64 for every field it parses, down to individual
// bytes (unz64local_getShort/getLong read 2/4 one-byte chunks). Served straight
// from the stream, one central-directory entry costs ~46 cc_readBuffer calls -
// each one a Buffer allocation, a v8 allocation and a read(2) - so walking a
// directory of a few thousand entries took hundreds of milliseconds and every
// unzLocateFile was O(n) of that. The cache turns those reads into memcpy from
// one read-ahead block. The block matches minizip's own 16KB data chunk: it
// covers hundreds of directory fields, while a random access (the VFS reads the
// EOCD, one directory entry and one local header per uncached member) only
// pays for the bytes it is about to use.
#define ZIP_IO_BUFF_SIZE (16 * 1024)

class StreamIO : public zlib_filefunc64_def {
public:
    StreamIO(SeekableStream_base* strm)
        : m_strm(strm)
    {
        zopen64_file = fopen64_file_func;
        zread_file = fread_file_func;
        zwrite_file = fwrite_file_func;
        ztell64_file = ftell64_file_func;
        zseek64_file = fseek64_file_func;
        zclose_file = fclose_file_func;
        zerror_file = ferror_file_func;
        // minizip keeps this pointer past the constructor call, so opaque and
        // the value returned by zopen both name this object. ZipFile owns it.
        opaque = this;
    }

private:
    static voidpf ZCALLBACK fopen64_file_func(voidpf opaque, const void* filename, int mode)
    {
        return opaque;
    }

    static uLong ZCALLBACK fread_file_func(voidpf opaque, voidpf stream, void* buf, uLong size)
    {
        return ((StreamIO*)opaque)->read(buf, size);
    }

    static uLong ZCALLBACK fwrite_file_func(voidpf opaque, voidpf stream, const void* buf, uLong size)
    {
        return ((StreamIO*)opaque)->write(buf, size);
    }

    static ZPOS64_T ZCALLBACK ftell64_file_func(voidpf opaque, voidpf stream)
    {
        return (ZPOS64_T)((StreamIO*)opaque)->m_pos;
    }

    static long ZCALLBACK fseek64_file_func(voidpf opaque, voidpf stream, ZPOS64_T offset, int origin)
    {
        return ((StreamIO*)opaque)->seek(offset, origin);
    }

    static int ZCALLBACK fclose_file_func(voidpf opaque, voidpf stream)
    {
        result_t hr = ((StreamIO*)opaque)->m_strm->cc_close();
        if (hr.failed())
            return -1;

        return 0;
    }

    static int ZCALLBACK ferror_file_func(voidpf opaque, voidpf stream)
    {
        return 0;
    }

    // Serve a read from the cache, refilling it with one read-ahead block when
    // the requested range is outside. Returns the number of bytes read.
    uLong read(void* buf, uLong size)
    {
        uLong total = 0;
        uint8_t* out = (uint8_t*)buf;

        while (total < size) {
            if (m_cache) {
                Buffer* buff = Buffer::Cast(m_cache);
                int64_t offset = m_pos - m_cache_start;

                if (offset >= 0 && offset < (int64_t)buff->length()) {
                    uLong n = (uLong)std::min<int64_t>(size - total, buff->length() - offset);

                    memcpy(out + total, buff->data() + offset, n);
                    m_pos += n;
                    total += n;
                    continue;
                }
            }

            if (!refill())
                break;
        }

        return total;
    }

    uLong write(const void* buf, uLong size)
    {
        result_t hr;

        // A write deletes whatever the cache held: the block may now overlap
        // data that changed underneath it.
        m_cache.Release();

        if (m_stream_pos != m_pos) {
            hr = m_strm->seek(m_pos, fs_base::C_SEEK_SET);
            if (hr.failed())
                return 0;

            m_stream_pos = m_pos;
        }

        obj_ptr<Buffer_base> data = new Buffer(buf, size);

        hr = m_strm->cc_writeBuffer(data);
        if (hr.failed())
            return 0;

        m_pos += size;
        m_stream_pos = m_pos;

        return size;
    }

    long seek(ZPOS64_T offset, int origin)
    {
        int64_t new_pos;

        switch (origin) {
        case ZLIB_FILEFUNC_SEEK_SET:
            new_pos = (int64_t)offset;
            break;
        case ZLIB_FILEFUNC_SEEK_CUR:
            new_pos = m_pos + (int64_t)offset;
            break;
        case ZLIB_FILEFUNC_SEEK_END: {
            int64_t size;

            if (m_strm->size(size).failed())
                return -1;

            new_pos = size + (int64_t)offset;
            break;
        }
        default:
            return -1;
        }

        if (new_pos < 0)
            return -1;

        // The stream itself is repositioned lazily: a seek inside the cached
        // block (which is what unzReadCurrentFile does before every 16KB read)
        // costs nothing, and a seek followed by another seek costs nothing at
        // all.
        m_pos = new_pos;

        return 0;
    }

    bool refill()
    {
        result_t hr;

        if (m_stream_pos != m_pos) {
            hr = m_strm->seek(m_pos, fs_base::C_SEEK_SET);
            if (hr.failed())
                return false;

            m_stream_pos = m_pos;
        }

        obj_ptr<Buffer_base> data;
        hr = m_strm->cc_readBuffer((int32_t)ZIP_IO_BUFF_SIZE, data);
        if (hr.failed() || hr.is_return_null())
            return false;

        Buffer* buff = Buffer::Cast(data);
        if (buff->length() == 0)
            return false;

        m_cache = data;
        m_cache_start = m_pos;
        m_stream_pos += buff->length();

        return true;
    }

private:
    SeekableStream_base* m_strm;
    int64_t m_pos = 0;
    int64_t m_stream_pos = 0;
    obj_ptr<Buffer_base> m_cache;
    int64_t m_cache_start = 0;
};

const char* zip_error(int32_t err)
{
    switch (err) {
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

static result_t zip_error_result(int32_t err)
{
    return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));
}

#ifdef _WIN32
static voidpf ZCALLBACK fopen64_file_func(voidpf opaque, const void* filename, int mode)
{
    return (voidpf)::_wfopen(UTF8_W((const char*)filename), UTF8_W((const char*)"rb"));
}
#endif

result_t ifZipFile(exlib::string filename, bool& retVal)
{
    unzFile unz;
#ifndef _WIN32
    if ((unz = unzOpen2_64(filename.c_str(), NULL)) == NULL)
#else
    zlib_filefunc64_def io;
    fill_fopen64_filefunc(&io);
    io.zopen64_file = fopen64_file_func;
    if ((unz = unzOpen2_64(filename.c_str(), &io)) == NULL)
#endif
        retVal = false;
    else {
        retVal = true;
        unzClose(unz);
    }

    return result_t();
}

result_t zip_base::isZipFile(exlib::string filename, bool& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    result_t hr = normalize_file_path_like(filename, filename);
    if (hr.failed())
        return hr;

    return ifZipFile(filename, retVal);
}

// the three forms of the merged open entry: a file path, the file data and an
// already opened stream. Only the path form can reach the continuation (its
// openFile handover); the data and stream forms never leave the fiber and are
// continuation-free (plans/async-handle-protocol-minimal-2026-10-08.md 8.1 P2).
static result_t zip_open_path(exlib::string path, exlib::string mod, exlib::string codec,
    obj_ptr<ZipFile_base>& retVal, AsyncHandle ac);
static result_t zip_open_data(Buffer_base* data, exlib::string mod, exlib::string codec,
    obj_ptr<ZipFile_base>& retVal);
static result_t zip_open_strm(SeekableStream_base* strm, exlib::string mod, exlib::string codec,
    obj_ptr<ZipFile_base>& retVal);

static result_t zip_open_path(exlib::string path, exlib::string mod, exlib::string codec,
    obj_ptr<ZipFile_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    obj_ptr<SeekableStream_base> file;
    result_t hr;
    bool exists;

    if ((mod == "w"))
        hr = fs_base::openFile(path, "w", file, std::move(ac));
    else if ((mod == "a") || (mod == "a+")) {
        hr = fs_base::cc_exists(path, exists);
        if (hr.failed())
            return hr;

        if (!exists)
            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_INVALID_ARG_VALUE").with_message("ZipFile: zip file not exists!");

        hr = fs_base::openFile(path, "r+", file, std::move(ac));
    }

    else
        hr = fs_base::openFile(path, "r", file, std::move(ac));

    if (hr.failed())
        return hr;

    return zip_open_strm(file.get(), mod, codec, retVal);
}

static result_t zip_open_data(Buffer_base* data, exlib::string mod, exlib::string codec,
    obj_ptr<ZipFile_base>& retVal)
{
    exlib::string strData;

    data->toString(strData);
    obj_ptr<SeekableStream_base> strm = new MemoryStream::CloneStream(std::move(strData), 0);

    return zip_open_strm(strm.get(), mod, codec, retVal);
}

static result_t zip_open_strm(SeekableStream_base* strm, exlib::string mod, exlib::string codec,
    obj_ptr<ZipFile_base>& retVal)
{
    retVal = new ZipFile(strm, mod, codec);
    return result_t();
}

result_t zip_base::open(Union_open_data data, exlib::string mod, exlib::string codec,
    obj_ptr<ZipFile_base>& retVal, AsyncHandle ac)
{
    if (std::holds_alternative<obj_ptr<Buffer_base>>(data))
        return zip_open_data(std::get<obj_ptr<Buffer_base>>(data).get(), mod, codec, retVal);

    if (std::holds_alternative<obj_ptr<SeekableStream_base>>(data))
        return zip_open_strm(std::get<obj_ptr<SeekableStream_base>>(data).get(), mod, codec, retVal);

    return zip_open_path(std::get<exlib::string>(data), mod, codec, retVal, std::move(ac));
}

// The zip handle is released: one helper so the fourteen call sites cannot
// drift apart, and so the code string is stated once.
static result_t zip_closed()
{
    return errhelp::closed("ERR_FIBJS_ZIP_CLOSED", "ZipFile: file is closed.");
}

ZipFile::ZipFile(SeekableStream_base* strm, exlib::string mod, exlib::string codec)
    : m_unz(NULL)
    , m_zip(NULL)
    , m_codec(codec)
    , m_mod(mod)
    , m_strm(strm)
{
    // minizip copies the function table but keeps the opaque pointer, so the
    // StreamIO (and its read-ahead cache) has to outlive this call.
    m_sio = new StreamIO(strm);
    if ((mod == "r"))
        m_unz = unzOpen2_64("", m_sio);
    else if ((mod == "w"))
        m_zip = zipOpen2_64("", APPEND_STATUS_CREATE, NULL, m_sio);
    else if ((mod == "a") || (mod == "a+"))
        m_zip = zipOpen2_64("", APPEND_STATUS_ADDINZIP, NULL, m_sio);
}

ZipFile::~ZipFile()
{
    // A reader holds no buffered write state: release the minizip handle,
    // which also closes the archive stream exactly like close(). The VFS opens
    // a reader per uncached member, so leaving this to the leak-free path is
    // what keeps those reads from accumulating handles. A writer is left
    // alone: closing it here would append a central directory while unwinding.
    if (m_unz) {
        unzClose(m_unz);
        m_unz = NULL;
    }

    delete m_sio;
}

static const char* compress_type_name(uint16_t method)
{
    if (method == 0)
        return "Stored";
    if (method == Z_DEFLATED)
        return "Deflate";
    if (method == Z_BZIP2ED)
        return "BZip2";

    return "Unknown";
}

// Walk the central directory once and remember where every entry lives, so a
// name lookup costs one read instead of an unzLocateFile scan over all the
// entries before it. The map keeps the first occurrence, which is the entry
// unzLocateFile would have stopped at.
result_t ZipFile::build_index()
{
    if (m_index_built)
        return result_t();

    unz_global_info64 gi;
    int32_t err = unzGetGlobalInfo64(m_unz, &gi);
    if (err != UNZ_OK)
        return zip_error_result(err);

    m_index.clear();

    if (gi.number_entry == 0) {
        m_index_built = true;
        return result_t();
    }

    err = unzGoToFirstFile(m_unz);
    if (err != UNZ_OK)
        return zip_error_result(err);

    uint64_t i;
    for (i = 0; i < gi.number_entry; i++) {
        unz_file_info64 file_info;

        err = unzGetCurrentFileInfo64(m_unz, &file_info, NULL, 0, NULL, 0, NULL, 0);
        if (err != UNZ_OK)
            return zip_error_result(err);

        exlib::string filename;

        if (file_info.size_filename > 0) {
            char stack_name[512];

            if (file_info.size_filename < sizeof(stack_name)) {
                err = unzGetCurrentFileInfo64(m_unz, NULL, stack_name, sizeof(stack_name), NULL, 0, NULL, 0);
                if (err != UNZ_OK)
                    return zip_error_result(err);
                filename.assign(stack_name, (size_t)file_info.size_filename);
            } else {
                std::vector<char> name((size_t)file_info.size_filename + 1);

                err = unzGetCurrentFileInfo64(m_unz, NULL, name.data(), (uLong)name.size(), NULL, 0, NULL, 0);
                if (err != UNZ_OK)
                    return zip_error_result(err);
                filename.assign(name.data(), (size_t)file_info.size_filename);
            }
        }

        unz64_file_pos pos;

        err = unzGetFilePos64(m_unz, &pos);
        if (err != UNZ_OK)
            return zip_error_result(err);

        index_entry entry;

        entry.pos = pos;
        entry.date.fromDosTime(file_info.dosDate);
        entry.compress_type = compress_type_name(file_info.compression_method);
        entry.compress_size = file_info.compressed_size;
        entry.file_size = file_info.uncompressed_size;
        entry.password = (file_info.flag & 1) != 0;

        result_t hr = encoding_conv(m_codec).decode(filename, entry.filename);
        if (hr.failed())
            entry.filename = filename;

        m_index.emplace(filename, entry);

        if ((i + 1) < gi.number_entry) {
            // unzGoToNextFile itself stops at number_entry, but the 16-bit
            // counter hack makes its boundary unreliable when the count is
            // exactly 0xffff, so the loop counts and this only advances
            // between entries.
            err = unzGoToNextFile(m_unz);
            if (err != UNZ_OK)
                return zip_error_result(err);
        }
    }

    m_index_built = true;
    return result_t();
}

result_t ZipFile::locate(exlib::string member)
{
    result_t hr = build_index();
    if (hr.failed())
        return hr;

    std::unordered_map<exlib::string, index_entry>::iterator it = m_index.find(member);
    if (it == m_index.end())
        return zip_error_result(UNZ_END_OF_LIST_OF_FILE);

    int32_t err = unzGoToFilePos64(m_unz, &it->second.pos);
    if (err != UNZ_OK)
        return zip_error_result(err);

    return result_t();
}

result_t ZipFile::get_index(std::unordered_map<exlib::string, index_entry>& retVal)
{
    if (!m_unz)
        return zip_closed();

    result_t hr = build_index();
    if (hr.failed())
        return hr;

    retVal = m_index;
    return result_t();
}

result_t ZipFile::read_at(const unz64_file_pos& pos, const exlib::string& member, exlib::string password, obj_ptr<Buffer_base>& retVal)
{
    if (!m_unz)
        return zip_closed();

    int32_t err = unzGoToFilePos64(m_unz, &pos);
    if (err != UNZ_OK)
        return zip_error_result(err);

    return read(password, retVal, &member);
}

template <class T>
result_t ZipFile::get_info(obj_ptr<T>& retVal)
{
    if (!m_unz)
        return zip_closed();

    int32_t err;
    unz_file_info64 file_info;

    err = unzGetCurrentFileInfo64(m_unz, &file_info, NULL, 0, NULL, 0, NULL, 0);
    if (err != UNZ_OK)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));

    // The entry name is sized by the directory record, not by a fixed 256-byte
    // buffer: longer names were silently truncated in namelist/infolist.
    std::vector<char> name((size_t)file_info.size_filename + 1);

    err = unzGetCurrentFileInfo64(m_unz, NULL, name.data(), (uLong)name.size(), NULL, 0, NULL, 0);
    if (err != UNZ_OK)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));

    exlib::string _filename;
    result_t hr = encoding_conv(m_codec).decode(name.data(), _filename);
    if (hr.failed())
        return hr;

    retVal = new T();
    retVal->filename = _filename;
    retVal->date.fromDosTime(file_info.dosDate);
    retVal->compress_type = compress_type_name(file_info.compression_method);

    retVal->file_size = file_info.uncompressed_size;
    retVal->compress_size = file_info.compressed_size;
    retVal->password = file_info.flag & 1;
    return result_t();
}

result_t ZipFile::namelist(std::vector<exlib::string>& retVal, AsyncHandle ac)
{
    if (!m_unz)
        return zip_closed();

    if (ac.isSync())
        return result_t::nosync();

    result_t hr;
    unz_global_info64 gi;
    int32_t err;
    uint32_t i;

    if (m_mod != "r")
        return result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code("ERR_INVALID_STATE").with_message("ZipFile: can not read!");

    err = unzGetGlobalInfo64(m_unz, &gi);
    if (err != UNZ_OK)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));

    err = unzGoToFirstFile(m_unz);
    if (err != UNZ_OK)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));

    for (i = 0; i < gi.number_entry; i++) {
        obj_ptr<ZipFile_base::GetinfoType> info;

        hr = get_info(info);
        if (hr.failed())
            return hr;

        retVal.push_back(info->filename);

        if ((i + 1) < gi.number_entry) {
            err = unzGoToNextFile(m_unz);
            if (err != UNZ_OK)
                return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));
        }
    }

    return result_t();
}

result_t ZipFile::infolist(std::vector<obj_ptr<ZipFile_base::InfolistType>>& retVal, AsyncHandle ac)
{
    if (!m_unz)
        return zip_closed();

    if (ac.isSync())
        return result_t::nosync();

    result_t hr;
    unz_global_info64 gi;
    int32_t err;
    uint32_t i;

    if (m_mod != "r")
        return result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code("ERR_INVALID_STATE").with_message("ZipFile: can not read!");

    err = unzGetGlobalInfo64(m_unz, &gi);
    if (err != UNZ_OK)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));

    err = unzGoToFirstFile(m_unz);
    if (err != UNZ_OK)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));

    for (i = 0; i < gi.number_entry; i++) {
        obj_ptr<ZipFile_base::InfolistType> item;

        hr = get_info(item);
        if (hr.failed())
            return hr;

        retVal.push_back(item);

        if ((i + 1) < gi.number_entry) {
            err = unzGoToNextFile(m_unz);
            if (err != UNZ_OK)
                return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));
        }
    }

    return result_t();
}

result_t ZipFile::getinfo(exlib::string member, obj_ptr<ZipFile_base::GetinfoType>& retVal, AsyncHandle ac)
{
    if (!m_unz)
        return zip_closed();

    if (ac.isSync())
        return result_t::nosync();

    if (m_mod != "r")
        return result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code("ERR_INVALID_STATE").with_message("ZipFile: can not read!");

    exlib::string _filename;
    result_t hr = encoding_conv(m_codec).encode(member, _filename);
    if (hr.failed())
        return hr;

    hr = locate(_filename);
    if (hr.failed())
        return hr;

    return get_info(retVal);
}

#define BUF_SIZE 65536

result_t ZipFile::extract(SeekableStream_base* strm, exlib::string password)
{
    if (!m_unz)
        return zip_closed();

    int32_t err;
    result_t hr;
    std::vector<uint8_t> buf(BUF_SIZE);

    if (!password.empty())
        err = unzOpenCurrentFilePassword(m_unz, password.c_str());
    else
        err = unzOpenCurrentFile(m_unz);
    if (err != UNZ_OK)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));

    do {
        err = unzReadCurrentFile(m_unz, buf.data(), BUF_SIZE);
        if (err < 0) {
            unzCloseCurrentFile(m_unz);
            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));
        }

        if (err > 0) {
            obj_ptr<Buffer_base> buffer = new Buffer(buf.data(), err);
            hr = strm->cc_writeBuffer(buffer);
            if (hr.failed()) {
                unzCloseCurrentFile(m_unz);
                return hr;
            }
        }
    } while (err > 0);

    err = unzCloseCurrentFile(m_unz);
    if (err != UNZ_OK)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));

    return result_t();
}

// Read the current entry straight into a Buffer of its documented size. The
// MemoryStream detour inflated in 8KB chunks, allocated one Buffer per chunk
// and then copied the whole entry again on readAll; a single call into
// unzReadCurrentFile fills the result buffer in place.
result_t ZipFile::read(exlib::string password, obj_ptr<Buffer_base>& retVal, const exlib::string* expect_name)
{
    if (!m_unz)
        return zip_closed();

    int32_t err;
    unz_file_info64 file_info;

    err = unzGetCurrentFileInfo64(m_unz, &file_info, NULL, 0, NULL, 0, NULL, 0);
    if (err != UNZ_OK)
        return zip_error_result(err);

    if (expect_name) {
        // The cached position only describes the entry if its name still
        // matches; a rewritten archive can leave a valid-looking record at
        // the old offset.
        std::vector<char> name((size_t)file_info.size_filename + 1);

        err = unzGetCurrentFileInfo64(m_unz, NULL, name.data(), (uLong)name.size(), NULL, 0, NULL, 0);
        if (err != UNZ_OK)
            return zip_error_result(err);

        if (file_info.size_filename != expect_name->length()
            || memcmp(name.data(), expect_name->c_str(), file_info.size_filename) != 0)
            return zip_error_result(UNZ_BADZIPFILE);
    }

    if (!password.empty())
        err = unzOpenCurrentFilePassword(m_unz, password.c_str());
    else
        err = unzOpenCurrentFile(m_unz);
    if (err != UNZ_OK)
        return zip_error_result(err);

    size_t size = (size_t)file_info.uncompressed_size;
    std::shared_ptr<v8::BackingStore> store = NewBackingStore(size);
    uint8_t* data = (uint8_t*)store->Data();
    size_t remain = size;

    while (remain > 0) {
        int32_t n = unzReadCurrentFile(m_unz, data, (unsigned)std::min<size_t>(remain, 0x40000000));
        if (n < 0) {
            unzCloseCurrentFile(m_unz);
            return zip_error_result(n);
        }

        if (n == 0)
            break;

        data += n;
        remain -= n;
    }

    err = unzCloseCurrentFile(m_unz);
    if (err != UNZ_OK)
        return zip_error_result(err);

    retVal = new Buffer(store, 0, size - remain);
    return result_t();
}

result_t ZipFile::read(exlib::string member, exlib::string password, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (!m_unz)
        return zip_closed();

    if (ac.isSync())
        return result_t::nosync();

    exlib::string _filename;
    result_t hr = encoding_conv(m_codec).encode(member, _filename);
    if (hr.failed())
        return hr;

    hr = locate(_filename);
    if (hr.failed())
        return hr;

    return read(password, retVal);
}

result_t ZipFile::extract(exlib::string member, exlib::string path, exlib::string password, AsyncHandle ac)
{
    if (!m_unz)
        return zip_closed();

    if (ac.isSync())
        return result_t::nosync();

    exlib::string _filename;
    result_t hr = encoding_conv(m_codec).encode(member, _filename);
    if (hr.failed())
        return hr;

    obj_ptr<SeekableStream_base> file;

    hr = locate(_filename);
    if (hr.failed())
        return hr;

    hr = fs_base::openFile(path, "w", file, std::move(ac));
    if (hr.failed())
        return hr;

    return extract(file, password);
}

result_t ZipFile::extract(exlib::string member, SeekableStream_base* strm, exlib::string password, AsyncHandle ac)
{
    if (!m_unz)
        return zip_closed();

    if (ac.isSync())
        return result_t::nosync();

    exlib::string _filename;
    result_t hr = encoding_conv(m_codec).encode(member, _filename);
    if (hr.failed())
        return hr;

    hr = locate(_filename);
    if (hr.failed())
        return hr;

    return extract(strm, password);
}

result_t ZipFile::checkGuard(exlib::string path)
{
    result_t hr;
    bool exists;
    const char* ptr = path.c_str();
    const char* tmpptr;

    for (tmpptr = ptr; *tmpptr; tmpptr++) {
        if (*tmpptr == PATH_SLASH) {
            exlib::string pathToCheck(ptr, tmpptr - ptr);
            hr = fs_base::cc_exists(pathToCheck, exists);
            if (hr.failed())
                return hr;
            if (!exists) {
                Variant mkdirResult;
                hr = fs_base::cc_mkdir(pathToCheck, 0777, mkdirResult);
                if (hr.failed())
                    return hr;
            }
        }
    }

    return result_t();
}

result_t ZipFile::extractAll(exlib::string path, exlib::string password, AsyncHandle ac)
{
    if (!m_unz)
        return zip_closed();

    if (ac.isSync())
        return result_t::nosync();

    // One open per entry, each one taking the continuation: a state machine
    // (plans/async-handle-protocol-minimal-2026-10-08.md 8.1 P3) keeps exactly
    // one handle per event instead of passing it through the loop.
    class asyncExtractAll : public AsyncState {
    public:
        asyncExtractAll(ZipFile* pThis, exlib::string path, exlib::string password, AsyncHandle ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_path(path)
            , m_password(password)
        {
            init(start);
        }

        ON_STATE(asyncExtractAll, start)
        {
            bool exists;

            result_t hr = fs_base::cc_exists(m_path, exists);
            if (hr.failed())
                return hr;
            if (!exists)
                return result_t::make(UV_ENOENT).with_code("ENOENT").with_message("ZipFile: no such file or directory");

            int32_t err = unzGetGlobalInfo64(m_pThis->m_unz, &m_gi);
            if (err != UNZ_OK)
                return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));

            err = unzGoToFirstFile(m_pThis->m_unz);
            if (err != UNZ_OK)
                return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));

            m_i = 0;
            return next(next_entry);
        }

        ON_STATE(asyncExtractAll, next_entry)
        {
            if (m_i >= m_gi.number_entry)
                return next();

            obj_ptr<ZipFile_base::GetinfoType> info;

            result_t hr = m_pThis->get_info(info);
            if (hr.failed())
                return hr;

            m_fpath1 = m_path;
            m_fpath1 += PATH_SLASH;
            path_base::normalize(m_fpath1 + info->filename, m_fpath1);
            m_pThis->checkGuard(m_fpath1);

            return next(find_name);
        }

        ON_STATE(asyncExtractAll, find_name)
        {
            bool exists;

            m_fpath = m_fpath1;
            result_t hr = fs_base::cc_exists(m_fpath, exists);
            if (hr.failed())
                return hr;

            m_fpath1 += "?";
            if (!exists)
                return next(open);

            return next(find_name);
        }

        ON_STATE(asyncExtractAll, open)
        {
            return fs_base::openFile(m_fpath, "w", m_file, next(extract));
        }

        ON_STATE(asyncExtractAll, extract)
        {
            result_t hr = m_pThis->extract(m_file, m_password);
            if (hr.failed())
                return hr;

            m_i++;
            if (m_i < m_gi.number_entry) {
                int32_t err = unzGoToNextFile(m_pThis->m_unz);
                if (err != UNZ_OK)
                    return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));
            }

            return next(next_entry);
        }

    private:
        obj_ptr<ZipFile> m_pThis;
        exlib::string m_path;
        exlib::string m_password;
        unz_global_info64 m_gi;
        uint32_t m_i;
        obj_ptr<SeekableStream_base> m_file;
        exlib::string m_fpath;
        exlib::string m_fpath1;
    };

    return (new asyncExtractAll(this, path, password, std::move(ac)))->post(result_t());
}

result_t ZipFile::readAll(exlib::string password, std::vector<obj_ptr<ZipFile_base::ReadAllType>>& retVal, AsyncHandle ac)
{
    if (!m_unz)
        return zip_closed();

    if (ac.isSync())
        return result_t::nosync();

    result_t hr;
    unz_global_info64 gi;
    int32_t err;
    uint32_t i;

    err = unzGetGlobalInfo64(m_unz, &gi);
    if (err != UNZ_OK)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));

    err = unzGoToFirstFile(m_unz);
    if (err != UNZ_OK)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));

    for (i = 0; i < gi.number_entry; i++) {
        obj_ptr<ZipFile_base::ReadAllType> info;

        hr = get_info(info);
        if (hr.failed())
            return hr;

        hr = read(password, info->data);
        if (hr.failed())
            return hr;

        retVal.push_back(info);

        if ((i + 1) < gi.number_entry) {
            err = unzGoToNextFile(m_unz);
            if (err != UNZ_OK)
                return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));
        }
    }

    return result_t();
}

result_t ZipFile::getFileCrc(SeekableStream_base* strm, uint32_t& crc)
{
    uint32_t fileCrc = 0;
    result_t hr;

    while (true) {
        obj_ptr<Buffer_base> buf;

        hr = strm->cc_readBuffer(BUF_SIZE, buf);
        if (hr.is_return_null())
            break;
        else if (hr.failed())
            return hr;

        Buffer* buff = Buffer::Cast(buf);
        fileCrc = crc32(fileCrc, buff->data(), buff->length());

        if (!buff->length())
            break;
    }

    crc = fileCrc;
    return result_t();
}

result_t ZipFile::write(exlib::string filename, exlib::string password, SeekableStream_base* strm)
{
    if (!m_zip)
        return zip_closed();

    int32_t err;
    result_t hr;
    uint32_t crc = 0;
    obj_ptr<Buffer_base> buf;
    obj_ptr<Buffer> buff;

    if (!password.empty()) {
        hr = getFileCrc(strm, crc);
        if (hr.failed())
            return hr;
    }

    zip_fileinfo zi = { { 0 } };
    date_t d;
    date_t::Part dp;

    d.now();
    d.toLocal();
    dp = d.getdate();

    zi.tmz_date.tm_year = dp.wYear;
    zi.tmz_date.tm_mon = dp.wMonth;
    zi.tmz_date.tm_mday = dp.wDay + 1;
    zi.tmz_date.tm_hour = dp.wHour;
    zi.tmz_date.tm_min = dp.wMinute;
    zi.tmz_date.tm_sec = dp.wSecond;

    exlib::string _filename;
    hr = encoding_conv(m_codec).encode(filename, _filename);
    if (hr.failed())
        return hr;

    err = zipOpenNewFileInZip3_64(m_zip, _filename.c_str(), &zi, NULL,
        0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 0,
        -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
        password.empty() ? NULL : password.c_str(), crc, 1);
    if (err != ZIP_OK)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));

    strm->rewind();
    do {
        hr = strm->cc_readBuffer(BUF_SIZE, buf);
        if (hr.is_return_null())
            break;
        else if (hr.failed())
            return hr;

        buff = Buffer::Cast(buf);

        err = zipWriteInFileInZip(m_zip, buff->data(), (uint32_t)buff->length());
        if (err != ZIP_OK) {
            zipCloseFileInZip(m_zip);
            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));
        }
    } while (buff->length() > 0);

    err = zipCloseFileInZip(m_zip);
    if (err != ZIP_OK)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));

    return result_t();
}

result_t ZipFile::write(exlib::string filename, exlib::string inZipName, exlib::string password, AsyncHandle ac)
{
    if (!m_zip)
        return zip_closed();

    if (ac.isSync())
        return result_t::nosync();

    result_t hr;
    obj_ptr<SeekableStream_base> file;

    hr = fs_base::openFile(filename, "r", file, std::move(ac));
    if (hr.failed())
        return hr;

    return write(inZipName, password, file);
}

result_t ZipFile::write(Buffer_base* data, exlib::string inZipName, exlib::string password, AsyncHandle ac)
{
    if (!m_zip)
        return zip_closed();

    if (ac.isSync())
        return result_t::nosync();

    result_t hr;
    obj_ptr<MemoryStream> strm;

    strm = new MemoryStream();
    strm->writeBuffer(data, nullptr);

    hr = strm->rewind();
    if (hr.failed())
        return hr;

    return write(inZipName, password, strm);
}

result_t ZipFile::write(SeekableStream_base* strm, exlib::string inZipName, exlib::string password, AsyncHandle ac)
{
    if (!m_zip)
        return zip_closed();

    if (ac.isSync())
        return result_t::nosync();

    return write(inZipName, password, strm);
}

result_t ZipFile::close(AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    int32_t err = ZIP_OK;

    if (m_unz) {
        err = unzClose(m_unz);
        m_unz = NULL;
    } else if (m_zip) {
        err = zipClose(m_zip, NULL);
        m_zip = NULL;
    }

    if (err != ZIP_OK)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_ZIP_INVALID_FORMAT").with_message(zip_error(err));

    return result_t();
}
}