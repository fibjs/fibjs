/**
 * @file scanner.h
 * @brief TypeScript lexical scanner (inspired by TypeRunner)
 * 
 * This scanner works directly on UTF-8 data for better performance.
 * Since TypeScript syntax (keywords, operators) is ASCII-only,
 * we can safely process UTF-8 byte by byte.
 */

#pragma once

#include "qstring.h"
#include <vector>
#include <functional>
#include <string_view>
#include "syntax_kind.h"

namespace fibjs {
namespace ts {

// Character codes for fast lookup (ASCII values)
namespace CharCode {
    constexpr uint8_t nullChar = 0;
    constexpr uint8_t lineFeed = 0x0A;       // \n
    constexpr uint8_t carriageReturn = 0x0D; // \r
    constexpr uint8_t space = 0x20;
    constexpr uint8_t tab = 0x09;
    constexpr uint8_t verticalTab = 0x0B;
    constexpr uint8_t formFeed = 0x0C;
    
    constexpr uint8_t _0 = 0x30;
    constexpr uint8_t _9 = 0x39;
    constexpr uint8_t a = 0x61;
    constexpr uint8_t z = 0x7A;
    constexpr uint8_t A = 0x41;
    constexpr uint8_t Z = 0x5A;
    constexpr uint8_t _ = 0x5F;
    constexpr uint8_t $ = 0x24;
    
    constexpr uint8_t openBrace = 0x7B;      // {
    constexpr uint8_t closeBrace = 0x7D;     // }
    constexpr uint8_t openParen = 0x28;      // (
    constexpr uint8_t closeParen = 0x29;     // )
    constexpr uint8_t openBracket = 0x5B;    // [
    constexpr uint8_t closeBracket = 0x5D;   // ]
    constexpr uint8_t dot = 0x2E;            // .
    constexpr uint8_t semicolon = 0x3B;      // ;
    constexpr uint8_t comma = 0x2C;          // ,
    constexpr uint8_t lessThan = 0x3C;       // <
    constexpr uint8_t greaterThan = 0x3E;    // >
    constexpr uint8_t equals = 0x3D;         // =
    constexpr uint8_t plus = 0x2B;           // +
    constexpr uint8_t minus = 0x2D;          // -
    constexpr uint8_t asterisk = 0x2A;       // *
    constexpr uint8_t slash = 0x2F;          // /
    constexpr uint8_t percent = 0x25;        // %
    constexpr uint8_t ampersand = 0x26;      // &
    constexpr uint8_t bar = 0x7C;            // |
    constexpr uint8_t caret = 0x5E;          // ^
    constexpr uint8_t exclamation = 0x21;    // !
    constexpr uint8_t tilde = 0x7E;          // ~
    constexpr uint8_t question = 0x3F;       // ?
    constexpr uint8_t colon = 0x3A;          // :
    constexpr uint8_t at = 0x40;             // @
    constexpr uint8_t hash = 0x23;           // #
    constexpr uint8_t backtick = 0x60;       // \`
    constexpr uint8_t backslash = 0x5C;      // backslash
    constexpr uint8_t doubleQuote = 0x22;    // "
    constexpr uint8_t singleQuote = 0x27;    // '
}

struct Token {
    SyntaxKind kind;
    int pos;           // start position (0-based byte offset)
    int end;           // end position (exclusive byte offset)
    bool hadLineBreak; // was there a line break before this token?
    
    Token() : kind(SyntaxKind::Unknown), pos(0), end(0), hadLineBreak(false) {}
    Token(SyntaxKind k, int p, int e, bool lb = false)
        : kind(k), pos(p), end(e), hadLineBreak(lb) {}
};

class Scanner {
public:
    // Constructor takes pointer and length to avoid copy
    Scanner(uint8_t* text, size_t length);
    
    // Scan next token
    SyntaxKind scan();
    
    // Current token info
    SyntaxKind getToken() const { return m_token; }
    int getTokenStart() const { return m_tokenStart; }
    int getTokenEnd() const { return m_pos; }
    std::string_view getTokenText() const;
    std::string_view getTokenValue() const { return m_tokenValue; }
    bool hasPrecedingLineBreak() const { return m_hasLineBreak; }
    
    // Position
    int getPos() const { return m_pos; }
    int getTextLength() const { return (int)m_length; }
    
    // Lookahead support
    int getStartPos() const { return m_startPos; }
    void setTextPos(int pos);
    
    // Get all tokens (for strip mode)
    std::vector<Token> scanAllTokens();
    
    // Rescan template token after closing brace
    SyntaxKind reScanTemplateToken();
    
    // Rescan slash token as regular expression literal
    SyntaxKind reScanSlashToken();
    
private:
    uint8_t charCodeAt(int pos) const;
    bool isDigit(uint8_t ch) const;
    bool isIdentifierStart(uint8_t ch) const;
    bool isIdentifierPart(uint8_t ch) const;
    bool isLineBreak(uint8_t ch) const;
    bool isWhiteSpace(uint8_t ch) const;
    
    void skipTrivia();
    SyntaxKind scanIdentifierOrKeyword();
    SyntaxKind scanNumber();
    SyntaxKind scanString(uint8_t quote);
    SyntaxKind scanTemplateOrTemplateTail();
    void scanRegExpFlags();
    
    SyntaxKind getIdentifierToken(std::string_view text) const;
    
private:
    uint8_t* m_text;    // direct pointer to buffer data
    size_t m_length;
    int m_pos;
    int m_startPos;
    int m_tokenStart;
    SyntaxKind m_token;
    std::string_view m_tokenValue;
    bool m_hasLineBreak;
};

} // namespace ts
} // namespace fibjs
