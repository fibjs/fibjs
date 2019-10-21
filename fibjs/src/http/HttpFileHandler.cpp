/*
 * HttpFileHandler.cpp
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/http.h"
#include "ifs/fs.h"
#include "ifs/os.h"
#include "path.h"
#include "HttpFileHandler.h"
#include "HttpRequest.h"
#include "Url.h"
#include "Buffer.h"
#include "MemoryStream.h"

namespace fibjs {

static const char* s_defType[2] = { NULL, "application/octet-stream" };

static const char* s_mimeTypes[][2] = {
    { "ai", "application/postscript" },
    { "asc", "text/plain" },
    { "au", "audio/basic" },
    { "avi", "video/x-msvideo" },
    { "bmp", "image/bmp" },
    { "cdf", "application/x-netcdf" },
    { "cpio", "application/x-cpio" },
    { "css", "text/css" },
    { "doc", "application/msword" },
    { "eot", "application/vnd.ms-fontobject" },
    { "eps", "application/postscript" },
    { "etx", "text/x-setext" },
    { "gif", "image/gif" },
    { "htm", "text/html" },
    { "html", "text/html" },
    { "jpe", "image/jpeg" },
    { "jpeg", "image/jpeg" },
    { "jpg", "image/jpeg" },
    { "js", "application/x-javascript" },
    { "json", "application/json" },
    { "latex", "application/x-latex" },
    { "m3u", "audio/x-mpegurl" },
    { "man", "application/x-troff-man" },
    { "mid", "audio/midi" },
    { "midi", "audio/midi" },
    { "mov", "video/quicktime" },
    { "movie", "video/x-sgi-movie" },
    { "mp2", "audio/mpeg" },
    { "mp3", "audio/mpeg" },
    { "mp4", "video/mpeg" },
    { "mpe", "video/mpeg" },
    { "mpeg", "video/mpeg" },
    { "mpg", "video/mpeg" },
    { "mpga", "audio/mpeg" },
    { "otf", "font/opentype" },
    { "pdf", "application/pdf" },
    { "png", "image/png" },
    { "ppt", "application/vnd.ms-powerpoint" },
    { "ps", "application/postscript" },
    { "qt", "video/quicktime" },
    { "ra", "audio/x-realaudio" },
    { "ram", "audio/x-pn-realaudio" },
    { "rar", "application/rar" },
    { "rgb", "image/x-rgb" },
    { "rm", "audio/x-pn-realaudio" },
    { "rtf", "text/rtf" },
    { "rtx", "text/richtext" },
    { "sgm", "text/sgml" },
    { "sgml", "text/sgml" },
    { "src", "application/x-wais-source" },
    { "svg", "image/svg+xml" },
    { "swf", "application/x-shockwave-flash" },
    { "tar", "application/x-tar" },
    { "tcl", "application/x-tcl" },
    { "tex", "application/x-tex" },
    { "texi", "application/x-texinfo" },
    { "texinfo", "application/x-texinfo" },
    { "tif", "image/tiff" },
    { "tiff", "image/tiff" },
    { "ttf", "application/x-font-ttf" },
    { "txt", "text/plain" },
    { "wav", "audio/x-wav" },
    { "wbmp", "image/vnd.wap.wbmp" },
    { "wbxml", "application/vnd.wap.wbxml" },
    { "wml", "text/vnd.wap.wml" },
    { "wmlc", "application/vnd.wap.wmlc" },
    { "wmls", "text/vnd.wap.wmlscript" },
    { "wmlsc", "application/vnd.wap.wmlscriptc" },
    { "woff", "application/x-font-woff" },
    { "xbm", "image/x-xbitmap" },
    { "xht", "application/xhtml+xml" },
    { "xhtml", "application/xhtml+xml" },
    { "xls", "application/vnd.ms-excel" },
    { "xml", "text/xml" },
    { "xpm", "image/x-xpixmap" },
    { "xsl", "text/xml" },
    { "xwd", "image/x-xwindowdump" },
    { "xyz", "chemical/x-xyz" },
    { "zip", "application/zip" }
};

result_t http_base::fileHandler(exlib::string root, v8::Local<v8::Object> mimes,
    bool autoIndex, obj_ptr<Handler_base>& retVal)
{
    obj_ptr<HttpFileHandler> hdlr = new HttpFileHandler(root, autoIndex);
    result_t hr = hdlr->set_mimes(mimes);
    if (hr < 0)
        return hr;

    retVal = hdlr;
    return 0;
}

result_t HttpFileHandler::set_mimes(v8::Local<v8::Object> mimes)
{
    JSArray keys = mimes->GetPropertyNames();
    int32_t len = keys->Length();
    int32_t i;
    result_t hr;

    for (i = 0; i < len; i++) {
        JSValue ks = keys->Get(i);
        JSValue v;
        exlib::string s;

        v = mimes->Get(ks);
        if (v.IsEmpty())
            return CALL_E_JAVASCRIPT;
        hr = GetArgumentValue(v, s);
        if (hr < 0)
            return CHECK_ERROR(hr);

        m_mimes.insert(std::pair<exlib::string, exlib::string>(ToCString(v8::String::Utf8Value(ks)), s));
    }

    return 0;
}

static int32_t mt_cmp(const void* p, const void* q)
{
    return qstricmp(*(const char**)p, *(const char**)q);
}

result_t HttpFileHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncEvent* ac)
{
    class asyncInvoke : public AsyncState {
    public:
        asyncInvoke(HttpFileHandler* pThis, HttpRequest_base* req, bool autoIndex,
            AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_req(req)
            , m_autoIndex(autoIndex)
            , m_index(false)
            , m_dirPos(0)
        {
            req->get_response(m_rep);
            m_req->get_value(m_value);

            if (m_value.empty()) {
                m_url = m_pThis->m_root;
                next(start);
                return;
            }

            Url::decodeURI(m_value, m_value);
            if (qstrchr(m_value.c_str(), '\\') || qstrchr(m_value.c_str(), '%')) {
                next(stop);
                return;
            }

            path_posix_base::normalize(m_value, m_value);
            if (!qstrcmp(m_value.c_str(), "../", 3)) {
                next(stop);
                return;
            }

            m_url = m_pThis->m_root + m_value;
            next(start);
        }

        ON_STATE(asyncInvoke, start)
        {
            m_path = m_url;

            if (isPathSlash(m_path[m_path.length() - 1])) {
                m_path.append("index.html", 10);
                m_index = true;
            }

            return fs_base::openFile(m_path, "r", m_file, next(open));
        }

        ON_STATE(asyncInvoke, stop)
        {
            m_rep->set_statusCode(400);
            return next(CALL_RETURN_NULL);
        }

        ON_STATE(asyncInvoke, autoindex)
        {
            m_path = m_url;
            return fs_base::readdir(m_path, m_dir, next(list_stat));
        }

        ON_STATE(asyncInvoke, list_stat)
        {
            int32_t length;
            static char padding[] = "                                                              ";
            obj_ptr<Buffer_base> buf;

            m_dir->get_length(length);

            if (m_dirPos == 0) {
                m_dir->sort();

                m_file = new MemoryStream();
                buf = new Buffer("<html>\n<head><title>Index of "
                    + m_value + "</title></head>\n<body bgcolor=white>\n<h1>Index of "
                    + m_value + "</h1><hr><pre>");
                m_file->cc_write(buf);

                if (m_value.length() > 1) {
                    buf = new Buffer("<a href=\"../\">../</a>\n");
                    m_file->cc_write(buf);
                }
            } else {
                exlib::string name, ds, ss;
                date_t d;
                int64_t sz;
                bool is_dir = false;
                int32_t padding_len;

                m_stat->get_name(name);
                m_stat->isDirectory(is_dir);
                if (is_dir)
                    name += '/';
                buf = new Buffer("<a href=\"" + name + "\">" + name + "</a>");
                m_file->cc_write(buf);
                padding_len = 40 - (int32_t)name.length();
                if (padding_len < 1)
                    padding_len = 1;
                buf = new Buffer(padding, padding_len);
                m_file->cc_write(buf);

                m_stat->get_mtime(d);
                d.sqlString(ds);
                buf = new Buffer(ds);
                m_file->cc_write(buf);

                m_stat->get_size(sz);
                ss = niceSize((intptr_t)sz);
                padding_len = 12 - (int32_t)ss.length();
                if (padding_len < 1)
                    padding_len = 1;
                buf = new Buffer(padding, padding_len);
                m_file->cc_write(buf);
                buf = new Buffer(ss + '\n');
                m_file->cc_write(buf);
            }

            if (m_dirPos >= length) {
                buf = new Buffer("</pre><hr></body>\n</html>");
                m_file->cc_write(buf);
                m_file->rewind();

                m_rep->set_body(m_file);
                m_rep->addHeader("Content-Type", "text/html");

                return next(CALL_RETURN_NULL);
            }

            Variant v;

            m_dir->_indexed_getter(m_dirPos, v);
            m_dirPos++;

            m_path = m_url;
            m_path += v.string();

            return fs_base::stat(m_path, m_stat, this);
        }

        ON_STATE(asyncInvoke, open)
        {
            exlib::string ext;

            if (m_index)
                m_rep->addHeader("Content-Type", "text/html");
            else {
                path_base::extname(m_url, ext);

                if (ext.length() > 0) {
                    const char* pKey = ext.c_str() + 1;
                    std::map<exlib::string, exlib::string>& _mimes = m_pThis->m_mimes;

                    std::map<exlib::string, exlib::string>::iterator it = _mimes.find(pKey);

                    if (it != _mimes.end())
                        m_rep->addHeader("Content-Type", it->second);
                    else {
                        const char** pMimeType = (const char**)bsearch(&pKey,
                            &s_mimeTypes, sizeof(s_mimeTypes) / sizeof(s_defType),
                            sizeof(s_defType), mt_cmp);

                        if (!pMimeType)
                            pMimeType = s_defType;

                        m_rep->addHeader("Content-Type", pMimeType[1]);
                    }
                }
            }

            return m_file->stat(m_stat, next(stat));
        }

        ON_STATE(asyncInvoke, stat)
        {
            date_t d;
            exlib::string str;

            m_stat->get_mtime(d);

            exlib::string lastModified;
            if (m_req->firstHeader("If-Modified-Since", lastModified)
                != CALL_RETURN_NULL) {
                date_t d1;
                double diff;

                d1.parse(lastModified);
                diff = d.diff(d1);

                if (diff > -1000 && diff < 1000) {
                    m_rep->set_statusCode(304);
                    return next(CALL_RETURN_NULL);
                }
            }

            d.toGMTString(lastModified);

            m_rep->addHeader("Last-Modified", lastModified);
            m_rep->set_body(m_file);

            return next(CALL_RETURN_NULL);
        }

        virtual int32_t error(int32_t v)
        {
            if (at(start)) {
                if (m_index) {
                    m_index = false;

                    if (m_autoIndex)
                        return next(autoindex);
                }
            }

            m_rep->set_statusCode(404);
            return next(CALL_RETURN_NULL);
        }

    private:
        obj_ptr<HttpFileHandler> m_pThis;
        obj_ptr<HttpRequest_base> m_req;
        obj_ptr<HttpResponse_base> m_rep;
        obj_ptr<SeekableStream_base> m_file;
        obj_ptr<Stat_base> m_stat;
        exlib::string m_value;
        exlib::string m_url;
        exlib::string m_path;
        bool m_autoIndex;
        bool m_index;
        obj_ptr<NArray> m_dir;
        int32_t m_dirPos;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<HttpRequest_base> req = HttpRequest_base::getInstance(v);

    if (req == NULL)
        return CHECK_ERROR(CALL_E_BADVARTYPE);

    return (new asyncInvoke(this, req, m_autoIndex, ac))->post(0);
}

} /* namespace fibjs */
