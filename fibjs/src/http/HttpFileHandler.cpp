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
    v8::Local<v8::Array> keys = mimes->GetPropertyNames();
    int32_t len = keys->Length();
    int32_t i;
    result_t hr;

    for (i = 0; i < len; i++) {
        v8::Local<v8::Value> ks = keys->Get(i);
        exlib::string v;

        hr = GetArgumentValue(mimes->Get(ks), v);
        if (hr < 0)
            return CHECK_ERROR(hr);

        m_mimes.insert(std::pair<exlib::string, exlib::string>(*v8::String::Utf8Value(ks), v));
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
            , m_pre_gz(false)
            , m_index(false)
            , m_dirPos(0)
        {
            obj_ptr<Message_base> m;
            req->get_response(m);
            m_rep = (HttpResponse_base*)(Message_base*)m;

            Variant hdr;

            if (m_req->firstHeader("Accept-Encoding", hdr) != CALL_RETURN_NULL) {
                exlib::string str = hdr.string();

                if (qstristr(str.c_str(), "gzip"))
                    m_pre_gz = true;
            }

            m_req->get_value(m_value);
            Url::decodeURI(m_value, m_value);
            path_base::normalize(m_pThis->m_root + m_value, m_url);

            set(start);
        }

        static int32_t start(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;

            if (qstrchr(pThis->m_url.c_str(), '%')) {
                pThis->m_rep->set_status(400);
                return pThis->done(CALL_RETURN_NULL);
            }

            pThis->m_path = pThis->m_url;

            if (isPathSlash(pThis->m_path[pThis->m_path.length() - 1])) {
                pThis->m_path.append("index.html", 10);
                pThis->m_index = true;
            }

            if (pThis->m_pre_gz)
                pThis->m_path.append(".gz", 3);

            pThis->set(open);
            return fs_base::openFile(pThis->m_path, "r", pThis->m_file, pThis);
        }

        static int32_t autoindex(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;

            pThis->m_path = pThis->m_url;
            pThis->set(list_stat);
            return fs_base::readdir(pThis->m_path, pThis->m_dir, pThis);
        }

        static int32_t list_stat(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;
            int32_t length;
            static char padding[] = "                                                              ";

            pThis->m_dir->get_length(length);

            if (pThis->m_dirPos == 0) {
                obj_ptr<List_base> _dir = pThis->m_dir;
                _dir->sort(pThis->m_dir);

                pThis->m_file = new MemoryStream();
                pThis->m_file->cc_write(new Buffer("<html>\n<head><title>Index of "
                    + pThis->m_value + "</title></head>\n<body bgcolor=white>\n<h1>Index of "
                    + pThis->m_value + "</h1><hr><pre>"));

                if (pThis->m_value.length() > 1)
                    pThis->m_file->cc_write(new Buffer("<a href=\"../\">../</a>\n"));
            } else {
                exlib::string name, ds, ss;
                date_t d;
                int64_t sz;
                bool is_dir = false;
                int32_t padding_len;

                pThis->m_stat->get_name(name);
                pThis->m_stat->isDirectory(is_dir);
                if (is_dir)
                    name += '/';
                pThis->m_file->cc_write(new Buffer("<a href=\"" + name + "\">" + name + "</a>"));
                padding_len = 40 - name.length();
                if (padding_len < 1)
                    padding_len = 1;
                pThis->m_file->cc_write(new Buffer(padding, padding_len));

                pThis->m_stat->get_mtime(d);
                d.sqlString(ds);
                pThis->m_file->cc_write(new Buffer(ds));

                pThis->m_stat->get_size(sz);
                ss = niceSize(sz);
                padding_len = 12 - ss.length();
                if (padding_len < 1)
                    padding_len = 1;
                pThis->m_file->cc_write(new Buffer(padding, padding_len));
                pThis->m_file->cc_write(new Buffer(ss + '\n'));
            }

            if (pThis->m_dirPos >= length) {
                pThis->m_file->cc_write(new Buffer("</pre><hr></body>\n</html>"));
                pThis->m_file->rewind();

                pThis->m_rep->set_body(pThis->m_file);
                pThis->m_rep->addHeader("Content-Type", "text/html");

                return pThis->done(CALL_RETURN_NULL);
            }

            Variant v;

            pThis->m_dir->_indexed_getter(pThis->m_dirPos, v);
            pThis->m_dirPos++;

            pThis->m_path = pThis->m_url;
            pThis->m_path += v.string();

            return fs_base::stat(pThis->m_path, pThis->m_stat, pThis);
        }

        static int32_t open(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;
            exlib::string ext;

            if (pThis->m_index)
                pThis->m_rep->addHeader("Content-Type", "text/html");
            else {
                path_base::extname(pThis->m_url, ext);

                if (ext.length() > 0) {
                    const char* pKey = ext.c_str() + 1;
                    std::map<exlib::string, exlib::string>& _mimes = pThis->m_pThis->m_mimes;

                    std::map<exlib::string, exlib::string>::iterator it = _mimes.find(pKey);

                    if (it != _mimes.end())
                        pThis->m_rep->addHeader("Content-Type", it->second);
                    else {
                        const char** pMimeType = (const char**)bsearch(&pKey,
                            &s_mimeTypes, sizeof(s_mimeTypes) / sizeof(s_defType),
                            sizeof(s_defType), mt_cmp);

                        if (!pMimeType)
                            pMimeType = s_defType;

                        pThis->m_rep->addHeader("Content-Type", pMimeType[1]);
                    }
                }
            }

            pThis->set(stat);
            return pThis->m_file->stat(pThis->m_stat, pThis);
        }

        static int32_t stat(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;
            date_t d;
            exlib::string str;

            pThis->m_stat->get_mtime(d);

            Variant v;
            if (pThis->m_req->firstHeader("If-Modified-Since", v)
                != CALL_RETURN_NULL) {
                date_t d1;
                double diff;
                exlib::string str = v.string();

                d1.parse(str);
                diff = d.diff(d1);

                if (diff > -1000 && diff < 1000) {
                    pThis->m_rep->set_status(304);
                    return pThis->done(CALL_RETURN_NULL);
                }
            }

            d.toGMTString(str);

            pThis->m_rep->addHeader("Last-Modified", str);
            pThis->m_rep->set_body(pThis->m_file);

            if (pThis->m_pre_gz)
                pThis->m_rep->addHeader("Content-Encoding", "gzip");

            return pThis->done(CALL_RETURN_NULL);
        }

        virtual int32_t error(int32_t v)
        {
            if (is(open)) {
                if (m_pre_gz) {
                    m_pre_gz = false;

                    set(start);
                    return 0;
                }

                if (m_index) {
                    m_index = false;

                    if (m_autoIndex) {
                        set(autoindex);
                        return 0;
                    }
                }
            }

            m_rep->set_status(404);
            return done(CALL_RETURN_NULL);
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
        bool m_pre_gz;
        bool m_index;
        obj_ptr<List_base> m_dir;
        int32_t m_dirPos;
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<HttpRequest_base> req = HttpRequest_base::getInstance(v);

    if (req == NULL)
        return CHECK_ERROR(CALL_E_BADVARTYPE);

    return (new asyncInvoke(this, req, m_autoIndex, ac))->post(0);
}

} /* namespace fibjs */
