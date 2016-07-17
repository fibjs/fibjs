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
#include "ifs/path.h"
#include "HttpFileHandler.h"
#include "HttpRequest.h"
#include "Url.h"

namespace fibjs
{

static const char *s_defType[2] =
{ NULL, "application/octet-stream" };

static const char *s_mimeTypes[][2] =
{
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

result_t http_base::fileHandler(const char *root, v8::Local<v8::Object> mimes,
                                obj_ptr<Handler_base> &retVal)
{
    obj_ptr<HttpFileHandler> hdlr = new HttpFileHandler(root);
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

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> ks = keys->Get(i);
        exlib::string v;

        hr = GetArgumentValue(mimes->Get(ks), v);
        if (hr < 0)
            return CHECK_ERROR(hr);

        m_mimes.insert(std::pair<exlib::string, exlib::string>(*v8::String::Utf8Value(ks), v));
    }

    return 0;
}

static int32_t mt_cmp(const void *p, const void *q)
{
    return qstricmp(*(const char **) p, *(const char **) q);
}

result_t HttpFileHandler::invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                                 AsyncEvent *ac)
{
    class asyncInvoke: public AsyncState
    {
    public:
        asyncInvoke(HttpFileHandler *pThis, HttpRequest_base *req,
                    AsyncEvent *ac) :
            AsyncState(ac), m_pThis(pThis), m_req(req), m_gzip(false)
        {
            obj_ptr<Message_base> m;
            req->get_response(m);
            m_rep = (HttpResponse_base *)(Message_base *)m;

            Variant hdr;

            if (m_req->firstHeader("Accept-Encoding", hdr) != CALL_RETURN_NULL)
            {
                exlib::string str = hdr.string();

                if (qstristr(str.c_str(), "gzip"))
                    m_gzip = true;
            }

            exlib::string value;

            m_req->get_value(value);

            Url::decodeURI(value.c_str(), (int32_t) value.length(), value);

            if (value.length() > 0 && isPathSlash(value[value.length() - 1]))
                value.append("index.html", 10);

            path_base::normalize((m_pThis->m_root + value).c_str(), m_url);

            set(start);
        }

        static int32_t start(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            if (qstrchr(pThis->m_url.c_str(), '%'))
            {
                pThis->m_rep->set_status(400);
                return pThis->done(CALL_RETURN_NULL);
            }

            pThis->m_path = pThis->m_url;
            if (pThis->m_gzip)
                pThis->m_path.append(".gz", 3);

            Variant v;

            if (pThis->m_req->firstHeader("If-Modified-Since",
                                          v) != CALL_RETURN_NULL)
            {
                exlib::string str = v.string();
                pThis->m_time.parse(str.c_str(), (int32_t) str.length());

                pThis->set(check);
                return fs_base::stat(pThis->m_path.c_str(), pThis->m_stat,
                                     pThis);
            }

            pThis->set(open);
            return fs_base::open(pThis->m_path.c_str(), "r", pThis->m_file,
                                 pThis);
        }

        static int32_t check(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            date_t d;
            double diff;

            pThis->m_stat->get_mtime(d);
            diff = d.diff(pThis->m_time);

            if (diff > -1000 && diff < 1000)
            {
                pThis->m_rep->set_status(304);
                return pThis->done(CALL_RETURN_NULL);
            }

            pThis->set(open);
            return fs_base::open(pThis->m_path.c_str(), "r", pThis->m_file,
                                 pThis);
        }

        static int32_t open(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;
            exlib::string ext;

            path_base::extname(pThis->m_url.c_str(), ext);

            if (ext.length() > 0)
            {
                const char *pKey = ext.c_str() + 1;
                std::map<exlib::string, exlib::string> &_mimes = pThis->m_pThis->m_mimes;

                std::map<exlib::string, exlib::string>::iterator it = _mimes.find(pKey);

                if (it != _mimes.end())
                    pThis->m_rep->addHeader("Content-Type", it->second);
                else
                {
                    const char **pMimeType = (const char **) bsearch(&pKey,
                                             &s_mimeTypes, sizeof(s_mimeTypes) / sizeof(s_defType),
                                             sizeof(s_defType), mt_cmp);

                    if (!pMimeType)
                        pMimeType = s_defType;

                    pThis->m_rep->addHeader("Content-Type", pMimeType[1]);
                }
            }

            pThis->set(stat);
            return pThis->m_file->stat(pThis->m_stat, pThis);
        }

        static int32_t stat(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;
            date_t d;
            exlib::string str;

            pThis->m_stat->get_mtime(d);

            d.toGMTString(str);

            pThis->m_rep->addHeader("Last-Modified", str);
            pThis->m_rep->set_body(pThis->m_file);

            if (pThis->m_gzip)
                pThis->m_rep->addHeader("Content-Encoding", "gzip");

            return pThis->done(CALL_RETURN_NULL);
        }

        virtual int32_t error(int32_t v)
        {
            if (m_gzip)
            {
                m_gzip = false;

                set(start);
                return 0;
            }

            m_rep->set_status(404);
            return done(CALL_RETURN_NULL);
        }

    private:
        obj_ptr<HttpFileHandler> m_pThis;
        obj_ptr<HttpRequest_base> m_req;
        obj_ptr<HttpResponse_base> m_rep;
        obj_ptr<File_base> m_file;
        obj_ptr<Stat_base> m_stat;
        exlib::string m_url;
        exlib::string m_path;
        date_t m_time;
        bool m_gzip;
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<HttpRequest_base> req = HttpRequest_base::getInstance(v);

    if (req == NULL)
        return CHECK_ERROR(CALL_E_BADVARTYPE);

    return (new asyncInvoke(this, req, ac))->post(0);
}

} /* namespace fibjs */
