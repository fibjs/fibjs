/**
 * @file scanner.cpp
 * @brief TypeScript lexical scanner implementation (UTF-8 version)
 */

#include "Scanner.h"
#include "keyword_table.h"
#include <cstring>

namespace fibjs {
namespace ts {

// Keyword lookup: see getIdentifierToken() below.

Scanner::Scanner(uint8_t* text, size_t length)
    : m_text(text)
    , m_length(length)
    , m_pos(0)
    , m_startPos(0)
    , m_tokenStart(0)
    , m_token(SyntaxKind::Unknown)
    , m_hasLineBreak(false)
{
}

// Inline for performance - no bounds check needed when caller ensures pos < m_length
inline uint8_t Scanner::charCodeAt(int pos) const {
    return m_text[pos];
}

inline bool Scanner::isDigit(uint8_t ch) const {
    return ch >= '0' && ch <= '9';
}

inline bool Scanner::isIdentifierStart(uint8_t ch) const {
    return (ch >= 'a' && ch <= 'z') ||
           (ch >= 'A' && ch <= 'Z') ||
           ch == '_' || ch == '$' ||
           ch > 127;
}

inline bool Scanner::isIdentifierPart(uint8_t ch) const {
    return (ch >= 'a' && ch <= 'z') ||
           (ch >= 'A' && ch <= 'Z') ||
           (ch >= '0' && ch <= '9') ||
           ch == '_' || ch == '$' ||
           ch > 127;
}

inline bool Scanner::isLineBreak(uint8_t ch) const {
    return ch == '\n' || ch == '\r';
}

inline bool Scanner::isUnicodeLineBreakAt(int pos) const {
    if (pos + 2 >= (int)m_length) {
        return false;
    }
    uint8_t b0 = charCodeAt(pos);
    if (b0 != 0xE2) {
        return false;
    }
    uint8_t b1 = charCodeAt(pos + 1);
    uint8_t b2 = charCodeAt(pos + 2);
    return b1 == 0x80 && (b2 == 0xA8 || b2 == 0xA9); // U+2028/U+2029
}

inline bool Scanner::isWhiteSpace(uint8_t ch) const {
    return ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f';
}

void Scanner::skipTrivia() {
    uint8_t* p = m_text + m_pos;
    uint8_t* end = m_text + m_length;
    
    // Handle shebang at the beginning of the file
    if (m_pos == 0 && p + 1 < end && p[0] == '#' && p[1] == '!') {
        // Skip shebang line (preserve it as-is)
        p += 2;
        while (p < end && *p != '\n' && *p != '\r') {
            if (isUnicodeLineBreakAt((int)(p - m_text))) {
                break;
            }
            p++;
        }
        // Don't erase shebang - just skip over it
        m_pos = p - m_text;
        if (p < end && (*p == '\n' || *p == '\r' || isUnicodeLineBreakAt((int)(p - m_text)))) {
            m_hasLineBreak = true;
        }
        p = m_text + m_pos;
    }
    
    while (p < end) {
        uint8_t ch = *p;

        if (ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f') {
            p++;
        } else if (ch == '\n') {
            m_hasLineBreak = true;
            p++;
        } else if (ch == '\r') {
            m_hasLineBreak = true;
            p += (p + 1 < end && p[1] == '\n') ? 2 : 1;
        } else if (isUnicodeLineBreakAt((int)(p - m_text))) {
            m_hasLineBreak = true;
            p += 3;
        } else if (ch == '/') {
            if (p + 1 < end && p[1] == '/') {
                // Single line comment - just skip over it (preserve in output)
                p += 2;
                while (p < end && *p != '\n' && *p != '\r') {
                    if (isUnicodeLineBreakAt((int)(p - m_text))) {
                        break;
                    }
                    p++;
                }
            } else if (p + 1 < end && p[1] == '*') {
                // Multi-line comment - just skip over it (preserve in output)
                p += 2;
                while (p < end) {
                    if (*p == '*' && p + 1 < end && p[1] == '/') {
                        p += 2;
                        break;
                    }
                    if (*p == '\n' || *p == '\r') {
                        m_hasLineBreak = true;
                    } else if (isUnicodeLineBreakAt((int)(p - m_text))) {
                        m_hasLineBreak = true;
                        p += 3;
                        continue;
                    }
                    p++;
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }
    m_pos = p - m_text;
}

std::string_view Scanner::getTokenText() const {
    return std::string_view((const char*)m_text + m_tokenStart, m_pos - m_tokenStart);
}

void Scanner::setTextPos(int pos) {
    m_pos = pos;
    m_tokenStart = pos;
    m_token = SyntaxKind::Unknown;
    m_tokenValue = {};
    m_hasLineBreak = false;
}

SyntaxKind Scanner::getIdentifierToken(std::string_view text) const {
    // Minimal perfect hash over the 83 keywords (see keyword_table.h). The key is
    // the first six bytes plus the last six bytes: for a name of at most 12 bytes
    // that is the whole name, so the slot can only hit a keyword when the text
    // *is* that keyword. The length goes into the comparison as well, which also
    // rejects a 13+ byte name that happens to share both halves.
    //
    // The obvious compact key - first two bytes, last byte, length - is NOT
    // injective: "return" and "region" both give 0x066e6572, so every identifier
    // of that shape was silently read as a keyword. Packing the ends is the same
    // number of loads and cannot alias.
    const size_t length = text.size();
    if (length < 2 || length > 11) {
        return SyntaxKind::Identifier;
    }

    const char* s = text.data();
    uint64_t hi = 0;
    uint64_t lo = 0;
    if (length <= 6) {
        memcpy(&hi, s, length);
        lo = hi;
    } else {
        memcpy(&hi, s, 6);
        memcpy(&lo, s + length - 6, 6);
    }

    // Mix the two halves into 64 bits, then hash to a slot. Both steps stay in
    // 64-bit arithmetic on purpose: a 96-bit key shifted right by 86 needs a
    // 128-bit product, and the 64-bit version of that expression evaluates to a
    // constant, which would leave every identifier unprefixed by a keyword.
    const uint64_t key = (hi * 0x9e3779b97f4a7c15ull) ^ (lo * 0xc2b2ae3d27d4eb4full);
    const KeywordEntry& e = kKeywordTable[(key * 5257846158329281261ull) >> 39 & 255];
    return (e.hi == hi && e.lo == lo && e.len == length) ? e.kind : SyntaxKind::Identifier;
}

// 1 = byte can continue an identifier (A-Z, a-z, 0-9, _, $). A table lookup
// is one load instead of the five comparisons the predicate used to chain, and
// it also folds the "is this a keyword run?" test into the same loop.
static const uint8_t kIdentifierChar[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

SyntaxKind Scanner::scanIdentifierOrKeyword() {
    int start = m_pos;
    uint8_t* p = m_text + m_pos;
    uint8_t* end = m_text + m_length;
    
    while (p < end) {
        uint8_t ch = *p;
        // Fast path: plain ASCII identifier characters (the overwhelming majority).
        if (kIdentifierChar[ch]) {
            p++;
            continue;
        }
        if (ch > 127) {
            // U+2028/U+2029 are line terminators, not identifier characters.
            if (isUnicodeLineBreakAt((int)(p - m_text))) {
                break;
            }
            p++;
            continue;
        }
        if (ch == '\\') {
            // Handle unicode escape sequences: \uXXXX or \u{XXXX}
            // Use lenient parsing - consume whatever looks like unicode escape
            if (p + 1 < end && *(p + 1) == 'u') {
                if (p + 2 < end && *(p + 2) == '{') {
                    // \u{XXXX...} - variable length, find closing }
                    uint8_t* q = p + 3;
                    while (q < end && *q != '}' && *q != '\n' && *q != '\r') {
                        q++;
                    }
                    if (q < end && *q == '}') {
                        p = q + 1;
                        continue;
                    }
                    // No closing } found - treat as invalid, break
                    break;
                } else {
                    // \uXXXX - consume \u and up to 4 hex digits (lenient)
                    p += 2; // consume \u
                    int hexCount = 0;
                    while (p < end && hexCount < 4) {
                        uint8_t c = *p;
                        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                            p++;
                            hexCount++;
                        } else {
                            break;
                        }
                    }
                    continue;
                }
            }
            break; // invalid escape - stop the identifier here
        }
        break; // not an identifier character
    }
    m_pos = p - m_text;
    m_tokenValue = std::string_view((const char*)m_text + start, m_pos - start);
    return getIdentifierToken(m_tokenValue);
}

SyntaxKind Scanner::scanNumber() {
    int start = m_pos;
    
    // Check for hex, binary, octal
    if (charCodeAt(m_pos) == CharCode::_0) {
        uint8_t next = charCodeAt(m_pos + 1);
        if (next == 'x' || next == 'X') {
            m_pos += 2;
            scanDigitRun([](uint8_t ch) { return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'); });
        } else if (next == 'b' || next == 'B') {
            m_pos += 2;
            scanDigitRun([](uint8_t ch) { return ch == '0' || ch == '1'; });
        } else if (next == 'o' || next == 'O') {
            m_pos += 2;
            scanDigitRun([](uint8_t ch) { return ch >= '0' && ch <= '7'; });
        }
    }
    
    // Decimal part
    scanDigitRun([](uint8_t ch) { return ch >= '0' && ch <= '9'; });
    
    // Fractional part
    if (charCodeAt(m_pos) == CharCode::dot) {
        m_pos++;
        scanDigitRun([](uint8_t ch) { return ch >= '0' && ch <= '9'; });
    }
    
    // Exponent part
    uint8_t ch = charCodeAt(m_pos);
    if (ch == 'e' || ch == 'E') {
        m_pos++;
        ch = charCodeAt(m_pos);
        if (ch == CharCode::plus || ch == CharCode::minus) {
            m_pos++;
        }
        scanDigitRun([](uint8_t c) { return c >= '0' && c <= '9'; });
    }
    
    // BigInt suffix
    if (charCodeAt(m_pos) == 'n') {
        m_pos++;
        return SyntaxKind::BigIntLiteral;
    }
    
    return SyntaxKind::NumericLiteral;
}

SyntaxKind Scanner::scanString(uint8_t quote) {
    m_pos++; // skip opening quote
    
    while (m_pos < (int)m_length) {
        uint8_t ch = charCodeAt(m_pos);
        if (ch == quote) {
            m_pos++; // skip closing quote
            return SyntaxKind::StringLiteral;
        }
        if (ch == CharCode::backslash) {
            m_pos++;
            if (m_pos < (int)m_length) {
                uint8_t next = charCodeAt(m_pos);
                m_pos++;
                // Handle \r\n as a single line continuation
                if (next == CharCode::carriageReturn && m_pos < (int)m_length 
                    && charCodeAt(m_pos) == CharCode::lineFeed) {
                    m_pos++;
                }
            }
            continue;
        }
        if (isLineBreak(ch) || isUnicodeLineBreakAt(m_pos)) {
            // Unterminated string - treat as end
            break;
        }
        m_pos++;
    }
    
    return SyntaxKind::StringLiteral;
}

SyntaxKind Scanner::scanTemplateOrTemplateTail() {
    m_pos++; // skip ` or }
    bool isHead = (charCodeAt(m_tokenStart) == CharCode::backtick);
    
    while (m_pos < (int)m_length) {
        uint8_t ch = charCodeAt(m_pos);
        if (ch == CharCode::backtick) {
            m_pos++;
            return isHead ? SyntaxKind::NoSubstitutionTemplateLiteral : SyntaxKind::TemplateTail;
        }
        if (ch == CharCode::$ && charCodeAt(m_pos + 1) == CharCode::openBrace) {
            m_pos += 2;
            return isHead ? SyntaxKind::TemplateHead : SyntaxKind::TemplateMiddle;
        }
        if (ch == CharCode::backslash) {
            m_pos += 2;
            continue;
        }
        m_pos++;
    }
    
    return SyntaxKind::NoSubstitutionTemplateLiteral;
}

SyntaxKind Scanner::reScanTemplateToken() {
    // After a closing brace in a template literal, rescan as template middle or tail
    // Position should be right after the }
    m_tokenStart = m_pos - 1; // Set token start to the } position
    
    while (m_pos < (int)m_length) {
        uint8_t ch = charCodeAt(m_pos);
        if (ch == CharCode::backtick) {
            m_pos++;
            m_token = SyntaxKind::TemplateTail;
            return m_token;
        }
        if (ch == CharCode::$ && charCodeAt(m_pos + 1) == CharCode::openBrace) {
            m_pos += 2;
            m_token = SyntaxKind::TemplateMiddle;
            return m_token;
        }
        if (ch == CharCode::backslash) {
            m_pos += 2;
            continue;
        }
        m_pos++;
    }
    
    m_token = SyntaxKind::TemplateTail;
    return m_token;
}

// Tokens that one byte decides entirely. Checking this table before the
// operator switch keeps the ~40% of tokens that are braces, parens, commas
// and semicolons out of a 300-line branch ladder.
static const uint8_t kSingleByteToken[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, (uint8_t)SyntaxKind::OpenParenToken, (uint8_t)SyntaxKind::CloseParenToken, 0, 0, (uint8_t)SyntaxKind::CommaToken, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, (uint8_t)SyntaxKind::ColonToken, (uint8_t)SyntaxKind::SemicolonToken, 0, 0, 0, 0,
    (uint8_t)SyntaxKind::AtToken, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, (uint8_t)SyntaxKind::OpenBracketToken, 0, (uint8_t)SyntaxKind::CloseBracketToken, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, (uint8_t)SyntaxKind::OpenBraceToken, 0, (uint8_t)SyntaxKind::CloseBraceToken, (uint8_t)SyntaxKind::TildeToken, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

SyntaxKind Scanner::scan() {
    m_startPos = m_pos;
    m_hasLineBreak = false;
    m_tokenValue = {};

rescan:
    // Fast path: most tokens are not preceded by trivia at all, and skipTrivia() is
    // too large to be inlined, so check the current byte before calling it. (The
    // position 0 case is excluded because skipTrivia() handles the shebang there.)
    if (m_pos != 0) {
        if (m_pos >= (int)m_length) {
            m_tokenStart = m_pos;
            m_token = SyntaxKind::EndOfFileToken;
            return m_token;
        }
        uint8_t c = m_text[m_pos];
        if (c == ' ' || c == '\t' || c == '\v' || c == '\f' ||
            c == '\n' || c == '\r' || c == '/' || c > 127) {
            skipTrivia();
        }
    } else {
        skipTrivia();
    }
    m_tokenStart = m_pos;
    
    if (m_pos >= (int)m_length) {
        m_token = SyntaxKind::EndOfFileToken;
        return m_token;
    }
    
    uint8_t ch = charCodeAt(m_pos);

    // One-byte tokens (braces, parens, brackets, ; , : @ ~) are ~40% of all
    // tokens; answer them from a table before the operator ladder.
    if (kSingleByteToken[ch]) {
        m_pos++;
        m_token = (SyntaxKind)kSingleByteToken[ch];
        return m_token;
    }
    
    // Identifier or keyword
    if (isIdentifierStart(ch)) {
        m_token = scanIdentifierOrKeyword();
        return m_token;
    }
    
    // Unicode escape at start of identifier: \uXXXX or \u{XXXX}
    if (ch == CharCode::backslash && charCodeAt(m_pos + 1) == 'u') {
        m_token = scanIdentifierOrKeyword();
        return m_token;
    }
    
    // Number
    if (isDigit(ch)) {
        m_token = scanNumber();
        return m_token;
    }
    
    // String
    if (ch == CharCode::doubleQuote || ch == CharCode::singleQuote) {
        m_token = scanString(ch);
        return m_token;
    }
    
    // Template
    if (ch == CharCode::backtick) {
        m_token = scanTemplateOrTemplateTail();
        return m_token;
    }
    
    // Punctuation
    switch (ch) {
        case CharCode::openBrace:
            m_pos++;
            m_token = SyntaxKind::OpenBraceToken;
            return m_token;
        case CharCode::closeBrace:
            m_pos++;
            m_token = SyntaxKind::CloseBraceToken;
            return m_token;
        case CharCode::openParen:
            m_pos++;
            m_token = SyntaxKind::OpenParenToken;
            return m_token;
        case CharCode::closeParen:
            m_pos++;
            m_token = SyntaxKind::CloseParenToken;
            return m_token;
        case CharCode::openBracket:
            m_pos++;
            m_token = SyntaxKind::OpenBracketToken;
            return m_token;
        case CharCode::closeBracket:
            m_pos++;
            m_token = SyntaxKind::CloseBracketToken;
            return m_token;
        case CharCode::semicolon:
            m_pos++;
            m_token = SyntaxKind::SemicolonToken;
            return m_token;
        case CharCode::comma:
            m_pos++;
            m_token = SyntaxKind::CommaToken;
            return m_token;
        case CharCode::colon:
            m_pos++;
            m_token = SyntaxKind::ColonToken;
            return m_token;
        case CharCode::at:
            m_pos++;
            m_token = SyntaxKind::AtToken;
            return m_token;
        case CharCode::hash:
            m_pos++;
            if (isIdentifierStart(charCodeAt(m_pos))) {
                scanIdentifierOrKeyword();
                m_token = SyntaxKind::PrivateIdentifier;
            } else {
                m_token = SyntaxKind::HashToken;
            }
            return m_token;
            
        case CharCode::dot:
            if (isDigit(charCodeAt(m_pos + 1))) {
                m_token = scanNumber();
                return m_token;
            }
            if (charCodeAt(m_pos + 1) == CharCode::dot && charCodeAt(m_pos + 2) == CharCode::dot) {
                m_pos += 3;
                m_token = SyntaxKind::DotDotDotToken;
                return m_token;
            }
            m_pos++;
            m_token = SyntaxKind::DotToken;
            return m_token;
            
        case CharCode::question:
            if (charCodeAt(m_pos + 1) == CharCode::question) {
                if (charCodeAt(m_pos + 2) == CharCode::equals) {
                    m_pos += 3;
                    m_token = SyntaxKind::QuestionQuestionEqualsToken;
                    return m_token;
                }
                m_pos += 2;
                m_token = SyntaxKind::QuestionQuestionToken;
                return m_token;
            }
            if (charCodeAt(m_pos + 1) == CharCode::dot && !isDigit(charCodeAt(m_pos + 2))) {
                m_pos += 2;
                m_token = SyntaxKind::QuestionDotToken;
                return m_token;
            }
            m_pos++;
            m_token = SyntaxKind::QuestionToken;
            return m_token;
            
        case CharCode::exclamation:
            if (charCodeAt(m_pos + 1) == CharCode::equals) {
                if (charCodeAt(m_pos + 2) == CharCode::equals) {
                    m_pos += 3;
                    m_token = SyntaxKind::ExclamationEqualsEqualsToken;
                    return m_token;
                }
                m_pos += 2;
                m_token = SyntaxKind::ExclamationEqualsToken;
                return m_token;
            }
            m_pos++;
            m_token = SyntaxKind::ExclamationToken;
            return m_token;
            
        case CharCode::equals:
            if (charCodeAt(m_pos + 1) == CharCode::equals) {
                if (charCodeAt(m_pos + 2) == CharCode::equals) {
                    m_pos += 3;
                    m_token = SyntaxKind::EqualsEqualsEqualsToken;
                    return m_token;
                }
                m_pos += 2;
                m_token = SyntaxKind::EqualsEqualsToken;
                return m_token;
            }
            if (charCodeAt(m_pos + 1) == CharCode::greaterThan) {
                m_pos += 2;
                m_token = SyntaxKind::EqualsGreaterThanToken;
                return m_token;
            }
            m_pos++;
            m_token = SyntaxKind::EqualsToken;
            return m_token;
            
        case CharCode::lessThan:
            if (charCodeAt(m_pos + 1) == CharCode::lessThan) {
                if (charCodeAt(m_pos + 2) == CharCode::equals) {
                    m_pos += 3;
                    m_token = SyntaxKind::LessThanLessThanEqualsToken;
                    return m_token;
                }
                m_pos += 2;
                m_token = SyntaxKind::LessThanLessThanToken;
                return m_token;
            }
            if (charCodeAt(m_pos + 1) == CharCode::equals) {
                m_pos += 2;
                m_token = SyntaxKind::LessThanEqualsToken;
                return m_token;
            }
            if (charCodeAt(m_pos + 1) == CharCode::slash) {
                // `</` is only a special token in JSX contexts.
                // In `.ts`, sequences like `</**doc*/ T>` appear in type parameter lists
                // when a comment immediately follows `<`. Treat `</*` as `<` and let
                // the comment scanner consume `/* ... */`.
                if (charCodeAt(m_pos + 2) != CharCode::asterisk) {
                    m_pos += 2;
                    m_token = SyntaxKind::LessThanSlashToken;
                    return m_token;
                }
            }
            m_pos++;
            m_token = SyntaxKind::LessThanToken;
            return m_token;
            
        case CharCode::greaterThan:
            // Note: we don't handle >> and >>> specially here,
            // parser handles those in type contexts
            if (charCodeAt(m_pos + 1) == CharCode::equals) {
                m_pos += 2;
                m_token = SyntaxKind::GreaterThanEqualsToken;
                return m_token;
            }
            m_pos++;
            m_token = SyntaxKind::GreaterThanToken;
            return m_token;
            
        case CharCode::plus:
            if (charCodeAt(m_pos + 1) == CharCode::plus) {
                m_pos += 2;
                m_token = SyntaxKind::PlusPlusToken;
                return m_token;
            }
            if (charCodeAt(m_pos + 1) == CharCode::equals) {
                m_pos += 2;
                m_token = SyntaxKind::PlusEqualsToken;
                return m_token;
            }
            m_pos++;
            m_token = SyntaxKind::PlusToken;
            return m_token;
            
        case CharCode::minus:
            if (charCodeAt(m_pos + 1) == CharCode::minus) {
                m_pos += 2;
                m_token = SyntaxKind::MinusMinusToken;
                return m_token;
            }
            if (charCodeAt(m_pos + 1) == CharCode::equals) {
                m_pos += 2;
                m_token = SyntaxKind::MinusEqualsToken;
                return m_token;
            }
            m_pos++;
            m_token = SyntaxKind::MinusToken;
            return m_token;
            
        case CharCode::asterisk:
            if (charCodeAt(m_pos + 1) == CharCode::asterisk) {
                if (charCodeAt(m_pos + 2) == CharCode::equals) {
                    m_pos += 3;
                    m_token = SyntaxKind::AsteriskAsteriskEqualsToken;
                    return m_token;
                }
                m_pos += 2;
                m_token = SyntaxKind::AsteriskAsteriskToken;
                return m_token;
            }
            if (charCodeAt(m_pos + 1) == CharCode::equals) {
                m_pos += 2;
                m_token = SyntaxKind::AsteriskEqualsToken;
                return m_token;
            }
            m_pos++;
            m_token = SyntaxKind::AsteriskToken;
            return m_token;
            
        case CharCode::slash:
            // Defensive: comments should normally be consumed by skipTrivia(),
            // but if we ever land on a comment start here, treat it as trivia
            // and rescan to avoid tokenizing comment contents.
            if (charCodeAt(m_pos + 1) == CharCode::slash || charCodeAt(m_pos + 1) == CharCode::asterisk) {
                skipTrivia();
                m_tokenStart = m_pos;
                if (m_pos >= (int)m_length) {
                    m_token = SyntaxKind::EndOfFileToken;
                    return m_token;
                }
                ch = charCodeAt(m_pos);
                goto rescan;
            }
            if (charCodeAt(m_pos + 1) == CharCode::equals) {
                m_pos += 2;
                m_token = SyntaxKind::SlashEqualsToken;
                return m_token;
            }
            m_pos++;
            m_token = SyntaxKind::SlashToken;
            return m_token;
            
        case CharCode::percent:
            if (charCodeAt(m_pos + 1) == CharCode::equals) {
                m_pos += 2;
                m_token = SyntaxKind::PercentEqualsToken;
                return m_token;
            }
            m_pos++;
            m_token = SyntaxKind::PercentToken;
            return m_token;
            
        case CharCode::ampersand:
            if (charCodeAt(m_pos + 1) == CharCode::ampersand) {
                if (charCodeAt(m_pos + 2) == CharCode::equals) {
                    m_pos += 3;
                    m_token = SyntaxKind::AmpersandAmpersandEqualsToken;
                    return m_token;
                }
                m_pos += 2;
                m_token = SyntaxKind::AmpersandAmpersandToken;
                return m_token;
            }
            if (charCodeAt(m_pos + 1) == CharCode::equals) {
                m_pos += 2;
                m_token = SyntaxKind::AmpersandEqualsToken;
                return m_token;
            }
            m_pos++;
            m_token = SyntaxKind::AmpersandToken;
            return m_token;
            
        case CharCode::bar:
            if (charCodeAt(m_pos + 1) == CharCode::bar) {
                if (charCodeAt(m_pos + 2) == CharCode::equals) {
                    m_pos += 3;
                    m_token = SyntaxKind::BarBarEqualsToken;
                    return m_token;
                }
                m_pos += 2;
                m_token = SyntaxKind::BarBarToken;
                return m_token;
            }
            if (charCodeAt(m_pos + 1) == CharCode::equals) {
                m_pos += 2;
                m_token = SyntaxKind::BarEqualsToken;
                return m_token;
            }
            m_pos++;
            m_token = SyntaxKind::BarToken;
            return m_token;
            
        case CharCode::caret:
            if (charCodeAt(m_pos + 1) == CharCode::equals) {
                m_pos += 2;
                m_token = SyntaxKind::CaretEqualsToken;
                return m_token;
            }
            m_pos++;
            m_token = SyntaxKind::CaretToken;
            return m_token;
            
        case CharCode::tilde:
            m_pos++;
            m_token = SyntaxKind::TildeToken;
            return m_token;
    }
    
    // Unknown character - advance and return unknown
    m_pos++;
    m_token = SyntaxKind::Unknown;
    return m_token;
}

SyntaxKind Scanner::reScanSlashToken() {
    // If current token is not slash, return as-is
    if (m_token != SyntaxKind::SlashToken && m_token != SyntaxKind::SlashEqualsToken) {
        return m_token;
    }
    
    // Try to scan as regex literal starting from token start
    int p = m_tokenStart + 1;
    bool inEscape = false;
    bool inCharacterClass = false;
    
    while (p < (int)m_length) {
        uint8_t ch = m_text[p];
        
        // Newline terminates regex (unterminated)
        if (ch == CharCode::lineFeed || ch == CharCode::carriageReturn || isUnicodeLineBreakAt(p)) {
            return m_token; // Not a valid regex, keep as slash
        }
        
        if (inEscape) {
            inEscape = false;
        } else if (ch == CharCode::slash && !inCharacterClass) {
            // Found closing slash
            p++;
            break;
        } else if (ch == '[') {
            inCharacterClass = true;
        } else if (ch == CharCode::backslash) {
            inEscape = true;
        } else if (ch == ']') {
            inCharacterClass = false;
        }
        p++;
    }
    
    // Check if we actually found the closing slash
    if (p <= m_tokenStart + 1 || m_text[p - 1] != CharCode::slash) {
        return m_token; // Not a valid regex
    }
    
    // Scan optional flags (a-z, A-Z)
    while (p < (int)m_length) {
        uint8_t ch = m_text[p];
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
            p++;
        } else {
            break;
        }
    }
    
    // Update scanner state
    m_pos = p;
    m_token = SyntaxKind::RegularExpressionLiteral;
    return m_token;
}

std::vector<Token> Scanner::scanAllTokens() {
    std::vector<Token> tokens;
    // Token density depends entirely on the file: ~3.4 bytes per token in dense
    // code, 10-25 in hand-written source that is mostly jsdoc. Reserve for the
    // dense case, because a short reservation costs a full reallocate-and-copy of
    // the vector on the files where scanning is most of the work, while the
    // overshoot is only 16 bytes a token for the sparse ones.
    tokens.reserve(m_length / 4 + 16);
    setTextPos(0);
    
    // Track template literal nesting depth
    std::vector<int> templateDepthStack; // Stack of brace depths at template start
    int braceDepth = 0;
    
    while (true) {
        SyntaxKind kind = scan();
        
        // Note: We do NOT try to rescan slash as regex here, because we don't have
        // syntactic context. The parser (TsStrip) will call reScanSlashTokenAsRegularExpressionLiteral()
        // when it determines that a slash is in a position where a regex is expected.
        // This avoids incorrectly treating division operators as regex delimiters.
        
        // Track template literal state
        if (kind == SyntaxKind::TemplateHead) {
            templateDepthStack.push_back(braceDepth);
        } else if (kind == SyntaxKind::OpenBraceToken) {
            braceDepth++;
        } else if (kind == SyntaxKind::CloseBraceToken) {
            // Check if we're closing a template expression
            if (!templateDepthStack.empty() && braceDepth == templateDepthStack.back()) {
                // This } closes a template expression, rescan as template middle/tail
                reScanTemplateToken();
                kind = m_token;
                if (kind == SyntaxKind::TemplateMiddle) {
                    // Continue in template, don't pop the stack
                } else if (kind == SyntaxKind::TemplateTail) {
                    templateDepthStack.pop_back();
                }
            } else {
                braceDepth--;
            }
        }
        
        tokens.push_back(Token(kind, m_tokenStart, m_pos, m_hasLineBreak));
        if (kind == SyntaxKind::EndOfFileToken) {
            break;
        }
    }
    
    return tokens;
}

} // namespace ts
} // namespace fibjs
