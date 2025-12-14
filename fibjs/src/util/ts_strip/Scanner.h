/**
 * @file scanner.h
 * @brief TypeScript lexical scanner (inspired by TypeRunner)
 */

#pragma once

#include "qstring.h"
#include <vector>
#include <functional>
#include "syntax_kind.h"

namespace fibjs {
namespace ts {

// Character codes for fast lookup
namespace CharCode {
    constexpr char16_t nullChar = 0;
    constexpr char16_t lineFeed = 0x0A;       // \n
    constexpr char16_t carriageReturn = 0x0D; // \r
    constexpr char16_t space = 0x20;
    constexpr char16_t tab = 0x09;
    constexpr char16_t verticalTab = 0x0B;
    constexpr char16_t formFeed = 0x0C;
    
    constexpr char16_t _0 = 0x30;
    constexpr char16_t _9 = 0x39;
    constexpr char16_t a = 0x61;
    constexpr char16_t z = 0x7A;
    constexpr char16_t A = 0x41;
    constexpr char16_t Z = 0x5A;
    constexpr char16_t _ = 0x5F;
    constexpr char16_t $ = 0x24;
    
    constexpr char16_t openBrace = 0x7B;      // {
    constexpr char16_t closeBrace = 0x7D;     // }
    constexpr char16_t openParen = 0x28;      // (
    constexpr char16_t closeParen = 0x29;     // )
    constexpr char16_t openBracket = 0x5B;    // [
    constexpr char16_t closeBracket = 0x5D;   // ]
    constexpr char16_t dot = 0x2E;            // .
    constexpr char16_t semicolon = 0x3B;      // ;
    constexpr char16_t comma = 0x2C;          // ,
    constexpr char16_t lessThan = 0x3C;       // <
    constexpr char16_t greaterThan = 0x3E;    // >
    constexpr char16_t equals = 0x3D;         // =
    constexpr char16_t plus = 0x2B;           // +
    constexpr char16_t minus = 0x2D;          // -
    constexpr char16_t asterisk = 0x2A;       // *
    constexpr char16_t slash = 0x2F;          // /
    constexpr char16_t percent = 0x25;        // %
    constexpr char16_t ampersand = 0x26;      // &
    constexpr char16_t bar = 0x7C;            // |
    constexpr char16_t caret = 0x5E;          // ^
    constexpr char16_t exclamation = 0x21;    // !
    constexpr char16_t tilde = 0x7E;          // ~
    constexpr char16_t question = 0x3F;       // ?
    constexpr char16_t colon = 0x3A;          // :
    constexpr char16_t at = 0x40;             // @
    constexpr char16_t hash = 0x23;           // #
    constexpr char16_t backtick = 0x60;       // `
    constexpr char16_t backslash = 0x5C;      // 
    constexpr char16_t doubleQuote = 0x22;    // "
    constexpr char16_t singleQuote = 0x27;    // '
}

struct Token {
    SyntaxKind kind;
    int pos;           // start position (0-based)
    int end;           // end position (exclusive)
    bool hadLineBreak; // was there a line break before this token?
    
    Token() : kind(SyntaxKind::Unknown), pos(0), end(0), hadLineBreak(false) {}
    Token(SyntaxKind k, int p, int e, bool lb = false)
        : kind(k), pos(p), end(e), hadLineBreak(lb) {}
};

class Scanner {
public:
    Scanner(const exlib::wstring& text);
    
    // Scan next token
    SyntaxKind scan();
    
    // Current token info
    SyntaxKind getToken() const { return m_token; }
    int getTokenStart() const { return m_tokenStart; }
    int getTokenEnd() const { return m_pos; }
    exlib::wstring getTokenText() const;
    exlib::wstring getTokenValue() const { return m_tokenValue; }
    bool hasPrecedingLineBreak() const { return m_hasLineBreak; }
    
    // Position
    int getPos() const { return m_pos; }
    int getTextLength() const { return (int)m_text.length(); }
    
    // Lookahead support
    int getStartPos() const { return m_startPos; }
    void setTextPos(int pos);
    
    // Get all tokens (for strip mode)
    std::vector<Token> scanAllTokens();
    
    // Get processed text (comments erased to spaces)
    const exlib::wstring& getProcessedText() const { return m_text; }
    
    // Rescan template token after closing brace
    SyntaxKind reScanTemplateToken();
    
    // Rescan slash token as regular expression literal
    SyntaxKind reScanSlashToken();
    
private:
    char16_t charCodeAt(int pos) const;
    bool isDigit(char16_t ch) const;
    bool isIdentifierStart(char16_t ch) const;
    bool isIdentifierPart(char16_t ch) const;
    bool isLineBreak(char16_t ch) const;
    bool isWhiteSpace(char16_t ch) const;
    
    void skipTrivia();
    SyntaxKind scanIdentifierOrKeyword();
    SyntaxKind scanNumber();
    SyntaxKind scanString(char16_t quote);
    SyntaxKind scanTemplateOrTemplateTail();
    void scanRegExpFlags();
    
    SyntaxKind getIdentifierToken(const exlib::wstring& text) const;
    
private:
    exlib::wstring m_text;  // mutable copy, comments will be erased to spaces
    int m_pos;
    int m_startPos;
    int m_tokenStart;
    SyntaxKind m_token;
    exlib::wstring m_tokenValue;
    bool m_hasLineBreak;
    
    // Erase range to spaces (preserving newlines)
    void eraseToSpaces(int start, int end);
};

} // namespace ts
} // namespace fibjs
