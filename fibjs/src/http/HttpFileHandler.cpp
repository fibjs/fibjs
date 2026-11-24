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
#include "ifs/mime.h"
#include "path.h"
#include "HttpFileHandler.h"
#include "HttpHeaders.h"
#include "RangeStream.h"
#include "HttpRequest.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include <inttypes.h>

namespace fibjs {

static bool is_url_path_char(unsigned char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
        || (c >= '0' && c <= '9')
        || strchr("-._~/", c) != NULL;
}

static exlib::string url_encode_path(const exlib::string& s)
{
    static const char hex[] = "0123456789ABCDEF";
    exlib::string out;

    out.reserve(s.length());

    for (size_t i = 0; i < s.length(); i++) {
        unsigned char c = (unsigned char)s[i];
        if (is_url_path_char(c))
            out.append(1, (char)c);
        else {
            out.append(1, '%');
            out.append(1, hex[c >> 4]);
            out.append(1, hex[c & 0x0f]);
        }
    }

    return out;
}

static exlib::string html_escape(const exlib::string& s)
{
    exlib::string out;

    out.reserve(s.length());

    for (size_t i = 0; i < s.length(); i++) {
        switch (s[i]) {
        case '&':
            out += "&amp;";
            break;
        case '<':
            out += "&lt;";
            break;
        case '>':
            out += "&gt;";
            break;
        case '"':
            out += "&quot;";
            break;
        case '\'':
            out += "&#39;";
            break;
        default:
            out.append(1, s[i]);
            break;
        }
    }

    return out;
}

// Last-Modified is emitted at second resolution while the on-disk mtime keeps
// milliseconds, so both sides are truncated to whole seconds before comparing.
// This is exact (rather than a ±1s tolerance) and also accepts dates in the
// future, which RFC 9110 13.1.3 requires to be treated as "not modified".
static bool since_date_matches(date_t now, const exlib::string& header)
{
    date_t given;
    given.parse(header);

    if (given.empty())
        return false;

    now.fix();
    given.fix();

    return now.date() <= given.date();
}

static bool etag_matches(const exlib::string& header, const exlib::string& etag)
{
    exlib::string expected = etag;
    size_t pos = 0;

    if (!qstricmp(expected.c_str(), "W/", 2))
        expected = expected.substr(2);

    while (pos <= header.length()) {
        size_t comma = header.find(',', pos);
        exlib::string tag = header.substr(pos,
            comma == exlib::string::npos ? exlib::string::npos : comma - pos);

        trim_ows(tag);

        if (tag == "*")
            return true;

        if (!qstricmp(tag.c_str(), "W/", 2))
            tag = tag.substr(2);

        if (tag == expected)
            return true;

        if (comma == exlib::string::npos)
            break;
        pos = comma + 1;
    }

    return false;
}

result_t http_base::fileHandler(exlib::string root, bool autoIndex, obj_ptr<Handler_base>& retVal)
{
    return HttpFileHandler::create(root, autoIndex, retVal);
}

result_t http_base::fileHandler(exlib::string root, v8::Local<v8::Object> options, obj_ptr<Handler_base>& retVal)
{
    return HttpFileHandler::create(root, options, retVal);
}

result_t HttpFileHandler::create(exlib::string root, bool autoIndex, obj_ptr<Handler_base>& retVal)
{
    exlib::string root_;
    obj_ptr<Stat_base> stat;

    path_base::normalize(root, root_);
    result_t hr = fs_base::ac_stat(root_, stat);
    if (hr.failed())
        return hr;

    bool isDir;
    stat->isDirectory(isDir);

    retVal = new HttpFileHandler(root_, isDir, autoIndex);
    return result_t();
}

result_t HttpFileHandler::create(exlib::string root, v8::Local<v8::Object> options, obj_ptr<Handler_base>& retVal)
{
    exlib::string root_;
    obj_ptr<Stat_base> stat;

    path_base::normalize(root, root_);
    result_t hr = fs_base::ac_stat(root_, stat);
    if (hr.failed())
        return hr;

    bool isDir;
    stat->isDirectory(isDir);

    std::optional<bool> autoIndex;
    std::optional<int64_t> maxAge;
    std::optional<bool> immutable;
    std::optional<bool> cacheControl;

    // maxAge is a cache lifetime: silently falling back to 0 would serve
    // no-cache for a handler the caller configured to cache.
    result_t ohr = GetConfigValue(options, "autoIndex", autoIndex);
    if (ohr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return ohr;
    ohr = GetConfigValue(options, "maxAge", maxAge);
    if (ohr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return ohr;
    ohr = GetConfigValue(options, "immutable", immutable);
    if (ohr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return ohr;
    ohr = GetConfigValue(options, "cacheControl", cacheControl);
    if (ohr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return ohr;

    obj_ptr<HttpFileHandler> hdlr = new HttpFileHandler(root_, isDir, autoIndex.value_or(false));
    hdlr->m_cacheControl = cacheControl.value_or(true);
    hdlr->m_maxAge = maxAge.value_or(0);
    hdlr->m_immutable = immutable.value_or(false);

    v8::Local<v8::Value> headersVal;
    if (GetConfigValue(options, "headers", headersVal).ok() && IsJSObject(headersVal)) {
        v8::Local<v8::Object> headersObj = headersVal.As<v8::Object>();
        Isolate* isolate = Isolate::current(headersObj);
        v8::Local<v8::Context> context = isolate->context();

        v8::Local<v8::Array> keys = headersObj->GetOwnPropertyNames(context).FromMaybe(v8::Local<v8::Array>());
        uint32_t len = keys.IsEmpty() ? 0 : keys->Length();

        for (uint32_t i = 0; i < len; i++) {
            v8::Local<v8::Value> key = keys->Get(context, i).FromMaybe(v8::Local<v8::Value>());
            v8::Local<v8::Value> val = headersObj->Get(context, key).FromMaybe(v8::Local<v8::Value>());
            if (!key->IsString() || !IsJSObject(val))
                continue;

            exlib::string pattern;
            GetArgumentValue(isolate, key, pattern);

            HeaderRule rule(MinimatchPattern(pattern, false));

            v8::Local<v8::Object> headerMap = val.As<v8::Object>();
            v8::Local<v8::Array> hkeys = headerMap->GetOwnPropertyNames(context).FromMaybe(v8::Local<v8::Array>());
            uint32_t hlen = hkeys.IsEmpty() ? 0 : hkeys->Length();

            for (uint32_t j = 0; j < hlen; j++) {
                v8::Local<v8::Value> hkey = hkeys->Get(context, j).FromMaybe(v8::Local<v8::Value>());
                v8::Local<v8::Value> hval = headerMap->Get(context, hkey).FromMaybe(v8::Local<v8::Value>());
                if (!hkey->IsString())
                    continue;

                exlib::string name, value;
                GetArgumentValue(isolate, hkey, name);
                GetArgumentValue(isolate, hval, value);
                if (name.empty())
                    continue;

                rule.headers.emplace_back(std::move(name), std::move(value));
            }

            if (!rule.headers.empty())
                hdlr->m_headers.push_back(std::move(rule));
        }
    }

    retVal = hdlr;
    return result_t();
}

void HttpFileHandler::applyHeaders(const exlib::string& target, HttpResponse_base* rep)
{
    exlib::string str;
    // the IDL header setters return the message for chaining; internal callers
    // that ignore it pass a throwaway slot
    obj_ptr<HttpMessage_base> hdrRet;

    for (auto& rule : m_headers) {
        if (rule.pattern.match(target)) {
            for (auto& h : rule.headers)
                rep->setHeader(h.first, h.second, hdrRet);
            break;
        }
    }

    if (m_cacheControl && m_maxAge > 0 && rep->firstHeader("Cache-Control", str).is_return_null()) {
        char s[64];
        if (m_immutable)
            snprintf(s, sizeof(s), "public, max-age=%lld, immutable", (long long)m_maxAge);
        else
            snprintf(s, sizeof(s), "public, max-age=%lld", (long long)m_maxAge);
        rep->appendHeader("Cache-Control", s, hdrRet);
    }
}

result_t HttpFileHandler::isRouting(bool& retVal)
{
    retVal = true;
    return result_t();
}

result_t HttpFileHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncHandle ac)
{
    class asyncInvoke : public AsyncState {
    public:
        asyncInvoke(HttpFileHandler* pThis, HttpRequest_base* req, bool autoIndex,
            AsyncHandle ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_req(req)
            , m_autoIndex(autoIndex)
            , m_index(false)
            , m_dirPos(0)
        {
            req->get_response(m_rep);
            m_req->get_value(m_value);

            if (m_value.empty() || !m_pThis->m_isDir) {
                m_url = m_pThis->m_root;
                init(start);
                return;
            }

            HttpRequest* httpReq = dynamic_cast<HttpRequest*>(m_req.get());
            bool canonical = httpReq && httpReq->value_is_canonical();

            if (!canonical) {
                result_t hr = canonicalize_request_path(m_value, m_value);
                if (hr.failed() || qstrchr(m_value.c_str(), '%')) {
                    init(stop);
                    return;
                }
            } else if (m_value.find('\0') != exlib::string::npos
                || qstrchr(m_value.c_str(), '\\')) {
                init(stop);
                return;
            }

            if (!qstrcmp(m_value.c_str(), "../", 3)
                || !qstrcmp(m_value.c_str(), "..")) {
                init(stop);
                return;
            }

            m_url = m_pThis->m_root + m_value;
            init(start);
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
            return next(result_t::return_null());
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
            exlib::string s;
            obj_ptr<Buffer_base> buf;
            // the IDL header setters return the message for chaining; internal
            // callers that ignore it pass a throwaway slot
            obj_ptr<HttpMessage_base> hdrRet;

            length = m_dir->length();

            if (m_dirPos == 0) {
                m_dir->sort();

                m_file = new MemoryStream();
                s = "<html>\n<head><title>Index of "
                    + html_escape(m_value) + "</title></head>\n<body bgcolor=white>\n<h1>Index of "
                    + html_escape(m_value) + "</h1><hr><pre>";
                buf = new Buffer(s.c_str(), s.length());
                m_file->writeBuffer(buf, nullptr);

                if (m_value.length() > 1) {
                    s = "<a href=\"../\">../</a>\n";
                    buf = new Buffer(s.c_str(), s.length());
                    m_file->writeBuffer(buf, nullptr);
                }
            } else {
                exlib::string name, ds, ss;
                date_t d;
                double sz;
                bool is_dir = false;
                int32_t padding_len;

                m_stat->get_name(name);
                m_stat->isDirectory(is_dir);
                if (is_dir)
                    name += '/';
                s = "<a href=\"" + url_encode_path(name) + "\">"
                    + html_escape(name) + "</a>";
                buf = new Buffer(s.c_str(), s.length());
                m_file->writeBuffer(buf, nullptr);
                padding_len = 40 - (int32_t)name.length();
                if (padding_len < 1)
                    padding_len = 1;
                buf = new Buffer(padding, padding_len);
                m_file->writeBuffer(buf, nullptr);

                m_stat->get_mtime(d);
                d.sqlString(ds);
                buf = new Buffer(ds.c_str(), ds.length());
                m_file->writeBuffer(buf, nullptr);

                m_stat->get_size(sz);
                ss = niceSize((int64_t)sz);
                padding_len = 12 - (int32_t)ss.length();
                if (padding_len < 1)
                    padding_len = 1;
                buf = new Buffer(padding, padding_len);
                m_file->writeBuffer(buf, nullptr);
                ss.append(1, '\n');
                buf = new Buffer(ss.c_str(), ss.length());
                m_file->writeBuffer(buf, nullptr);
            }

            if (m_dirPos >= length) {
                buf = new Buffer("</pre><hr></body>\n</html>");
                m_file->writeBuffer(buf, nullptr);
                m_file->rewind();

                m_rep->set_body(m_file);
                m_rep->appendHeader("Content-Type", "text/html", hdrRet);

                return next(result_t::return_null());
            }

            Variant v;

            m_dir->_indexed_getter(m_dirPos, v);
            m_dirPos++;

            m_path = m_url;
            m_path += v.string();

            // Next directory entry: re-enter this state through a ticket.
            // Passing `this` would dispatch straight into a machine that is
            // already running (the phase lock rejects a non-initial post()).
            return fs_base::stat(m_path, m_stat, next(list_stat));
        }

        ON_STATE(asyncInvoke, open)
        {
            obj_ptr<HttpMessage_base> hdrRet;

            if (m_index)
                m_rep->appendHeader("Content-Type", "text/html", hdrRet);
            else {
                exlib::string type;
                mime_base::getType(m_url, type);
                m_rep->appendHeader("Content-Type", type, hdrRet);
            }

            return m_file->stat(m_stat, next(stat));
        }

        ON_STATE(asyncInvoke, stat)
        {
            date_t d;
            exlib::string str;
            obj_ptr<HttpMessage_base> hdrRet;

            m_stat->get_mtime(d);

            {
                exlib::string target = m_index ? "index.html" : m_value;
                while (!target.empty() && target[0] == '/')
                    target = target.substr(1);

                m_pThis->applyHeaders(target, m_rep);
            }

            bool isDir = false;
            m_stat->isDirectory(isDir);

            if (isDir && !m_index) {
                exlib::string location = m_value;
                if (location.empty())
                    location = "/";
                else if (location[0] != '/')
                    location = "/" + location;
                if (location[location.length() - 1] != '/')
                    location += "/";

                exlib::string qs;
                if (m_req->get_queryString(qs).ok() && !qs.empty())
                    location += "?" + qs;

                m_rep->set_statusCode(301);
                m_rep->appendHeader("Location", location, hdrRet);
                return next(result_t::return_null());
            }

            double sz;
            m_stat->get_size(sz);
            int64_t fsz = (int64_t)sz;

            char etag[128];
            snprintf(etag, sizeof(etag), "W/\"%llx-%llx\"",
                (unsigned long long)(int64_t)d.date(),
                (unsigned long long)fsz);
            exlib::string etagValue = etag;

            exlib::string lastModified;
            d.toGMTString(lastModified);

            m_rep->appendHeader("Last-Modified", lastModified, hdrRet);
            m_rep->appendHeader("ETag", etagValue, hdrRet);
            m_rep->appendHeader("Accept-Ranges", "bytes", hdrRet);

            exlib::string ifNoneMatch;
            if (!m_req->firstHeader("If-None-Match", ifNoneMatch).is_return_null()) {
                if (etag_matches(ifNoneMatch, etagValue)) {
                    m_rep->set_statusCode(304);
                    return next(result_t::return_null());
                }
            } else {
                exlib::string ifModifiedSince;
                if (!m_req->firstHeader("If-Modified-Since", ifModifiedSince).is_return_null()) {
                    if (since_date_matches(d, ifModifiedSince)) {
                        m_rep->set_statusCode(304);
                        return next(result_t::return_null());
                    }
                }
            }

            exlib::string range;
            if (!m_req->firstHeader("Range", range).is_return_null()) {
                bool applyRange = true;
                exlib::string ifRange;
                if (!m_req->firstHeader("If-Range", ifRange).is_return_null()) {
                    if (ifRange == etagValue || ifRange == lastModified)
                        applyRange = true;
                    else
                        applyRange = since_date_matches(d, ifRange);
                }

                size_t p = 0;
                while (p < range.length() && (range[p] == ' ' || range[p] == '\t'))
                    p++;

                bool bytesUnit = range.length() - p >= 6
                    && !qstricmp(range.c_str() + p, "bytes=", 6);

                if (applyRange) {
                    if (!bytesUnit) {
                        // An unknown range unit must be ignored; a header
                        // without a unit is malformed.
                        if (range.find('=') == exlib::string::npos) {
                            m_rep->set_statusCode(416);
                            char s[64];
                            snprintf(s, sizeof(s), "bytes */%" PRId64, fsz);
                            m_rep->appendHeader("Content-Range", s, hdrRet);
                            return next(result_t::return_null());
                        }
                    } else {
                        int64_t bpos, epos;
                        exlib::string spec = range.substr(p + 6);
                        range_parse_result r = parse_range(spec, fsz, true, bpos, epos);

                        if (r == range_parse_result::ok) {
                            m_rep->set_statusCode(206);

                            char s[256];
                            snprintf(s, sizeof(s), "bytes %" PRId64 "-%" PRId64 "/%" PRId64 "",
                                bpos, epos - 1, fsz);
                            m_rep->appendHeader("Content-Range", s, hdrRet);

                            obj_ptr<RangeStream_base> stm = new RangeStream(m_file, bpos, epos);
                            m_rep->set_body(stm);

                            return next(result_t::return_null());
                        }

                        if (r == range_parse_result::unsupported) {
                            // Multiple ranges: ignore Range and send the whole
                            // representation.
                        } else {
                            m_rep->set_statusCode(416);
                            char s[64];
                            snprintf(s, sizeof(s), "bytes */%" PRId64, fsz);
                            m_rep->appendHeader("Content-Range", s, hdrRet);
                            return next(result_t::return_null());
                        }
                    }
                }
            }

            m_rep->set_body(m_file);

            return next(result_t::return_null());
        }

        virtual result_t error(result_t v)
        {
            if (at(start)) {
                if (m_index) {
                    m_index = false;

                    if (m_autoIndex)
                        return next(autoindex);
                }
            }

            m_rep->set_statusCode(404);
            return next(result_t::return_null());
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

    if (ac.isSync())
        return result_t::nosync();

    obj_ptr<HttpRequest_base> req = HttpRequest_base::getInstance(v);

    if (req == NULL)
        return result_t::bad_vartype();

    return (new asyncInvoke(this, req, m_autoIndex, std::move(ac)))->post(result_t());
}

} /* namespace fibjs */
