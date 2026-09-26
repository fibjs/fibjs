/**
 * @file scanner.cpp
 * @brief TypeScript lexical scanner implementation (UTF-8 version)
 */

#include "Scanner.h"
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
    // Keywords are few and all ASCII lowercase, so comparing against the few
    // candidates of the same length beats the hash lookup this replaces (that
    // lookup was ~20% of strip time). Constant-size memcmp compiles to a
    // single load + compare.
    const size_t length = text.size();
    const char* s = text.data();
    switch (length) {
        case 2:
            if (std::memcmp(s, "as", 2) == 0) return SyntaxKind::AsKeyword;
            if (std::memcmp(s, "do", 2) == 0) return SyntaxKind::DoKeyword;
            if (std::memcmp(s, "if", 2) == 0) return SyntaxKind::IfKeyword;
            if (std::memcmp(s, "in", 2) == 0) return SyntaxKind::InKeyword;
            if (std::memcmp(s, "is", 2) == 0) return SyntaxKind::IsKeyword;
            if (std::memcmp(s, "of", 2) == 0) return SyntaxKind::OfKeyword;
            break;
        case 3:
            if (std::memcmp(s, "any", 3) == 0) return SyntaxKind::AnyKeyword;
            if (std::memcmp(s, "for", 3) == 0) return SyntaxKind::ForKeyword;
            if (std::memcmp(s, "get", 3) == 0) return SyntaxKind::GetKeyword;
            if (std::memcmp(s, "let", 3) == 0) return SyntaxKind::LetKeyword;
            if (std::memcmp(s, "new", 3) == 0) return SyntaxKind::NewKeyword;
            if (std::memcmp(s, "out", 3) == 0) return SyntaxKind::OutKeyword;
            if (std::memcmp(s, "set", 3) == 0) return SyntaxKind::SetKeyword;
            if (std::memcmp(s, "try", 3) == 0) return SyntaxKind::TryKeyword;
            if (std::memcmp(s, "var", 3) == 0) return SyntaxKind::VarKeyword;
            break;
        case 4:
            if (std::memcmp(s, "case", 4) == 0) return SyntaxKind::CaseKeyword;
            if (std::memcmp(s, "else", 4) == 0) return SyntaxKind::ElseKeyword;
            if (std::memcmp(s, "enum", 4) == 0) return SyntaxKind::EnumKeyword;
            if (std::memcmp(s, "from", 4) == 0) return SyntaxKind::FromKeyword;
            if (std::memcmp(s, "null", 4) == 0) return SyntaxKind::NullKeyword;
            if (std::memcmp(s, "this", 4) == 0) return SyntaxKind::ThisKeyword;
            if (std::memcmp(s, "true", 4) == 0) return SyntaxKind::TrueKeyword;
            if (std::memcmp(s, "type", 4) == 0) return SyntaxKind::TypeKeyword;
            if (std::memcmp(s, "void", 4) == 0) return SyntaxKind::VoidKeyword;
            if (std::memcmp(s, "with", 4) == 0) return SyntaxKind::WithKeyword;
            break;
        case 5:
            if (std::memcmp(s, "async", 5) == 0) return SyntaxKind::AsyncKeyword;
            if (std::memcmp(s, "await", 5) == 0) return SyntaxKind::AwaitKeyword;
            if (std::memcmp(s, "break", 5) == 0) return SyntaxKind::BreakKeyword;
            if (std::memcmp(s, "catch", 5) == 0) return SyntaxKind::CatchKeyword;
            if (std::memcmp(s, "class", 5) == 0) return SyntaxKind::ClassKeyword;
            if (std::memcmp(s, "const", 5) == 0) return SyntaxKind::ConstKeyword;
            if (std::memcmp(s, "false", 5) == 0) return SyntaxKind::FalseKeyword;
            if (std::memcmp(s, "infer", 5) == 0) return SyntaxKind::InferKeyword;
            if (std::memcmp(s, "keyof", 5) == 0) return SyntaxKind::KeyOfKeyword;
            if (std::memcmp(s, "never", 5) == 0) return SyntaxKind::NeverKeyword;
            if (std::memcmp(s, "super", 5) == 0) return SyntaxKind::SuperKeyword;
            if (std::memcmp(s, "throw", 5) == 0) return SyntaxKind::ThrowKeyword;
            if (std::memcmp(s, "using", 5) == 0) return SyntaxKind::UsingKeyword;
            if (std::memcmp(s, "while", 5) == 0) return SyntaxKind::WhileKeyword;
            if (std::memcmp(s, "yield", 5) == 0) return SyntaxKind::YieldKeyword;
            break;
        case 6:
            if (std::memcmp(s, "assert", 6) == 0) return SyntaxKind::AssertKeyword;
            if (std::memcmp(s, "bigint", 6) == 0) return SyntaxKind::BigIntKeyword;
            if (std::memcmp(s, "delete", 6) == 0) return SyntaxKind::DeleteKeyword;
            if (std::memcmp(s, "export", 6) == 0) return SyntaxKind::ExportKeyword;
            if (std::memcmp(s, "global", 6) == 0) return SyntaxKind::GlobalKeyword;
            if (std::memcmp(s, "import", 6) == 0) return SyntaxKind::ImportKeyword;
            if (std::memcmp(s, "module", 6) == 0) return SyntaxKind::ModuleKeyword;
            if (std::memcmp(s, "number", 6) == 0) return SyntaxKind::NumberKeyword;
            if (std::memcmp(s, "object", 6) == 0) return SyntaxKind::ObjectKeyword;
            if (std::memcmp(s, "public", 6) == 0) return SyntaxKind::PublicKeyword;
            if (std::memcmp(s, "return", 6) == 0) return SyntaxKind::ReturnKeyword;
            if (std::memcmp(s, "static", 6) == 0) return SyntaxKind::StaticKeyword;
            if (std::memcmp(s, "string", 6) == 0) return SyntaxKind::StringKeyword;
            if (std::memcmp(s, "switch", 6) == 0) return SyntaxKind::SwitchKeyword;
            if (std::memcmp(s, "symbol", 6) == 0) return SyntaxKind::SymbolKeyword;
            if (std::memcmp(s, "typeof", 6) == 0) return SyntaxKind::TypeOfKeyword;
            if (std::memcmp(s, "unique", 6) == 0) return SyntaxKind::UniqueKeyword;
            break;
        case 7:
            if (std::memcmp(s, "asserts", 7) == 0) return SyntaxKind::AssertsKeyword;
            if (std::memcmp(s, "boolean", 7) == 0) return SyntaxKind::BooleanKeyword;
            if (std::memcmp(s, "declare", 7) == 0) return SyntaxKind::DeclareKeyword;
            if (std::memcmp(s, "default", 7) == 0) return SyntaxKind::DefaultKeyword;
            if (std::memcmp(s, "extends", 7) == 0) return SyntaxKind::ExtendsKeyword;
            if (std::memcmp(s, "finally", 7) == 0) return SyntaxKind::FinallyKeyword;
            if (std::memcmp(s, "package", 7) == 0) return SyntaxKind::PackageKeyword;
            if (std::memcmp(s, "private", 7) == 0) return SyntaxKind::PrivateKeyword;
            if (std::memcmp(s, "require", 7) == 0) return SyntaxKind::RequireKeyword;
            if (std::memcmp(s, "unknown", 7) == 0) return SyntaxKind::UnknownKeyword;
            break;
        case 8:
            if (std::memcmp(s, "abstract", 8) == 0) return SyntaxKind::AbstractKeyword;
            if (std::memcmp(s, "accessor", 8) == 0) return SyntaxKind::AccessorKeyword;
            if (std::memcmp(s, "continue", 8) == 0) return SyntaxKind::ContinueKeyword;
            if (std::memcmp(s, "debugger", 8) == 0) return SyntaxKind::DebuggerKeyword;
            if (std::memcmp(s, "function", 8) == 0) return SyntaxKind::FunctionKeyword;
            if (std::memcmp(s, "override", 8) == 0) return SyntaxKind::OverrideKeyword;
            if (std::memcmp(s, "readonly", 8) == 0) return SyntaxKind::ReadonlyKeyword;
            break;
        case 9:
            if (std::memcmp(s, "interface", 9) == 0) return SyntaxKind::InterfaceKeyword;
            if (std::memcmp(s, "intrinsic", 9) == 0) return SyntaxKind::IntrinsicKeyword;
            if (std::memcmp(s, "namespace", 9) == 0) return SyntaxKind::NamespaceKeyword;
            if (std::memcmp(s, "protected", 9) == 0) return SyntaxKind::ProtectedKeyword;
            if (std::memcmp(s, "satisfies", 9) == 0) return SyntaxKind::SatisfiesKeyword;
            if (std::memcmp(s, "undefined", 9) == 0) return SyntaxKind::UndefinedKeyword;
            break;
        case 10:
            if (std::memcmp(s, "implements", 10) == 0) return SyntaxKind::ImplementsKeyword;
            if (std::memcmp(s, "instanceof", 10) == 0) return SyntaxKind::InstanceOfKeyword;
            break;
        case 11:
            if (std::memcmp(s, "constructor", 11) == 0) return SyntaxKind::ConstructorKeyword;
            break;
        default:
            break;
    }
    return SyntaxKind::Identifier;
}

SyntaxKind Scanner::scanIdentifierOrKeyword() {
    int start = m_pos;
    uint8_t* p = m_text + m_pos;
    uint8_t* end = m_text + m_length;
    
    while (p < end) {
        uint8_t ch = *p;
        // Fast path: plain ASCII identifier characters (the overwhelming majority).
        // The Unicode line-break check below is only relevant for bytes >= 0x80, so
        // it does not have to run for every byte.
        if ((ch >= 'a' && ch <= 'z') ||
            (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9') ||
            ch == '_' || ch == '$') {
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
            while (m_pos < (int)m_length) {
                uint8_t ch = charCodeAt(m_pos);
                if (isDigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) {
                    m_pos++;
                } else {
                    break;
                }
            }
        } else if (next == 'b' || next == 'B') {
            m_pos += 2;
            while (m_pos < (int)m_length) {
                uint8_t ch = charCodeAt(m_pos);
                if (ch == '0' || ch == '1') {
                    m_pos++;
                } else {
                    break;
                }
            }
        } else if (next == 'o' || next == 'O') {
            m_pos += 2;
            while (m_pos < (int)m_length) {
                uint8_t ch = charCodeAt(m_pos);
                if (ch >= '0' && ch <= '7') {
                    m_pos++;
                } else {
                    break;
                }
            }
        }
    }
    
    // Decimal part
    while (m_pos < (int)m_length && isDigit(charCodeAt(m_pos))) {
        m_pos++;
    }
    
    // Fractional part
    if (charCodeAt(m_pos) == CharCode::dot) {
        m_pos++;
        while (m_pos < (int)m_length && isDigit(charCodeAt(m_pos))) {
            m_pos++;
        }
    }
    
    // Exponent part
    uint8_t ch = charCodeAt(m_pos);
    if (ch == 'e' || ch == 'E') {
        m_pos++;
        ch = charCodeAt(m_pos);
        if (ch == CharCode::plus || ch == CharCode::minus) {
            m_pos++;
        }
        while (m_pos < (int)m_length && isDigit(charCodeAt(m_pos))) {
            m_pos++;
        }
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
    // Rough estimate (a token plus its trivia averages a handful of bytes) to avoid
    // repeatedly reallocating and copying the token vector on large files.
    tokens.reserve(m_length / 8 + 16);
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
