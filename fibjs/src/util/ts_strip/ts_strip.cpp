/**
 * @file ts_strip.cpp
 * @brief TypeScript type stripping implementation
 * 
 * This implementation strictly follows TypeRunner's parser logic.
 * Key functions are ported from TypeRunner/src/parser2.h
 */

#include "ts_strip.h"
#include "Scanner.h"
#include "utf8.h"
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <iostream>

// Debug mode: disable all replacements, just parse
#define TS_STRIP_DEBUG_PARSE 0

namespace fibjs {
namespace ts_strip {

using namespace ts;

/**
 * Operator precedence levels (from TypeRunner/src/utilities.h)
 */
enum class OperatorPrecedence {
    Comma,
    Spread,
    Yield,
    Assignment,
    Conditional,
    Coalesce = Conditional,
    LogicalOR,
    LogicalAND,
    BitwiseOR,
    BitwiseXOR,
    BitwiseAND,
    Equality,
    Relational,
    Shift,
    Additive,
    Multiplicative,
    Exponentiation,
    Unary,
    Update,
    LeftHandSide,
    Member,
    Primary,
    Highest = Primary,
    Lowest = Comma,
    Invalid = -1,
};

/**
 * Get binary operator precedence (from TypeRunner/src/utilities.cpp)
 */
int getBinaryOperatorPrecedence(SyntaxKind kind) {
    switch (kind) {
        case SyntaxKind::QuestionQuestionToken: return (int)OperatorPrecedence::Coalesce;
        case SyntaxKind::BarBarToken: return (int)OperatorPrecedence::LogicalOR;
        case SyntaxKind::AmpersandAmpersandToken: return (int)OperatorPrecedence::LogicalAND;
        case SyntaxKind::BarToken: return (int)OperatorPrecedence::BitwiseOR;
        case SyntaxKind::CaretToken: return (int)OperatorPrecedence::BitwiseXOR;
        case SyntaxKind::AmpersandToken: return (int)OperatorPrecedence::BitwiseAND;
        case SyntaxKind::EqualsEqualsToken:
        case SyntaxKind::ExclamationEqualsToken:
        case SyntaxKind::EqualsEqualsEqualsToken:
        case SyntaxKind::ExclamationEqualsEqualsToken: return (int)OperatorPrecedence::Equality;
        case SyntaxKind::LessThanToken:
        case SyntaxKind::GreaterThanToken:
        case SyntaxKind::LessThanEqualsToken:
        case SyntaxKind::GreaterThanEqualsToken:
        case SyntaxKind::InstanceOfKeyword:
        case SyntaxKind::InKeyword:
        case SyntaxKind::AsKeyword:
        case SyntaxKind::SatisfiesKeyword: return (int)OperatorPrecedence::Relational;
        case SyntaxKind::LessThanLessThanToken:
        case SyntaxKind::GreaterThanGreaterThanToken:
        case SyntaxKind::GreaterThanGreaterThanGreaterThanToken: return (int)OperatorPrecedence::Shift;
        case SyntaxKind::PlusToken:
        case SyntaxKind::MinusToken: return (int)OperatorPrecedence::Additive;
        case SyntaxKind::AsteriskToken:
        case SyntaxKind::SlashToken:
        case SyntaxKind::PercentToken: return (int)OperatorPrecedence::Multiplicative;
        case SyntaxKind::AsteriskAsteriskToken: return (int)OperatorPrecedence::Exponentiation;
        default: return -1;
    }
}

// Span to be replaced with spaces
struct Replacement {
    int start;
    int end;
    Replacement(int s, int e) : start(s), end(e) {}
};

// Single byte overwrite
struct Overwrite {
    int pos;
    char16_t value;
    Overwrite(int p, char16_t v) : pos(p), value(v) {}
};

/**
 * TypeScript stripper - follows TypeRunner's parser logic
 */
class TsStrip {
public:
    static constexpr int MAX_RECURSION_DEPTH = 500;

    TsStrip(const exlib::wstring& src, std::vector<Token> tokens)
        : m_src(src)
        , m_tokens(std::move(tokens))
        , m_tokenIndex(0)
        , m_disallowInContext(false)
        , m_recursionDepth(0)
    {
    }
    
    exlib::wstring strip();
    
private:
    const exlib::wstring& m_src;
    std::vector<Token> m_tokens;
    size_t m_tokenIndex;
    bool m_disallowInContext;
    int m_recursionDepth;
    
    std::vector<Replacement> m_replacements;
    std::vector<Overwrite> m_overwrites;

    void checkRecursionDepth() {
        if (m_recursionDepth > MAX_RECURSION_DEPTH) {
            throw std::runtime_error("Maximum recursion depth exceeded");
        }
    }

    // ========== Token access (basic) ==========
    const Token& currentToken() const {
        if (m_tokenIndex >= m_tokens.size()) {
            return m_tokens.back();
        }
        return m_tokens[m_tokenIndex];
    }
    
    const Token& nextToken() {
        const int prevEnd = currentToken().end;
        if (m_tokenIndex < m_tokens.size() - 1) {
            m_tokenIndex++;
            // If the previous token was logically "rescanned" into a larger token span
            // (e.g. /.../g as RegularExpressionLiteral), skip any pre-scanned tokens
            // that fall within that merged span.
            while (m_tokenIndex < m_tokens.size() - 1 && m_tokens[m_tokenIndex].pos < prevEnd) {
                m_tokenIndex++;
            }
        }
        return currentToken();
    }

    bool reScanSlashTokenAsRegularExpressionLiteral() {
        SyntaxKind k = token();
        if (k != SyntaxKind::SlashToken && k != SyntaxKind::SlashEqualsToken) {
            return false;
        }

        Token& t = m_tokens[m_tokenIndex];
        const int start = t.pos;
        int p = start + 1;
        bool inEscape = false;
        bool inCharacterClass = false;

        const int end = (int)m_src.size();
        while (true) {
            if (p >= end) {
                return false;
            }

            const char16_t ch = m_src[p];
            if (ch == L'\n' || ch == L'\r') {
                return false;
            }

            if (inEscape) {
                inEscape = false;
            } else if (ch == L'/' && !inCharacterClass) {
                p++;
                break;
            } else if (ch == L'[') {
                inCharacterClass = true;
            } else if (ch == L'\\') {
                inEscape = true;
            } else if (ch == L']') {
                inCharacterClass = false;
            }

            p++;
        }

        // Scan flags (ASCII identifier parts are enough for regexp flags)
        while (p < end) {
            const char16_t ch = m_src[p];
            if ((ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z')) {
                p++;
                continue;
            }
            break;
        }

        t.kind = SyntaxKind::RegularExpressionLiteral;
        t.end = p;
        return true;
    }
    
    const Token& peekToken(int offset = 1) const {
        size_t idx = m_tokenIndex + offset;
        if (idx >= m_tokens.size()) {
            return m_tokens.back();
        }
        return m_tokens[idx];
    }
    
    SyntaxKind token() const { return currentToken().kind; }
    int getNodePos() const { return currentToken().pos; }
    int getNodeEnd() const { return currentToken().end; }
    int getPrevTokenEnd() const {
        // Get end position of previous token
        if (m_tokenIndex > 0) {
            return m_tokens[m_tokenIndex - 1].end;
        }
        return 0;
    }
    bool isEOF() const { return token() == SyntaxKind::EndOfFileToken; }
    
    // ========== Replacement recording ==========
    void addReplacement(int start, int end) {
#if !TS_STRIP_DEBUG_PARSE
        if (start < end) {
            m_replacements.push_back(Replacement(start, end));
        }
#endif
    }
    
    void addOverwrite(int pos, char16_t value) {
#if !TS_STRIP_DEBUG_PARSE
        m_overwrites.push_back(Overwrite(pos, value));
#endif
    }

    // ========== Core parsing helpers (from TypeRunner) ==========
    
    /**
     * canParseSemicolon - from TypeRunner parser2.h line 1544
     */
    bool canParseSemicolon() const {
        // If there's a real semicolon, then we can always parse it out.
        if (token() == SyntaxKind::SemicolonToken) {
            return true;
        }
        // We can parse out an optional semicolon in ASI cases in the following cases.
        return token() == SyntaxKind::CloseBraceToken 
            || token() == SyntaxKind::EndOfFileToken 
            || currentToken().hadLineBreak;
    }
    
    /**
     * tryParseSemicolon - from TypeRunner parser2.h line 1554
     */
    bool tryParseSemicolon() {
        if (!canParseSemicolon()) {
            return false;
        }
        if (token() == SyntaxKind::SemicolonToken) {
            // consume the semicolon if it was explicitly provided.
            nextToken();
        }
        return true;
    }
    
    bool parseOptional(SyntaxKind kind) {
        if (token() == kind) {
            nextToken();
            return true;
        }
        return false;
    }
    
    bool parseExpected(SyntaxKind kind) {
        if (token() == kind) {
            nextToken();
            return true;
        }
        return false;
    }

    bool isIdentifier() const {
        return token() == SyntaxKind::Identifier;
    }
    
    bool isKeyword(SyntaxKind t) const {
        // Keep this consistent with SyntaxKind::FirstKeyword..LastKeyword.
        // We intentionally delegate to the helper in syntax_kind.h to avoid
        // range drift when new keywords are added (e.g. AccessorKeyword).
        return ts::isKeyword(t);
    }
    
    // Check if token can be used as binding identifier (identifier or contextual keyword)
    bool isBindingIdentifier() const {
        if (token() == SyntaxKind::Identifier) {
            return true;
        }
        // Contextual keywords (after LastReservedWord) can be used as identifiers
        // LastReservedWord is WithKeyword
        return token() > SyntaxKind::WithKeyword && isKeyword(token());
    }
    
    bool inDisallowInContext() const {
        return m_disallowInContext;
    }

    // ========== Forward declarations ==========
    void parseSourceFile();
    void parseStatement();
    void parseExpressionOrLabeledStatement();
    void parseExpression();
    void parseAssignmentExpressionOrHigher();
    void parseBinaryExpressionOrHigher(int precedence);
    void parseBinaryExpressionRest(int precedence);
    void parseUnaryExpressionOrHigher();
    void parseUpdateExpression();
    void parseLeftHandSideExpressionOrHigher();
    void parseMemberExpressionOrHigher();
    void parsePrimaryExpression();
    void parseCallExpressionRest();
    bool trySkipTypeArgumentsAndCall();
    
    // Type stripping
    void skipType();
    void skipTypeArguments();
    void skipBalanced(SyntaxKind open, SyntaxKind close);
    void skipBlock() { skipBalanced(SyntaxKind::OpenBraceToken, SyntaxKind::CloseBraceToken); }
    void skipJsxElement();
    
    // Declarations
    void parseVariableStatement();
    void parseVariableDeclarationList();
    void parseVariableDeclaration();
    void parseObjectBindingPattern();
    void parseArrayBindingPattern();
    void parseFunctionDeclaration(int outerStart = -1);
    void parseClassDeclaration();
    void parseInterfaceDeclaration(int start);
    void parseTypeAliasDeclaration(int start);
    void parseEnumDeclaration(int start, bool isDeclare = false);
    void parseModuleDeclaration(int start, bool isDeclare = false);
    void parseImportDeclaration();
    void parseExportDeclaration();
    void parseDeclaration();
    
    // Class
    void parseClassBody();
    void parseClassMember();
    
    // Function
    void parseParameters();
    
    // Control flow
    void parseBlock();
    void parseForStatement();
    void parseSwitchStatement();
    void parseTryStatement();
    void parseTemplateLiteral();

    
    // Helpers
    bool isStartOfExpression() const;
    bool isStartOfStatement() const;
    bool isStartOfDeclaration() const;
    bool isLetDeclaration() const;
    
    // ASI fix
    void fixASI(int start, int end, bool isStatement = true);
    
    // Apply
    exlib::wstring applyReplacements();
};

// ========================================================================
// Implementation - Expression parsing (from TypeRunner parser2.h)
// ========================================================================

/**
 * parseExpression - from TypeRunner parser2.h line 6314
 * Expression[in]:
 *     AssignmentExpression[in]
 *     Expression[in] , AssignmentExpression[in]
 */
void TsStrip::parseExpression() {
    parseAssignmentExpressionOrHigher();
    while (token() == SyntaxKind::CommaToken) {
        nextToken();
        parseAssignmentExpressionOrHigher();
    }
}

/**
 * parseAssignmentExpressionOrHigher - from TypeRunner parser2.h line 6778
 * For strip purposes, we just need to parse the expression structure
 */
void TsStrip::parseAssignmentExpressionOrHigher() {
    m_recursionDepth++;
    checkRecursionDepth();
    
    // Parse binary expression first
    parseBinaryExpressionOrHigher((int)OperatorPrecedence::Lowest);

    // Handle conditional expression (a ? b : c)
    if (token() == SyntaxKind::QuestionToken) {
        nextToken();
        parseAssignmentExpressionOrHigher();
        parseExpected(SyntaxKind::ColonToken);
        parseAssignmentExpressionOrHigher();
    }
    
    // TypeRunner line 6823-6825: Handle simple arrow function (x => ...)
    // If we just parsed an identifier and see =>, it's a simple arrow function
    if (token() == SyntaxKind::EqualsGreaterThanToken) {
        nextToken(); // consume '=>'
        if (token() == SyntaxKind::OpenBraceToken) {
            parseBlock();
        } else {
            parseAssignmentExpressionOrHigher();
        }
        m_recursionDepth--;
        return;
    }
    
    // Handle assignment operators
    switch (token()) {
        case SyntaxKind::EqualsToken:
        case SyntaxKind::PlusEqualsToken:
        case SyntaxKind::MinusEqualsToken:
        case SyntaxKind::AsteriskEqualsToken:
        case SyntaxKind::AsteriskAsteriskEqualsToken:
        case SyntaxKind::SlashEqualsToken:
        case SyntaxKind::PercentEqualsToken:
        case SyntaxKind::LessThanLessThanEqualsToken:
        case SyntaxKind::GreaterThanGreaterThanEqualsToken:
        case SyntaxKind::GreaterThanGreaterThanGreaterThanEqualsToken:
        case SyntaxKind::AmpersandEqualsToken:
        case SyntaxKind::BarEqualsToken:
        case SyntaxKind::CaretEqualsToken:
        case SyntaxKind::BarBarEqualsToken:
        case SyntaxKind::AmpersandAmpersandEqualsToken:
        case SyntaxKind::QuestionQuestionEqualsToken:
            nextToken();
            parseAssignmentExpressionOrHigher();
            break;
        default:
            break;
    }
    
    m_recursionDepth--;
}

/**
 * parseBinaryExpressionOrHigher - from TypeRunner parser2.h line 5455
 */
void TsStrip::parseBinaryExpressionOrHigher(int precedence) {
    parseUnaryExpressionOrHigher();
    parseBinaryExpressionRest(precedence);
}

/**
 * parseBinaryExpressionRest - from TypeRunner parser2.h line 5462
 * This is the KEY function that handles `as` keyword!
 */
void TsStrip::parseBinaryExpressionRest(int precedence) {
    while (true) {
        int newPrecedence = getBinaryOperatorPrecedence(token());
        
        // Check precedence for left/right associativity
        bool consumeCurrentOperator = token() == SyntaxKind::AsteriskAsteriskToken
            ? newPrecedence >= precedence
            : newPrecedence > precedence;
        
        if (!consumeCurrentOperator) {
            break;
        }
        
        if (token() == SyntaxKind::InKeyword && inDisallowInContext()) {
            break;
        }
        
        if (token() == SyntaxKind::AsKeyword) {
            // TypeRunner parser2.h line 5503-5515
            // Make sure we *do* perform ASI for constructs like this:
            //    var x = foo
            //    as (Bar)
            // This should be parsed as an initialized variable, followed
            // by a function call to 'as' with the argument 'Bar'
            if (currentToken().hadLineBreak) {
                break;
            } else {
                int asStart = getNodePos();
                nextToken(); // consume 'as'
                
                // Handle 'as const'
                if (token() == SyntaxKind::ConstKeyword) {
                    nextToken();
                } else {
                    skipType();
                }
                int asEnd = getNodePos();
                addReplacement(asStart, asEnd);
                
                // Check if we need a semicolon for ASI safety
                // e.g., const x = 10 as any\n(1) should become const x = 10 ;\n(1)
                if (currentToken().hadLineBreak) {
                    fixASI(asStart, asEnd, false);  // Expression-level
                }
            }
        } else if (token() == SyntaxKind::SatisfiesKeyword) {
            // Handle 'satisfies Type' similarly
            if (currentToken().hadLineBreak) {
                break;
            } else {
                int satStart = getNodePos();
                nextToken();
                skipType();
                int satEnd = getNodePos();
                addReplacement(satStart, satEnd);
                
                // Check if we need a semicolon for ASI safety
                if (currentToken().hadLineBreak) {
                    fixASI(satStart, satEnd, false);  // Expression-level
                }
            }
        } else {
            // Normal binary operator
            nextToken();
            parseBinaryExpressionOrHigher(newPrecedence);
        }
    }
}

/**
 * parseUnaryExpressionOrHigher - from TypeRunner
 * 
 * Handles unary operators and <Type>expr type assertions.
 * Note: JSX is NOT handled here - we're in strip-only mode for .ts files.
 */
void TsStrip::parseUnaryExpressionOrHigher() {
    switch (token()) {
        case SyntaxKind::PlusToken:
        case SyntaxKind::MinusToken:
        case SyntaxKind::TildeToken:
        case SyntaxKind::ExclamationToken:
            nextToken();
            parseUnaryExpressionOrHigher();
            return;
        case SyntaxKind::DeleteKeyword:
        case SyntaxKind::TypeOfKeyword:
        case SyntaxKind::VoidKeyword:
            nextToken();
            parseUnaryExpressionOrHigher();
            return;
        case SyntaxKind::LessThanToken:
            // Could be <Type>expr type assertion - try to parse it
            // This is the old-style type assertion syntax
            // Note: we don't try to parse JSX here since we can't reliably
            // distinguish it from comparison operators in strip-only mode
            {
                // Look ahead to see if this could be a type assertion
                SyntaxKind next = peekToken().kind;
                // Type assertions typically start with: <TypeName>, <typeof ...>, etc.
                // But NOT: <123 (number), <= (operator), << (shift)
                if (next == SyntaxKind::Identifier || isKeyword(next) ||
                    next == SyntaxKind::OpenParenToken || next == SyntaxKind::OpenBraceToken ||
                    next == SyntaxKind::OpenBracketToken) {
                    
                    // Save position for potential rollback
                    size_t startIndex = m_tokenIndex;
                    int start = getNodePos();
                    
                    nextToken(); // consume <
                    skipType();
                    
                    if (token() == SyntaxKind::GreaterThanToken) {
                        // Successfully parsed <Type> - this is a type assertion
                        nextToken(); // consume >
                        addReplacement(start, getNodePos());
                        parseUnaryExpressionOrHigher();
                        return;
                    } else {
                        // Not a valid type assertion - rollback
                        // This happens when we have something like:
                        // `i < n` where `n` looks like it could start a type
                        m_tokenIndex = startIndex;
                        // Fall through to parseUpdateExpression
                    }
                }
            }
            break;
        default:
            break;
    }
    parseUpdateExpression();
}

/**
 * parseUpdateExpression - from TypeRunner parser2.h line 5526
 * 
 * Note: JSX detection is ONLY done when languageVariant == JSX in TypeRunner.
 * In strip-only mode for .ts files, we should NOT try to parse JSX here
 * because `< identifier` would conflict with comparison operators.
 * 
 * For now, we disable JSX detection entirely. If JSX support is needed,
 * it should be done in parsePrimaryExpression with proper lookahead to
 * distinguish JSX from comparison operators.
 */
void TsStrip::parseUpdateExpression() {
    if (token() == SyntaxKind::PlusPlusToken || token() == SyntaxKind::MinusMinusToken) {
        nextToken();
        parseLeftHandSideExpressionOrHigher();
        return;
    }
    
    // Note: TypeRunner checks languageVariant == JSX before treating < as JSX
    // Since we're in strip-only mode and can't reliably distinguish JSX from
    // comparison operators, we skip JSX detection here.
    // The binary expression parser will handle < as comparison operator.
    
    parseLeftHandSideExpressionOrHigher();
    
    // Postfix ++/--
    if ((token() == SyntaxKind::PlusPlusToken || token() == SyntaxKind::MinusMinusToken) 
        && !currentToken().hadLineBreak) {
        nextToken();
    }
}

/**
 * parseLeftHandSideExpressionOrHigher
 */
void TsStrip::parseLeftHandSideExpressionOrHigher() {
    parseMemberExpressionOrHigher();
    parseCallExpressionRest();
}

/**
 * parseMemberExpressionOrHigher
 */
void TsStrip::parseMemberExpressionOrHigher() {
    parsePrimaryExpression();
    
    while (true) {
        switch (token()) {
            case SyntaxKind::DotToken:
                nextToken();
                if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                    nextToken();
                }
                continue;
            case SyntaxKind::ExclamationToken:
                // Non-null assertion `expr!` - remove if no line break
                if (!currentToken().hadLineBreak) {
                    int start = getNodePos();
                    nextToken();
                    addReplacement(start, getNodePos());
                    continue;
                }
                return;
            case SyntaxKind::QuestionDotToken:
                nextToken();
                if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                    nextToken();
                }
                continue;
            case SyntaxKind::OpenBracketToken:
                nextToken();
                parseExpression();
                parseExpected(SyntaxKind::CloseBracketToken);
                continue;
            case SyntaxKind::NoSubstitutionTemplateLiteral:
            case SyntaxKind::TemplateHead:
                // Tagged template
                parseTemplateLiteral();
                continue;
            default:
                return;
        }
    }
}

/**
 * trySkipTypeArgumentsAndCall - Try to parse <TypeArgs>( pattern
 * Returns true if successfully parsed type arguments followed by (, false otherwise.
 * On false, caller should treat < as comparison operator.
 * 
 * This is a conservative approach: only treat <...> as type arguments when
 * IMMEDIATELY followed by ( (function call).
 */
bool TsStrip::trySkipTypeArgumentsAndCall() {
    if (token() != SyntaxKind::LessThanToken) {
        return false;
    }
    
    // Save position for potential rollback
    size_t startIndex = m_tokenIndex;
    int start = getNodePos();
    
    // Quick check: next token should look like start of a type
    SyntaxKind next = peekToken().kind;
    if (next != SyntaxKind::Identifier && 
        next != SyntaxKind::GreaterThanToken &&
        next != SyntaxKind::OpenParenToken &&
        next != SyntaxKind::OpenBracketToken &&
        next != SyntaxKind::OpenBraceToken &&
        next != SyntaxKind::KeyOfKeyword &&
        next != SyntaxKind::TypeOfKeyword &&
        next != SyntaxKind::ReadonlyKeyword &&
        next != SyntaxKind::UniqueKeyword &&
        next != SyntaxKind::InferKeyword &&
        !isKeyword(next)) {
        return false;
    }
    
    // Try to skip the type argument list
    int depth = 1;
    nextToken(); // consume <
    
    while (!isEOF() && depth > 0) {
        SyntaxKind t = token();
        
        // Check for tokens that can't appear in type arguments
        // These indicate we've gone past a valid type argument list
        if (t == SyntaxKind::SemicolonToken ||
            t == SyntaxKind::EndOfFileToken) {
            // Definitely not type arguments - rollback
            m_tokenIndex = startIndex;
            return false;
        }
        
        switch (t) {
            case SyntaxKind::LessThanToken:
                depth++;
                nextToken();
                break;
            case SyntaxKind::GreaterThanToken:
                depth--;
                nextToken();
                break;
            case SyntaxKind::GreaterThanGreaterThanToken:
                depth -= 2;
                if (depth < 0) depth = 0;
                nextToken();
                break;
            case SyntaxKind::GreaterThanGreaterThanGreaterThanToken:
                depth -= 3;
                if (depth < 0) depth = 0;
                nextToken();
                break;
            case SyntaxKind::OpenParenToken:
                // ( inside type args - like arrow function type: <T>(x: T) => T
                skipBalanced(SyntaxKind::OpenParenToken, SyntaxKind::CloseParenToken);
                break;
            case SyntaxKind::OpenBraceToken:
                skipBalanced(SyntaxKind::OpenBraceToken, SyntaxKind::CloseBraceToken);
                break;
            case SyntaxKind::OpenBracketToken:
                skipBalanced(SyntaxKind::OpenBracketToken, SyntaxKind::CloseBracketToken);
                break;
            default:
                nextToken();
                break;
        }
    }
    
    // CRITICAL: If depth > 0, we didn't find the closing >, so this is NOT type arguments
    // This handles cases like `i < n` where `n` is followed by EOF or other non-type tokens
    if (depth > 0) {
        m_tokenIndex = startIndex;
        return false;
    }
    
    // After closing >, check what follows
    // Type arguments are valid if followed by:
    // 1. ( - function call: func<T>()
    // 2. Template literal - tagged template: func<T>`str`
    // 3. Expression terminators - instantiation expression: func<T>; func<T>, etc.
    if (token() == SyntaxKind::OpenParenToken) {
        // func<T>() - this is definitely type arguments for function call
        addReplacement(start, getNodePos());
        return true;
    } else if (token() == SyntaxKind::NoSubstitutionTemplateLiteral ||
               token() == SyntaxKind::TemplateHead) {
        // func<T>`template` - tagged template with type args
        addReplacement(start, getNodePos());
        return true;
    } else if (token() == SyntaxKind::SemicolonToken ||
               token() == SyntaxKind::CommaToken ||
               token() == SyntaxKind::CloseParenToken ||
               token() == SyntaxKind::CloseBraceToken ||
               token() == SyntaxKind::CloseBracketToken ||
               token() == SyntaxKind::EndOfFileToken ||
               currentToken().hadLineBreak) {
        // Instantiation expression: func<T>; or func<T>, or func<T>) etc.
        // These are valid TypeScript 4.7+ instantiation expressions
        addReplacement(start, getNodePos());
        return true;
    } else {
        // Followed by something that continues expression (like .)
        // This was a comparison, not type args
        // Rollback and treat as comparison
        m_tokenIndex = startIndex;
        return false;
    }
}

/**
 * parseCallExpressionRest - handle function calls and type arguments
 * 
 * TypeScript type arguments in expressions (like func<T>()) need careful handling
 * to distinguish from comparison operators (like i < n).
 * 
 * Conservative approach: only treat <...> as type arguments when followed by (
 */
void TsStrip::parseCallExpressionRest() {
    while (true) {
        size_t savedIndex = m_tokenIndex; // Track position to detect no progress
        
        switch (token()) {
            case SyntaxKind::LessThanToken:
                // Could be type arguments like func<T>() or comparison operator x < y
                // Use conservative approach: only treat as type args if followed by (
                if (trySkipTypeArgumentsAndCall()) {
                    // Successfully parsed <TypeArgs>( - continue to parse call arguments
                    if (token() == SyntaxKind::OpenParenToken) {
                        nextToken();
                        while (!isEOF() && token() != SyntaxKind::CloseParenToken) {
                            parseAssignmentExpressionOrHigher();
                            if (token() == SyntaxKind::CommaToken) {
                                nextToken();
                            } else {
                                break;
                            }
                        }
                        parseExpected(SyntaxKind::CloseParenToken);
                        continue;
                    } else if (token() == SyntaxKind::NoSubstitutionTemplateLiteral ||
                               token() == SyntaxKind::TemplateHead) {
                        // Tagged template
                        parseTemplateLiteral();
                        continue;
                    }
                }
                // Not type arguments - this is comparison operator, exit
                return;

            case SyntaxKind::OpenParenToken:
                nextToken();
                while (!isEOF() && token() != SyntaxKind::CloseParenToken) {
                    parseAssignmentExpressionOrHigher();
                    if (token() == SyntaxKind::CommaToken) {
                        nextToken();
                    } else {
                        break;
                    }
                }
                parseExpected(SyntaxKind::CloseParenToken);
                continue;
            case SyntaxKind::DotToken:
                nextToken();
                if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                    nextToken();
                }
                continue;
            case SyntaxKind::QuestionDotToken:
                nextToken();
                if (token() == SyntaxKind::OpenBracketToken) {
                    nextToken();
                    parseExpression();
                    parseExpected(SyntaxKind::CloseBracketToken);
                } else if (token() == SyntaxKind::OpenParenToken) {
                    nextToken();
                    while (!isEOF() && token() != SyntaxKind::CloseParenToken) {
                        parseAssignmentExpressionOrHigher();
                        if (token() == SyntaxKind::CommaToken) {
                            nextToken();
                        } else {
                            break;
                        }
                    }
                    parseExpected(SyntaxKind::CloseParenToken);
                } else if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                    nextToken();
                }
                continue;
            case SyntaxKind::OpenBracketToken:
                nextToken();
                parseExpression();
                parseExpected(SyntaxKind::CloseBracketToken);
                continue;
            case SyntaxKind::ExclamationToken:
                // Non-null assertion after call expression: func()!
                if (!currentToken().hadLineBreak) {
                    int start = getNodePos();
                    nextToken();
                    addReplacement(start, getNodePos());
                    continue;
                }
                return;
            case SyntaxKind::NoSubstitutionTemplateLiteral:
            case SyntaxKind::TemplateHead:
                parseTemplateLiteral();
                continue;
            default:
                return;
        }
    }
}

void TsStrip::parseTemplateLiteral() {
    if (token() == SyntaxKind::NoSubstitutionTemplateLiteral) {
        nextToken();
        return;
    }
    // TemplateHead
    nextToken();
    while (!isEOF()) {
        parseExpression();
        if (token() == SyntaxKind::TemplateMiddle) {
            nextToken();
            // Continue parsing template expressions
        } else if (token() == SyntaxKind::TemplateTail) {
            nextToken();
            break;
        } else {
            break;
        }
    }
}

/**
 * parsePrimaryExpression - from TypeRunner
 */
void TsStrip::parsePrimaryExpression() {
    switch (token()) {
        case SyntaxKind::NumericLiteral:
        case SyntaxKind::BigIntLiteral:
        case SyntaxKind::StringLiteral:
        case SyntaxKind::RegularExpressionLiteral:
        case SyntaxKind::NoSubstitutionTemplateLiteral:
            nextToken();
            return;
        case SyntaxKind::TemplateHead:
            parseTemplateLiteral();
            return;
        case SyntaxKind::ThisKeyword:
        case SyntaxKind::SuperKeyword:
        case SyntaxKind::NullKeyword:
        case SyntaxKind::TrueKeyword:
        case SyntaxKind::FalseKeyword:
            nextToken();
            return;
        case SyntaxKind::Identifier:
            nextToken();
            return;
        case SyntaxKind::OpenParenToken:
            // Parenthesized expression or arrow function parameters
            // We need to distinguish between:
            // 1. (expr) - parenthesized expression
            // 2. () => body - arrow function
            // 3. (params) => body - arrow function with params
            // 4. (params: types) => body - arrow function with typed params
            // 
            // Strategy: Parse as expression, but track type annotations for potential arrow function
            {
                nextToken();
                const size_t contentStartIndex = m_tokenIndex;
                bool reparsedAsExpression = false;
                
                // Empty parens - definitely arrow function
                if (token() == SyntaxKind::CloseParenToken) {
                    nextToken();
                    // Check for return type
                    if (token() == SyntaxKind::ColonToken) {
                        int start = getNodePos();
                        nextToken();
                        skipType();
                        addReplacement(start, getNodePos());
                    }
                    // Must be arrow function
                    if (token() == SyntaxKind::EqualsGreaterThanToken) {
                        nextToken();
                        if (token() == SyntaxKind::OpenBraceToken) {
                            parseBlock();
                        } else {
                            parseAssignmentExpressionOrHigher();
                        }
                    }
                    return;
                }
                
                // Track if we see parameter-like syntax (type annotations)
                // Parse contents allowing for both expression and parameter syntax
                while (!isEOF() && token() != SyntaxKind::CloseParenToken) {
                    if (token() == SyntaxKind::CommaToken) {
                        nextToken();
                        continue;
                    }
                    if (token() == SyntaxKind::DotDotDotToken) {
                        // Rest - could be rest param or spread
                        nextToken();
                    }
                    
                    // Parse either parameter or expression
                    if (isBindingIdentifier()) {
                        nextToken();
                        // Optional '?' - arrow function param
                        // BUT: only if followed by ':', ',', ')' or '='
                        // If followed by something else, it's a ternary operator
                        if (token() == SyntaxKind::QuestionToken) {
                            SyntaxKind next = peekToken().kind;
                            if (next == SyntaxKind::ColonToken || 
                                next == SyntaxKind::CommaToken || 
                                next == SyntaxKind::CloseParenToken ||
                                next == SyntaxKind::EqualsToken) {
                                // This is optional parameter marker
                                int start = getNodePos();
                                nextToken();
                                addReplacement(start, getNodePos());
                            } else {
                                // This is likely a ternary operator - reparse as expression
                                m_tokenIndex = contentStartIndex;
                                reparsedAsExpression = true;
                                parseExpression();
                                break;
                            }
                        }
                        // Type annotation ': Type' - could be arrow param or type assertion style
                        if (token() == SyntaxKind::ColonToken) {
                            int start = getNodePos();
                            nextToken();
                            skipType();
                            addReplacement(start, getNodePos());
                        }
                        // Handle as/satisfies after identifier (part of expression)
                        while (token() == SyntaxKind::AsKeyword || token() == SyntaxKind::SatisfiesKeyword) {
                            if (currentToken().hadLineBreak) break;
                            int asStart = getNodePos();
                            nextToken();
                            if (token() == SyntaxKind::ConstKeyword) {
                                nextToken();
                            } else {
                                skipType();
                            }
                            addReplacement(asStart, getNodePos());
                        }
                        // Default value '= expr'
                        if (token() == SyntaxKind::EqualsToken) {
                            nextToken();
                            parseAssignmentExpressionOrHigher();
                            // After parsing default value, we've committed to expression parsing
                            // Don't backtrack to avoid exponential complexity
                            reparsedAsExpression = true;
                        }
                    } else if (token() == SyntaxKind::OpenBraceToken || token() == SyntaxKind::OpenBracketToken) {
                        // Destructuring pattern in parameter
                        if (token() == SyntaxKind::OpenBraceToken) {
                            parseObjectBindingPattern();
                        } else {
                            parseArrayBindingPattern();
                        }
                        // Type annotation after destructuring
                        if (token() == SyntaxKind::ColonToken) {
                            int start = getNodePos();
                            nextToken();
                            skipType();
                            addReplacement(start, getNodePos());
                        }
                        // Default value
                        if (token() == SyntaxKind::EqualsToken) {
                            nextToken();
                            parseAssignmentExpressionOrHigher();
                            reparsedAsExpression = true;
                        }
                    } else {
                        // Some other expression (e.g., nested parentheses with arrow function)
                        // This means the parentheses contain an expression, not a parameter list
                        reparsedAsExpression = true;
                        parseAssignmentExpressionOrHigher();
                    }
                    // Check for comma or close paren
                    if (token() != SyntaxKind::CommaToken && token() != SyntaxKind::CloseParenToken) {
                        // If we didn't end at ',' or ')', this can't be a well-formed arrow
                        // parameter list. Re-parse the whole parenthesized contents as a full
                        // expression to correctly consume operators like calls, property access,
                        // and nested conditionals.
                        // But if we already parsed as expression, don't backtrack again to avoid
                        // exponential complexity on malformed input like (E = (E = (E = ...
                        if (!reparsedAsExpression) {
                            m_tokenIndex = contentStartIndex;
                            reparsedAsExpression = true;
                            parseExpression();
                        }
                        break;
                    }
                }
                parseExpected(SyntaxKind::CloseParenToken);

                if (reparsedAsExpression) {
                    return;
                }
                // Check for arrow function return type annotation
                // Only check for return type if we're actually going to see =>
                // This distinguishes (a, b): T => ... from (a, b) : c in ternary
                if (token() == SyntaxKind::ColonToken) {
                    // Look ahead to see if there's => after the type
                    // If not, this colon is part of a ternary expression
                    SyntaxKind afterColon = peekToken().kind;
                    // Return types usually start with identifier/keyword/brace/paren/etc.
                    // If it looks like a type and we'll see => after, strip it
                    // But we can't easily look past the type, so we use a different approach:
                    // Only strip the colon+type if we're sure there's no => (meaning it IS a return type)
                    // Actually, the problem is: (a, b): T => ... vs x ? (a, b) : c
                    // In the latter, after ), we see : and then c, not a type followed by =>
                    
                    // Simple heuristic: check if there's => on this line or next few tokens
                    // Better approach: save position, try to parse type, check for =>
                    size_t savedIndex = m_tokenIndex;
                    int start = getNodePos();
                    nextToken(); // consume ':'
                    skipType();
                    if (token() == SyntaxKind::EqualsGreaterThanToken) {
                        // Yes, this is arrow function with return type - keep the replacement
                        addReplacement(start, getNodePos());
                    } else {
                        // No =>, this was a ternary colon, not a return type annotation
                        // Restore position
                        m_tokenIndex = savedIndex;
                        return;  // Just a parenthesized expression
                    }
                }
                // Check for arrow function body
                if (token() == SyntaxKind::EqualsGreaterThanToken) {
                    nextToken();
                    if (token() == SyntaxKind::OpenBraceToken) {
                        parseBlock();
                    } else {
                        parseAssignmentExpressionOrHigher();
                    }
                }
            }
            return;
        case SyntaxKind::OpenBracketToken:
            // Array literal
            nextToken();
            while (!isEOF() && token() != SyntaxKind::CloseBracketToken) {
                if (token() == SyntaxKind::CommaToken) {
                    nextToken();
                    continue;
                }
                if (token() == SyntaxKind::DotDotDotToken) {
                    nextToken();
                }
                parseAssignmentExpressionOrHigher();
                if (token() == SyntaxKind::CommaToken) {
                    nextToken();
                }
            }
            parseExpected(SyntaxKind::CloseBracketToken);
            return;
        case SyntaxKind::OpenBraceToken:
            // Object literal
            nextToken();
            while (!isEOF() && token() != SyntaxKind::CloseBraceToken) {
                size_t beforeIndex = m_tokenIndex;
                // Property
                if (token() == SyntaxKind::DotDotDotToken) {
                    nextToken();
                    parseAssignmentExpressionOrHigher();
                } else if (token() == SyntaxKind::OpenBracketToken) {
                    // Computed property
                    nextToken();
                    parseExpression();
                    parseExpected(SyntaxKind::CloseBracketToken);
                    if (token() == SyntaxKind::ColonToken) {
                        nextToken();
                        parseAssignmentExpressionOrHigher();
                    }
                } else if (token() == SyntaxKind::Identifier || isKeyword(token()) 
                           || token() == SyntaxKind::StringLiteral 
                           || token() == SyntaxKind::NumericLiteral) {
                    nextToken();
                    if (token() == SyntaxKind::ColonToken) {
                        nextToken();
                        parseAssignmentExpressionOrHigher();
                    } else if (token() == SyntaxKind::OpenParenToken) {
                        // Method shorthand
                        parseParameters();
                        if (token() == SyntaxKind::ColonToken) {
                            int start = getNodePos();
                            nextToken();
                            skipType();
                            addReplacement(start, getNodePos());
                        }
                        if (token() == SyntaxKind::OpenBraceToken) {
                            parseBlock();
                        }
                    }
                    // else shorthand property
                }
                // In valid JS object literals, only ',' is a separator.
                // However, when we desync (or when parsing TS-like constructs in
                // strip mode), we may encounter ';' here. Consume it to avoid
                // getting stuck.
                if (token() == SyntaxKind::CommaToken || token() == SyntaxKind::SemicolonToken) {
                    nextToken();
                }

                // Safety: ensure we always make progress in this loop.
                if (m_tokenIndex == beforeIndex) {
                    nextToken();
                }
            }
            parseExpected(SyntaxKind::CloseBraceToken);
            return;
        case SyntaxKind::FunctionKeyword:
            // Function expression
            nextToken();
            if (token() == SyntaxKind::AsteriskToken) {
                nextToken();
            }
            if (token() == SyntaxKind::Identifier) {
                nextToken();
            }
            if (token() == SyntaxKind::LessThanToken) {
                int start = getNodePos();
                skipTypeArguments();
                addReplacement(start, getNodePos());
            }
            parseParameters();
            if (token() == SyntaxKind::ColonToken) {
                int start = getNodePos();
                nextToken();
                skipType();
                addReplacement(start, getNodePos());
            }
            if (token() == SyntaxKind::OpenBraceToken) {
                parseBlock();
            }
            return;
        case SyntaxKind::ClassKeyword:
            // Class expression
            parseClassDeclaration();
            return;
        case SyntaxKind::NewKeyword:
            nextToken();
            if (token() == SyntaxKind::DotToken) {
                // new.target
                nextToken();
                if (token() == SyntaxKind::Identifier) {
                    nextToken();
                }
                return;
            }
            parseMemberExpressionOrHigher();
            if (token() == SyntaxKind::LessThanToken) {
                int start = getNodePos();
                skipTypeArguments();
                addReplacement(start, getNodePos());
            }
            if (token() == SyntaxKind::OpenParenToken) {
                nextToken();
                while (!isEOF() && token() != SyntaxKind::CloseParenToken) {
                    parseAssignmentExpressionOrHigher();
                    if (token() == SyntaxKind::CommaToken) {
                        nextToken();
                    } else {
                        break;
                    }
                }
                parseExpected(SyntaxKind::CloseParenToken);
            }
            return;
        case SyntaxKind::SlashToken:
        case SyntaxKind::SlashEqualsToken:
            // RegExp literal - re-scan from raw text (like TypeRunner reScanSlashToken)
            if (reScanSlashTokenAsRegularExpressionLiteral()) {
                nextToken();
                return;
            }
            nextToken();
            return;
        case SyntaxKind::AsyncKeyword:
            nextToken();
            if (token() == SyntaxKind::FunctionKeyword && !currentToken().hadLineBreak) {
                // async function
                nextToken();
                if (token() == SyntaxKind::AsteriskToken) {
                    nextToken();
                }
                if (token() == SyntaxKind::Identifier) {
                    nextToken();
                }
                if (token() == SyntaxKind::LessThanToken) {
                    int start = getNodePos();
                    skipTypeArguments();
                    addReplacement(start, getNodePos());
                }
                parseParameters();
                if (token() == SyntaxKind::ColonToken) {
                    int start = getNodePos();
                    nextToken();
                    skipType();
                    addReplacement(start, getNodePos());
                }
                if (token() == SyntaxKind::OpenBraceToken) {
                    parseBlock();
                }
            } else if (token() == SyntaxKind::LessThanToken && !currentToken().hadLineBreak) {
                // async <T>(v: T) => v - async arrow function with type parameters
                int start = getNodePos();
                skipTypeArguments();
                addReplacement(start, getNodePos());
                if (token() == SyntaxKind::OpenParenToken) {
                    parseParameters();
                    if (token() == SyntaxKind::ColonToken) {
                        start = getNodePos();
                        nextToken();
                        skipType();
                        addReplacement(start, getNodePos());
                    }
                    if (token() == SyntaxKind::EqualsGreaterThanToken) {
                        nextToken();
                        if (token() == SyntaxKind::OpenBraceToken) {
                            parseBlock();
                        } else {
                            parseExpression();
                        }
                    }
                }
            } else if (token() == SyntaxKind::OpenParenToken && !currentToken().hadLineBreak) {
                // async (url: string) => {} - async arrow function
                parseParameters();
                if (token() == SyntaxKind::ColonToken) {
                    int start = getNodePos();
                    nextToken();
                    skipType();
                    addReplacement(start, getNodePos());
                }
                if (token() == SyntaxKind::EqualsGreaterThanToken) {
                    nextToken();
                    if (token() == SyntaxKind::OpenBraceToken) {
                        parseBlock();
                    } else {
                        parseExpression();
                    }
                }
            }
            // else 'async' as identifier
            return;
        case SyntaxKind::YieldKeyword:
            nextToken();
            if (!currentToken().hadLineBreak) {
                parseOptional(SyntaxKind::AsteriskToken);
                if (isStartOfExpression()) {
                    parseAssignmentExpressionOrHigher();
                }
            }
            return;
        case SyntaxKind::AwaitKeyword:
            nextToken();
            parseUnaryExpressionOrHigher();
            return;
        case SyntaxKind::ImportKeyword:
            nextToken();
            if (token() == SyntaxKind::DotToken) {
                // import.meta
                nextToken();
                if (token() == SyntaxKind::Identifier) {
                    nextToken();
                }
            } else if (token() == SyntaxKind::OpenParenToken) {
                // Dynamic import
                nextToken();
                parseAssignmentExpressionOrHigher();
                parseExpected(SyntaxKind::CloseParenToken);
            }
            return;
        default:
            // Check for contextual keywords used as identifiers
            if (isKeyword(token())) {
                nextToken();
                return;
            }
            // Unknown token - skip
            nextToken();
            return;
    }
}

/**
 * isStartOfExpression - from TypeRunner
 */
bool TsStrip::isStartOfExpression() const {
    switch (token()) {
        case SyntaxKind::NumericLiteral:
        case SyntaxKind::BigIntLiteral:
        case SyntaxKind::StringLiteral:
        case SyntaxKind::NoSubstitutionTemplateLiteral:
        case SyntaxKind::TemplateHead:
        case SyntaxKind::ThisKeyword:
        case SyntaxKind::SuperKeyword:
        case SyntaxKind::NullKeyword:
        case SyntaxKind::TrueKeyword:
        case SyntaxKind::FalseKeyword:
        case SyntaxKind::Identifier:
        case SyntaxKind::OpenParenToken:
        case SyntaxKind::OpenBracketToken:
        case SyntaxKind::OpenBraceToken:
        case SyntaxKind::FunctionKeyword:
        case SyntaxKind::ClassKeyword:
        case SyntaxKind::NewKeyword:
        case SyntaxKind::SlashToken:
        case SyntaxKind::SlashEqualsToken:
        case SyntaxKind::PlusToken:
        case SyntaxKind::MinusToken:
        case SyntaxKind::TildeToken:
        case SyntaxKind::ExclamationToken:
        case SyntaxKind::DeleteKeyword:
        case SyntaxKind::TypeOfKeyword:
        case SyntaxKind::VoidKeyword:
        case SyntaxKind::PlusPlusToken:
        case SyntaxKind::MinusMinusToken:
        case SyntaxKind::LessThanToken:
        case SyntaxKind::AwaitKeyword:
        case SyntaxKind::YieldKeyword:
        case SyntaxKind::AsyncKeyword:
        case SyntaxKind::ImportKeyword:
            return true;
        default:
            // Contextual keywords can be identifiers
            if (isKeyword(token())) {
                return true;
            }
            return false;
    }
}

/**
 * isLetDeclaration - check if 'let' starts a declaration
 */
bool TsStrip::isLetDeclaration() const {
    // 'let' is a declaration if followed by identifier, keyword (as var name), {, or [
    // Note: contextual keywords like 'symbol', 'type', 'get', 'set' can be variable names
    SyntaxKind next = peekToken().kind;
    return next == SyntaxKind::Identifier 
        || isKeyword(next)
        || next == SyntaxKind::OpenBraceToken
        || next == SyntaxKind::OpenBracketToken;
}

// ========================================================================
// Statement parsing (from TypeRunner parser2.h line 7315)
// ========================================================================

/**
 * parseStatement - from TypeRunner parser2.h line 7315
 */
void TsStrip::parseStatement() {
    switch (token()) {
        case SyntaxKind::SemicolonToken:
            // Empty statement
            nextToken();
            return;
        case SyntaxKind::OpenBraceToken:
            parseBlock();
            return;
        case SyntaxKind::VarKeyword:
            parseVariableStatement();
            return;
        case SyntaxKind::LetKeyword:
            if (isLetDeclaration()) {
                parseVariableStatement();
                return;
            }
            break;
        case SyntaxKind::FunctionKeyword:
            parseFunctionDeclaration();
            return;
        case SyntaxKind::ClassKeyword:
            parseClassDeclaration();
            return;
        case SyntaxKind::IfKeyword:
            nextToken();
            parseExpected(SyntaxKind::OpenParenToken);
            parseExpression();
            parseExpected(SyntaxKind::CloseParenToken);
            parseStatement();
            if (parseOptional(SyntaxKind::ElseKeyword)) {
                parseStatement();
            }
            return;
        case SyntaxKind::DoKeyword:
            nextToken();
            parseStatement();
            parseExpected(SyntaxKind::WhileKeyword);
            parseExpected(SyntaxKind::OpenParenToken);
            parseExpression();
            parseExpected(SyntaxKind::CloseParenToken);
            tryParseSemicolon();
            return;
        case SyntaxKind::WhileKeyword:
            nextToken();
            parseExpected(SyntaxKind::OpenParenToken);
            parseExpression();
            parseExpected(SyntaxKind::CloseParenToken);
            parseStatement();
            return;
        case SyntaxKind::ForKeyword:
            parseForStatement();
            return;
        case SyntaxKind::ContinueKeyword:
        case SyntaxKind::BreakKeyword:
            nextToken();
            if (!canParseSemicolon() && token() == SyntaxKind::Identifier) {
                nextToken();
            }
            tryParseSemicolon();
            return;
        case SyntaxKind::ReturnKeyword:
            nextToken();
            if (!canParseSemicolon() && isStartOfExpression()) {
                parseExpression();
            }
            tryParseSemicolon();
            return;
        case SyntaxKind::WithKeyword:
            nextToken();
            parseExpected(SyntaxKind::OpenParenToken);
            parseExpression();
            parseExpected(SyntaxKind::CloseParenToken);
            parseStatement();
            return;
        case SyntaxKind::SwitchKeyword:
            parseSwitchStatement();
            return;
        case SyntaxKind::ThrowKeyword:
            nextToken();
            if (!currentToken().hadLineBreak) {
                parseExpression();
            }
            tryParseSemicolon();
            return;
        case SyntaxKind::TryKeyword:
            parseTryStatement();
            return;
        case SyntaxKind::DebuggerKeyword:
            nextToken();
            tryParseSemicolon();
            return;
        case SyntaxKind::AtToken:
            // Decorator
            parseDeclaration();
            return;
        case SyntaxKind::ConstKeyword:
            if (peekToken().kind == SyntaxKind::EnumKeyword) {
                // const enum
                int start = getNodePos();
                nextToken(); // const
                nextToken(); // enum
                parseEnumDeclaration(start);
                return;
            }
            parseVariableStatement();
            return;
        // TypeScript declarations
        case SyntaxKind::InterfaceKeyword: {
            int start = getNodePos();
            nextToken();
            parseInterfaceDeclaration(start);
            return;
        }
        case SyntaxKind::TypeKeyword:
            if (peekToken().kind == SyntaxKind::Identifier) {
                int start = getNodePos();
                nextToken();
                parseTypeAliasDeclaration(start);
                return;
            }
            break;
        case SyntaxKind::EnumKeyword: {
            int start = getNodePos();
            nextToken();
            parseEnumDeclaration(start);
            return;
        }
        case SyntaxKind::NamespaceKeyword:
        case SyntaxKind::ModuleKeyword: {
            // module X { } or module "x" { } is a module declaration
            // module.x or module[0] is using module as a variable name
            SyntaxKind next = peekToken().kind;
            if (next == SyntaxKind::Identifier || next == SyntaxKind::StringLiteral ||
                next == SyntaxKind::OpenBraceToken) {
                int start = getNodePos();
                nextToken();
                parseModuleDeclaration(start);
                return;
            }
            // Otherwise, module is being used as an identifier (e.g., module.exports)
            break;
        }
        case SyntaxKind::DeclareKeyword: {
            int start = getNodePos();
            nextToken();
            parseDeclaration();
            // Use getPrevTokenEnd() to avoid erasing comments after the declaration
            addReplacement(start, getPrevTokenEnd());
            return;
        }
        case SyntaxKind::ImportKeyword:
            parseImportDeclaration();
            return;
        case SyntaxKind::ExportKeyword:
            parseExportDeclaration();
            return;
        case SyntaxKind::AbstractKeyword:
            if (peekToken().kind == SyntaxKind::ClassKeyword) {
                int start = getNodePos();
                nextToken();
                addReplacement(start, getNodePos());
                parseClassDeclaration();
                return;
            }
            break;
        case SyntaxKind::AsyncKeyword:
            if (peekToken().kind == SyntaxKind::FunctionKeyword && !peekToken().hadLineBreak) {
                parseFunctionDeclaration();
                return;
            }
            break;
        default:
            break;
    }
    // Default: expression or labeled statement
    parseExpressionOrLabeledStatement();
}

/**
 * parseExpressionOrLabeledStatement - from TypeRunner parser2.h line 7084
 * This is the KEY function that properly handles expression statements!
 */
void TsStrip::parseExpressionOrLabeledStatement() {
    // Avoiding having to do the lookahead for a labeled statement by just trying to parse
    // out an expression, seeing if it is identifier and then seeing if it is followed by
    // a colon.
    bool wasIdentifier = (token() == SyntaxKind::Identifier);
    
    parseExpression();
    
    if (wasIdentifier && token() == SyntaxKind::ColonToken) {
        // It's a labeled statement: label: statement
        nextToken();
        parseStatement();
    } else {
        if (!tryParseSemicolon()) {
            // Error: missing semicolon, but continue
        }
    }
}

/**
 * parseBlock - from TypeRunner
 */
void TsStrip::parseBlock() {
    parseExpected(SyntaxKind::OpenBraceToken);
    int loopCount = 0;
    while (!isEOF() && token() != SyntaxKind::CloseBraceToken) {
        if (++loopCount > 10000) {
            std::cerr << "LOOP in parseBlock: token=" << (int)token() << " pos=" << getNodePos() << std::endl;
            throw std::runtime_error("Infinite loop in parseBlock");
        }
        size_t beforeIndex = m_tokenIndex;
        parseStatement();
        if (m_tokenIndex == beforeIndex) {
            std::cerr << "WARNING: parseStatement didn't advance in block at token=" << (int)token() << std::endl;
            nextToken();
        }
    }
    parseExpected(SyntaxKind::CloseBraceToken);
}

/**
 * parseForStatement
 */
void TsStrip::parseForStatement() {
    nextToken(); // 'for'
    bool isAwait = parseOptional(SyntaxKind::AwaitKeyword);
    parseExpected(SyntaxKind::OpenParenToken);
    
    // Initializer
    if (token() != SyntaxKind::SemicolonToken) {
        if (token() == SyntaxKind::VarKeyword || token() == SyntaxKind::LetKeyword || token() == SyntaxKind::ConstKeyword) {
            nextToken(); // Skip var/let/const
            parseVariableDeclarationList();
        } else {
            m_disallowInContext = true;
            parseExpression();
            m_disallowInContext = false;
        }
    }
    
    // Check for for-in or for-of
    if (parseOptional(SyntaxKind::InKeyword)) {
        parseExpression();
        parseExpected(SyntaxKind::CloseParenToken);
        parseStatement();
        return;
    }
    if (parseOptional(SyntaxKind::OfKeyword)) {
        parseAssignmentExpressionOrHigher();
        parseExpected(SyntaxKind::CloseParenToken);
        parseStatement();
        return;
    }
    
    // Regular for loop
    parseExpected(SyntaxKind::SemicolonToken);
    if (token() != SyntaxKind::SemicolonToken) {
        parseExpression();
    }
    parseExpected(SyntaxKind::SemicolonToken);
    if (token() != SyntaxKind::CloseParenToken) {
        parseExpression();
    }
    parseExpected(SyntaxKind::CloseParenToken);
    parseStatement();
}

/**
 * parseSwitchStatement
 */
void TsStrip::parseSwitchStatement() {
    nextToken(); // 'switch'
    parseExpected(SyntaxKind::OpenParenToken);
    parseExpression();
    parseExpected(SyntaxKind::CloseParenToken);
    parseExpected(SyntaxKind::OpenBraceToken);
    
    while (!isEOF() && token() != SyntaxKind::CloseBraceToken) {
        if (token() == SyntaxKind::CaseKeyword || token() == SyntaxKind::DefaultKeyword) {
            bool isDefault = (token() == SyntaxKind::DefaultKeyword);
            nextToken();
            if (!isDefault) {
                parseExpression();
            }
            parseExpected(SyntaxKind::ColonToken);
            // Parse statements until next case/default/}
            while (!isEOF() && token() != SyntaxKind::CaseKeyword 
                   && token() != SyntaxKind::DefaultKeyword 
                   && token() != SyntaxKind::CloseBraceToken) {
                parseStatement();
            }
        } else {
            break;
        }
    }
    parseExpected(SyntaxKind::CloseBraceToken);
}

/**
 * parseTryStatement
 */
void TsStrip::parseTryStatement() {
    nextToken(); // 'try'
    parseBlock();
    
    if (token() == SyntaxKind::CatchKeyword) {
        nextToken();
        if (parseOptional(SyntaxKind::OpenParenToken)) {
            // Catch binding
            if (token() == SyntaxKind::Identifier || token() == SyntaxKind::OpenBraceToken || token() == SyntaxKind::OpenBracketToken) {
                if (token() == SyntaxKind::Identifier) {
                    nextToken();
                } else {
                    skipBalanced(token(), token() == SyntaxKind::OpenBraceToken ? SyntaxKind::CloseBraceToken : SyntaxKind::CloseBracketToken);
                }
                // Type annotation
                if (token() == SyntaxKind::ColonToken) {
                    int start = getNodePos();
                    nextToken();
                    skipType();
                    addReplacement(start, getNodePos());
                }
            }
            parseExpected(SyntaxKind::CloseParenToken);
        }
        parseBlock();
    }
    
    if (parseOptional(SyntaxKind::FinallyKeyword)) {
        parseBlock();
    }
}

// ========================================================================
// Variable and Function declarations
// ========================================================================

/**
 * parseVariableStatement
 */
void TsStrip::parseVariableStatement() {
    nextToken(); // var/let/const
    parseVariableDeclarationList();
    tryParseSemicolon();
}

/**
 * parseVariableDeclarationList
 */
void TsStrip::parseVariableDeclarationList() {
    // Parse first declaration
    parseVariableDeclaration();
    
    // Parse additional declarations
    while (parseOptional(SyntaxKind::CommaToken)) {
        parseVariableDeclaration();
    }
}

/**
 * parseVariableDeclaration - from TypeRunner parser2.h
 */
void TsStrip::parseVariableDeclaration() {
    // Binding pattern or identifier
    // Note: contextual keywords like 'symbol', 'type' can also be variable names
    if (token() == SyntaxKind::Identifier || isKeyword(token())) {
        nextToken();
    } else if (token() == SyntaxKind::OpenBraceToken) {
        parseObjectBindingPattern();
    } else if (token() == SyntaxKind::OpenBracketToken) {
        parseArrayBindingPattern();
    }
    
    // Definite assignment assertion `!`
    if (token() == SyntaxKind::ExclamationToken && !currentToken().hadLineBreak) {
        int start = getNodePos();
        nextToken();
        addReplacement(start, getPrevTokenEnd());
    }
    
    // Type annotation
    if (token() == SyntaxKind::ColonToken) {
        int start = getNodePos();
        nextToken();
        skipType();
        // Use getPrevTokenEnd() to avoid erasing comments after type annotation
        addReplacement(start, getPrevTokenEnd());
    }
    
    // Initializer
    if (parseOptional(SyntaxKind::EqualsToken)) {
        parseAssignmentExpressionOrHigher();
    }
}

void TsStrip::parseObjectBindingPattern() {
    parseExpected(SyntaxKind::OpenBraceToken);
    while (!isEOF() && token() != SyntaxKind::CloseBraceToken) {
        if (token() == SyntaxKind::DotDotDotToken) {
            nextToken();
        }
        if (token() == SyntaxKind::Identifier) {
            nextToken();
            if (parseOptional(SyntaxKind::ColonToken)) {
                // propertyName: bindingName
                if (token() == SyntaxKind::Identifier) {
                    nextToken();
                } else if (token() == SyntaxKind::OpenBraceToken) {
                    parseObjectBindingPattern();
                } else if (token() == SyntaxKind::OpenBracketToken) {
                    parseArrayBindingPattern();
                }
            }
        } else if (token() == SyntaxKind::OpenBracketToken) {
            // Computed property name
            nextToken();
            parseExpression();
            parseExpected(SyntaxKind::CloseBracketToken);
            parseExpected(SyntaxKind::ColonToken);
            if (token() == SyntaxKind::Identifier) {
                nextToken();
            } else if (token() == SyntaxKind::OpenBraceToken) {
                parseObjectBindingPattern();
            } else if (token() == SyntaxKind::OpenBracketToken) {
                parseArrayBindingPattern();
            }
        }
        
        // Default value
        if (parseOptional(SyntaxKind::EqualsToken)) {
            parseAssignmentExpressionOrHigher();
        }
        
        if (!parseOptional(SyntaxKind::CommaToken)) {
            break;
        }
    }
    parseExpected(SyntaxKind::CloseBraceToken);
}

void TsStrip::parseArrayBindingPattern() {
    parseExpected(SyntaxKind::OpenBracketToken);
    while (!isEOF() && token() != SyntaxKind::CloseBracketToken) {
        if (token() == SyntaxKind::CommaToken) {
            nextToken();
            continue;
        }
        if (token() == SyntaxKind::DotDotDotToken) {
            nextToken();
        }
        if (token() == SyntaxKind::Identifier) {
            nextToken();
        } else if (token() == SyntaxKind::OpenBraceToken) {
            parseObjectBindingPattern();
        } else if (token() == SyntaxKind::OpenBracketToken) {
            parseArrayBindingPattern();
        }
        
        // Default value
        if (parseOptional(SyntaxKind::EqualsToken)) {
            parseAssignmentExpressionOrHigher();
        }
        
        if (!parseOptional(SyntaxKind::CommaToken)) {
            break;
        }
    }
    parseExpected(SyntaxKind::CloseBracketToken);
}

/**
 * parseFunctionDeclaration
 */
void TsStrip::parseFunctionDeclaration(int outerStart) {
    int declStart = getNodePos();
    bool hasBody = false;
    
    // async?
    if (token() == SyntaxKind::AsyncKeyword) {
        nextToken();
    }
    
    parseExpected(SyntaxKind::FunctionKeyword);
    
    // Generator *
    parseOptional(SyntaxKind::AsteriskToken);
    
    // Name
    if (token() == SyntaxKind::Identifier) {
        nextToken();
    }
    
    // Type parameters
    if (token() == SyntaxKind::LessThanToken) {
        int start = getNodePos();
        skipTypeArguments();
        addReplacement(start, getNodePos());
    }
    
    // Parameters
    parseParameters();
    
    // Return type
    if (token() == SyntaxKind::ColonToken) {
        int start = getNodePos();
        nextToken();
        skipType();
        addReplacement(start, getNodePos());
    }
    
    // Body or semicolon (overload)
    if (token() == SyntaxKind::OpenBraceToken) {
        hasBody = true;
        parseBlock();
    } else {
        tryParseSemicolon();
    }
    
    // If no body, remove entire overload signature
    // Use outerStart if provided (e.g., for 'export function ...')
    if (!hasBody) {
        int eraseStart = (outerStart >= 0) ? outerStart : declStart;
        addReplacement(eraseStart, getNodePos());
    }
}

/**
 * parseParameters - from TypeRunner
 */
void TsStrip::parseParameters() {
    parseExpected(SyntaxKind::OpenParenToken);
    
    // Check for `this` parameter
    if (token() == SyntaxKind::ThisKeyword) {
        int thisStart = getNodePos();
        nextToken();
        parseOptional(SyntaxKind::QuestionToken);
        if (token() == SyntaxKind::ColonToken) {
            nextToken();
            skipType();
        }
        // Remove this param including comma
        if (parseOptional(SyntaxKind::CommaToken)) {
            addReplacement(thisStart, getNodePos());
        } else {
            addReplacement(thisStart, getNodePos());
        }
    }
    
    while (!isEOF() && token() != SyntaxKind::CloseParenToken) {
        // Decorators
        while (token() == SyntaxKind::AtToken) {
            nextToken();
            if (token() == SyntaxKind::Identifier) {
                nextToken();
            }
            while (token() == SyntaxKind::DotToken) {
                nextToken();
                if (token() == SyntaxKind::Identifier) {
                    nextToken();
                }
            }
            if (token() == SyntaxKind::OpenParenToken) {
                skipBalanced(SyntaxKind::OpenParenToken, SyntaxKind::CloseParenToken);
            }
        }
        
        // Modifiers (parameter properties in constructor)
        int modStart = getNodePos();
        bool hasModifier = false;
        while (token() == SyntaxKind::PublicKeyword || token() == SyntaxKind::PrivateKeyword ||
             token() == SyntaxKind::ProtectedKeyword || token() == SyntaxKind::ReadonlyKeyword ||
             token() == SyntaxKind::OverrideKeyword) {
            hasModifier = true;
            nextToken();
        }
        if (hasModifier) {
            addReplacement(modStart, getNodePos());
        }
        
        // Rest parameter
        parseOptional(SyntaxKind::DotDotDotToken);
        
        // Parameter name or binding pattern
        if (isBindingIdentifier()) {
            nextToken();
        } else if (token() == SyntaxKind::OpenBraceToken) {
            parseObjectBindingPattern();
        } else if (token() == SyntaxKind::OpenBracketToken) {
            parseArrayBindingPattern();
        }
        
        // Optional ?
        if (token() == SyntaxKind::QuestionToken) {
            int start = getNodePos();
            nextToken();
            addReplacement(start, getNodePos());
        }
        
        // Type annotation
        if (token() == SyntaxKind::ColonToken) {
            int start = getNodePos();
            nextToken();
            skipType();
            addReplacement(start, getNodePos());
        }
        
        // Default value
        if (parseOptional(SyntaxKind::EqualsToken)) {
            parseAssignmentExpressionOrHigher();
        }
        
        if (!parseOptional(SyntaxKind::CommaToken)) {
            break;
        }
    }
    
    parseExpected(SyntaxKind::CloseParenToken);
}

// ========================================================================
// Class declaration
// ========================================================================

/**
 * parseClassDeclaration
 */
void TsStrip::parseClassDeclaration() {
    parseExpected(SyntaxKind::ClassKeyword);
    
    // Name
    if (token() == SyntaxKind::Identifier) {
        nextToken();
    }
    
    // Type parameters
    if (token() == SyntaxKind::LessThanToken) {
        int start = getNodePos();
        skipTypeArguments();
        addReplacement(start, getNodePos());
    }
    
    // extends
    if (parseOptional(SyntaxKind::ExtendsKeyword)) {
        // Base class expression (keep)
        parseLeftHandSideExpressionOrHigher();
        // Type arguments on base class (remove)
        if (token() == SyntaxKind::LessThanToken) {
            int start = getNodePos();
            skipTypeArguments();
            addReplacement(start, getNodePos());
        }
    }
    
    // implements (remove entirely)
    if (token() == SyntaxKind::ImplementsKeyword) {
        int start = getNodePos();
        nextToken();
        while (!isEOF() && token() != SyntaxKind::OpenBraceToken) {
            nextToken();
        }
        addReplacement(start, getNodePos());
    }
    
    // Body
    if (token() == SyntaxKind::OpenBraceToken) {
        parseClassBody();
    }
}

/**
 * parseClassBody
 */
void TsStrip::parseClassBody() {
    parseExpected(SyntaxKind::OpenBraceToken);
    while (!isEOF() && token() != SyntaxKind::CloseBraceToken) {
        parseClassMember();
    }
    parseExpected(SyntaxKind::CloseBraceToken);
}

/**
 * parseClassMember
 */
void TsStrip::parseClassMember() {
    int memberStart = getNodePos();
    
    // Decorators
    while (token() == SyntaxKind::AtToken) {
        nextToken();
        if (token() == SyntaxKind::Identifier) {
            nextToken();
        }
        while (token() == SyntaxKind::DotToken) {
            nextToken();
            if (token() == SyntaxKind::Identifier) {
                nextToken();
            }
        }
        if (token() == SyntaxKind::OpenParenToken) {
            skipBalanced(SyntaxKind::OpenParenToken, SyntaxKind::CloseParenToken);
        }
    }
    
    int modifierStart = getNodePos();
    
    // Track positions for separate replacement of TS-only modifiers
    std::vector<std::pair<int, int>> tsModifierRanges;
    
    // Modifiers
    bool hasTsModifiers = false;
    bool isAbstract = false;
    bool isDeclare = false;
    bool isStatic = false;
    bool hasDecorators = (modifierStart != memberStart);
    
    while (true) {
        int modStart = getNodePos();
        switch (token()) {
            case SyntaxKind::PublicKeyword:
            case SyntaxKind::PrivateKeyword:
            case SyntaxKind::ProtectedKeyword:
            case SyntaxKind::ReadonlyKeyword:
            case SyntaxKind::OverrideKeyword:
                hasTsModifiers = true;
                nextToken();
                tsModifierRanges.push_back({modStart, getNodePos()});
                continue;
            case SyntaxKind::AbstractKeyword:
                hasTsModifiers = true;
                isAbstract = true;
                nextToken();
                tsModifierRanges.push_back({modStart, getNodePos()});
                continue;
            case SyntaxKind::DeclareKeyword:
                hasTsModifiers = true;
                isDeclare = true;
                nextToken();
                tsModifierRanges.push_back({modStart, getNodePos()});
                continue;
            case SyntaxKind::StaticKeyword:
                isStatic = true;
                nextToken();
                // static is JS valid, don't add to tsModifierRanges
                continue;
            case SyntaxKind::AsyncKeyword:
                nextToken();
                // async is JS valid, don't add to tsModifierRanges
                continue;
            case SyntaxKind::AccessorKeyword:
                nextToken();
                // accessor is JS valid, don't add to tsModifierRanges
                continue;
            default:
                break;
        }
        break;
    }
    
    int keyStart = getNodePos();
    
    // Static block: static { ... }
    if (isStatic && token() == SyntaxKind::OpenBraceToken) {
        parseBlock();
        return;
    }
    
    // Danger check for stripping modifiers (SWC logic)
    bool isDangerous = false;
    if (hasTsModifiers && !isStatic && !hasDecorators) {
        if (token() == SyntaxKind::OpenBracketToken || 
            token() == SyntaxKind::AsteriskToken ||
            token() == SyntaxKind::InKeyword || 
            token() == SyntaxKind::InstanceOfKeyword) {
            isDangerous = true;
        }
    }
    
    // Strip TS-only modifiers (preserve static, async, accessor)
    for (const auto& range : tsModifierRanges) {
        addReplacement(range.first, range.second);
    }
    if (isDangerous && !tsModifierRanges.empty()) {
        addOverwrite(tsModifierRanges[0].first, ';');
    }
    
    // Index signature [key: type]: type
    if (token() == SyntaxKind::OpenBracketToken) {
        int bracketStart = getNodePos();
        nextToken();
        
        // Check if index signature (has : inside)
        if (token() == SyntaxKind::Identifier) {
            nextToken();
            if (token() == SyntaxKind::ColonToken) {
                // Index signature - skip and remove
                while (!isEOF() && token() != SyntaxKind::CloseBracketToken) {
                    nextToken();
                }
                parseExpected(SyntaxKind::CloseBracketToken);
                if (token() == SyntaxKind::ColonToken) {
                    nextToken();
                    skipType();
                }
                tryParseSemicolon();
                addReplacement(memberStart, getNodePos());
                return;
            }
        }
        // Computed property - continue
        while (!isEOF() && token() != SyntaxKind::CloseBracketToken) {
            parseExpression();
        }
        parseExpected(SyntaxKind::CloseBracketToken);
    }
    
    // Constructor
    if (token() == SyntaxKind::ConstructorKeyword) {
        int ctorDeclStart = memberStart;
        nextToken();
        parseParameters();
        
        if (token() == SyntaxKind::ColonToken) {
            int start = getNodePos();
            nextToken();
            skipType();
            addReplacement(start, getNodePos());
        }
        
        if (token() == SyntaxKind::OpenBraceToken) {
            parseBlock();
        } else {
            // Constructor overload - remove
            tryParseSemicolon();
            addReplacement(ctorDeclStart, getNodePos());
        }
        return;
    }
    
    // Property or method name
    bool isGenerator = false;
    bool isGetter = false;
    bool isSetter = false;
    
    if (token() == SyntaxKind::AsteriskToken) {
        isGenerator = true;
        nextToken();
    }
    
    if (token() == SyntaxKind::GetKeyword || token() == SyntaxKind::SetKeyword) {
        SyntaxKind accessor = token();
        nextToken();
        // Could be get/set as property name if followed by other name or (
        if (token() == SyntaxKind::Identifier || token() == SyntaxKind::StringLiteral ||
            token() == SyntaxKind::NumericLiteral || token() == SyntaxKind::OpenBracketToken ||
            token() == SyntaxKind::PrivateIdentifier) {
            isGetter = (accessor == SyntaxKind::GetKeyword);
            isSetter = (accessor == SyntaxKind::SetKeyword);
        }
        // else get/set is the property name, already consumed
    }
    
    // Member name
    if (token() == SyntaxKind::Identifier || token() == SyntaxKind::StringLiteral ||
        token() == SyntaxKind::NumericLiteral || token() == SyntaxKind::PrivateIdentifier ||
        isKeyword(token())) {
        nextToken();
    } else if (token() == SyntaxKind::OpenBracketToken) {
        // Computed property
        nextToken();
        parseExpression();
        parseExpected(SyntaxKind::CloseBracketToken);
    } else if (token() != SyntaxKind::OpenParenToken && token() != SyntaxKind::ColonToken &&
               token() != SyntaxKind::EqualsToken && token() != SyntaxKind::SemicolonToken &&
               token() != SyntaxKind::CloseBraceToken && token() != SyntaxKind::LessThanToken &&
               token() != SyntaxKind::QuestionToken && token() != SyntaxKind::ExclamationToken) {
        // Unrecognized token - skip to avoid infinite loop on syntax errors
        nextToken();
        return;
    }
    
    // Optional ?
    if (token() == SyntaxKind::QuestionToken) {
        int start = getNodePos();
        nextToken();
        addReplacement(start, getNodePos());
    }
    
    // Definite assignment !
    if (token() == SyntaxKind::ExclamationToken) {
        if (!currentToken().hadLineBreak) {
            int start = getNodePos();
            nextToken();
            addReplacement(start, getNodePos());
        } else {
            // ExclamationToken at line start is a syntax error - skip to avoid infinite loop
            nextToken();
            return;
        }
    }
    
    // Type parameters
    if (token() == SyntaxKind::LessThanToken) {
        int start = getNodePos();
        skipTypeArguments();
        addReplacement(start, getNodePos());
    }
    
    // Method
    if (token() == SyntaxKind::OpenParenToken) {
        int methodDeclStart = memberStart;
        parseParameters();
        
        // Return type
        if (token() == SyntaxKind::ColonToken) {
            int start = getNodePos();
            nextToken();
            skipType();
            addReplacement(start, getNodePos());
        }
        
        // Body
        if (token() == SyntaxKind::OpenBraceToken) {
            if (isAbstract || isDeclare) {
                parseBlock();
                addReplacement(methodDeclStart, getNodePos());
            } else {
                parseBlock();
            }
        } else {
            // Method overload signature - remove
            tryParseSemicolon();
            addReplacement(methodDeclStart, getNodePos());
        }
        return;
    }
    
    // Property
    // Type annotation
    int typeAnnotationStart = -1;
    int typeAnnotationEnd = -1;
    bool hadTypeAnnotation = false;
    if (token() == SyntaxKind::ColonToken) {
        typeAnnotationStart = getNodePos();
        nextToken();
        skipType();
        typeAnnotationEnd = getNodePos();
        addReplacement(typeAnnotationStart, typeAnnotationEnd);
        hadTypeAnnotation = true;
    }
    
    // Initializer
    bool hadInitializer = false;
    if (parseOptional(SyntaxKind::EqualsToken)) {
        parseAssignmentExpressionOrHigher();
        hadInitializer = true;
    }
    
    // Check if we need a semicolon for ASI safety
    // If we erased a type annotation but have no initializer and no explicit semicolon,
    // and the next line could be a method, we need a semicolon.
    // e.g., static: any\n    foo() { } should become static;\n    foo() { }
    if (hadTypeAnnotation && !hadInitializer && token() != SyntaxKind::SemicolonToken) {
        if (currentToken().hadLineBreak) {
            // Next line could be a method - insert semicolon
            addOverwrite(typeAnnotationStart, ';');
        }
    }
    
    tryParseSemicolon();
    
    // If abstract/declare, remove entire member
    if (isAbstract || isDeclare) {
        addReplacement(memberStart, getNodePos());
    }
}

// ========================================================================
// TypeScript-only declarations (to be fully removed)
// ========================================================================

/**
 * parseInterfaceDeclaration - remove entirely
 */
void TsStrip::parseInterfaceDeclaration(int start) {
    // interface Name<T> extends ... { ... }
    if (token() == SyntaxKind::Identifier) {
        nextToken();
    }
    if (token() == SyntaxKind::LessThanToken) {
        skipTypeArguments();
    }
    if (parseOptional(SyntaxKind::ExtendsKeyword)) {
        while (!isEOF() && token() != SyntaxKind::OpenBraceToken) {
            nextToken();
        }
    }
    if (token() == SyntaxKind::OpenBraceToken) {
        skipBlock();
    }
    
    // Use getPrevTokenEnd() to avoid erasing comments after the declaration
    addReplacement(start, getPrevTokenEnd());
    fixASI(start, getPrevTokenEnd());
}

/**
 * parseTypeAliasDeclaration - remove entirely
 */
void TsStrip::parseTypeAliasDeclaration(int start) {
    // type Name<T> = Type;
    if (token() == SyntaxKind::Identifier) {
        nextToken();
    }
    if (token() == SyntaxKind::LessThanToken) {
        skipTypeArguments();
    }
    if (parseOptional(SyntaxKind::EqualsToken)) {
        skipType();
    }
    tryParseSemicolon();
    
    // Use getPrevTokenEnd() to avoid erasing comments after the declaration
    addReplacement(start, getPrevTokenEnd());
    fixASI(start, getPrevTokenEnd());
}

/**
 * parseEnumDeclaration - handle enum
 * isDeclare: if true, we're in a declare context so just erase it
 *            if false, throw error (enum is not supported in strip-only)
 */
void TsStrip::parseEnumDeclaration(int start, bool isDeclare) {
    if (!isDeclare) {
        throw std::runtime_error("TypeScript 'enum' is not supported in strip-only mode.");
    }
    // declare enum - skip the name and body, then erase
    if (token() == SyntaxKind::Identifier) {
        nextToken();
    }
    if (token() == SyntaxKind::OpenBraceToken) {
        skipBlock();
    }
    // Use getPrevTokenEnd() to avoid erasing comments after the declaration
    addReplacement(start, getPrevTokenEnd());
    fixASI(start, getPrevTokenEnd());
}

/**
 * parseModuleDeclaration - handle namespace/module
 * isDeclare: if true, we're in a declare context so just erase it
 *            if false, throw error for runtime namespace/module
 */
void TsStrip::parseModuleDeclaration(int start, bool isDeclare) {
    // namespace/module Name { ... } or declare module "foo" { ... }
    if (token() == SyntaxKind::Identifier || token() == SyntaxKind::StringLiteral) {
        nextToken();
    }
    
    if (token() == SyntaxKind::OpenBraceToken) {
        if (isDeclare) {
            // declare module/namespace - just skip the body and erase everything
            skipBlock();
        } else {
            // Runtime namespace/module with body - not supported
            throw std::runtime_error("TypeScript namespace/module with body is not supported in strip-only mode.");
        }
    }
    
    // Use getPrevTokenEnd() to avoid erasing comments after the declaration
    addReplacement(start, getPrevTokenEnd());
    fixASI(start, getPrevTokenEnd());
}

/**
 * parseDeclaration - for decorated declarations
 */
void TsStrip::parseDeclaration() {
    // Skip decorators first
    while (token() == SyntaxKind::AtToken) {
        nextToken();
        if (token() == SyntaxKind::Identifier) {
            nextToken();
        }
        while (token() == SyntaxKind::DotToken) {
            nextToken();
            if (token() == SyntaxKind::Identifier) {
                nextToken();
            }
        }
        if (token() == SyntaxKind::OpenParenToken) {
            skipBalanced(SyntaxKind::OpenParenToken, SyntaxKind::CloseParenToken);
        }
    }
    
    switch (token()) {
        case SyntaxKind::VarKeyword:
        case SyntaxKind::LetKeyword:
            parseVariableStatement();
            break;
        case SyntaxKind::ConstKeyword:
            // Check for const enum
            if (peekToken().kind == SyntaxKind::EnumKeyword) {
                int start = getNodePos();
                nextToken(); // const
                nextToken(); // enum
                parseEnumDeclaration(start, true);  // In declare context, just erase
            } else {
                parseVariableStatement();
            }
            break;
        case SyntaxKind::FunctionKeyword:
            parseFunctionDeclaration();
            break;
        case SyntaxKind::ClassKeyword:
            parseClassDeclaration();
            break;
        case SyntaxKind::InterfaceKeyword: {
            int start = getNodePos();
            nextToken();
            parseInterfaceDeclaration(start);
            break;
        }
        case SyntaxKind::TypeKeyword: {
            int start = getNodePos();
            nextToken();
            parseTypeAliasDeclaration(start);
            break;
        }
        case SyntaxKind::EnumKeyword: {
            int start = getNodePos();
            nextToken();
            parseEnumDeclaration(start, true);  // In declare context, just erase
            break;
        }
        case SyntaxKind::NamespaceKeyword:
        case SyntaxKind::ModuleKeyword: {
            int start = getNodePos();
            nextToken();
            parseModuleDeclaration(start, true);  // In declare context, just erase
            break;
        }
        case SyntaxKind::GlobalKeyword:
            nextToken();
            if (token() == SyntaxKind::OpenBraceToken) {
                skipBlock();
            }
            break;
        case SyntaxKind::AsyncKeyword:
            nextToken();
            if (token() == SyntaxKind::FunctionKeyword) {
                parseFunctionDeclaration();
            }
            break;
        case SyntaxKind::AbstractKeyword:
            nextToken();
            if (token() == SyntaxKind::ClassKeyword) {
                parseClassDeclaration();
            }
            break;
        default:
            while (!isEOF() && token() != SyntaxKind::SemicolonToken) {
                nextToken();
            }
            tryParseSemicolon();
            break;
    }
}

// ========================================================================
// Import/Export declarations
// ========================================================================

/**
 * parseImportDeclaration
 */
void TsStrip::parseImportDeclaration() {
    int start = getNodePos();
    nextToken(); // 'import'
    
    // import type - remove entire statement
    if (token() == SyntaxKind::TypeKeyword) {
        nextToken(); // consume 'type'
        // Handle: import type X from "..."
        // Handle: import type { A, B } from "..."
        // Handle: import type * as X from "..."
        if (token() == SyntaxKind::OpenBraceToken) {
            // import type { ... } - skip to matching brace
            skipBlock();
        } else if (token() == SyntaxKind::AsteriskToken) {
            // import type * as X
            nextToken(); // *
            parseOptional(SyntaxKind::AsKeyword);
            if (token() == SyntaxKind::Identifier) {
                nextToken();
            }
        } else if (token() == SyntaxKind::Identifier) {
            // import type X
            nextToken();
        }
        // from clause
        if (parseOptional(SyntaxKind::FromKeyword)) {
            if (token() == SyntaxKind::StringLiteral) {
                nextToken();
            }
        }
        // assert/with clause
        if (token() == SyntaxKind::AssertKeyword || token() == SyntaxKind::WithKeyword) {
            nextToken();
            if (token() == SyntaxKind::OpenBraceToken) {
                skipBlock();
            }
        }
        tryParseSemicolon();
        addReplacement(start, getNodePos());
        fixASI(start, getNodePos());
        return;
    }
    
    // Regular import - parse but check for type specifiers
    // import { type X, Y } from ...
    // import X, { type Y } from ...
    
    // Default import or namespace import or named imports
    if (token() == SyntaxKind::Identifier) {
        nextToken();
        if (parseOptional(SyntaxKind::CommaToken)) {
            // import default, { ... } or import default, * as ...
        }
    }
    
    if (token() == SyntaxKind::AsteriskToken) {
        // import * as name
        nextToken();
        parseExpected(SyntaxKind::AsKeyword);
        if (token() == SyntaxKind::Identifier) {
            nextToken();
        }
    } else if (token() == SyntaxKind::OpenBraceToken) {
        // Named imports
        nextToken();
        while (!isEOF() && token() != SyntaxKind::CloseBraceToken) {
            if (token() == SyntaxKind::TypeKeyword) {
                // type specifier - remove
                int typeStart = getNodePos();
                nextToken();
                while (!isEOF() && token() != SyntaxKind::CommaToken && token() != SyntaxKind::CloseBraceToken) {
                    nextToken();
                }
                int typeEnd = getNodePos();
                if (token() == SyntaxKind::CommaToken) {
                    nextToken();
                    typeEnd = getNodePos();
                }
                addReplacement(typeStart, typeEnd);
            } else {
                // Regular import specifier
                if (token() == SyntaxKind::Identifier) {
                    nextToken();
                }
                if (parseOptional(SyntaxKind::AsKeyword)) {
                    if (token() == SyntaxKind::Identifier) {
                        nextToken();
                    }
                }
                if (!parseOptional(SyntaxKind::CommaToken)) {
                    // Skip unexpected token to avoid infinite loop on syntax errors
                    if (token() != SyntaxKind::CloseBraceToken) {
                        nextToken();
                    }
                }
            }
        }
        parseExpected(SyntaxKind::CloseBraceToken);
    }
    
    // from clause
    if (parseOptional(SyntaxKind::FromKeyword)) {
        if (token() == SyntaxKind::StringLiteral) {
            nextToken();
        }
    } else if (token() == SyntaxKind::StringLiteral) {
        // import "module" (side-effect import)
        nextToken();
    }
    
    // assert/with clause
    if (token() == SyntaxKind::AssertKeyword || token() == SyntaxKind::WithKeyword) {
        nextToken();
        if (token() == SyntaxKind::OpenBraceToken) {
            skipBlock();
        }
    }
    
    tryParseSemicolon();
}

/**
 * parseExportDeclaration
 */
void TsStrip::parseExportDeclaration() {
    int start = getNodePos();
    nextToken(); // 'export'
    
    // export type ...
    // NOTE: do NOT scan until the next semicolon. Type literals can contain
    // semicolons inside `{ ... }`, e.g. `export type X = { a: any; };`.
    // We must parse enough structure to safely reach the statement terminator.
    if (token() == SyntaxKind::TypeKeyword) {
        nextToken(); // consume 'type'

        // export type Name = ...;
        if (token() == SyntaxKind::Identifier) {
            parseTypeAliasDeclaration(start);
            return;
        }

        // export type { A, B as C } [from "..."];  (type-only export => remove entire stmt)
        if (token() == SyntaxKind::OpenBraceToken) {
            nextToken();
            while (!isEOF() && token() != SyntaxKind::CloseBraceToken) {
                if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                    nextToken();
                }
                if (parseOptional(SyntaxKind::AsKeyword)) {
                    if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                        nextToken();
                    }
                }
                if (!parseOptional(SyntaxKind::CommaToken)) {
                    // Skip unexpected token to avoid infinite loop on syntax errors
                    if (token() != SyntaxKind::CloseBraceToken) {
                        nextToken();
                    }
                }
            }
            parseExpected(SyntaxKind::CloseBraceToken);
            if (parseOptional(SyntaxKind::FromKeyword)) {
                if (token() == SyntaxKind::StringLiteral) {
                    nextToken();
                }
            }
            tryParseSemicolon();

            addReplacement(start, getPrevTokenEnd());
            fixASI(start, getPrevTokenEnd());
            return;
        }

        // Fallback: conservatively skip the rest of the statement.
        while (!isEOF() && token() != SyntaxKind::SemicolonToken && !currentToken().hadLineBreak) {
            nextToken();
        }
        tryParseSemicolon();
        addReplacement(start, getPrevTokenEnd());
        fixASI(start, getPrevTokenEnd());
        return;
    }
    
    // export default
    if (parseOptional(SyntaxKind::DefaultKeyword)) {
        // export default class/function/expression
        if (token() == SyntaxKind::ClassKeyword) {
            parseClassDeclaration();
        } else if (token() == SyntaxKind::FunctionKeyword || 
                   (token() == SyntaxKind::AsyncKeyword && peekToken().kind == SyntaxKind::FunctionKeyword)) {
            parseFunctionDeclaration();
        } else if (token() == SyntaxKind::AbstractKeyword && peekToken().kind == SyntaxKind::ClassKeyword) {
            int absStart = getNodePos();
            nextToken();
            addReplacement(absStart, getNodePos());
            parseClassDeclaration();
        } else if (token() == SyntaxKind::InterfaceKeyword) {
            int ifStart = getNodePos();
            nextToken();
            parseInterfaceDeclaration(ifStart);
        } else {
            parseAssignmentExpressionOrHigher();
            tryParseSemicolon();
        }
        return;
    }
    
    switch (token()) {
        case SyntaxKind::VarKeyword:
        case SyntaxKind::LetKeyword:
        case SyntaxKind::ConstKeyword:
            if (peekToken().kind == SyntaxKind::EnumKeyword) {
                // export const enum
                int enumStart = getNodePos();
                nextToken(); // const
                nextToken(); // enum
                parseEnumDeclaration(enumStart);
            } else {
                parseVariableStatement();
            }
            break;
        case SyntaxKind::FunctionKeyword:
            parseFunctionDeclaration(start);  // Pass export start for overload erasure
            break;
        case SyntaxKind::ClassKeyword:
            parseClassDeclaration();
            break;
        case SyntaxKind::AsyncKeyword:
            if (peekToken().kind == SyntaxKind::FunctionKeyword) {
                parseFunctionDeclaration(start);  // Pass export start for overload erasure
            }
            break;
        case SyntaxKind::AbstractKeyword:
            if (peekToken().kind == SyntaxKind::ClassKeyword) {
                int absStart = getNodePos();
                nextToken();
                addReplacement(absStart, getNodePos());
                parseClassDeclaration();
            }
            break;
        case SyntaxKind::InterfaceKeyword: {
            nextToken();
            parseInterfaceDeclaration(start);
            break;
        }
        case SyntaxKind::TypeKeyword:
            if (peekToken().kind == SyntaxKind::Identifier) {
                nextToken();
                parseTypeAliasDeclaration(start);
            } else if (peekToken().kind == SyntaxKind::OpenBraceToken) {
                // export type { ... }
                nextToken();
                goto parseNamedExports;
            }
            break;
        case SyntaxKind::EnumKeyword: {
            nextToken();
            parseEnumDeclaration(start);
            break;
        }
        case SyntaxKind::NamespaceKeyword:
        case SyntaxKind::ModuleKeyword: {
            nextToken();
            parseModuleDeclaration(start);
            break;
        }
        case SyntaxKind::DeclareKeyword: {
            nextToken();
            parseDeclaration();
            addReplacement(start, getNodePos());
            break;
        }
        case SyntaxKind::AsteriskToken:
            // export * from "..."
            nextToken();
            if (parseOptional(SyntaxKind::AsKeyword)) {
                if (token() == SyntaxKind::Identifier) {
                    nextToken();
                }
            }
            parseExpected(SyntaxKind::FromKeyword);
            if (token() == SyntaxKind::StringLiteral) {
                nextToken();
            }
            tryParseSemicolon();
            break;
        case SyntaxKind::OpenBraceToken:
        parseNamedExports:
            // export { ... }
            nextToken();
            while (!isEOF() && token() != SyntaxKind::CloseBraceToken) {
                if (token() == SyntaxKind::TypeKeyword) {
                    // type specifier
                    int typeStart = getNodePos();
                    nextToken();
                    while (!isEOF() && token() != SyntaxKind::CommaToken && token() != SyntaxKind::CloseBraceToken) {
                        nextToken();
                    }
                    int typeEnd = getNodePos();
                    if (token() == SyntaxKind::CommaToken) {
                        nextToken();
                        typeEnd = getNodePos();
                    }
                    addReplacement(typeStart, typeEnd);
                } else {
                    if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                        nextToken();
                    }
                    if (parseOptional(SyntaxKind::AsKeyword)) {
                        if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                            nextToken();
                        }
                    }
                    if (!parseOptional(SyntaxKind::CommaToken)) {
                        // Skip unexpected token to avoid infinite loop on syntax errors
                        if (token() != SyntaxKind::CloseBraceToken) {
                            nextToken();
                        }
                    }
                }
            }
            parseExpected(SyntaxKind::CloseBraceToken);
            if (parseOptional(SyntaxKind::FromKeyword)) {
                if (token() == SyntaxKind::StringLiteral) {
                    nextToken();
                }
            }
            tryParseSemicolon();
            break;
        case SyntaxKind::EqualsToken:
            // export = expression
            nextToken();
            parseAssignmentExpressionOrHigher();
            tryParseSemicolon();
            break;
        default:
            break;
    }
}

// ========================================================================
// Type skipping (for types that need to be removed)
// ========================================================================

/**
 * skipType - skip over a type annotation
 */
void TsStrip::skipType() {
    // Handle leading | or & for union/intersection types that start with operator
    // e.g., type X = | A | B; or type Y = & A & B;
    while (token() == SyntaxKind::BarToken || token() == SyntaxKind::AmpersandToken) {
        nextToken();
    }
    
    // Handle leading modifiers
    while (token() == SyntaxKind::ReadonlyKeyword || 
           token() == SyntaxKind::UniqueKeyword ||
           token() == SyntaxKind::KeyOfKeyword ||
           token() == SyntaxKind::TypeOfKeyword ||
           token() == SyntaxKind::InferKeyword) {
        nextToken();
    }
    
    // Primary type
    switch (token()) {
        case SyntaxKind::VoidKeyword:
        case SyntaxKind::NeverKeyword:
        case SyntaxKind::AnyKeyword:
        case SyntaxKind::UnknownKeyword:
        case SyntaxKind::BooleanKeyword:
        case SyntaxKind::NumberKeyword:
        case SyntaxKind::StringKeyword:
        case SyntaxKind::SymbolKeyword:
        case SyntaxKind::BigIntKeyword:
        case SyntaxKind::ObjectKeyword:
        case SyntaxKind::UndefinedKeyword:
        case SyntaxKind::NullKeyword:
        case SyntaxKind::ThisKeyword:
            nextToken();
            break;
        case SyntaxKind::Identifier:
            nextToken();
            while (token() == SyntaxKind::DotToken) {
                nextToken();
                if (token() == SyntaxKind::Identifier) {
                    nextToken();
                }
            }
            if (token() == SyntaxKind::LessThanToken) {
                skipTypeArguments();
            }
            break;
        case SyntaxKind::OpenParenToken:
            skipBalanced(SyntaxKind::OpenParenToken, SyntaxKind::CloseParenToken);
            if (token() == SyntaxKind::EqualsGreaterThanToken) {
                nextToken();
                skipType();
            }
            break;
        case SyntaxKind::OpenBraceToken:
            skipBalanced(SyntaxKind::OpenBraceToken, SyntaxKind::CloseBraceToken);
            break;
        case SyntaxKind::OpenBracketToken:
            skipBalanced(SyntaxKind::OpenBracketToken, SyntaxKind::CloseBracketToken);
            break;
        case SyntaxKind::TypeOfKeyword:
            nextToken();
            if (token() == SyntaxKind::Identifier) {
                nextToken();
                while (token() == SyntaxKind::DotToken) {
                    nextToken();
                    if (token() == SyntaxKind::Identifier) {
                        nextToken();
                    }
                }
            }
            if (token() == SyntaxKind::LessThanToken) {
                skipTypeArguments();
            }
            break;
        case SyntaxKind::NewKeyword:
            nextToken();
            if (token() == SyntaxKind::OpenParenToken) {
                skipBalanced(SyntaxKind::OpenParenToken, SyntaxKind::CloseParenToken);
            }
            if (token() == SyntaxKind::EqualsGreaterThanToken) {
                nextToken();
                skipType();
            }
            break;
        case SyntaxKind::LessThanToken:
            skipTypeArguments();
            if (token() == SyntaxKind::OpenParenToken) {
                skipBalanced(SyntaxKind::OpenParenToken, SyntaxKind::CloseParenToken);
            }
            if (token() == SyntaxKind::EqualsGreaterThanToken) {
                nextToken();
                skipType();
            }
            break;
        case SyntaxKind::StringLiteral:
        case SyntaxKind::NumericLiteral:
        case SyntaxKind::BigIntLiteral:
        case SyntaxKind::TrueKeyword:
        case SyntaxKind::FalseKeyword:
            nextToken();
            break;
        case SyntaxKind::MinusToken:
            nextToken();
            if (token() == SyntaxKind::NumericLiteral || token() == SyntaxKind::BigIntLiteral) {
                nextToken();
            }
            break;
        case SyntaxKind::ImportKeyword:
            nextToken();
            if (token() == SyntaxKind::OpenParenToken) {
                skipBalanced(SyntaxKind::OpenParenToken, SyntaxKind::CloseParenToken);
            }
            if (token() == SyntaxKind::DotToken) {
                nextToken();
                if (token() == SyntaxKind::Identifier) {
                    nextToken();
                }
            }
            if (token() == SyntaxKind::LessThanToken) {
                skipTypeArguments();
            }
            break;
        case SyntaxKind::AssertsKeyword:
            nextToken();
            if (token() == SyntaxKind::ThisKeyword || token() == SyntaxKind::Identifier) {
                nextToken();
            }
            if (parseOptional(SyntaxKind::IsKeyword)) {
                skipType();
            }
            break;
        case SyntaxKind::NoSubstitutionTemplateLiteral:
        case SyntaxKind::TemplateHead:
            // Template literal type
            if (token() == SyntaxKind::NoSubstitutionTemplateLiteral) {
                nextToken();
            } else {
                nextToken();
                while (!isEOF()) {
                    skipType();
                    if (token() == SyntaxKind::CloseBraceToken) {
                        nextToken();
                    }
                    if (token() == SyntaxKind::TemplateTail) {
                        nextToken();
                        break;
                    } else if (token() == SyntaxKind::TemplateMiddle) {
                        nextToken();
                    } else {
                        break;
                    }
                }
            }
            break;
        default:
            // Error recovery: ensure progress when asked to skip a type.
            // If we hit a likely terminator, leave it for the caller.
            if (isKeyword(token())) {
                nextToken();
            } else {
                switch (token()) {
                    case SyntaxKind::CloseParenToken:
                    case SyntaxKind::CloseBracketToken:
                    case SyntaxKind::CloseBraceToken:
                    case SyntaxKind::CommaToken:
                    case SyntaxKind::SemicolonToken:
                    case SyntaxKind::ColonToken:
                    case SyntaxKind::QuestionToken:
                    case SyntaxKind::EqualsGreaterThanToken:
                    case SyntaxKind::EndOfFileToken:
                        break;
                    default:
                        nextToken();
                        break;
                }
            }
            break;
    }
    
    // Postfix type operators
    while (!isEOF()) {
        if (token() == SyntaxKind::OpenBracketToken) {
            nextToken();
            if (token() != SyntaxKind::CloseBracketToken) {
                skipType();
            }
            parseExpected(SyntaxKind::CloseBracketToken);
        } else if (token() == SyntaxKind::BarToken || token() == SyntaxKind::AmpersandToken) {
            nextToken();
            skipType();
        } else if (token() == SyntaxKind::ExtendsKeyword) {
            nextToken();
            skipType();
            if (parseOptional(SyntaxKind::QuestionToken)) {
                skipType();
                if (parseOptional(SyntaxKind::ColonToken)) {
                    skipType();
                }
            }
        } else if (token() == SyntaxKind::IsKeyword) {
            nextToken();
            skipType();
        } else {
            break;
        }
    }
}

/**
 * skipTypeArguments - skip <T, U, V>
 */
void TsStrip::skipTypeArguments() {
    if (token() != SyntaxKind::LessThanToken) {
        return;
    }
    
    int depth = 1;
    nextToken();
    
    while (!isEOF() && depth > 0) {
        switch (token()) {
            case SyntaxKind::LessThanToken:
                depth++;
                nextToken();
                break;
            case SyntaxKind::GreaterThanToken:
                depth--;
                nextToken();
                break;
            case SyntaxKind::GreaterThanGreaterThanToken:
                depth -= 2;
                if (depth < 0) depth = 0;
                nextToken();
                break;
            case SyntaxKind::GreaterThanGreaterThanGreaterThanToken:
                depth -= 3;
                if (depth < 0) depth = 0;
                nextToken();
                break;
            case SyntaxKind::OpenParenToken:
                skipBalanced(SyntaxKind::OpenParenToken, SyntaxKind::CloseParenToken);
                break;
            case SyntaxKind::OpenBraceToken:
                skipBalanced(SyntaxKind::OpenBraceToken, SyntaxKind::CloseBraceToken);
                break;
            case SyntaxKind::OpenBracketToken:
                skipBalanced(SyntaxKind::OpenBracketToken, SyntaxKind::CloseBracketToken);
                break;
            default:
                nextToken();
                break;
        }
    }
}

/**
 * skipJsxElement - skip JSX element like <div>...</div> or <Component />
 */
void TsStrip::skipJsxElement() {
    if (token() != SyntaxKind::LessThanToken) {
        return;
    }
    
    nextToken(); // consume <
    
    // Check for fragment <>...</>
    if (token() == SyntaxKind::GreaterThanToken) {
        nextToken(); // consume >
        // Skip children until </>
        while (!isEOF()) {
            if (token() == SyntaxKind::LessThanToken) {
                SyntaxKind next = peekToken().kind;
                if (next == SyntaxKind::SlashToken) {
                    // Closing fragment </>
                    nextToken(); // <
                    nextToken(); // /
                    parseExpected(SyntaxKind::GreaterThanToken);
                    return;
                }
                // Nested JSX element
                skipJsxElement();
            } else if (token() == SyntaxKind::OpenBraceToken) {
                // JSX expression {expr}
                nextToken();
                if (token() != SyntaxKind::CloseBraceToken) {
                    parseExpression();
                }
                parseExpected(SyntaxKind::CloseBraceToken);
            } else {
                nextToken();
            }
        }
        return;
    }
    
    // Parse tag name (identifier, namespaced, or member expression)
    if (token() == SyntaxKind::Identifier || isKeyword(token())) {
        nextToken();
        // Handle namespaced name or member expression: Foo.Bar, ns:name
        while (token() == SyntaxKind::DotToken || token() == SyntaxKind::ColonToken) {
            nextToken();
            if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                nextToken();
            }
        }
    }
    
    // Skip type arguments <Component<T> ...>
    if (token() == SyntaxKind::LessThanToken) {
        int start = getNodePos();
        skipTypeArguments();
        addReplacement(start, getNodePos());
    }
    
    // Parse attributes until > or />
    while (!isEOF() && token() != SyntaxKind::GreaterThanToken && token() != SyntaxKind::SlashToken) {
        if (token() == SyntaxKind::OpenBraceToken) {
            // Spread attribute {...props}
            nextToken();
            if (token() == SyntaxKind::DotDotDotToken) {
                nextToken();
            }
            parseExpression();
            parseExpected(SyntaxKind::CloseBraceToken);
        } else if (token() == SyntaxKind::Identifier || isKeyword(token())) {
            nextToken(); // attribute name
            if (token() == SyntaxKind::EqualsToken) {
                nextToken();
                if (token() == SyntaxKind::StringLiteral) {
                    nextToken();
                } else if (token() == SyntaxKind::OpenBraceToken) {
                    nextToken();
                    if (token() != SyntaxKind::CloseBraceToken) {
                        parseExpression();
                    }
                    parseExpected(SyntaxKind::CloseBraceToken);
                } else if (token() == SyntaxKind::LessThanToken) {
                    // JSX element as attribute value
                    skipJsxElement();
                }
            }
        } else {
            break;
        }
    }
    
    // Self-closing tag />
    if (token() == SyntaxKind::SlashToken) {
        nextToken();
        parseExpected(SyntaxKind::GreaterThanToken);
        return;
    }
    
    // Opening tag >
    parseExpected(SyntaxKind::GreaterThanToken);
    
    // Parse children until closing tag
    while (!isEOF()) {
        if (token() == SyntaxKind::LessThanToken) {
            SyntaxKind next = peekToken().kind;
            if (next == SyntaxKind::SlashToken) {
                // Closing tag </tag>
                nextToken(); // <
                nextToken(); // /
                // Skip tag name
                while (token() == SyntaxKind::Identifier || isKeyword(token()) || token() == SyntaxKind::DotToken) {
                    nextToken();
                }
                parseExpected(SyntaxKind::GreaterThanToken);
                return;
            }
            // Nested JSX element
            skipJsxElement();
        } else if (token() == SyntaxKind::OpenBraceToken) {
            // JSX expression {expr}
            nextToken();
            if (token() != SyntaxKind::CloseBraceToken) {
                parseExpression();
            }
            parseExpected(SyntaxKind::CloseBraceToken);
        } else {
            nextToken();
        }
    }
}

/**
 * skipBalanced - skip balanced tokens like { }, [ ], ( )
 */
void TsStrip::skipBalanced(SyntaxKind open, SyntaxKind close) {
    if (token() != open) {
        return;
    }
    
    int depth = 1;
    nextToken();
    
    while (!isEOF() && depth > 0) {
        if (token() == open) {
            depth++;
        } else if (token() == close) {
            depth--;
        }
        nextToken();
    }
}

// ========================================================================
// ASI fix
// ========================================================================

/**
 * fixASI - insert semicolon when needed after type erasure
 * @param start - start position of the erased type
 * @param end - end position of the erased type  
 * @param isStatement - true if this is a statement-level erasure (type/interface)
 *                      false if this is an expression-level erasure (as/satisfies)
 */
void TsStrip::fixASI(int start, int end, bool isStatement) {
    // After removing a type declaration, we may need to insert a semicolon
    // to prevent ASI hazards. Example:
    //   if (false) type Foo = string
    //   console.log("Hello");
    // Should become:
    //   if (false) ;
    //   console.log("Hello");
    // Without the semicolon, console.log would become the body of if.
    
    // Check if the next token could cause ASI problems
    SyntaxKind nextTok = token();
    
    // ASI hazard tokens for expressions: tokens that could change semantics
    // For as/satisfies, only certain tokens are problematic
    bool needsSemicolon = false;
    
    if (!isStatement) {
        // Expression-level erasure (as/satisfies)
        // Only insert semicolon for truly hazardous tokens
        switch (nextTok) {
            case SyntaxKind::OpenParenToken:    // Could be function call
            case SyntaxKind::OpenBracketToken:  // Could be array subscript
            case SyntaxKind::NoSubstitutionTemplateLiteral:
            case SyntaxKind::TemplateHead:      // Could be tagged template
                needsSemicolon = true;
                break;
            default:
                // +, -, / are safe because they just continue the expression
                needsSemicolon = false;
                break;
        }
    } else {
        // Statement-level erasure (type/interface declarations)
        // More tokens are problematic here
        switch (nextTok) {
            case SyntaxKind::OpenParenToken:    // Could be function call
            case SyntaxKind::OpenBracketToken:  // Could be array subscript
            case SyntaxKind::SlashToken:        // Could be division (or regex)
            case SyntaxKind::PlusToken:         // Could be unary +
            case SyntaxKind::MinusToken:        // Could be unary -
            case SyntaxKind::NoSubstitutionTemplateLiteral:
            case SyntaxKind::TemplateHead:      // Could be tagged template
                needsSemicolon = true;
                break;
            default:
                needsSemicolon = false;
                break;
        }
    }
    
    // For statement-level: also check if we're after a control flow statement
    // (like after if/while/for without braces)
    // In these cases, we ALWAYS need a semicolon to provide an empty statement
    if (isStatement && !needsSemicolon && start > 0) {
        // Check if there's no explicit statement after control flow
        // This is a simplified check - look for patterns like:
        // if (...) type Foo = ...
        // or:
        // while (false)
        //     interface X { }
        // We detect this by checking if the previous non-whitespace char is ')'
        int checkPos = start - 1;
        while (checkPos >= 0 && (m_src[checkPos] == L' ' || m_src[checkPos] == L'\t' || 
                                 m_src[checkPos] == L'\n' || m_src[checkPos] == L'\r')) {
            checkPos--;
        }
        if (checkPos >= 0 && m_src[checkPos] == L')') {
            needsSemicolon = true;
        }
    }
    
    if (needsSemicolon && start < (int)m_src.length()) {
        addOverwrite(start, L';');
    }
}

// ========================================================================
// Main entry points
// ========================================================================

void TsStrip::parseSourceFile() {
    int loopCount = 0;
    size_t lastIndex = m_tokenIndex;
    while (!isEOF()) {
        if (++loopCount > 100000) {
            std::cerr << "LOOP in parseSourceFile: token=" << (int)token() << " pos=" << getNodePos() << std::endl;
            throw std::runtime_error("Infinite loop in parseSourceFile");
        }
        size_t beforeIndex = m_tokenIndex;
        parseStatement();
        // If we didn't advance, force advance to prevent infinite loop
        if (m_tokenIndex == beforeIndex) {
            std::cerr << "WARNING: parseStatement didn't advance at token=" << (int)token() << " pos=" << getNodePos() << std::endl;
            nextToken();
        }
    }
}

exlib::wstring TsStrip::applyReplacements() {
    exlib::wstring result = m_src;
    
    // Apply replacements FIRST (replace with spaces, preserve newlines)
    // This must be done before overwrites so that overwrites can override
    for (const auto& r : m_replacements) {
        for (int i = r.start; i < r.end && i < (int)result.length(); i++) {
            char16_t ch = result[i];
            if (ch != L'\n' && ch != L'\r') {
                result[i] = L' ';
            }
        }
    }
    
    // Apply overwrites AFTER replacements (so semicolons aren't erased)
    for (const auto& ow : m_overwrites) {
        if (ow.pos >= 0 && ow.pos < (int)result.length()) {
            result[ow.pos] = ow.value;
        }
    }
    
    return result;
}

exlib::wstring TsStrip::strip() {
    parseSourceFile();
    return applyReplacements();
}

// Public API
exlib::string strip(const exlib::string& source) {
    // Convert UTF-8 to UTF-16 for internal processing
    exlib::wstring source16 = utf8to16String(source);
    
    Scanner scanner(source16);
    std::vector<Token> tokens = scanner.scanAllTokens();
    
    // Get processed text from scanner (comments already erased to spaces)
    const exlib::wstring& processedText = scanner.getProcessedText();
    
    TsStrip stripper(processedText, std::move(tokens));
    exlib::wstring result16 = stripper.strip();
    
    // Convert back to UTF-8
    return utf16to8String(result16);
}

} // namespace ts_strip
} // namespace fibjs
