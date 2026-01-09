/**
 * @file scanner.cpp
 * @brief TypeScript lexical scanner implementation (UTF-8 version)
 */

#include "Scanner.h"
#include <unordered_map>

namespace fibjs {
namespace ts {

// Keyword map using string_view as key for zero-copy lookup
static const std::unordered_map<std::string_view, SyntaxKind>& getKeywordMap() {
    static std::unordered_map<std::string_view, SyntaxKind> map = {
        {"abstract", SyntaxKind::AbstractKeyword},
        {"any", SyntaxKind::AnyKeyword},
        {"as", SyntaxKind::AsKeyword},
        {"asserts", SyntaxKind::AssertsKeyword},
        {"assert", SyntaxKind::AssertKeyword},
        {"async", SyntaxKind::AsyncKeyword},
        {"await", SyntaxKind::AwaitKeyword},
        {"bigint", SyntaxKind::BigIntKeyword},
        {"boolean", SyntaxKind::BooleanKeyword},
        {"break", SyntaxKind::BreakKeyword},
        {"case", SyntaxKind::CaseKeyword},
        {"catch", SyntaxKind::CatchKeyword},
        {"class", SyntaxKind::ClassKeyword},
        {"const", SyntaxKind::ConstKeyword},
        {"constructor", SyntaxKind::ConstructorKeyword},
        {"continue", SyntaxKind::ContinueKeyword},
        {"debugger", SyntaxKind::DebuggerKeyword},
        {"declare", SyntaxKind::DeclareKeyword},
        {"default", SyntaxKind::DefaultKeyword},
        {"delete", SyntaxKind::DeleteKeyword},
        {"do", SyntaxKind::DoKeyword},
        {"else", SyntaxKind::ElseKeyword},
        {"enum", SyntaxKind::EnumKeyword},
        {"export", SyntaxKind::ExportKeyword},
        {"extends", SyntaxKind::ExtendsKeyword},
        {"false", SyntaxKind::FalseKeyword},
        {"finally", SyntaxKind::FinallyKeyword},
        {"for", SyntaxKind::ForKeyword},
        {"from", SyntaxKind::FromKeyword},
        {"function", SyntaxKind::FunctionKeyword},
        {"get", SyntaxKind::GetKeyword},
        {"global", SyntaxKind::GlobalKeyword},
        {"if", SyntaxKind::IfKeyword},
        {"implements", SyntaxKind::ImplementsKeyword},
        {"import", SyntaxKind::ImportKeyword},
        {"in", SyntaxKind::InKeyword},
        {"infer", SyntaxKind::InferKeyword},
        {"instanceof", SyntaxKind::InstanceOfKeyword},
        {"interface", SyntaxKind::InterfaceKeyword},
        {"intrinsic", SyntaxKind::IntrinsicKeyword},
        {"is", SyntaxKind::IsKeyword},
        {"keyof", SyntaxKind::KeyOfKeyword},
        {"let", SyntaxKind::LetKeyword},
        {"module", SyntaxKind::ModuleKeyword},
        {"namespace", SyntaxKind::NamespaceKeyword},
        {"never", SyntaxKind::NeverKeyword},
        {"new", SyntaxKind::NewKeyword},
        {"null", SyntaxKind::NullKeyword},
        {"number", SyntaxKind::NumberKeyword},
        {"object", SyntaxKind::ObjectKeyword},
        {"of", SyntaxKind::OfKeyword},
        {"out", SyntaxKind::OutKeyword},
        {"override", SyntaxKind::OverrideKeyword},
        {"package", SyntaxKind::PackageKeyword},
        {"private", SyntaxKind::PrivateKeyword},
        {"protected", SyntaxKind::ProtectedKeyword},
        {"public", SyntaxKind::PublicKeyword},
        {"readonly", SyntaxKind::ReadonlyKeyword},
        {"require", SyntaxKind::RequireKeyword},
        {"return", SyntaxKind::ReturnKeyword},
        {"satisfies", SyntaxKind::SatisfiesKeyword},
        {"set", SyntaxKind::SetKeyword},
        {"static", SyntaxKind::StaticKeyword},
        {"string", SyntaxKind::StringKeyword},
        {"super", SyntaxKind::SuperKeyword},
        {"switch", SyntaxKind::SwitchKeyword},
        {"symbol", SyntaxKind::SymbolKeyword},
        {"this", SyntaxKind::ThisKeyword},
        {"throw", SyntaxKind::ThrowKeyword},
        {"true", SyntaxKind::TrueKeyword},
        {"try", SyntaxKind::TryKeyword},
        {"type", SyntaxKind::TypeKeyword},
        {"typeof", SyntaxKind::TypeOfKeyword},
        {"undefined", SyntaxKind::UndefinedKeyword},
        {"unique", SyntaxKind::UniqueKeyword},
        {"unknown", SyntaxKind::UnknownKeyword},
        {"using", SyntaxKind::UsingKeyword},
        {"var", SyntaxKind::VarKeyword},
        {"void", SyntaxKind::VoidKeyword},
        {"while", SyntaxKind::WhileKeyword},
        {"with", SyntaxKind::WithKeyword},
        {"yield", SyntaxKind::YieldKeyword},
        {"accessor", SyntaxKind::AccessorKeyword},
    };
    return map;
}

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
            p++;
        }
        // Don't erase shebang - just skip over it
        m_pos = p - m_text;
        if (p < end && (*p == '\n' || *p == '\r')) {
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
        } else if (ch == '/') {
            if (p + 1 < end && p[1] == '/') {
                // Single line comment - just skip over it (preserve in output)
                p += 2;
                while (p < end && *p != '\n' && *p != '\r') {
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
    auto& map = getKeywordMap();
    auto it = map.find(text);
    if (it != map.end()) {
        return it->second;
    }
    return SyntaxKind::Identifier;
}

SyntaxKind Scanner::scanIdentifierOrKeyword() {
    int start = m_pos;
    uint8_t* p = m_text + m_pos;
    uint8_t* end = m_text + m_length;
    
    while (p < end) {
        uint8_t ch = *p;
        if ((ch >= 'a' && ch <= 'z') ||
            (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9') ||
            ch == '_' || ch == '$' || ch > 127) {
            p++;
        } else if (ch == '\\') {
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
            break;
        } else {
            break;
        }
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
        if (isLineBreak(ch)) {
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
    skipTrivia();
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
        if (ch == CharCode::lineFeed || ch == CharCode::carriageReturn) {
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
