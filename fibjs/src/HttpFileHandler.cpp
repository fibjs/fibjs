/*
 * HttpFileHandler.cpp
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#include "HttpFileHandler.h"
#include "HttpRequest.h"
#include "ifs/io.h"
#include "ifs/path.h"

namespace fibjs
{

static const char s_gzip[] = "gzip";
static const char* s_defType[3] =
{ NULL, "text/html", s_gzip };
static const char* s_mimeTypes[][3] =
{
{ "ai", "application/postscript", s_gzip },
{ "asc", "text/plain", s_gzip },
{ "au", "audio/basic" },
{ "avi", "video/x-msvideo" },
{ "bmp", "image/bmp", s_gzip },
{ "cdf", "application/x-netcdf" },
{ "cpio", "application/x-cpio" },
{ "css", "text/css", s_gzip },
{ "doc", "application/msword", s_gzip },
{ "eps", "application/postscript", s_gzip },
{ "etx", "text/x-setext", s_gzip },
{ "gif", "image/gif" },
{ "htm", "text/html", s_gzip },
{ "html", "text/html", s_gzip },
{ "jpe", "image/jpeg" },
{ "jpeg", "image/jpeg" },
{ "jpg", "image/jpeg" },
{ "js", "application/x-javascript", s_gzip },
{ "json", "application/json", s_gzip },
{ "latex", "application/x-latex", s_gzip },
{ "m3u", "audio/x-mpegurl" },
{ "man", "application/x-troff-man", s_gzip },
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
{ "pdf", "application/pdf" },
{ "png", "image/png" },
{ "ppt", "application/vnd.ms-powerpoint" },
{ "ps", "application/postscript", s_gzip },
{ "qt", "video/quicktime" },
{ "ra", "audio/x-realaudio" },
{ "ram", "audio/x-pn-realaudio" },
{ "rar", "application/rar" },
{ "rgb", "image/x-rgb" },
{ "rm", "audio/x-pn-realaudio" },
{ "rtf", "text/rtf", s_gzip },
{ "rtx", "text/richtext", s_gzip },
{ "sgm", "text/sgml", s_gzip },
{ "sgml", "text/sgml", s_gzip },
{ "src", "application/x-wais-source", s_gzip },
{ "svg", "image/svg+xml", s_gzip },
{ "swf", "application/x-shockwave-flash" },
{ "tar", "application/x-tar" },
{ "tcl", "application/x-tcl", s_gzip },
{ "tex", "application/x-tex", s_gzip },
{ "texi", "application/x-texinfo", s_gzip },
{ "texinfo", "application/x-texinfo", s_gzip },
{ "tif", "image/tiff" },
{ "tiff", "image/tiff" },
{ "txt", "text/plain", s_gzip },
{ "wav", "audio/x-wav", s_gzip },
{ "wbmp", "image/vnd.wap.wbmp", s_gzip },
{ "wbxml", "application/vnd.wap.wbxml", s_gzip },
{ "wml", "text/vnd.wap.wml", s_gzip },
{ "wmlc", "application/vnd.wap.wmlc", s_gzip },
{ "wmls", "text/vnd.wap.wmlscript", s_gzip },
{ "wmlsc", "application/vnd.wap.wmlscriptc", s_gzip },
{ "xbm", "image/x-xbitmap", s_gzip },
{ "xht", "application/xhtml+xml", s_gzip },
{ "xhtml", "application/xhtml+xml", s_gzip },
{ "xls", "application/vnd.ms-excel", s_gzip },
{ "xml", "text/xml", s_gzip },
{ "xpm", "image/x-xpixmap", s_gzip },
{ "xsl", "text/xml", s_gzip },
{ "xwd", "image/x-xwindowdump" },
{ "xyz", "chemical/x-xyz" },
{ "zip", "application/zip" } };

static int mt_cmp(const void *p, const void *q)
{
	return qstricmp(*(const char**) p, *(const char**) q);
}

result_t HttpFileHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
		exlib::AsyncEvent* ac)
{
	class asyncInvoke: public asyncState
	{
	public:
		asyncInvoke(HttpFileHandler* pThis, HttpRequest_base* req,
				exlib::AsyncEvent* ac) :
				asyncState(ac), m_pThis(pThis), m_req(req)
		{
			req->get_response(m_rep);
			set(start);
		}

		static int start(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;
			std::string value;

			pThis->m_req->get_value(value);
			if (value.length() > 0 && isPathSlash(value[value.length() - 1]))
				value.append("index.html", 10);
			path_base::normalize((pThis->m_pThis->m_root + value).c_str(),
					pThis->m_path);

			pThis->set(open);
			return io_base::open(pThis->m_path.c_str(), "r", pThis->m_file,
					pThis);
		}

		static int open(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;
			std::string ext;

			path_base::extname(pThis->m_path.c_str(), ext);

			if (ext.length() > 0)
			{
				Variant v;
				const char* pKey = ext.c_str() + 1;
				const char** pMimeType = (const char**) bsearch(&pKey,
						&s_mimeTypes, sizeof(s_mimeTypes) / sizeof(s_defType),
						sizeof(s_defType), mt_cmp);

				if (!pMimeType)
					pMimeType = s_defType;

				v = pMimeType[1];
				pThis->m_rep->addHeader("Content-Type", v);
			}

			pThis->set(stat);
			return pThis->m_file->stat(pThis->m_stat, pThis);
		}

		static int stat(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;
			date_t d, d1;
			std::string str;
			Variant v;

			pThis->m_stat->get_mtime(d);

			if (pThis->m_req->firstHeader("If-Modified-Since",
					v) != CALL_RETURN_NULL)
			{
				str = v.string();
				d1.parse(str.c_str(), str.length());

				if (abs(d.diff(d1)) < 1000)
				{
					pThis->m_rep->set_status(304);
					return pThis->done(CALL_RETURN_NULL);
				}
			}

			d.toString(str);
			v = str;

			pThis->m_rep->addHeader("Last-Modified", v);
			pThis->m_rep->set_body(pThis->m_file);

			return pThis->done(CALL_RETURN_NULL);
		}

		virtual int error(int v)
		{
			m_rep->set_status(404);
			return done(CALL_RETURN_NULL);
		}

	private:
		obj_ptr<HttpFileHandler> m_pThis;
		obj_ptr<HttpRequest_base> m_req;
		obj_ptr<HttpResponse_base> m_rep;
		obj_ptr<File_base> m_file;
		obj_ptr<Stat_base> m_stat;
		std::string m_path;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	obj_ptr<HttpRequest_base> req = HttpRequest_base::getInstance(v);

	if (req == NULL)
		return CALL_E_BADVARTYPE;

	return (new asyncInvoke(this, req, ac))->post(0);
}

} /* namespace fibjs */
