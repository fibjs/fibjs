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
#include "RangeStream.h"
#include "HttpRequest.h"
#include "Url.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include <inttypes.h>

namespace fibjs {

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
    if (hr != 0)
        return hr;

    bool isDir;
    stat->isDirectory(isDir);

    retVal = new HttpFileHandler(root_, isDir, autoIndex);
    return 0;
}

result_t HttpFileHandler::create(exlib::string root, v8::Local<v8::Object> options, obj_ptr<Handler_base>& retVal)
{
    exlib::string root_;
    obj_ptr<Stat_base> stat;

    path_base::normalize(root, root_);
    result_t hr = fs_base::ac_stat(root_, stat);
    if (hr != 0)
        return hr;

    bool isDir;
    stat->isDirectory(isDir);

    std::optional<bool> autoIndex;
    std::optional<int64_t> maxAge;
    std::optional<bool> immutable;
    std::optional<bool> cacheControl;

    GetConfigValue(options, "autoIndex", autoIndex);
    GetConfigValue(options, "maxAge", maxAge);
    GetConfigValue(options, "immutable", immutable);
    GetConfigValue(options, "cacheControl", cacheControl);

    obj_ptr<HttpFileHandler> hdlr = new HttpFileHandler(root_, isDir, autoIndex.value_or(false));
    hdlr->m_cacheControl = cacheControl.value_or(true);
    hdlr->m_maxAge = maxAge.value_or(0);
    hdlr->m_immutable = immutable.value_or(false);

    v8::Local<v8::Value> headersVal;
    if (GetConfigValue(options, "headers", headersVal) == 0 && IsJSObject(headersVal)) {
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
    return 0;
}

void HttpFileHandler::applyHeaders(const exlib::string& target, HttpResponse_base* rep)
{
    exlib::string str;

    for (auto& rule : m_headers) {
        if (rule.pattern.match(target)) {
            for (auto& h : rule.headers)
                rep->setHeader(h.first, h.second);
            break;
        }
    }

    if (m_cacheControl && m_maxAge > 0 && rep->firstHeader("Cache-Control", str) == CALL_RETURN_NULL) {
        char s[64];
        if (m_immutable)
            snprintf(s, sizeof(s), "public, max-age=%lld, immutable", (long long)m_maxAge);
        else
            snprintf(s, sizeof(s), "public, max-age=%lld", (long long)m_maxAge);
        rep->appendHeader("Cache-Control", s);
    }
}

result_t HttpFileHandler::isRouting(bool& retVal)
{
    retVal = true;
    return 0;
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

            if (m_value.empty() || !m_pThis->m_isDir) {
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
            exlib::string s;
            obj_ptr<Buffer_base> buf;

            length = m_dir->length();

            if (m_dirPos == 0) {
                m_dir->sort();

                m_file = new MemoryStream();
                s = "<html>\n<head><title>Index of "
                    + m_value + "</title></head>\n<body bgcolor=white>\n<h1>Index of "
                    + m_value + "</h1><hr><pre>";
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
                s = "<a href=\"" + name + "\">" + name + "</a>";
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
                m_rep->appendHeader("Content-Type", "text/html");

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

            if (m_index)
                m_rep->appendHeader("Content-Type", "text/html");
            else {
                exlib::string type;
                mime_base::getType(m_url, type);
                m_rep->appendHeader("Content-Type", type);
            }

            return m_file->stat(m_stat, next(stat));
        }

        ON_STATE(asyncInvoke, stat)
        {
            date_t d;
            exlib::string str;

            m_stat->get_mtime(d);

            {
                exlib::string target = m_index ? "index.html" : m_value;
                while (!target.empty() && target[0] == '/')
                    target = target.substr(1);

                m_pThis->applyHeaders(target, m_rep);
            }

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

            m_rep->appendHeader("Last-Modified", lastModified);

            exlib::string range;
            if (m_req->firstHeader("Range", range) != CALL_RETURN_NULL) {
                if (qstricmp(range.c_str(), "bytes=", 6)) {
                    m_rep->set_statusCode(416);

                    return next(CALL_RETURN_NULL);
                }

                range = range.substr(6);

                obj_ptr<RangeStream_base> stm;
                if (RangeStream_base::_new(m_file, range, stm) != 0) {
                    m_rep->set_statusCode(416);

                    return next(CALL_RETURN_NULL);
                }

                int64_t fsz, bpos, epos;

                m_file->size(fsz);
                stm->get_begin(bpos);
                stm->get_end(epos);

                m_rep->set_statusCode(206);

                char s[256];
                snprintf(s, sizeof(s), "bytes %" PRId64 "-%" PRId64 "/%" PRId64 "", bpos, epos - 1, fsz);
                m_rep->appendHeader("Content-Range", s);

                m_rep->set_body(stm);

                return next(CALL_RETURN_NULL);
            }

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
