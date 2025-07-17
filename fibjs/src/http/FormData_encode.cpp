/*
 * FormData_encode.cpp
 *
 *  Created on: Jul 16, 2025
 *      Author: lion
 */

#include "object.h"
#include "FormData.h"
#include "Blob.h"
#include "Buffer.h"
#include "ifs/encoding.h"
#include "StringBuffer.h"
#include <openssl/rand.h>
#include <chrono>
#include <algorithm>

namespace fibjs {

static bool parseBoundary(const char* contentType, exlib::string& boundary)
{
    if (!contentType) {
        return false;
    }

    // Find "boundary" parameter (case-insensitive using qstristr)
    const char* boundaryPos = qstristr(contentType, "boundary");
    if (!boundaryPos) {
        return false;
    }

    // Move past "boundary"
    const char* originalPos = boundaryPos + 8; // Skip "boundary"

    // Skip whitespace after "boundary"
    while (*originalPos && (*originalPos == ' ' || *originalPos == '\t')) {
        originalPos++;
    }

    // Expect '=' character
    if (*originalPos != '=') {
        return false;
    }
    originalPos++; // Skip '='

    // Skip whitespace after '='
    while (*originalPos && (*originalPos == ' ' || *originalPos == '\t')) {
        originalPos++;
    }

    // Check if value is quoted
    bool isQuoted = (*originalPos == '"');
    if (isQuoted) {
        originalPos++; // Skip opening quote
    }

    // Extract boundary value
    const char* valueStart = originalPos;
    const char* valueEnd = valueStart;

    if (isQuoted) {
        // Find closing quote, handle escaped quotes
        while (*valueEnd && *valueEnd != '"') {
            if (*valueEnd == '\\' && *(valueEnd + 1)) {
                valueEnd += 2; // Skip escaped character
            } else {
                valueEnd++;
            }
        }
        if (*valueEnd != '"') {
            return false; // Unclosed quote
        }
    } else {
        // Unquoted value - stop at whitespace, semicolon, or end
        while (*valueEnd && *valueEnd != ';' && *valueEnd != ' ' && *valueEnd != '\t' && *valueEnd != '\r' && *valueEnd != '\n') {
            valueEnd++;
        }
    }

    // Extract the boundary value
    size_t boundaryLength = valueEnd - valueStart;
    if (boundaryLength == 0 || boundaryLength > 70) {
        // RFC 2046: boundary must be 1-70 characters
        return false;
    }

    boundary.assign(valueStart, boundaryLength);

    // RFC 2046: Validate boundary characters
    // Allowed: DIGIT / ALPHA / "'" / "(" / ")" / "+" / "_" / "," / "-" / "." / "/" / ":" / "=" / "?"
    for (size_t i = 0; i < boundary.length(); i++) {
        char c = boundary[i];
        if (!((c >= '0' && c <= '9') || // DIGIT
                (c >= 'A' && c <= 'Z') || // ALPHA (uppercase)
                (c >= 'a' && c <= 'z') || // ALPHA (lowercase)
                c == '\'' || c == '(' || c == ')' || // Special chars part 1
                c == '+' || c == '_' || c == ',' || // Special chars part 2
                c == '-' || c == '.' || c == '/' || // Special chars part 3
                c == ':' || c == '=' || c == '?')) { // Special chars part 4
            // Invalid character found
            return false;
        }
    }

    return true;
}

result_t FormData::parseMultipart(Buffer_base* init, const char* boundary)
{
    Buffer* buffer = Buffer::Cast(init);
    const char* pstr = (const char*)buffer->data();
    size_t nSize = buffer->length();
    exlib::string strName;
    exlib::string strFileName;
    exlib::string strContentType;
    exlib::string strContentTransferEncoding;
    const char *p, *p1, *p2, *szQueryString;
    const char* pstrSplit;
    size_t uiSplitSize;
    char ch;

    // Parse boundary using RFC 2046 compliant parser
    if (!parseBoundary(boundary, m_boundary)) {
        return 0;
    }
    uiSplitSize = m_boundary.length();

    pstrSplit = szQueryString = pstr;

    if (nSize < uiSplitSize + 2 || szQueryString[0] != '-'
        || szQueryString[1] != '-'
        || qstrcmp(szQueryString + 2, m_boundary.c_str(), uiSplitSize))
        return 0;

    uiSplitSize += 2;
    szQueryString += uiSplitSize;
    nSize -= uiSplitSize;

    while (nSize) {
        strFileName.clear();
        strContentType.clear();
        strContentTransferEncoding.clear();

        while (nSize > 0) {
            ch = *szQueryString++;
            nSize--;
            if (ch != '\r')
                return 0;
            if (nSize > 0 && *szQueryString == '\n') {
                nSize--;
                szQueryString++;
            }

            p = szQueryString;
            while (nSize > 0 && *p != '\r') {
                nSize--;
                p++;
            }

            if (nSize == 0)
                break;

            p1 = szQueryString;
            szQueryString = p;

            if (p != p1) {
                if (p1 + 20 < p && !qstricmp(p1, "Content-Disposition:", 20)) {
                    p1 += 20;
                    while (p1 < p && *p1 == ' ')
                        p1++;
                    if (p1 + 10 >= p || qstricmp(p1, "form-data;", 10))
                        return 0;

                    p1 += 10;
                    while (p1 < p && *p1 == ' ')
                        p1++;
                    if (p1 + 5 >= p || qstricmp(p1, "name=", 5))
                        return 0;

                    p1 += 5;

                    while (p1 < p && *p1 == ' ')
                        p1++;

                    ch = ';';
                    if (*p1 == '\"') {
                        p1++;
                        ch = '\"';
                    }

                    p2 = p1;
                    while (p1 < p && *p1 != ch)
                        p1++;

                    strName.assign(p2, (size_t)(p1 - p2));

                    if (p1 < p && *p1 == '\"')
                        p1++;

                    if (p1 < p && *p1 == ';')
                        p1++;

                    while (p1 < p && *p1 == ' ')
                        p1++;

                    if (p1 + 9 < p && !qstricmp(p1, "filename=", 9)) {
                        p1 += 9;

                        while (p1 < p && *p1 == ' ')
                            p1++;

                        ch = ';';
                        if (*p1 == '\"') {
                            p1++;
                            ch = '\"';
                        }

                        p2 = p1;
                        while (p1 < p && *p1 != ch) {
                            if (*p1 == '/' || *p1 == '\\')
                                p2 = p1 + 1;
                            p1++;
                        }

                        strFileName.assign(p2, (size_t)(p1 - p2));
                    }
                } else if (p1 + 13 < p && !qstricmp(p1, "Content-Type:", 13)) {
                    p1 += 13;
                    while (p1 < p && *p1 == ' ')
                        p1++;
                    strContentType.assign(p1, (size_t)(p - p1));
                } else if (p1 + 26 < p && !qstricmp(p1, "Content-Transfer-Encoding:", 26)) {
                    p1 += 26;
                    while (p1 < p && *p1 == ' ')
                        p1++;
                    strContentTransferEncoding.assign(p1, (size_t)(p - p1));
                }
            } else {
                ch = *szQueryString++;
                nSize--;
                if (ch != '\r')
                    return 0;
                if (nSize > 0 && *szQueryString == '\n') {
                    nSize--;
                    szQueryString++;
                }
                break;
            }
        }

        p = szQueryString;
        p1 = p + nSize;
        while (p1 > p && (p = (char*)memchr(p, '-', p1 - p))
            && p1 > p + uiSplitSize && memcmp(p, pstrSplit, uiSplitSize))
            p++;

        if (!p || p1 <= p + uiSplitSize)
            break;

        nSize = (size_t)(p1 - p - uiSplitSize);
        p1 = szQueryString;
        szQueryString = p + uiSplitSize;

        // Fix content boundary calculation to match HttpCollection.cpp exactly
        if (p > p1) {
            p--;
            ch = *p;
            if (ch != '\n')
                return 0;

            if (p > p1 && *(p - 1) == '\r')
                p--;
        }

        if (!strName.empty()) {
            size_t uiSize = (size_t)(p - p1);

            if (strFileName.empty()) {
                // Store text field content, not empty string
                m_map.emplace_back(strName, exlib::string(p1, uiSize));
            } else {
                obj_ptr<Buffer> bufBody = new Buffer(p1, uiSize);
                obj_ptr<File> objFile = new File();

                objFile->m_impl.init(bufBody, strContentType);
                objFile->m_name = strFileName;

                date_t tm;
                tm.now();
                objFile->m_lastModified = tm.date();

                m_map.emplace_back(strName, objFile);
            }
        }
    }

    return 0;
}

result_t FormData::encode(exlib::string type, obj_ptr<Buffer_base>& retVal)
{
    if (qstristr(type.c_str(), "urlencoded") != NULL) {
        StringBuffer bufs;

        for (size_t i = 0; i < m_map.size(); i++) {
            if (i > 0)
                bufs.append("&");

            pair& _pair = m_map[i];
            if (_pair.second.type() != Variant::VT_String) {
                return Runtime::setError("FormData encode: value must be a string");
            }

            exlib::string encodedKey, encodedValue;
            encoding_base::encodeURIComponent(_pair.first, false, encodedKey);
            encoding_base::encodeURIComponent(_pair.second.string(), false, encodedValue);

            bufs.append(encodedKey);
            bufs.append("=");
            bufs.append(encodedValue);
        }

        retVal = bufs.buffer();
        return 0;
    } else if (qstristr(type.c_str(), "multipart") != NULL) {
        // Determine boundary to use - parse with RFC 2046 compliance
        exlib::string extractedBoundary;
        if (parseBoundary(type.c_str(), extractedBoundary)) {
            m_boundary = extractedBoundary;
        } else if (m_boundary.empty()) {
            // Generate random boundary using timestamp and random hex
            char boundaryBuf[64];
            uint8_t randomBytes[8];

            // Use OpenSSL random function like in uuid.cpp
            if (RAND_bytes(randomBytes, 8) == 1) {
                sprintf(boundaryBuf, "----formdata----%08x%08x",
                    *(uint32_t*)randomBytes, *(uint32_t*)(randomBytes + 4));
            } else {
                // Fallback to timestamp-based boundary
                auto now = std::chrono::system_clock::now();
                auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
                sprintf(boundaryBuf, "----formdata----%llx", (long long)timestamp);
            }
            m_boundary = boundaryBuf;
        }
        // If m_boundary is not empty, use the existing boundary

        // First pass: calculate total size needed
        size_t totalSize = 0;

        for (size_t i = 0; i < m_map.size(); i++) {
            pair& _pair = m_map[i];

            // Boundary delimiter: "--" + boundary + "\r\n"
            totalSize += 2 + m_boundary.length() + 2;

            if (_pair.second.type() == Variant::VT_String) {
                // Text field
                // Content-Disposition header
                totalSize += 38; // "Content-Disposition: form-data; name=\""
                totalSize += _pair.first.length();
                totalSize += 5; // "\"\r\n\r\n"
                // Content
                totalSize += _pair.second.string().length();
                totalSize += 2; // "\r\n"
            } else {
                // File field - must be a File object
                obj_ptr<File_base> file = File_base::getInstance(_pair.second.object());
                if (!file) {
                    return Runtime::setError("FormData encode: invalid file object");
                }

                exlib::string filename;
                file->get_name(filename);

                exlib::string contentType;
                file->get_type(contentType);
                if (contentType.empty()) {
                    contentType = "application/octet-stream";
                }

                // Content-Disposition header
                totalSize += 38; // "Content-Disposition: form-data; name=\""
                totalSize += _pair.first.length();
                totalSize += 13; // "\"; filename=\""
                totalSize += filename.length();
                totalSize += 3; // "\"\r\n"

                // Content-Type header
                totalSize += 14; // "Content-Type: "
                totalSize += contentType.length();
                totalSize += 4; // "\r\n\r\n"

                // Get file size for total calculation (getBuffer is zero-copy)
                File* filePtr = file.As<File>();
                obj_ptr<Buffer_base> fileBuffer = filePtr->m_impl.getBuffer();
                int32_t fileSize;
                fileBuffer->get_length(fileSize);
                totalSize += fileSize;
                totalSize += 2; // "\r\n"
            }
        }

        // Final boundary: "--" + boundary + "--\r\n"
        totalSize += 2 + m_boundary.length() + 4;

        // Second pass: write to buffer
        obj_ptr<Buffer> resultBuffer = new Buffer(NULL, totalSize);
        uint8_t* bufPtr = resultBuffer->data();
        size_t offset = 0;

        for (size_t i = 0; i < m_map.size(); i++) {
            pair& _pair = m_map[i];

            // Write boundary delimiter
            memcpy(bufPtr + offset, "--", 2);
            offset += 2;
            memcpy(bufPtr + offset, m_boundary.c_str(), m_boundary.length());
            offset += m_boundary.length();
            memcpy(bufPtr + offset, "\r\n", 2);
            offset += 2;

            if (_pair.second.type() == Variant::VT_String) {
                // Text field
                const char* header = "Content-Disposition: form-data; name=\"";
                memcpy(bufPtr + offset, header, 38);
                offset += 38;
                memcpy(bufPtr + offset, _pair.first.c_str(), _pair.first.length());
                offset += _pair.first.length();
                memcpy(bufPtr + offset, "\"\r\n\r\n", 5);
                offset += 5;

                exlib::string content = _pair.second.string();
                memcpy(bufPtr + offset, content.c_str(), content.length());
                offset += content.length();
                memcpy(bufPtr + offset, "\r\n", 2);
                offset += 2;
            } else {
                // File field
                obj_ptr<File_base> file = File_base::getInstance(_pair.second.object());

                exlib::string filename;
                file->get_name(filename);

                exlib::string contentType;
                file->get_type(contentType);
                if (contentType.empty()) {
                    contentType = "application/octet-stream";
                }

                // Write Content-Disposition header
                const char* header1 = "Content-Disposition: form-data; name=\"";
                memcpy(bufPtr + offset, header1, 38);
                offset += 38;
                memcpy(bufPtr + offset, _pair.first.c_str(), _pair.first.length());
                offset += _pair.first.length();
                memcpy(bufPtr + offset, "\"; filename=\"", 13);
                offset += 13;
                memcpy(bufPtr + offset, filename.c_str(), filename.length());
                offset += filename.length();
                memcpy(bufPtr + offset, "\"\r\n", 3);
                offset += 3;

                // Write Content-Type header
                const char* header2 = "Content-Type: ";
                memcpy(bufPtr + offset, header2, 14);
                offset += 14;
                memcpy(bufPtr + offset, contentType.c_str(), contentType.length());
                offset += contentType.length();
                memcpy(bufPtr + offset, "\r\n\r\n", 4);
                offset += 4;

                // Write file content (getBuffer is zero-copy, called when needed)
                File* filePtr = file.As<File>();
                obj_ptr<Buffer_base> fileBuffer = filePtr->m_impl.getBuffer();
                int32_t fileSize;
                fileBuffer->get_length(fileSize);
                if (fileSize > 0) {
                    // Copy binary data directly from buffer
                    Buffer* fileBuf = fileBuffer.As<Buffer>();
                    memcpy(bufPtr + offset, fileBuf->data(), fileSize);
                    offset += fileSize;
                }
                memcpy(bufPtr + offset, "\r\n", 2);
                offset += 2;
            }
        }

        // Write final boundary
        memcpy(bufPtr + offset, "--", 2);
        offset += 2;
        memcpy(bufPtr + offset, m_boundary.c_str(), m_boundary.length());
        offset += m_boundary.length();
        memcpy(bufPtr + offset, "--\r\n", 4);

        retVal = resultBuffer;
        return 0;
    } else {
        return Runtime::setError("FormData encode: unsupported content type: " + type);
    }
}

}