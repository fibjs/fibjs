/**
 * @file syntax_kind.h
 * @brief Token and AST node type definitions (from TypeRunner)
 */

#pragma once

namespace fibjs {
namespace ts {

enum class SyntaxKind {
    Unknown,
    EndOfFileToken,
    
    // Trivia
    SingleLineCommentTrivia,
    MultiLineCommentTrivia,
    NewLineTrivia,
    WhitespaceTrivia,
    ShebangTrivia,
    ConflictMarkerTrivia,
    
    // Literals
    NumericLiteral,
    BigIntLiteral,
    StringLiteral,
    JsxText,
    JsxTextAllWhiteSpaces,
    RegularExpressionLiteral,
    NoSubstitutionTemplateLiteral,
    
    // Pseudo-literals
    TemplateHead,
    TemplateMiddle,
    TemplateTail,
    
    // Punctuation
    OpenBraceToken,      // {
    CloseBraceToken,     // }
    OpenParenToken,      // (
    CloseParenToken,     // )
    OpenBracketToken,    // [
    CloseBracketToken,   // ]
    DotToken,            // .
    DotDotDotToken,      // ...
    SemicolonToken,      // ;
    CommaToken,          // ,
    QuestionDotToken,    // ?.
    LessThanToken,       // <
    LessThanSlashToken,  // </
    GreaterThanToken,    // >
    LessThanEqualsToken,
    GreaterThanEqualsToken,
    EqualsEqualsToken,
    ExclamationEqualsToken,
    EqualsEqualsEqualsToken,
    ExclamationEqualsEqualsToken,
    EqualsGreaterThanToken, // =>
    PlusToken,
    MinusToken,
    AsteriskToken,
    AsteriskAsteriskToken,
    SlashToken,
    PercentToken,
    PlusPlusToken,
    MinusMinusToken,
    LessThanLessThanToken,
    GreaterThanGreaterThanToken,
    GreaterThanGreaterThanGreaterThanToken,
    AmpersandToken,
    BarToken,
    CaretToken,
    ExclamationToken,    // !
    TildeToken,
    AmpersandAmpersandToken,
    BarBarToken,
    QuestionToken,       // ?
    ColonToken,          // :
    AtToken,             // @
    QuestionQuestionToken, // ??
    BacktickToken,       // `
    HashToken,           // #
    
    // Assignments
    EqualsToken,         // =
    PlusEqualsToken,
    MinusEqualsToken,
    AsteriskEqualsToken,
    AsteriskAsteriskEqualsToken,
    SlashEqualsToken,
    PercentEqualsToken,
    LessThanLessThanEqualsToken,
    GreaterThanGreaterThanEqualsToken,
    GreaterThanGreaterThanGreaterThanEqualsToken,
    AmpersandEqualsToken,
    BarEqualsToken,
    BarBarEqualsToken,
    AmpersandAmpersandEqualsToken,
    QuestionQuestionEqualsToken,
    CaretEqualsToken,
    
    // Identifiers
    Identifier,
    PrivateIdentifier,
    
    // Reserved words
    BreakKeyword,
    CaseKeyword,
    CatchKeyword,
    ClassKeyword,
    ConstKeyword,
    ContinueKeyword,
    DebuggerKeyword,
    DefaultKeyword,
    DeleteKeyword,
    DoKeyword,
    ElseKeyword,
    EnumKeyword,
    ExportKeyword,
    ExtendsKeyword,
    FalseKeyword,
    FinallyKeyword,
    ForKeyword,
    FunctionKeyword,
    IfKeyword,
    ImportKeyword,
    InKeyword,
    InstanceOfKeyword,
    NewKeyword,
    NullKeyword,
    ReturnKeyword,
    SuperKeyword,
    SwitchKeyword,
    ThisKeyword,
    ThrowKeyword,
    TrueKeyword,
    TryKeyword,
    TypeOfKeyword,
    VarKeyword,
    VoidKeyword,
    WhileKeyword,
    WithKeyword,
    
    // Strict mode reserved words
    ImplementsKeyword,
    InterfaceKeyword,
    LetKeyword,
    PackageKeyword,
    PrivateKeyword,
    ProtectedKeyword,
    PublicKeyword,
    StaticKeyword,
    YieldKeyword,
    
    // Contextual keywords
    AbstractKeyword,
    AsKeyword,
    AssertsKeyword,
    AssertKeyword,
    AnyKeyword,
    AsyncKeyword,
    AwaitKeyword,
    BooleanKeyword,
    ConstructorKeyword,
    DeclareKeyword,
    GetKeyword,
    InferKeyword,
    IntrinsicKeyword,
    IsKeyword,
    KeyOfKeyword,
    ModuleKeyword,
    NamespaceKeyword,
    NeverKeyword,
    OutKeyword,
    ReadonlyKeyword,
    RequireKeyword,
    NumberKeyword,
    ObjectKeyword,
    SetKeyword,
    StringKeyword,
    SymbolKeyword,
    TypeKeyword,
    UndefinedKeyword,
    UniqueKeyword,
    UnknownKeyword,
    FromKeyword,
    GlobalKeyword,
    BigIntKeyword,
    OverrideKeyword,
    OfKeyword,
    SatisfiesKeyword,
    UsingKeyword,
    AccessorKeyword,
    
    // Markers for keyword ranges
    FirstKeyword = BreakKeyword,
    LastKeyword = AccessorKeyword,
    FirstReservedWord = BreakKeyword,
    LastReservedWord = WithKeyword,
};

// Helper functions
inline bool isKeyword(SyntaxKind kind) {
    return kind >= SyntaxKind::FirstKeyword && kind <= SyntaxKind::LastKeyword;
}

} // namespace ts
} // namespace fibjs
