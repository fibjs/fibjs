/**
 * @file ts_strip.cpp
 * @brief TypeScript type stripping implementation (UTF-8 version)
 * 
 * This implementation strictly follows TypeRunner's parser logic.
 * Key functions are ported from TypeRunner/src/parser2.h
 */

#include "ts_strip.h"
#include "Scanner.h"
#include <vector>
#include <algorithm>
#include <stdexcept>
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

// Single byte overwrite
struct Overwrite {
    int pos;
    uint8_t value;
    Overwrite(int p, uint8_t v) : pos(p), value(v) {}
};

// Result containing overwrites (replacements are now applied inline)
struct StripResult {
    std::vector<Overwrite> overwrites;
};

/**
 * TypeScript stripper - follows TypeRunner's parser logic (UTF-8 version)
 */
class TsStrip {
public:
    static constexpr int MAX_RECURSION_DEPTH = 500;

    TsStrip(uint8_t* src, size_t length, std::vector<Token> tokens)
        : m_src(src)
        , m_length(length)
        , m_tokens(std::move(tokens))
        , m_tokenIndex(0)
        , m_disallowInContext(false)
        , m_allowReturnTypeInArrowFunction(true)
        , m_recursionDepth(0)
    {
    }
    
    void strip();

    // Parse and return replacements/overwrites without applying them
    StripResult parse();
    
private:
    uint8_t* m_src;
    size_t m_length;
    std::vector<Token> m_tokens;
    size_t m_tokenIndex;
    bool m_disallowInContext;
    bool m_allowReturnTypeInArrowFunction;  // Like TypeScript's allowReturnTypeInArrowFunction parameter
    int m_recursionDepth;
    
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

        const int end = (int)m_length;
        while (true) {
            if (p >= end) {
                return false;
            }

            const uint8_t ch = m_src[p];
            if (ch == '\n' || ch == '\r') {
                return false;
            }

            if (inEscape) {
                inEscape = false;
            } else if (ch == '/' && !inCharacterClass) {
                p++;
                break;
            } else if (ch == '[') {
                inCharacterClass = true;
            } else if (ch == '\\') {
                inEscape = true;
            } else if (ch == ']') {
                inCharacterClass = false;
            }

            p++;
        }

        // Scan flags (ASCII identifier parts are enough for regexp flags)
        while (p < end) {
            const uint8_t ch = m_src[p];
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
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
        // Directly replace in-place instead of collecting spans
        for (int i = start; i < end && i < (int)m_length; i++) {
            uint8_t ch = m_src[i];
            if (ch != '\n' && ch != '\r') {
                m_src[i] = ' ';
            }
        }
    }
    
    void addOverwrite(int pos, uint8_t value) {
        m_overwrites.push_back(Overwrite(pos, value));
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
        return isBindingIdentifier(token());
    }
    
    // Check if a specific token kind can be used as binding identifier
    bool isBindingIdentifier(SyntaxKind t) const {
        if (t == SyntaxKind::Identifier) {
            return true;
        }
        // `global` is tokenized as GlobalKeyword but is valid as an identifier
        // in many JS/TS contexts (e.g., `class global {}` or `function f(global) {}`).
        if (t == SyntaxKind::GlobalKeyword) {
            return true;
        }
        // Contextual keywords (after LastReservedWord) can be used as identifiers
        // LastReservedWord is WithKeyword
        return t > SyntaxKind::WithKeyword && isKeyword(t);
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
    bool skipBalancedEx(SyntaxKind open, SyntaxKind close);
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
    void parseTypeAliasDeclaration(int start, bool isExported = false);
    void parseEnumDeclaration(int start, bool isDeclare = false);
    void parseModuleDeclaration(int start, bool isDeclare = false);
    void parseImportDeclaration();
    void parseExportDeclaration();
    void parseDeclaration();
    
    // Decorator
    void parseDecorator();
    
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
    void applyReplacements();
};

// ========================================================================
// Implementation - Expression parsing (from TypeRunner parser2.h)
// ========================================================================

/**
 * parseExpression - from TypeRunner parser2.h line 6314
 * Expression[in]:
 *     AssignmentExpression[in]
 *     Expression[in] , AssignmentExpression[in]
 * 
 * TypeScript always calls parseAssignmentExpressionOrHigher(true) inside parseExpression.
 * This is important for parenthesized expressions: even if we're in the true branch of
 * a ternary (where allowReturnTypeInArrowFunction=false), entering a parenthesized
 * expression resets it to true.
 */
void TsStrip::parseExpression() {
    // Reset allowReturnTypeInArrowFunction to true when entering a new expression context
    // This matches TypeScript's behavior: parseExpression always calls
    // parseAssignmentExpressionOrHigher(/*allowReturnTypeInArrowFunction*/ true)
    bool savedAllowReturnType = m_allowReturnTypeInArrowFunction;
    m_allowReturnTypeInArrowFunction = true;
    
    parseAssignmentExpressionOrHigher();
    while (token() == SyntaxKind::CommaToken) {
        nextToken();
        parseAssignmentExpressionOrHigher();
    }
    
    m_allowReturnTypeInArrowFunction = savedAllowReturnType;
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
    // TypeScript parser.ts line 5588: In true branch, allowReturnTypeInArrowFunction = false
    if (token() == SyntaxKind::QuestionToken) {
        nextToken();
        // In the true branch, disallow return type in arrow function to distinguish
        // `cond ? (a) : v => v` (ternary colon) from `cond ? (a): T => a : b` (return type)
        bool savedAllowReturnType = m_allowReturnTypeInArrowFunction;
        m_allowReturnTypeInArrowFunction = false;
        parseAssignmentExpressionOrHigher();
        m_allowReturnTypeInArrowFunction = savedAllowReturnType;
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
 * Also handles generic arrow functions like <T>(x) => x
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
            // Could be:
            // 1. <Type>expr - type assertion
            // 2. <T>(x) => expr - generic arrow function
            // 3. <T = D>(x) => expr - generic arrow function with default
            {
                // Look ahead to see if this could be a type assertion or generic arrow
                SyntaxKind next = peekToken().kind;
                // Type assertions/generics typically start with: <TypeName>, <typeof ...>, etc.
                // But NOT: <123 (number), <= (operator), << (shift)
                if (next == SyntaxKind::Identifier || isKeyword(next) ||
                    next == SyntaxKind::OpenParenToken || next == SyntaxKind::OpenBraceToken ||
                    next == SyntaxKind::OpenBracketToken) {
                    
                    // Save position for potential rollback
                    size_t startIndex = m_tokenIndex;
                    int start = getNodePos();
                    
                    // Skip the type parameter/argument list using skipTypeArguments
                    // which handles <T = Default, U extends V> correctly
                    skipTypeArguments();
                    
                    if (token() == SyntaxKind::OpenParenToken) {
                        // <T>(x) => expr - generic arrow function
                        // Strip the type parameters
                        addReplacement(start, getNodePos());
                        // Now parse the rest as arrow function
                        // parsePrimaryExpression will handle (x) => expr
                        parsePrimaryExpression();
                        parseCallExpressionRest();
                        return;
                    } else {
                        // Rollback and try as type assertion
                        m_tokenIndex = startIndex;
                        nextToken(); // consume <
                        
                        // Check if this is a const assertion <const>
                        // Const assertions like <const> should be preserved, not erased
                        if (token() == SyntaxKind::ConstKeyword) {
                            SyntaxKind nextAfterConst = peekToken().kind;
                            if (nextAfterConst == SyntaxKind::GreaterThanToken) {
                                // This is <const> - keep it, don't erase
                                // Rollback and treat as binary operator
                                m_tokenIndex = startIndex;
                                // Fall through to parseUpdateExpression
                            } else {
                                // <const Type> - treat as type assertion
                                skipType();
                                if (token() == SyntaxKind::GreaterThanToken) {
                                    nextToken(); // consume >
                                    addReplacement(start, getNodePos());
                                    parseUnaryExpressionOrHigher();
                                    return;
                                } else {
                                    m_tokenIndex = startIndex;
                                }
                            }
                        } else {
                            // Regular type assertion <Type>
                            skipType();
                            
                            if (token() == SyntaxKind::GreaterThanToken) {
                                // Successfully parsed <Type> - this is a type assertion
                                nextToken(); // consume >
                                addReplacement(start, getNodePos());
                                parseUnaryExpressionOrHigher();
                                return;
                            } else {
                                // Not a valid type assertion - rollback
                                m_tokenIndex = startIndex;
                                // Fall through to parseUpdateExpression
                            }
                        }
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
                if (token() == SyntaxKind::Identifier || token() == SyntaxKind::PrivateIdentifier || isKeyword(token())) {
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
                if (token() == SyntaxKind::Identifier || token() == SyntaxKind::PrivateIdentifier || isKeyword(token())) {
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
    if (token() != SyntaxKind::LessThanToken && token() != SyntaxKind::LessThanLessThanToken) {
        return false;
    }
    
    // Save position for potential rollback
    size_t startIndex = m_tokenIndex;
    int start = getNodePos();
    
    // Check if this is << (LessThanLessThanToken) - handle as depth=2
    int depth = (token() == SyntaxKind::LessThanLessThanToken) ? 2 : 1;
    
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
        next != SyntaxKind::StringLiteral &&
        next != SyntaxKind::NumericLiteral &&
        next != SyntaxKind::BigIntLiteral &&
        next != SyntaxKind::TrueKeyword &&
        next != SyntaxKind::FalseKeyword &&
        next != SyntaxKind::NullKeyword &&
        next != SyntaxKind::UndefinedKeyword &&
        next != SyntaxKind::VoidKeyword &&
        next != SyntaxKind::NeverKeyword &&
        next != SyntaxKind::UnknownKeyword &&
        next != SyntaxKind::AnyKeyword &&
        next != SyntaxKind::MinusToken &&
        next != SyntaxKind::NoSubstitutionTemplateLiteral &&
        next != SyntaxKind::TemplateHead &&
        next != SyntaxKind::BarToken &&  // Union type can start with |
        next != SyntaxKind::AmpersandToken &&  // Intersection type can start with &
        !isKeyword(next)) {
        return false;
    }
    
    // Try to skip the type argument list
    nextToken(); // consume < or <<
    
    while (!isEOF() && depth > 0) {
        SyntaxKind t = token();
        
        // Check for tokens that can't appear in type arguments
        // These indicate we've gone past a valid type argument list
        if (t == SyntaxKind::SemicolonToken ||
            t == SyntaxKind::EndOfFileToken ||
            // Binary logical operators can't appear in type arguments
            t == SyntaxKind::BarBarToken ||        // ||
            t == SyntaxKind::AmpersandAmpersandToken ||  // &&
            t == SyntaxKind::QuestionQuestionToken) {   // ??
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
    // 4. Assignment operators - instantiation expression: func<T> = x
    if (token() == SyntaxKind::OpenParenToken) {
        // func<T>() - this is definitely type arguments for function call
        addReplacement(start, getNodePos());
        return true;
    } else if (token() == SyntaxKind::NoSubstitutionTemplateLiteral ||
               token() == SyntaxKind::TemplateHead) {
        // func<T>`template` - tagged template with type args
        addReplacement(start, getNodePos());
        return true;
    } else if (token() == SyntaxKind::QuestionDotToken) {
        // func<T>?.() - instantiation expression with optional call
        addReplacement(start, getNodePos());
        return true;
    } else if (token() == SyntaxKind::SemicolonToken ||
               token() == SyntaxKind::CommaToken ||
               token() == SyntaxKind::CloseParenToken ||
               token() == SyntaxKind::CloseBraceToken ||
               token() == SyntaxKind::CloseBracketToken ||
               token() == SyntaxKind::EndOfFileToken ||
               token() == SyntaxKind::EqualsToken ||
               token() == SyntaxKind::PlusEqualsToken ||
               token() == SyntaxKind::MinusEqualsToken ||
               token() == SyntaxKind::AsteriskEqualsToken ||
               token() == SyntaxKind::SlashEqualsToken ||
               token() == SyntaxKind::PercentEqualsToken ||
               token() == SyntaxKind::AmpersandEqualsToken ||
               token() == SyntaxKind::BarEqualsToken ||
               token() == SyntaxKind::CaretEqualsToken ||
               token() == SyntaxKind::BarBarEqualsToken ||
               token() == SyntaxKind::AmpersandAmpersandEqualsToken ||
               token() == SyntaxKind::QuestionQuestionEqualsToken ||
               token() == SyntaxKind::InstanceOfKeyword ||
               token() == SyntaxKind::InKeyword ||
               currentToken().hadLineBreak) {
        // Instantiation expression: func<T>; or func<T>, or func<T>) or func<T> = x etc.
        // Also handles: func<T> instanceof x, func<T> in x
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
            case SyntaxKind::LessThanLessThanToken:
                // Could be type arguments like func<T>() or comparison operator x < y
                // Also handle << which scanner tokenizes as LessThanLessThanToken
                // Use conservative approach: only treat as type args if followed by (
                if (trySkipTypeArgumentsAndCall()) {
                    // Successfully parsed <TypeArgs> - type args have been replaced
                    if (token() == SyntaxKind::OpenParenToken) {
                        // func<T>() - parse call arguments
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
                        // Tagged template: func<T>`str`
                        parseTemplateLiteral();
                        continue;
                    }
                    // Instantiation expression: func<T> followed by other tokens
                    // Type args already replaced, just return to let binary parsing handle rest
                    return;
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
                if (token() == SyntaxKind::Identifier || token() == SyntaxKind::PrivateIdentifier || isKeyword(token())) {
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
                } else if (token() == SyntaxKind::Identifier || token() == SyntaxKind::PrivateIdentifier || isKeyword(token())) {
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
                        // Use parseExpression() to reset allowReturnTypeInArrowFunction like TypeScript does
                        reparsedAsExpression = true;
                        parseExpression();
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

                // Check for arrow function return type annotation: (params): ReturnType => body
                // This follows TypeScript's logic in parseParenthesizedArrowFunctionExpression
                //
                // Key insight from TypeScript (parser.ts line 5517-5528):
                // In ternary's true branch (allowReturnTypeInArrowFunction=false), if we see 
                // `: Type =>` pattern but no following `:` for the false branch, then the colon
                // was the ternary separator, not a return type annotation.
                //
                // Examples:
                //   `cond ? (a) : v => v`     - `:` is ternary separator, `v => v` is false branch
                //   `cond ? (a): T => a : b`  - `: T` is return type, `: b` is false branch
                if (token() == SyntaxKind::ColonToken) {
                    // Save position for potential rollback
                    size_t savedIndex = m_tokenIndex;
                    int start = getNodePos();
                    nextToken(); // consume ':'
                    skipType();
                    
                    if (token() == SyntaxKind::EqualsGreaterThanToken) {
                        // We have `: Type =>` pattern
                        if (!m_allowReturnTypeInArrowFunction) {
                            // We're in a context where return types are disallowed
                            // (e.g., true branch of a ternary at the top level).
                            // Need to check if there's a following `:` for the false branch.
                            // TypeScript parses the arrow body first, then checks.
                            int typeEnd = getNodePos();
                            nextToken(); // consume '=>'
                            if (token() == SyntaxKind::OpenBraceToken) {
                                parseBlock();
                            } else {
                                parseAssignmentExpressionOrHigher();
                            }
                            // After parsing arrow body, check if followed by `:` (false branch)
                            // In practice the conditional `:` may appear after one or more
                            // closing parens/brackets when the arrow is nested inside a call
                            // or parenthesized expression.
                            size_t lookaheadIndex = m_tokenIndex;
                            while (lookaheadIndex < m_tokens.size()) {
                                SyntaxKind lk = m_tokens[lookaheadIndex].kind;
                                if (lk == SyntaxKind::CloseParenToken || lk == SyntaxKind::CloseBracketToken) {
                                    lookaheadIndex++;
                                    continue;
                                }
                                break;
                            }

                            if (lookaheadIndex < m_tokens.size() && m_tokens[lookaheadIndex].kind == SyntaxKind::ColonToken) {
                                // Yes, there's a following `:` for the false branch
                                // So `: Type` was a return type annotation - add replacement
                                addReplacement(start, typeEnd);
                            }
                            // If no `:`, this means we parsed `cond ? (a) : v => v` incorrectly.
                            // The `:` was the ternary separator. But we can't rollback now since
                            // we've already parsed the arrow body. This is okay because:
                            // - For `cond ? (a) : v => v`, we parsed `v` as type, `v` as body
                            // - The original was: ternary's false branch is `v => v` 
                            // - We're not stripping anything (no addReplacement if no following `:`)
                            // - The parsed structure is equivalent for stripping purposes
                            return;  // Already parsed body above
                        }
                        // allowReturnTypeInArrowFunction is true - this is definitely a return type
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
                } else if (token() == SyntaxKind::GetKeyword || token() == SyntaxKind::SetKeyword ||
                           token() == SyntaxKind::AsyncKeyword) {
                    // get/set/async could be:
                    // 1. Accessor/async method: get foo() {}, async bar() {}
                    // 2. Property with get/set/async as name: { get: value, set: value }
                    // 3. async arrow function property: { callback: async (args) => body } - but only for async
                    
                    SyntaxKind keyword = token();
                    nextToken();
                    
                    // If followed by ':', it's a simple property like { get: value }
                    if (token() == SyntaxKind::ColonToken) {
                        nextToken();
                        parseAssignmentExpressionOrHigher();
                        if (!parseOptional(SyntaxKind::CommaToken)) {
                            break;
                        }
                        continue;
                    }
                    
                    // For async specifically: if followed by '(' or '<', it could be an async arrow function property
                    // Check if there's a method name. If not, treat as value expression.
                    if (keyword == SyntaxKind::AsyncKeyword && 
                        (token() == SyntaxKind::OpenParenToken || 
                         (token() == SyntaxKind::LessThanToken && peekToken().kind != SyntaxKind::GreaterThanToken))) {
                        // This is likely: callback: async (args) => body
                        // or: callback: async <T>(args) => body
                        // Already consumed 'async', now parse the rest as expression
                        parseAssignmentExpressionOrHigher();
                        if (!parseOptional(SyntaxKind::CommaToken)) {
                            break;
                        }
                        continue;
                    }
                    
                    if (token() == SyntaxKind::AsteriskToken) {
                        nextToken(); // async generator
                    }
                    // Property name (identifier, keyword, string, number, or computed)
                    if (token() == SyntaxKind::OpenBracketToken) {
                        nextToken();
                        parseExpression();
                        parseExpected(SyntaxKind::CloseBracketToken);
                    } else if (token() == SyntaxKind::Identifier || isKeyword(token()) 
                               || token() == SyntaxKind::StringLiteral 
                               || token() == SyntaxKind::NumericLiteral) {
                        nextToken();
                    }
                    // Type parameters for get/set/async method
                    if (token() == SyntaxKind::LessThanToken) {
                        int start = getNodePos();
                        skipTypeArguments();
                        addReplacement(start, getNodePos());
                    }
                    if (token() == SyntaxKind::OpenParenToken) {
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
                } else if (token() == SyntaxKind::AsteriskToken) {
                    // Generator method
                    nextToken();
                    // Property name (identifier, keyword, string, number, or computed)
                    if (token() == SyntaxKind::OpenBracketToken) {
                        nextToken();
                        parseExpression();
                        parseExpected(SyntaxKind::CloseBracketToken);
                    } else if (token() == SyntaxKind::Identifier || isKeyword(token()) 
                               || token() == SyntaxKind::StringLiteral 
                               || token() == SyntaxKind::NumericLiteral) {
                        nextToken();
                    }
                    // Type parameters for generator method
                    if (token() == SyntaxKind::LessThanToken) {
                        int start = getNodePos();
                        skipTypeArguments();
                        addReplacement(start, getNodePos());
                    }
                    if (token() == SyntaxKind::OpenParenToken) {
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
                } else if (token() == SyntaxKind::OpenBracketToken) {
                    // Computed property
                    nextToken();
                    parseExpression();
                    parseExpected(SyntaxKind::CloseBracketToken);
                    if (token() == SyntaxKind::ColonToken) {
                        nextToken();
                        parseAssignmentExpressionOrHigher();
                    } else if (token() == SyntaxKind::LessThanToken) {
                        // Computed property method with type params: { [key]<T>(params) { } }
                        int start = getNodePos();
                        skipTypeArguments();
                        addReplacement(start, getNodePos());
                        if (token() == SyntaxKind::OpenParenToken) {
                            parseParameters();
                            if (token() == SyntaxKind::ColonToken) {
                                int retStart = getNodePos();
                                nextToken();
                                skipType();
                                addReplacement(retStart, getNodePos());
                            }
                            if (token() == SyntaxKind::OpenBraceToken) {
                                parseBlock();
                            }
                        }
                    } else if (token() == SyntaxKind::OpenParenToken) {
                        // Computed property method: { [key](params) { } }
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
                } else if (token() == SyntaxKind::Identifier || isKeyword(token()) 
                           || token() == SyntaxKind::StringLiteral 
                           || token() == SyntaxKind::NumericLiteral) {
                    nextToken();
                    if (token() == SyntaxKind::ColonToken) {
                        nextToken();
                        parseAssignmentExpressionOrHigher();
                    } else if (token() == SyntaxKind::LessThanToken) {
                        // Method shorthand with type params: { method<T>(params) { } }
                        int start = getNodePos();
                        skipTypeArguments();
                        addReplacement(start, getNodePos());
                        if (token() == SyntaxKind::OpenParenToken) {
                            parseParameters();
                            if (token() == SyntaxKind::ColonToken) {
                                int retStart = getNodePos();
                                nextToken();
                                skipType();
                                addReplacement(retStart, getNodePos());
                            }
                            if (token() == SyntaxKind::OpenBraceToken) {
                                parseBlock();
                            }
                        }
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
                            // Use parseAssignmentExpressionOrHigher instead of parseExpression
                            // to avoid parsing comma-separated expressions as part of the arrow body.
                            parseAssignmentExpressionOrHigher();
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
                        // Use parseAssignmentExpressionOrHigher instead of parseExpression
                        // to avoid parsing comma-separated expressions as part of the arrow body.
                        // In object literals like { fn: async () => 1, x: 2 }, the comma should
                        // be a property separator, not part of the arrow function body.
                        parseAssignmentExpressionOrHigher();
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
        case SyntaxKind::AtToken:
            // Decorated class expression: @decorator class { }
            // Parse all decorators first
            while (token() == SyntaxKind::AtToken) {
                parseDecorator();
            }
            // Now parse the class expression
            if (token() == SyntaxKind::ClassKeyword) {
                parseClassDeclaration();
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
            // If there's a line break after 'interface', it should be treated as identifier (ASI)
            if (!peekToken().hadLineBreak) {
                int start = getNodePos();
                nextToken();
                parseInterfaceDeclaration(start);
                return;
            }
            break;
        }
        case SyntaxKind::TypeKeyword: {
            // Type alias name can be identifier or any keyword (like 'default', 'type', 'class', etc.)
            // If there's a line break after 'type', it should be treated as identifier (ASI)
            SyntaxKind next = peekToken().kind;
            if (!peekToken().hadLineBreak && (next == SyntaxKind::Identifier || isKeyword(next))) {
                int start = getNodePos();
                nextToken();
                parseTypeAliasDeclaration(start);
                return;
            }
            break;
        }
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
            // module in {} is using 'in' operator, not module declaration
            // If there's a line break after 'module', it should be treated as identifier (ASI)
            SyntaxKind next = peekToken().kind;
            if (!peekToken().hadLineBreak &&
                next != SyntaxKind::InKeyword &&  // 'module in {}' is 'in' operator, not module decl
                (next == SyntaxKind::Identifier || next == SyntaxKind::StringLiteral ||
                next == SyntaxKind::OpenBraceToken || next == SyntaxKind::GlobalKeyword || isKeyword(next))) {
                int start = getNodePos();
                nextToken();
                parseModuleDeclaration(start);
                return;
            }
            // Otherwise, module is being used as an identifier (e.g., module.exports)
            break;
        }
        case SyntaxKind::DeclareKeyword: {
            // Check if this is a declare declaration or just 'declare' as identifier
            // If there's a line break after 'declare', it should be treated as identifier (ASI)
            SyntaxKind next = peekToken().kind;
            if (!peekToken().hadLineBreak &&
                (next == SyntaxKind::VarKeyword || next == SyntaxKind::LetKeyword ||
                next == SyntaxKind::ConstKeyword || next == SyntaxKind::FunctionKeyword ||
                next == SyntaxKind::ClassKeyword || next == SyntaxKind::EnumKeyword ||
                next == SyntaxKind::InterfaceKeyword || next == SyntaxKind::TypeKeyword ||
                next == SyntaxKind::ModuleKeyword || next == SyntaxKind::NamespaceKeyword ||
                next == SyntaxKind::GlobalKeyword || next == SyntaxKind::AbstractKeyword ||
                next == SyntaxKind::AsyncKeyword)) {
                int start = getNodePos();
                nextToken();
                parseDeclaration();
                // Use getPrevTokenEnd() to avoid erasing comments after the declaration
                addReplacement(start, getPrevTokenEnd());
                fixASI(start, getPrevTokenEnd());
                return;
            }
            // Otherwise, 'declare' is being used as an identifier
            break;
        }
        case SyntaxKind::ImportKeyword:
            parseImportDeclaration();
            return;
        case SyntaxKind::ExportKeyword:
            parseExportDeclaration();
            return;
        case SyntaxKind::AbstractKeyword:
            // Only treat as class modifier if class keyword is on the same line
            // If there's a line break, 'abstract' is a standalone identifier (ASI)
            if (peekToken().kind == SyntaxKind::ClassKeyword && !peekToken().hadLineBreak) {
                int start = getNodePos();
                nextToken();
                addReplacement(start, getNodePos());
                parseClassDeclaration();
                return;
            }
            break;
        case SyntaxKind::GlobalKeyword: {
            // global { } - global scope augmentation, should be erased
            // e.g., global { interface Array<T> { x } }
            SyntaxKind next = peekToken().kind;
            if (next == SyntaxKind::OpenBraceToken) {
                int start = getNodePos();
                nextToken(); // consume 'global'
                skipBlock(); // skip the block
                addReplacement(start, getPrevTokenEnd());
                fixASI(start, getPrevTokenEnd());
                return;
            }
            // Otherwise, 'global' is being used as an identifier
            break;
        }
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
    // A labeled statement is: identifier ':' statement
    // We need to check if the current identifier is followed DIRECTLY by ':'
    // NOT: fn2() : void  (function call followed by colon - not a label!)
    // YES: myLabel: statement
    
    // Check for labeled statement BEFORE parsing the expression
    if (token() == SyntaxKind::Identifier && peekToken().kind == SyntaxKind::ColonToken) {
        // This is a labeled statement: label: statement
        nextToken(); // consume identifier
        nextToken(); // consume ':'
        parseStatement();
        return;
    }
    
    // Parse as expression statement
    parseExpression();
    if (!tryParseSemicolon()) {
        // Error: missing semicolon, but continue
    }
}

/**
 * parseBlock - from TypeRunner
 */
void TsStrip::parseBlock() {
    parseExpected(SyntaxKind::OpenBraceToken);
    while (!isEOF() && token() != SyntaxKind::CloseBraceToken) {
        size_t beforeIndex = m_tokenIndex;
        parseStatement();
        if (m_tokenIndex == beforeIndex) {
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
        // Use getNodePos() to include any rescanned partial token (like > from >=)
        addReplacement(start, getNodePos());
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
        // Property name can be any identifier OR keyword (including reserved words)
        // e.g., { enum: x, function: fn, class: c } is valid JavaScript
        // This is different from binding identifier which has restrictions
        if (token() == SyntaxKind::Identifier || isKeyword(token())) {
            nextToken();
            if (parseOptional(SyntaxKind::ColonToken)) {
                // propertyName: bindingName
                if (isBindingIdentifier()) {
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
            if (isBindingIdentifier()) {
                nextToken();
            } else if (token() == SyntaxKind::OpenBraceToken) {
                parseObjectBindingPattern();
            } else if (token() == SyntaxKind::OpenBracketToken) {
                parseArrayBindingPattern();
            }
        } else if (token() == SyntaxKind::StringLiteral || token() == SyntaxKind::NumericLiteral) {
            // String or number property name: { "prop": x } or { 123: x }
            nextToken();
            parseExpected(SyntaxKind::ColonToken);
            if (isBindingIdentifier()) {
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
        // Use isBindingIdentifier() to support contextual keywords
        if (isBindingIdentifier()) {
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
    
    // Name (can be identifier or contextual keyword like 'satisfies')
    if (isBindingIdentifier()) {
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
    
    bool isFirstParameter = true;
    
    while (!isEOF() && token() != SyntaxKind::CloseParenToken) {
        // Mark the start of this parameter (including any decorators)
        int paramStart = getNodePos();
        
        // Decorators (skip but don't erase - amaro also preserves parameter decorators)
        // Exception: decorators on 'this' parameter should be removed with the parameter
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
            // Parse decorator arguments as expressions to strip type annotations
            // inside them (e.g., @dec((x: T) => x))
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
        }
        
        // Check for `this` parameter (TypeScript only)
        // In TypeScript, `this` parameter can only be the first parameter
        // JavaScript does not allow `this` as a parameter name, so any first parameter
        // that is `this` must be a TypeScript this-parameter and should always be erased.
        if (isFirstParameter && token() == SyntaxKind::ThisKeyword) {
            nextToken();
            parseOptional(SyntaxKind::QuestionToken);
            if (token() == SyntaxKind::ColonToken) {
                nextToken();
                skipType();
            }
            // Remove this param including comma and any preceding decorators
            parseOptional(SyntaxKind::CommaToken);
            addReplacement(paramStart, getNodePos());
            // isFirstParameter remains true since this param was removed
            continue; // Skip to next parameter
        }
        
        // Modifiers (parameter properties in constructor)
        // Only treat as modifiers if followed by a parameter name (identifier or binding pattern)
        int modStart = getNodePos();
        bool hasModifier = false;
        while (token() == SyntaxKind::PublicKeyword || token() == SyntaxKind::PrivateKeyword ||
             token() == SyntaxKind::ProtectedKeyword || token() == SyntaxKind::ReadonlyKeyword ||
             token() == SyntaxKind::OverrideKeyword) {
            // Check if this is a modifier or a parameter name
            // It's a modifier only if followed by:
            // - another modifier keyword
            // - identifier (parameter name)
            // - binding pattern { or [
            // - ... (rest parameter)
            SyntaxKind next = peekToken().kind;
            if (next == SyntaxKind::PublicKeyword || next == SyntaxKind::PrivateKeyword ||
                next == SyntaxKind::ProtectedKeyword || next == SyntaxKind::ReadonlyKeyword ||
                next == SyntaxKind::OverrideKeyword || next == SyntaxKind::DotDotDotToken ||
                next == SyntaxKind::OpenBraceToken || next == SyntaxKind::OpenBracketToken ||
                isBindingIdentifier(next)) {
                // This is a modifier
                hasModifier = true;
                nextToken();
            } else {
                // This keyword is being used as a parameter name, not a modifier
                break;
            }
        }
        if (hasModifier) {
            addReplacement(modStart, getNodePos());
        }
        
        // Rest parameter
        parseOptional(SyntaxKind::DotDotDotToken);
        
        // Parameter name or binding pattern
        // Note: `this` can be used as parameter name in non-first position (unusual but valid)
        if (isBindingIdentifier() || token() == SyntaxKind::ThisKeyword) {
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
        
        // Mark that we've processed a parameter, so next one is not the first
        isFirstParameter = false;
        
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
    // Keep this narrow: treating contextual keywords as class names breaks
    // `class implements I {}` class expressions where `implements` starts a clause.
    if (token() == SyntaxKind::Identifier || token() == SyntaxKind::GlobalKeyword) {
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
        // Skip types until we reach class body {
        // Need to handle balanced < > and nested { } in type expressions
        int depth = 0;
        while (!isEOF()) {
            if (token() == SyntaxKind::OpenBraceToken && depth == 0) {
                break;
            }
            if (token() == SyntaxKind::LessThanToken) {
                depth++;
            } else if (token() == SyntaxKind::GreaterThanToken) {
                if (depth > 0) depth--;
            }
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
 * parseDecorator - parse a single decorator and its arguments
 * Properly handles type assertions in decorator arguments like @deco(x as T)
 */
void TsStrip::parseDecorator() {
    nextToken(); // consume @
    
    // Decorator name (possibly dotted: @namespace.decorator)
    // with optional non-null assertions between parts: @x!.y
    if (token() == SyntaxKind::Identifier) {
        nextToken();
    }
    
    // Loop to handle interleaved dots, non-null assertions, and type arguments
    while (true) {
        if (token() == SyntaxKind::DotToken) {
            nextToken();
            if (token() == SyntaxKind::Identifier) {
                nextToken();
            }
            continue;
        }
        
        // Remove non-null assertions (!) - can appear after any member access
        if (token() == SyntaxKind::ExclamationToken) {
            int start = getNodePos();
            nextToken();
            addReplacement(start, getPrevTokenEnd());
            continue;
        }
        
        // Remove type arguments: @decorator<T>()
        if (token() == SyntaxKind::LessThanToken || token() == SyntaxKind::LessThanLessThanToken) {
            int start = getNodePos();
            skipTypeArguments();
            addReplacement(start, getPrevTokenEnd());
            continue;
        }
        
        break;
    }
    
    // Decorator arguments
    if (token() == SyntaxKind::OpenParenToken) {
        nextToken();
        // Parse argument list as expressions to handle 'as' type assertions
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
        parseDecorator();
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
    
    // Track TS-only modifiers to detect duplicates used as property names
    bool hasPublic = false;
    bool hasPrivate = false;
    bool hasProtected = false;
    bool hasReadonly = false;
    bool hasOverride = false;
    
    // For ASI safety: track if a JS modifier keyword (static/get/set) is used as property name
    bool jsModifierAsPropertyName = false;
    
    while (true) {
        int modStart = getNodePos();
        switch (token()) {
            case SyntaxKind::PublicKeyword:
                // If already has public, this second 'public' is a property name
                if (hasPublic) {
                    // Don't set jsModifierAsPropertyName - the first modifier provides spacing
                    break; // Exit modifier loop
                }
                hasPublic = true;
                hasTsModifiers = true;
                nextToken();
                tsModifierRanges.push_back({modStart, getNodePos()});
                continue;
            case SyntaxKind::PrivateKeyword:
                // If already has private, this second 'private' is a property name
                if (hasPrivate) {
                    break; // Exit modifier loop
                }
                hasPrivate = true;
                hasTsModifiers = true;
                nextToken();
                tsModifierRanges.push_back({modStart, getNodePos()});
                continue;
            case SyntaxKind::ProtectedKeyword:
                // If already has protected, this second 'protected' is a property name
                if (hasProtected) {
                    break; // Exit modifier loop
                }
                hasProtected = true;
                hasTsModifiers = true;
                nextToken();
                tsModifierRanges.push_back({modStart, getNodePos()});
                continue;
            case SyntaxKind::ReadonlyKeyword:
                // If already has readonly, this second 'readonly' is a property name
                if (hasReadonly) {
                    break; // Exit modifier loop
                }
                hasReadonly = true;
                hasTsModifiers = true;
                nextToken();
                tsModifierRanges.push_back({modStart, getNodePos()});
                continue;
            case SyntaxKind::OverrideKeyword:
                // If already has override, this second 'override' is a property name
                if (hasOverride) {
                    break; // Exit modifier loop
                }
                nextToken();
                // Check if override is used as property name (followed by : ? ! = ; } or EOF)
                if (token() == SyntaxKind::ColonToken || token() == SyntaxKind::QuestionToken ||
                    token() == SyntaxKind::ExclamationToken || token() == SyntaxKind::EqualsToken ||
                    token() == SyntaxKind::SemicolonToken || token() == SyntaxKind::CloseBraceToken ||
                    isEOF()) {
                    // override is the property name, not a modifier
                    break; // Exit modifier loop
                }
                hasOverride = true;
                hasTsModifiers = true;
                tsModifierRanges.push_back({modStart, getNodePos()});
                continue;
            case SyntaxKind::AbstractKeyword:
                nextToken();
                // Check if abstract is used as property name (followed by : ? ! = ; } or EOF)
                if (token() == SyntaxKind::ColonToken || token() == SyntaxKind::QuestionToken ||
                    token() == SyntaxKind::ExclamationToken || token() == SyntaxKind::EqualsToken ||
                    token() == SyntaxKind::SemicolonToken || token() == SyntaxKind::CloseBraceToken ||
                    isEOF()) {
                    // abstract is the property name, not a modifier
                    break; // Exit modifier loop
                }
                hasTsModifiers = true;
                isAbstract = true;
                tsModifierRanges.push_back({modStart, getNodePos()});
                continue;
            case SyntaxKind::DeclareKeyword:
                hasTsModifiers = true;
                isDeclare = true;
                nextToken();
                tsModifierRanges.push_back({modStart, getNodePos()});
                continue;
            case SyntaxKind::StaticKeyword:
                // If already static, this second 'static' is a property name, not a modifier
                if (isStatic) {
                    jsModifierAsPropertyName = true;
                    break; // Exit modifier loop - static is the property name
                }
                isStatic = true;
                nextToken();
                // Check if static is used as property name (followed by : ? ! = ; } or EOF)
                if (token() == SyntaxKind::ColonToken || token() == SyntaxKind::QuestionToken ||
                    token() == SyntaxKind::ExclamationToken || token() == SyntaxKind::EqualsToken ||
                    token() == SyntaxKind::SemicolonToken || token() == SyntaxKind::CloseBraceToken ||
                    isEOF()) {
                    jsModifierAsPropertyName = true;
                    break; // Exit modifier loop - static is the property name
                }
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
    
    // Index signature [key: type]: type
    // Check BEFORE applying modifier replacements, because if it's an index signature,
    // we remove the entire member including modifiers without any ASI semicolon
    if (token() == SyntaxKind::OpenBracketToken) {
        size_t savedIndex = m_tokenIndex;
        int bracketStart = getNodePos();
        nextToken();
        
        // Check if index signature (has : inside)
        if (token() == SyntaxKind::Identifier) {
            nextToken();
            if (token() == SyntaxKind::ColonToken) {
                // Index signature - skip and remove entirely (including modifiers)
                while (!isEOF() && token() != SyntaxKind::CloseBracketToken) {
                    nextToken();
                }
                parseExpected(SyntaxKind::CloseBracketToken);
                if (token() == SyntaxKind::ColonToken) {
                    nextToken();
                    skipType();
                }
                tryParseSemicolon();
                // Remove from memberStart to include any modifiers like 'readonly'
                addReplacement(memberStart, getNodePos());
                return;
            }
        }
        // Not an index signature - restore position and continue
        m_tokenIndex = savedIndex;
    }
    
    // Danger check for stripping modifiers (SWC logic)
    // Check if removing TS modifiers would cause ASI issues
    bool isDangerous = false;
    if (hasTsModifiers && !isStatic && !hasDecorators) {
        SyntaxKind firstKeyToken = token();
        // Direct dangerous tokens
        if (firstKeyToken == SyntaxKind::OpenBracketToken || 
            firstKeyToken == SyntaxKind::AsteriskToken ||
            firstKeyToken == SyntaxKind::InKeyword || 
            firstKeyToken == SyntaxKind::InstanceOfKeyword) {
            isDangerous = true;
        }
        // Check for get/set followed by computed property
        // e.g., `public get [Symbol.toStringTag]()` -> `; get [Symbol.toStringTag]()`
        else if (firstKeyToken == SyntaxKind::GetKeyword || firstKeyToken == SyntaxKind::SetKeyword) {
            // Peek next token to see if it's a computed property
            SyntaxKind nextTok = peekToken().kind;
            if (nextTok == SyntaxKind::OpenBracketToken) {
                isDangerous = true;
            }
        }
    }
    
    // Strip TS-only modifiers (preserve static, async, accessor)
    for (const auto& range : tsModifierRanges) {
        addReplacement(range.first, range.second);
    }
    // Only insert dangerous semicolon if the member will NOT be entirely removed
    // (If member is abstract/declare, it will be fully erased at the end, so no semicolon needed)
    if (isDangerous && !tsModifierRanges.empty() && !isAbstract && !isDeclare) {
        addOverwrite(tsModifierRanges[0].first, ';');
    }
    
    // Computed property [expr]
    if (token() == SyntaxKind::OpenBracketToken) {
        nextToken();
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
        } else {
            // get/set is the property name, already consumed
            // Need semicolon for ASI safety
            jsModifierAsPropertyName = true;
        }
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
    
    // For ASI safety: if property name is static/get/set and type was erased,
    // we need a semicolon to prevent the property name from being parsed as a modifier
    // for the next member (e.g., "static: any\nfoo() {}" would become "static\nfoo() {}"
    // which parses as "static foo() {}" instead of property "static" + method "foo")
    // Note: We insert semicolon regardless of whether original code has one,
    // because the type annotation position needs the semicolon for correct ASI.
    // Example: "set: boolean;" -> "set;       ;" (semicolon at colon position)
    if (jsModifierAsPropertyName && hadTypeAnnotation && !hadInitializer) {
        addOverwrite(typeAnnotationStart, ';');
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
 * parseInterfaceDeclaration - convert to var declaration to preserve export
 * 
 * Transform: export interface Name<T> extends Base { ... }
 * To:        export var       Name                       ;
 * 
 * This preserves the name so it can be imported, while maintaining
 * character positions for source maps.
 */
void TsStrip::parseInterfaceDeclaration(int start) {
    // interface Name<T> extends ... { ... }
    // At this point, 'interface' keyword has been consumed, current token is the name
    // 'start' points to either 'interface' or 'export' (if exported)
    
    // Record position right after 'interface' keyword (before name)
    int nameStart = getNodePos();
    
    // Interface name can be an identifier or contextual keyword (like 'abstract', 'type', etc.)
    int nameEnd = nameStart;
    if (token() == SyntaxKind::Identifier || isKeyword(token())) {
        nextToken();
        nameEnd = getPrevTokenEnd();
    }
    
    // Record start of type parameters/extends/body (everything after name to be erased)
    int afterNameStart = getNodePos();
    
    if (token() == SyntaxKind::LessThanToken) {
        skipTypeArguments();
    }
    if (parseOptional(SyntaxKind::ExtendsKeyword)) {
        // Parse heritage clauses: extends Type1, Type2, ...
        do {
            skipType();
        } while (parseOptional(SyntaxKind::CommaToken));
    }
    if (token() == SyntaxKind::OpenBraceToken) {
        skipBlock();
    }
    
    int declEnd = getPrevTokenEnd();
    
    // Find the 'interface' keyword position
    // We need to locate it by scanning backwards from nameStart
    // 'interface' is 9 characters, 'var' is 3 characters
    // We'll replace 'interface' with 'var      ' (var + 6 spaces)
    
    // The 'interface' keyword ends at nameStart (after any whitespace)
    // We need to find where it starts
    int interfaceKeywordStart = start;
    int interfaceKeywordEnd = nameStart;
    
    // If start != interface position (e.g., start is 'export'), find interface keyword
    // by looking at the source text
    // We scan from start to find 'interface'
    const char* interfaceStr = "interface";
    int interfaceLen = 9;
    
    for (int pos = start; pos < nameStart; pos++) {
        bool match = true;
        for (int j = 0; j < interfaceLen && pos + j < (int)m_length; j++) {
            if (m_src[pos + j] != (uint8_t)interfaceStr[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            interfaceKeywordStart = pos;
            interfaceKeywordEnd = pos + interfaceLen;
            break;
        }
    }
    
    // Replace 'interface' with 'var' + spaces
    // Write 'var' at interfaceKeywordStart
    addOverwrite(interfaceKeywordStart, 'v');
    addOverwrite(interfaceKeywordStart + 1, 'a');
    addOverwrite(interfaceKeywordStart + 2, 'r');
    
    // Replace remaining 'erface' (6 chars) with spaces
    addReplacement(interfaceKeywordStart + 3, interfaceKeywordEnd);
    
    // Erase everything between name end and declaration end (type params, extends, body)
    if (afterNameStart < declEnd) {
        addReplacement(afterNameStart, declEnd);
    }
    
    // Add semicolon at the end position (overwrite last char of the erased range)
    if (declEnd > 0) {
        addOverwrite(declEnd - 1, ';');
    }
    
    fixASI(start, declEnd);
}

/**
 * parseTypeAliasDeclaration - handle type alias
 * 
 * If isExported=true (export type / export declare type):
 *   Transform: export type Name<T> = Type;
 *   To:        export var  Name          ;
 *   This preserves the name so it can be imported.
 * 
 * If isExported=false (local type alias):
 *   Completely erase the declaration.
 */
void TsStrip::parseTypeAliasDeclaration(int start, bool isExported) {
    // type Name<T> = Type;
    // At this point, 'type' keyword has been consumed, current token is the name
    // 'start' points to either 'type' or 'export' (if exported)
    
    // Record position right after 'type' keyword (before name)
    int nameStart = getNodePos();
    
    // Type alias name can be an identifier or contextual keyword
    int nameEnd = nameStart;
    if (token() == SyntaxKind::Identifier || isKeyword(token())) {
        nextToken();
        nameEnd = getPrevTokenEnd();
    }
    
    // Record start of type parameters/body (everything after name to be erased)
    int afterNameStart = getNodePos();
    
    if (token() == SyntaxKind::LessThanToken) {
        skipTypeArguments();
    }
    if (parseOptional(SyntaxKind::EqualsToken)) {
        skipType();
    }
    tryParseSemicolon();
    
    int declEnd = getPrevTokenEnd();
    
    if (!isExported) {
        // Not exported - completely erase
        addReplacement(start, declEnd);
        fixASI(start, declEnd);
        return;
    }
    
    // Exported - convert 'type' to 'var '
    // Find the 'type' keyword position
    // 'type' is 4 characters, 'var' is 3 characters
    // We'll replace 'type' with 'var ' (var + 1 space)
    const char* typeStr = "type";
    int typeLen = 4;
    int typeKeywordStart = start;
    
    for (int pos = start; pos < nameStart; pos++) {
        bool match = true;
        for (int j = 0; j < typeLen && pos + j < (int)m_length; j++) {
            if (m_src[pos + j] != (uint8_t)typeStr[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            typeKeywordStart = pos;
            break;
        }
    }
    
    // Replace 'type' with 'var '
    addOverwrite(typeKeywordStart, 'v');
    addOverwrite(typeKeywordStart + 1, 'a');
    addOverwrite(typeKeywordStart + 2, 'r');
    addOverwrite(typeKeywordStart + 3, ' ');
    
    // Erase everything between name end and declaration end (type params, = Type, etc.)
    if (afterNameStart < declEnd) {
        addReplacement(afterNameStart, declEnd);
    }
    
    // Add semicolon at the end position
    if (declEnd > 0) {
        addOverwrite(declEnd - 1, ';');
    }
    
    fixASI(start, declEnd);
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
    // Name can be dotted: namespace Foo.Bar.Baz { ... }
    // "global" is a keyword but can be used as namespace name: namespace global { }
    if (token() == SyntaxKind::Identifier || token() == SyntaxKind::GlobalKeyword || isKeyword(token())) {
        nextToken();
        // Handle dotted names like Foo.Bar.Baz
        while (token() == SyntaxKind::DotToken) {
            nextToken(); // consume .
            if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                nextToken();
            }
        }
    } else if (token() == SyntaxKind::StringLiteral) {
        nextToken();
    }
    
    if (token() == SyntaxKind::OpenBraceToken) {
        // Check if the body is empty (uninstantiated)
        // If empty, just erase it; if non-empty and not declare, throw error
        if (peekToken().kind == SyntaxKind::CloseBraceToken) {
            // Empty body - uninstantiated namespace, just erase
            skipBlock();
        } else if (isDeclare) {
            // declare module/namespace - just skip the body and erase everything
            skipBlock();
        } else {
            // Runtime namespace/module with body - not supported
            throw std::runtime_error("TypeScript namespace/module with body is not supported in strip-only mode.");
        }
    } else {
        // No body - consume optional semicolon (e.g., "declare module 'foo';")
        tryParseSemicolon();
    }
    
    // When called from parseStatement directly (for standalone namespace/module),
    // apply replacement and fixASI.
    // When called from parseDeclaration (for declare namespace/module),
    // the caller will handle replacement and fixASI.
    if (!isDeclare) {
        // Use getPrevTokenEnd() to avoid erasing comments after the declaration
        addReplacement(start, getPrevTokenEnd());
        fixASI(start, getPrevTokenEnd());
    }
}

/**
 * parseDeclaration - for decorated declarations
 */
void TsStrip::parseDeclaration() {
    // Skip decorators first
    while (token() == SyntaxKind::AtToken) {
        parseDecorator();
    }
    
    switch (token()) {
        case SyntaxKind::ExportKeyword:
            parseExportDeclaration();
            break;
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
    
    // import type - check if 'type' is a modifier or a default import name
    if (token() == SyntaxKind::TypeKeyword) {
        // Check what follows 'type'
        // - import type from './type.js' - 'type' is the default import name (JS)
        // - import type from from '...' - 'type' is a modifier, 'from' is the import name (TS)
        // - import type X from '...' - 'type' is a modifier (TS)
        // - import type { A } from '...' - 'type' is a modifier (TS)
        // - import type * as X from '...' - 'type' is a modifier (TS)
        SyntaxKind next = peekToken().kind;
        if (next == SyntaxKind::CommaToken) {
            // import type, { ... } or import type, * as ...
            // 'type' is a default import name - treat as regular import
            nextToken(); // consume 'type' as identifier
            parseOptional(SyntaxKind::CommaToken);
            // Continue to parse named imports below
        } else if (next == SyntaxKind::FromKeyword) {
            // Could be:
            // - import type from '...' - 'type' is default import, 'from' is keyword
            // - import type from from '...' - 'type' is modifier, first 'from' is import name
            // - import type from = require('..') - 'type' is modifier, 'from' is import alias
            // Need to look two tokens ahead
            nextToken(); // consume 'type'
            nextToken(); // consume first 'from'
            if (token() == SyntaxKind::FromKeyword || token() == SyntaxKind::EqualsToken) {
                // import type from from '...' or import type from = require('..')
                // 'type' is modifier - This is a type-only import, erase entire statement
                // Skip to end of statement
                while (!isEOF() && token() != SyntaxKind::SemicolonToken && !currentToken().hadLineBreak) {
                    nextToken();
                }
                tryParseSemicolon();
                addReplacement(start, getNodePos());
                fixASI(start, getNodePos());
                return;
            } else {
                // import type from '...' - 'type' is default import name
                // 'from' already consumed, just need module specifier
                goto parse_from_clause;
            }
        } else {
            // 'type' is a modifier - remove entire statement
            nextToken(); // consume 'type'
            // Handle: import type X from "..."
            // Handle: import type { A, B } from "..."
            // Handle: import type * as X from "..."
            // Handle: import type defer * as X from "..." (with defer modifier)
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
                // import type X or import type defer ...
                nextToken();
                // Check if there's more after (import type defer * as X)
                if (token() == SyntaxKind::CommaToken) {
                    nextToken();
                }
                if (token() == SyntaxKind::AsteriskToken) {
                    nextToken(); // *
                    parseOptional(SyntaxKind::AsKeyword);
                    if (token() == SyntaxKind::Identifier) {
                        nextToken();
                    }
                } else if (token() == SyntaxKind::OpenBraceToken) {
                    skipBlock();
                }
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
    } else if (token() == SyntaxKind::Identifier) {
        // Default import
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
                // Check if 'type' is a modifier or an identifier
                // If followed by ',', '}', or 'as', it's an identifier (JS variable named 'type')
                // If followed by another identifier, it's a type modifier (TypeScript)
                SyntaxKind next = peekToken().kind;
                if (next == SyntaxKind::CommaToken || next == SyntaxKind::CloseBraceToken || next == SyntaxKind::AsKeyword) {
                    // 'type' is an identifier, treat as regular import
                    nextToken();
                    if (parseOptional(SyntaxKind::AsKeyword)) {
                        if (token() == SyntaxKind::Identifier) {
                            nextToken();
                        }
                    }
                    parseOptional(SyntaxKind::CommaToken);
                } else {
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
                }
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
    
parse_from_clause:
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
    
    // export as namespace X; - UMD global namespace declaration, preserve it
    if (token() == SyntaxKind::AsKeyword) {
        // This is `export as namespace X;` syntax for UMD modules
        // It's a runtime declaration, not a type-only construct, so preserve it
        nextToken(); // consume 'as'
        if (parseOptional(SyntaxKind::NamespaceKeyword)) {
            if (token() == SyntaxKind::Identifier) {
                nextToken();
            }
        }
        tryParseSemicolon();
        return;
    }
    
    // export type ...
    // NOTE: do NOT scan until the next semicolon. Type literals can contain
    // semicolons inside `{ ... }`, e.g. `export type X = { a: any; };`.
    // We must parse enough structure to safely reach the statement terminator.
    if (token() == SyntaxKind::TypeKeyword) {
        nextToken(); // consume 'type'

        // export type Name = ...;
        if (token() == SyntaxKind::Identifier) {
            parseTypeAliasDeclaration(start, true);  // isExported=true
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
            // Handle assert/with clause
            if (token() == SyntaxKind::AssertKeyword || token() == SyntaxKind::WithKeyword) {
                nextToken();
                if (token() == SyntaxKind::OpenBraceToken) {
                    skipBalanced(SyntaxKind::OpenBraceToken, SyntaxKind::CloseBraceToken);
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
            parseFunctionDeclaration(start);  // Pass export start for overload erasure
        } else if (token() == SyntaxKind::AbstractKeyword && peekToken().kind == SyntaxKind::ClassKeyword) {
            int absStart = getNodePos();
            nextToken();
            addReplacement(absStart, getNodePos());
            parseClassDeclaration();
        } else if (token() == SyntaxKind::InterfaceKeyword) {
            // export default interface X { } - erase entirely including "export default"
            nextToken();
            parseInterfaceDeclaration(start);  // Use export start to erase everything
        } else if (token() == SyntaxKind::TypeKeyword && 
                   (peekToken().kind == SyntaxKind::Identifier || isKeyword(peekToken().kind))) {
            // export default type X = ... -> export default var  X ;
            nextToken();
            parseTypeAliasDeclaration(start, true);  // isExported=true
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
                parseTypeAliasDeclaration(start, true);  // isExported=true
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
            // For interface and type, handle specially to preserve export var
            if (token() == SyntaxKind::InterfaceKeyword) {
                // export declare interface X { } -> export var       X ;
                // Erase 'declare ' (between export and interface)
                addReplacement(start + 6, getNodePos()); // 6 = strlen("export")
                int ifaceStart = getNodePos();
                nextToken();
                parseInterfaceDeclaration(ifaceStart);
            } else if (token() == SyntaxKind::TypeKeyword) {
                // export declare type X = ... -> export var  X ;
                // Erase 'declare ' (between export and type)
                addReplacement(start + 6, getNodePos()); // 6 = strlen("export")
                int typeStart = getNodePos();
                nextToken();
                parseTypeAliasDeclaration(typeStart, true);  // isExported=true
            } else {
                parseDeclaration();
                addReplacement(start, getNodePos());
            }
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
            // Handle assert/with clause
            if (token() == SyntaxKind::AssertKeyword || token() == SyntaxKind::WithKeyword) {
                nextToken();
                if (token() == SyntaxKind::OpenBraceToken) {
                    skipBalanced(SyntaxKind::OpenBraceToken, SyntaxKind::CloseBraceToken);
                }
            }
            tryParseSemicolon();
            break;
        case SyntaxKind::OpenBraceToken:
        parseNamedExports:
            // export { ... }
            nextToken();
            while (!isEOF() && token() != SyntaxKind::CloseBraceToken) {
                if (token() == SyntaxKind::TypeKeyword) {
                    // Check if 'type' is a modifier or an identifier
                    // If followed by ',', '}', or 'as', it's an identifier (JS variable named 'type')
                    // If followed by another identifier, it's a type modifier (TypeScript)
                    SyntaxKind next = peekToken().kind;
                    if (next == SyntaxKind::CommaToken || next == SyntaxKind::CloseBraceToken || next == SyntaxKind::AsKeyword) {
                        // 'type' is an identifier, treat as regular export
                        nextToken();
                        if (parseOptional(SyntaxKind::AsKeyword)) {
                            if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                                nextToken();
                            }
                        }
                        parseOptional(SyntaxKind::CommaToken);
                    } else {
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
                    }
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
            // Handle assert/with clause
            if (token() == SyntaxKind::AssertKeyword || token() == SyntaxKind::WithKeyword) {
                nextToken();
                if (token() == SyntaxKind::OpenBraceToken) {
                    skipBalanced(SyntaxKind::OpenBraceToken, SyntaxKind::CloseBraceToken);
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
    
    // Handle leading modifiers (but typeof needs special handling for member access)
    while (token() == SyntaxKind::ReadonlyKeyword || 
           token() == SyntaxKind::UniqueKeyword ||
           token() == SyntaxKind::KeyOfKeyword ||
           token() == SyntaxKind::InferKeyword ||
           token() == SyntaxKind::AbstractKeyword) {
        nextToken();
    }
    
    // Handle typeof specially - it can be followed by qualified names like typeof this.foo
    if (token() == SyntaxKind::TypeOfKeyword) {
        nextToken(); // consume 'typeof'
        // typeof can be followed by: identifier, this, or import(...)
        // Check for import(...) first since 'import' is also a keyword
        if (token() == SyntaxKind::ImportKeyword) {
            // typeof import("module")
            nextToken();
            if (token() == SyntaxKind::OpenParenToken) {
                skipBalanced(SyntaxKind::OpenParenToken, SyntaxKind::CloseParenToken);
            }
            while (token() == SyntaxKind::DotToken) {
                nextToken();
                if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                    nextToken();
                } else {
                    break;
                }
            }
            // Handle type arguments for instantiation expressions
            if (token() == SyntaxKind::LessThanToken || token() == SyntaxKind::LessThanLessThanToken) {
                skipTypeArguments();
            }
        } else if (token() == SyntaxKind::Identifier || token() == SyntaxKind::ThisKeyword || isKeyword(token())) {
            nextToken();
            // Handle member access: typeof this.foo.bar or typeof x.y.z
            while (token() == SyntaxKind::DotToken) {
                nextToken();
                if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                    nextToken();
                } else {
                    break;
                }
            }
            // Handle type arguments
            if (token() == SyntaxKind::LessThanToken || token() == SyntaxKind::LessThanLessThanToken) {
                skipTypeArguments();
            }
        }
        // Go to postfix handling below (skip the switch)
        goto handlePostfix;
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
                // After dot, can be identifier or any keyword as property name
                if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                    nextToken();
                }
            }
            // Check for type arguments - can be < or << (when followed by another <)
            if (token() == SyntaxKind::LessThanToken || token() == SyntaxKind::LessThanLessThanToken) {
                skipTypeArguments();
            }
            break;
        case SyntaxKind::OpenParenToken: {
            // Parse parenthesized type
            // Two cases:
            // 1. (Type) - parenthesized type grouping, e.g., (() => T)
            // 2. (param: Type) => ReturnType - function type parameters
            // 
            // To distinguish: if parentheses contain "identifier :" pattern at the start,
            // it's function parameters, and outer => is part of this function type.
            // Otherwise, it's a grouping, and we should not parse outer =>.
            
            // Save position to peek at content
            size_t savedIndex = m_tokenIndex;
            nextToken(); // consume '('
            
            bool isFunctionParams = false;
            bool contentStartsWithParen = false;
            
            // Check if this looks like function parameters: starts with identifier (or keyword) followed by :
            // or is empty () or has rest parameter ...
            if (token() == SyntaxKind::CloseParenToken) {
                // Empty () - if followed by =>, it's a function type () => T
                isFunctionParams = true;
            } else if (token() == SyntaxKind::DotDotDotToken) {
                // Rest parameter like (...args: T[])
                isFunctionParams = true;
            } else if (token() == SyntaxKind::OpenParenToken) {
                // Content starts with ( - likely a grouped type like (() => T)
                contentStartsWithParen = true;
                isFunctionParams = false;
            } else if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                // Check if followed by : or ? or , (parameter with type annotation or separator)
                SyntaxKind next = peekToken().kind;
                if (next == SyntaxKind::ColonToken || next == SyntaxKind::QuestionToken ||
                    next == SyntaxKind::CommaToken) {
                    // identifier: or identifier? or identifier, - likely function params
                    isFunctionParams = true;
                }
            }
            
            // Restore and skip the balanced parens
            m_tokenIndex = savedIndex;
            bool hasArrowInside = skipBalancedEx(SyntaxKind::OpenParenToken, SyntaxKind::CloseParenToken);
            
            // Now check for =>
            if (token() == SyntaxKind::EqualsGreaterThanToken) {
                if (isFunctionParams) {
                    // This is definitely function type: (params) => ReturnType
                    nextToken();
                    skipType();
                } else if (contentStartsWithParen && hasArrowInside) {
                    // Content starts with ( and has arrow inside - likely (() => T)
                    // The outer => is not part of this type
                    // Don't consume the outer =>
                } else {
                    // For other cases like (T) => ..., treat as function type
                    nextToken();
                    skipType();
                }
            }
            break;
        }
        case SyntaxKind::OpenBraceToken:
            skipBalanced(SyntaxKind::OpenBraceToken, SyntaxKind::CloseBraceToken);
            break;
        case SyntaxKind::OpenBracketToken:
            skipBalanced(SyntaxKind::OpenBracketToken, SyntaxKind::CloseBracketToken);
            break;
        case SyntaxKind::TypeOfKeyword:
            nextToken();
            // typeof expr.member.access or typeof this.member
            if (token() == SyntaxKind::Identifier || token() == SyntaxKind::ThisKeyword) {
                nextToken();
                while (token() == SyntaxKind::DotToken) {
                    nextToken();
                    if (token() == SyntaxKind::Identifier) {
                        nextToken();
                    } else {
                        break;
                    }
                }
            }
            if (token() == SyntaxKind::LessThanToken) {
                skipTypeArguments();
            }
            break;
        case SyntaxKind::NewKeyword:
            nextToken();
            // Handle type parameters: new <T>() => type
            if (token() == SyntaxKind::LessThanToken) {
                skipTypeArguments();
            }
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
            // Handle dotted access: import("foo").bar.baz (bar can be keyword like 'default')
            while (token() == SyntaxKind::DotToken) {
                nextToken();
                if (token() == SyntaxKind::Identifier || isKeyword(token())) {
                    nextToken();
                } else {
                    break;
                }
            }
            // Handle type arguments: import("foo").Bar<T> or import("foo").Bar<<T>...>
            if (token() == SyntaxKind::LessThanToken || token() == SyntaxKind::LessThanLessThanToken) {
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
    
handlePostfix:
    // Postfix type operators
    while (!isEOF()) {
        if (token() == SyntaxKind::OpenBracketToken) {
            // Array type suffix like T[] or indexed access type T[K]
            // But if [ is on a new line, it's likely the start of a new statement
            // e.g., declare const x: any\n[].push(...) - the [] is array literal, not type suffix
            if (currentToken().hadLineBreak) {
                break;
            }
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
    if (token() != SyntaxKind::LessThanToken && token() != SyntaxKind::LessThanLessThanToken) {
        return;
    }
    
    int depth = 1;
    
    // Handle << token - it's two < tokens merged by the scanner
    // We need to increase depth by 2 and stay on current token (will be handled as second <)
    if (token() == SyntaxKind::LessThanLessThanToken) {
        depth = 2;
        nextToken();
    } else {
        nextToken();
    }
    
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
            // Handle >= >>= >>>= tokens by "rescanning" them
            // When we need to consume just the > part, we modify the token to keep the = part
            case SyntaxKind::GreaterThanEqualsToken:
                // >= is > followed by =. Consume one > (depth--), leave = for next token.
                depth--;
                if (depth <= 0) {
                    // Rescan: change >= to = by adjusting token pos
                    m_tokens[m_tokenIndex].kind = SyntaxKind::EqualsToken;
                    m_tokens[m_tokenIndex].pos += 1; // Skip the '>'
                    depth = 0;
                } else {
                    nextToken();
                }
                break;
            case SyntaxKind::GreaterThanGreaterThanEqualsToken:
                // >>= is >> followed by =. Consume two > (depth-=2).
                depth -= 2;
                if (depth < 0) {
                    // Only needed one >, rescan to keep >= 
                    m_tokens[m_tokenIndex].kind = SyntaxKind::GreaterThanEqualsToken;
                    m_tokens[m_tokenIndex].pos += 1; // Skip one '>'
                    depth = 0;
                } else if (depth == 0) {
                    // Needed exactly two >, rescan to keep =
                    m_tokens[m_tokenIndex].kind = SyntaxKind::EqualsToken;
                    m_tokens[m_tokenIndex].pos += 2; // Skip '>>'
                } else {
                    nextToken();
                }
                break;
            case SyntaxKind::GreaterThanGreaterThanGreaterThanEqualsToken:
                // >>>= is >>> followed by =. Consume three > (depth-=3).
                depth -= 3;
                if (depth == -2) {
                    // Only needed one >, rescan to keep >>=
                    m_tokens[m_tokenIndex].kind = SyntaxKind::GreaterThanGreaterThanEqualsToken;
                    m_tokens[m_tokenIndex].pos += 1; // Skip one '>'
                    depth = 0;
                } else if (depth == -1) {
                    // Needed two >, rescan to keep >=
                    m_tokens[m_tokenIndex].kind = SyntaxKind::GreaterThanEqualsToken;
                    m_tokens[m_tokenIndex].pos += 2; // Skip '>>'
                    depth = 0;
                } else if (depth == 0) {
                    // Needed exactly three >, rescan to keep =
                    m_tokens[m_tokenIndex].kind = SyntaxKind::EqualsToken;
                    m_tokens[m_tokenIndex].pos += 3; // Skip '>>>'
                } else {
                    nextToken();
                }
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
            // Handle template strings (both `...` and `...${...}...`)
            case SyntaxKind::NoSubstitutionTemplateLiteral:
                // Simple template string without ${}, just skip it
                nextToken();
                break;
            case SyntaxKind::TemplateHead:
                // Template string with ${}, need to recursively process expressions inside
                nextToken(); // consume template head
                while (!isEOF()) {
                    // Recursively parse expression inside ${}, which will handle types
                    parseExpression();
                    
                    // After expression, we should be at TemplateMiddle or TemplateTail
                    if (token() == SyntaxKind::TemplateMiddle) {
                        // More template parts, continue
                        nextToken();
                    } else if (token() == SyntaxKind::TemplateTail) {
                        // End of template string
                        nextToken();
                        break;
                    } else {
                        // Unexpected token, bail out
                        break;
                    }
                }
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
    skipBalancedEx(open, close);
}

/**
 * skipBalancedEx - skip balanced tokens and return whether arrow was found inside
 * @return true if '=>' was found at the top level (depth = 1) inside the balanced region
 *         This is used to detect if a parenthesized type contains a function type
 *         e.g., in `(() => T)`, the arrow is at depth 2 (inside inner parens)
 *         while in `(T) =>` the arrow would be at depth 1 (direct child)
 */
bool TsStrip::skipBalancedEx(SyntaxKind open, SyntaxKind close) {
    if (token() != open) {
        return false;
    }
    
    int depth = 1;
    bool hasArrowAtTopLevel = false;
    nextToken();
    
    while (!isEOF() && depth > 0) {
        if (token() == open) {
            depth++;
        } else if (token() == close) {
            depth--;
        } else if (token() == SyntaxKind::EqualsGreaterThanToken && depth == 1) {
            // Arrow found at the top level (depth 1) means this is a function type
            // e.g., (x) => T where the arrow is directly inside the parens
            hasArrowAtTopLevel = true;
        }
        nextToken();
    }
    return hasArrowAtTopLevel;
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
    
    bool needsSemicolon = false;
    
    // For statement-level: check if we're the body of a control flow statement
    // (like if/while/for without braces)
    // In these cases, we ALWAYS need a semicolon to provide an empty statement
    // Example: while (false) interface X { } -> while (false) ;
    // This check is done BEFORE hadLineBreak because control flow body needs semicolon
    // regardless of line breaks
    if (isStatement && start > 0) {
        // Find the token before 'start'
        size_t idx = 0;
        for (size_t i = 0; i < m_tokens.size(); i++) {
            if (m_tokens[i].pos >= start) {
                idx = i;
                break;
            }
            idx = i + 1;
        }
        
        if (idx > 0) {
            const Token& prevTok = m_tokens[idx - 1];
            if (prevTok.kind == SyntaxKind::CloseParenToken) {
                // Check if this ')' belongs to a control flow statement
                // by looking for matching '(' and then the keyword before it
                int parenDepth = 1;
                size_t openParenIdx = idx - 1;
                while (openParenIdx > 0) {
                    openParenIdx--;
                    if (m_tokens[openParenIdx].kind == SyntaxKind::CloseParenToken) {
                        parenDepth++;
                    } else if (m_tokens[openParenIdx].kind == SyntaxKind::OpenParenToken) {
                        parenDepth--;
                        if (parenDepth == 0) {
                            break;
                        }
                    }
                }
                
                if (parenDepth == 0 && openParenIdx > 0) {
                    // Check the token before the open paren
                    const Token& beforeParen = m_tokens[openParenIdx - 1];
                    if (beforeParen.kind == SyntaxKind::IfKeyword ||
                        beforeParen.kind == SyntaxKind::WhileKeyword ||
                        beforeParen.kind == SyntaxKind::ForKeyword ||
                        beforeParen.kind == SyntaxKind::WithKeyword) {
                        needsSemicolon = true;
                    }
                }
            }
        }
    }
    
    // If not a control flow body, check hadLineBreak for regular ASI
    if (!needsSemicolon) {
        // Only need semicolon if next token has line break (SWC logic)
        if (!currentToken().hadLineBreak) {
            return;
        }
        
        // Check if the next token could cause ASI problems
        SyntaxKind nextTok = token();
        
        // ASI hazard tokens for expressions: tokens that could change semantics
        // For as/satisfies, only certain tokens are problematic
        
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
    }
    
    if (needsSemicolon && start < (int)m_length) {
        // SWC logic: check the token before 'start'
        // 1. If there's no token before (first statement), don't insert semicolon
        // 2. If the token before is a semicolon, overwrite it with ';' (to preserve it after replacement)
        // 3. Otherwise, insert semicolon at start
        
        // Find the token that is at or starts before 'start'
        size_t idx = 0;
        for (size_t i = 0; i < m_tokens.size(); i++) {
            if (m_tokens[i].pos >= start) {
                idx = i;
                break;
            }
            idx = i + 1;
        }
        
        // If this is the first token (no token before), skip
        if (idx == 0) {
            return;
        }
        
        // Check if the token before is a semicolon
        // If so, we need to overwrite it with ';' to preserve it after replacement
        // (because the semicolon might be within the replaced range)
        // HOWEVER, if the semicolon is at the END of the erased range (the statement's own semicolon),
        // and there's NO code before the erased range, we should NOT preserve it
        // because it would leave a dangling semicolon at the start of the file.
        if (m_tokens[idx - 1].kind == SyntaxKind::SemicolonToken) {
            int semicolonPos = m_tokens[idx - 1].pos;
            // Only preserve the semicolon if it's NOT the trailing semicolon of an erased statement
            // at the start of the file or after other erased statements
            if (semicolonPos >= start && semicolonPos < end) {
                // Semicolon is within erased range
                // Check if there's any actual code BEFORE the erased range
                bool hasCodeBefore = false;
                for (size_t i = 0; i < idx - 1; i++) {
                    int tokPos = m_tokens[i].pos;
                    int tokEnd = (i + 1 < m_tokens.size()) ? m_tokens[i + 1].pos : m_length;
                    // Only consider tokens that end BEFORE the erased range starts
                    if (tokEnd <= start) {
                        // Check if this token is actual code (not comment/whitespace)
                        SyntaxKind k = m_tokens[i].kind;
                        if (k != SyntaxKind::SingleLineCommentTrivia &&
                            k != SyntaxKind::MultiLineCommentTrivia) {
                            hasCodeBefore = true;
                            break;
                        }
                    }
                }
                if (hasCodeBefore) {
                    addOverwrite(semicolonPos, ';');
                }
                // If no code before, don't preserve the semicolon
                return;
            } else {
                // Semicolon is outside erased range, always preserve
                addOverwrite(semicolonPos, ';');
                return;
            }
        }
        
        addOverwrite(start, ';');
    }
}

// ========================================================================
// Main entry points
// ========================================================================

void TsStrip::parseSourceFile() {
    while (!isEOF()) {
        size_t beforeIndex = m_tokenIndex;
        parseStatement();
        // If we didn't advance, force advance to prevent infinite loop
        if (m_tokenIndex == beforeIndex) {
            nextToken();
        }
    }
}

void TsStrip::applyReplacements() {
    // Replacements are now applied inline in addReplacement()
    // Only apply overwrites here (so semicolons aren't erased by inline replacements)
    for (const auto& ow : m_overwrites) {
        if (ow.pos >= 0 && ow.pos < (int)m_length) {
            m_src[ow.pos] = ow.value;
        }
    }
}

void TsStrip::strip() {
    parseSourceFile();
    applyReplacements();
}

StripResult TsStrip::parse() {
    parseSourceFile();
    return StripResult { std::move(m_overwrites) };
}

// Public API
exlib::string strip(const exlib::string& source) {
    // Make a mutable copy
    exlib::string result = source;
    
    Scanner scanner((uint8_t*)result.data(), result.length());
    std::vector<Token> tokens = scanner.scanAllTokens();
    
    TsStrip stripper((uint8_t*)result.data(), result.length(), std::move(tokens));
    stripper.strip();
    
    return result;
}

void stripInPlace(uint8_t* data, size_t length) {
    Scanner scanner(data, length);
    std::vector<Token> tokens = scanner.scanAllTokens();
    
    TsStrip stripper(data, length, std::move(tokens));
    stripper.strip();
}

} // namespace ts_strip
} // namespace fibjs
