/**
 * @file scanner.cpp
 * @brief TypeScript lexical scanner implementation
 */

#include "Scanner.h"
#include <unordered_map>

namespace fibjs {
namespace ts {

// Keyword map
static const std::unordered_map<exlib::wstring, SyntaxKind>& getKeywordMap() {
    static std::unordered_map<exlib::wstring, SyntaxKind> map = {
        {u"abstract", SyntaxKind::AbstractKeyword},
        {u"any", SyntaxKind::AnyKeyword},
        {u"as", SyntaxKind::AsKeyword},
        {u"asserts", SyntaxKind::AssertsKeyword},
        {u"assert", SyntaxKind::AssertKeyword},
        {u"async", SyntaxKind::AsyncKeyword},
        {u"await", SyntaxKind::AwaitKeyword},
        {u"bigint", SyntaxKind::BigIntKeyword},
        {u"boolean", SyntaxKind::BooleanKeyword},
        {u"break", SyntaxKind::BreakKeyword},
        {u"case", SyntaxKind::CaseKeyword},
        {u"catch", SyntaxKind::CatchKeyword},
        {u"class", SyntaxKind::ClassKeyword},
        {u"const", SyntaxKind::ConstKeyword},
        {u"constructor", SyntaxKind::ConstructorKeyword},
        {u"continue", SyntaxKind::ContinueKeyword},
        {u"debugger", SyntaxKind::DebuggerKeyword},
        {u"declare", SyntaxKind::DeclareKeyword},
        {u"default", SyntaxKind::DefaultKeyword},
        {u"delete", SyntaxKind::DeleteKeyword},
        {u"do", SyntaxKind::DoKeyword},
        {u"else", SyntaxKind::ElseKeyword},
        {u"enum", SyntaxKind::EnumKeyword},
        {u"export", SyntaxKind::ExportKeyword},
        {u"extends", SyntaxKind::ExtendsKeyword},
        {u"false", SyntaxKind::FalseKeyword},
        {u"finally", SyntaxKind::FinallyKeyword},
        {u"for", SyntaxKind::ForKeyword},
        {u"from", SyntaxKind::FromKeyword},
        {u"function", SyntaxKind::FunctionKeyword},
        {u"get", SyntaxKind::GetKeyword},
        {u"global", SyntaxKind::GlobalKeyword},
        {u"if", SyntaxKind::IfKeyword},
        {u"implements", SyntaxKind::ImplementsKeyword},
        {u"import", SyntaxKind::ImportKeyword},
        {u"in", SyntaxKind::InKeyword},
        {u"infer", SyntaxKind::InferKeyword},
        {u"instanceof", SyntaxKind::InstanceOfKeyword},
        {u"interface", SyntaxKind::InterfaceKeyword},
        {u"intrinsic", SyntaxKind::IntrinsicKeyword},
        {u"is", SyntaxKind::IsKeyword},
        {u"keyof", SyntaxKind::KeyOfKeyword},
        {u"let", SyntaxKind::LetKeyword},
        {u"module", SyntaxKind::ModuleKeyword},
        {u"namespace", SyntaxKind::NamespaceKeyword},
        {u"never", SyntaxKind::NeverKeyword},
        {u"new", SyntaxKind::NewKeyword},
        {u"null", SyntaxKind::NullKeyword},
        {u"number", SyntaxKind::NumberKeyword},
        {u"object", SyntaxKind::ObjectKeyword},
        {u"of", SyntaxKind::OfKeyword},
        {u"out", SyntaxKind::OutKeyword},
        {u"override", SyntaxKind::OverrideKeyword},
        {u"package", SyntaxKind::PackageKeyword},
        {u"private", SyntaxKind::PrivateKeyword},
        {u"protected", SyntaxKind::ProtectedKeyword},
        {u"public", SyntaxKind::PublicKeyword},
        {u"readonly", SyntaxKind::ReadonlyKeyword},
        {u"require", SyntaxKind::RequireKeyword},
        {u"return", SyntaxKind::ReturnKeyword},
        {u"satisfies", SyntaxKind::SatisfiesKeyword},
        {u"set", SyntaxKind::SetKeyword},
        {u"static", SyntaxKind::StaticKeyword},
        {u"string", SyntaxKind::StringKeyword},
        {u"super", SyntaxKind::SuperKeyword},
        {u"switch", SyntaxKind::SwitchKeyword},
        {u"symbol", SyntaxKind::SymbolKeyword},
        {u"this", SyntaxKind::ThisKeyword},
        {u"throw", SyntaxKind::ThrowKeyword},
        {u"true", SyntaxKind::TrueKeyword},
        {u"try", SyntaxKind::TryKeyword},
        {u"type", SyntaxKind::TypeKeyword},
        {u"typeof", SyntaxKind::TypeOfKeyword},
        {u"undefined", SyntaxKind::UndefinedKeyword},
        {u"unique", SyntaxKind::UniqueKeyword},
        {u"unknown", SyntaxKind::UnknownKeyword},
        {u"using", SyntaxKind::UsingKeyword},
        {u"var", SyntaxKind::VarKeyword},
        {u"void", SyntaxKind::VoidKeyword},
        {u"while", SyntaxKind::WhileKeyword},
        {u"with", SyntaxKind::WithKeyword},
        {u"yield", SyntaxKind::YieldKeyword},
        {u"accessor", SyntaxKind::AccessorKeyword},
    };
    return map;
}

Scanner::Scanner(const exlib::wstring& text)
    : m_text(text)
    , m_pos(0)
    , m_startPos(0)
    , m_tokenStart(0)
    , m_token(SyntaxKind::Unknown)
    , m_hasLineBreak(false)
{
}

char16_t Scanner::charCodeAt(int pos) const {
    if (pos < 0 || pos >= (int)m_text.length()) {
        return 0;
    }
    return m_text[pos];
}

bool Scanner::isDigit(char16_t ch) const {
    return ch >= CharCode::_0 && ch <= CharCode::_9;
}

bool Scanner::isIdentifierStart(char16_t ch) const {
    return (ch >= CharCode::a && ch <= CharCode::z) ||
           (ch >= CharCode::A && ch <= CharCode::Z) ||
           ch == CharCode::_ || ch == CharCode::$ ||
           ch > 127; // Unicode
}

bool Scanner::isIdentifierPart(char16_t ch) const {
    return isIdentifierStart(ch) || isDigit(ch);
}

bool Scanner::isLineBreak(char16_t ch) const {
    return ch == CharCode::lineFeed || 
           ch == CharCode::carriageReturn ||
           ch == 0x2028 || ch == 0x2029;
}

bool Scanner::isWhiteSpace(char16_t ch) const {
    return ch == CharCode::space || 
           ch == CharCode::tab ||
           ch == CharCode::verticalTab ||
           ch == CharCode::formFeed ||
           ch == 0xA0 ||  // NBSP
           ch == 0xFEFF;  // BOM
}

void Scanner::skipTrivia() {
    while (m_pos < (int)m_text.length()) {
        char16_t ch = charCodeAt(m_pos);
        
        if (isWhiteSpace(ch)) {
            m_pos++;
        } else if (isLineBreak(ch)) {
            m_hasLineBreak = true;
            if (ch == CharCode::carriageReturn && charCodeAt(m_pos + 1) == CharCode::lineFeed) {
                m_pos += 2;
            } else {
                m_pos++;
            }
        } else if (ch == CharCode::slash) {
            char16_t next = charCodeAt(m_pos + 1);
            if (next == CharCode::slash) {
                // Single line comment - erase to spaces
                int commentStart = m_pos;
                m_pos += 2;
                while (m_pos < (int)m_text.length() && !isLineBreak(charCodeAt(m_pos))) {
                    m_pos++;
                }
                eraseToSpaces(commentStart, m_pos);
            } else if (next == CharCode::asterisk) {
                // Multi-line comment - erase to spaces
                int commentStart = m_pos;
                m_pos += 2;
                while (m_pos < (int)m_text.length()) {
                    if (charCodeAt(m_pos) == CharCode::asterisk && 
                        charCodeAt(m_pos + 1) == CharCode::slash) {
                        m_pos += 2;
                        break;
                    }
                    if (isLineBreak(charCodeAt(m_pos))) {
                        m_hasLineBreak = true;
                    }
                    m_pos++;
                }
                eraseToSpaces(commentStart, m_pos);
            } else {
                break;
            }
        } else {
            break;
        }
    }
}

void Scanner::eraseToSpaces(int start, int end) {
    for (int i = start; i < end && i < (int)m_text.length(); i++) {
        char16_t ch = m_text[i];
        // Preserve newlines for line number tracking
        if (ch != L'\n' && ch != L'\r') {
            m_text[i] = L' ';
        }
    }
}

exlib::wstring Scanner::getTokenText() const {
    return m_text.substr(m_tokenStart, m_pos - m_tokenStart);
}

void Scanner::setTextPos(int pos) {
    m_pos = pos;
    m_tokenStart = pos;
    m_token = SyntaxKind::Unknown;
    m_tokenValue.clear();
    m_hasLineBreak = false;
}

SyntaxKind Scanner::getIdentifierToken(const exlib::wstring& text) const {
    auto& map = getKeywordMap();
    auto it = map.find(text);
    if (it != map.end()) {
        return it->second;
    }
    return SyntaxKind::Identifier;
}

SyntaxKind Scanner::scanIdentifierOrKeyword() {
    int start = m_pos;
    while (m_pos < (int)m_text.length() && isIdentifierPart(charCodeAt(m_pos))) {
        m_pos++;
    }
    m_tokenValue = m_text.substr(start, m_pos - start);
    return getIdentifierToken(m_tokenValue);
}

SyntaxKind Scanner::scanNumber() {
    int start = m_pos;
    
    // Check for hex, binary, octal
    if (charCodeAt(m_pos) == CharCode::_0) {
        char16_t next = charCodeAt(m_pos + 1);
        if (next == L'x' || next == L'X') {
            m_pos += 2;
            while (m_pos < (int)m_text.length()) {
                char16_t ch = charCodeAt(m_pos);
                if (isDigit(ch) || (ch >= L'a' && ch <= L'f') || (ch >= L'A' && ch <= L'F')) {
                    m_pos++;
                } else {
                    break;
                }
            }
        } else if (next == L'b' || next == L'B') {
            m_pos += 2;
            while (m_pos < (int)m_text.length()) {
                char16_t ch = charCodeAt(m_pos);
                if (ch == L'0' || ch == L'1') {
                    m_pos++;
                } else {
                    break;
                }
            }
        } else if (next == L'o' || next == L'O') {
            m_pos += 2;
            while (m_pos < (int)m_text.length()) {
                char16_t ch = charCodeAt(m_pos);
                if (ch >= L'0' && ch <= L'7') {
                    m_pos++;
                } else {
                    break;
                }
            }
        }
    }
    
    // Decimal part
    while (m_pos < (int)m_text.length() && isDigit(charCodeAt(m_pos))) {
        m_pos++;
    }
    
    // Fractional part
    if (charCodeAt(m_pos) == CharCode::dot) {
        m_pos++;
        while (m_pos < (int)m_text.length() && isDigit(charCodeAt(m_pos))) {
            m_pos++;
        }
    }
    
    // Exponent part
    char16_t ch = charCodeAt(m_pos);
    if (ch == L'e' || ch == L'E') {
        m_pos++;
        ch = charCodeAt(m_pos);
        if (ch == CharCode::plus || ch == CharCode::minus) {
            m_pos++;
        }
        while (m_pos < (int)m_text.length() && isDigit(charCodeAt(m_pos))) {
            m_pos++;
        }
    }
    
    // BigInt suffix
    if (charCodeAt(m_pos) == L'n') {
        m_pos++;
        m_tokenValue = m_text.substr(start, m_pos - start);
        return SyntaxKind::BigIntLiteral;
    }
    
    m_tokenValue = m_text.substr(start, m_pos - start);
    return SyntaxKind::NumericLiteral;
}

SyntaxKind Scanner::scanString(char16_t quote) {
    m_pos++; // skip opening quote
    exlib::wstring result;
    int start = m_pos;
    
    while (m_pos < (int)m_text.length()) {
        char16_t ch = charCodeAt(m_pos);
        if (ch == quote) {
            result += m_text.substr(start, m_pos - start);
            m_pos++; // skip closing quote
            m_tokenValue = result;
            return SyntaxKind::StringLiteral;
        }
        if (ch == CharCode::backslash) {
            result += m_text.substr(start, m_pos - start);
            m_pos++;
            if (m_pos < (int)m_text.length()) {
                m_pos++;
            }
            start = m_pos;
            continue;
        }
        if (isLineBreak(ch)) {
            // Unterminated string - treat as end
            break;
        }
        m_pos++;
    }
    
    m_tokenValue = m_text.substr(start, m_pos - start);
    return SyntaxKind::StringLiteral;
}

SyntaxKind Scanner::scanTemplateOrTemplateTail() {
    m_pos++; // skip ` or }
    int start = m_pos;
    bool isHead = (charCodeAt(m_tokenStart) == CharCode::backtick);
    
    while (m_pos < (int)m_text.length()) {
        char16_t ch = charCodeAt(m_pos);
        if (ch == CharCode::backtick) {
            m_tokenValue = m_text.substr(start, m_pos - start);
            m_pos++;
            return isHead ? SyntaxKind::NoSubstitutionTemplateLiteral : SyntaxKind::TemplateTail;
        }
        if (ch == CharCode::$ && charCodeAt(m_pos + 1) == CharCode::openBrace) {
            m_tokenValue = m_text.substr(start, m_pos - start);
            m_pos += 2;
            return isHead ? SyntaxKind::TemplateHead : SyntaxKind::TemplateMiddle;
        }
        if (ch == CharCode::backslash) {
            m_pos += 2;
            continue;
        }
        m_pos++;
    }
    
    m_tokenValue = m_text.substr(start);
    return SyntaxKind::NoSubstitutionTemplateLiteral;
}

SyntaxKind Scanner::reScanTemplateToken() {
    // After a closing brace in a template literal, rescan as template middle or tail
    // Position should be right after the }
    m_tokenStart = m_pos - 1; // Set token start to the } position
    int start = m_pos;
    
    while (m_pos < (int)m_text.length()) {
        char16_t ch = charCodeAt(m_pos);
        if (ch == CharCode::backtick) {
            m_tokenValue = m_text.substr(start, m_pos - start);
            m_pos++;
            m_token = SyntaxKind::TemplateTail;
            return m_token;
        }
        if (ch == CharCode::$ && charCodeAt(m_pos + 1) == CharCode::openBrace) {
            m_tokenValue = m_text.substr(start, m_pos - start);
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
    
    m_tokenValue = m_text.substr(start);
    m_token = SyntaxKind::TemplateTail;
    return m_token;
}

SyntaxKind Scanner::scan() {
    m_startPos = m_pos;
    m_hasLineBreak = false;
    m_tokenValue.clear();

rescan:
    skipTrivia();
    m_tokenStart = m_pos;
    
    if (m_pos >= (int)m_text.length()) {
        m_token = SyntaxKind::EndOfFileToken;
        return m_token;
    }
    
    char16_t ch = charCodeAt(m_pos);
    
    // Identifier or keyword
    if (isIdentifierStart(ch)) {
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
                m_pos += 2;
                m_token = SyntaxKind::LessThanSlashToken;
                return m_token;
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
                if (m_pos >= (int)m_text.length()) {
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

std::vector<Token> Scanner::scanAllTokens() {
    std::vector<Token> tokens;
    setTextPos(0);
    
    // Track template literal nesting depth
    std::vector<int> templateDepthStack; // Stack of brace depths at template start
    int braceDepth = 0;
    
    while (true) {
        SyntaxKind kind = scan();
        
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
