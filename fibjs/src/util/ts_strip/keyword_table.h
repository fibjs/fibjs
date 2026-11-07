// Generated table: a minimal perfect hash over the TS keyword set.
//
// Both ends of the name go into the key: the first six bytes and the last
// six, which together are the whole name for anything up to 12 bytes. They
// are mixed into 64 bits and the length is compared on the way out, so the
// only way to hit a keyword slot is to spell that keyword.
//
// Do not shrink this to a few bytes plus the length. That variant is not
// injective - "return" and "region" both reduce to 0x066e6572 - and an
// identifier read as a keyword silently changes how the file is parsed.
// ("region" as a parameter name really did break real code.)
//
//     hi   = little-endian uint48 of s[0..6)   (zero padded)
//     lo   = little-endian uint48 of s[n-6..n) (zero padded)
//     key  = (hi * 0x9e3779b97f4a7c15) ^ (lo * 0xc2b2ae3d27d4eb4f)
//     slot = (key * 5257846158329281261) >> 39 & 255
//
// See getIdentifierToken() in Scanner.cpp.

#pragma once

#include <stdint.h>
#include "syntax_kind.h"

namespace fibjs {
namespace ts {

struct KeywordEntry {
    uint64_t hi;  // first six bytes of the name
    uint64_t lo;  // last six bytes of the name
    uint8_t len;
    SyntaxKind kind;
};

static const KeywordEntry kKeywordTable[256] = {
    { 0x63696c627570ull, 0x63696c627570ull, 6, SyntaxKind::PublicKeyword }, // public
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x0065736c6166ull, 0x0065736c6166ull, 5, SyntaxKind::FalseKeyword }, // false
    { 0x746176697270ull, 0x657461766972ull, 7, SyntaxKind::PrivateKeyword }, // private
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x617274736261ull, 0x746361727473ull, 8, SyntaxKind::AbstractKeyword }, // abstract
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x0074736e6f63ull, 0x0074736e6f63ull, 5, SyntaxKind::ConstKeyword }, // const
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x007265666e69ull, 0x007265666e69ull, 5, SyntaxKind::InferKeyword }, // infer
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x737365636361ull, 0x726f73736563ull, 8, SyntaxKind::AccessorKeyword }, // accessor
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x6365746f7270ull, 0x646574636574ull, 9, SyntaxKind::ProtectedKeyword }, // protected
    { 0x00000000666full, 0x00000000666full, 2, SyntaxKind::OfKeyword }, // of
    { 0x000000797274ull, 0x000000797274ull, 3, SyntaxKind::TryKeyword }, // try
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x67616b636170ull, 0x6567616b6361ull, 7, SyntaxKind::PackageKeyword }, // package
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x000064696f76ull, 0x000064696f76ull, 4, SyntaxKind::VoidKeyword }, // void
    { 0x656c75646f6dull, 0x656c75646f6dull, 6, SyntaxKind::ModuleKeyword }, // module
    { 0x00000077656eull, 0x00000077656eull, 3, SyntaxKind::NewKeyword }, // new
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x000000007369ull, 0x000000007369ull, 2, SyntaxKind::IsKeyword }, // is
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x000068746977ull, 0x000068746977ull, 4, SyntaxKind::WithKeyword }, // with
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x7265626d756eull, 0x7265626d756eull, 6, SyntaxKind::NumberKeyword }, // number
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x646e65747865ull, 0x73646e657478ull, 7, SyntaxKind::ExtendsKeyword }, // extends
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x000000726176ull, 0x000000726176ull, 3, SyntaxKind::VarKeyword }, // var
    { 0x747265737361ull, 0x747265737361ull, 6, SyntaxKind::AssertKeyword }, // assert
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x000000746573ull, 0x000000746573ull, 3, SyntaxKind::SetKeyword }, // set
    { 0x667369746173ull, 0x736569667369ull, 9, SyntaxKind::SatisfiesKeyword }, // satisfies
    { 0x00656c696877ull, 0x00656c696877ull, 5, SyntaxKind::WhileKeyword }, // while
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x666f65707974ull, 0x666f65707974ull, 6, SyntaxKind::TypeOfKeyword }, // typeof
    { 0x6c61626f6c67ull, 0x6c61626f6c67ull, 6, SyntaxKind::GlobalKeyword }, // global
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x726975716572ull, 0x657269757165ull, 7, SyntaxKind::RequireKeyword }, // require
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x00726576656eull, 0x00726576656eull, 5, SyntaxKind::NeverKeyword }, // never
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x007265707573ull, 0x007265707573ull, 5, SyntaxKind::SuperKeyword }, // super
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x6974636e7566ull, 0x6e6f6974636eull, 8, SyntaxKind::FunctionKeyword }, // function
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x000065736c65ull, 0x000065736c65ull, 4, SyntaxKind::ElseKeyword }, // else
    { 0x747265737361ull, 0x737472657373ull, 7, SyntaxKind::AssertsKeyword }, // asserts
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x74726f706d69ull, 0x74726f706d69ull, 6, SyntaxKind::ImportKeyword }, // import
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x000000746567ull, 0x000000746567ull, 3, SyntaxKind::GetKeyword }, // get
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x007373616c63ull, 0x007373616c63ull, 5, SyntaxKind::ClassKeyword }, // class
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x000000006f64ull, 0x000000006f64ull, 2, SyntaxKind::DoKeyword }, // do
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x61656c6f6f62ull, 0x6e61656c6f6full, 7, SyntaxKind::BooleanKeyword }, // boolean
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x00000074756full, 0x00000074756full, 3, SyntaxKind::OutKeyword }, // out
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x00006d756e65ull, 0x00006d756e65ull, 4, SyntaxKind::EnumKeyword }, // enum
    { 0x000065736163ull, 0x000065736163ull, 4, SyntaxKind::CaseKeyword }, // case
    { 0x636974617473ull, 0x636974617473ull, 6, SyntaxKind::StaticKeyword }, // static
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x000000726f66ull, 0x000000726f66ull, 3, SyntaxKind::ForKeyword }, // for
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x6e6972746e69ull, 0x6369736e6972ull, 9, SyntaxKind::IntrinsicKeyword }, // intrinsic
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x6574656c6564ull, 0x6574656c6564ull, 6, SyntaxKind::DeleteKeyword }, // delete
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x00646c656979ull, 0x00646c656979ull, 5, SyntaxKind::YieldKeyword }, // yield
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x000000796e61ull, 0x000000796e61ull, 3, SyntaxKind::AnyKeyword }, // any
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x7274736e6f63ull, 0x726f74637572ull, 11, SyntaxKind::ConstructorKeyword }, // constructor
    { 0x000000006669ull, 0x000000006669ull, 2, SyntaxKind::IfKeyword }, // if
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x657571696e75ull, 0x657571696e75ull, 6, SyntaxKind::UniqueKeyword }, // unique
    { 0x00676e697375ull, 0x00676e697375ull, 5, SyntaxKind::UsingKeyword }, // using
    { 0x6c6c616e6966ull, 0x796c6c616e69ull, 7, SyntaxKind::FinallyKeyword }, // finally
    { 0x776f6e6b6e75ull, 0x6e776f6e6b6eull, 7, SyntaxKind::UnknownKeyword }, // unknown
    { 0x007469617761ull, 0x007469617761ull, 5, SyntaxKind::AwaitKeyword }, // await
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x000000006e69ull, 0x000000006e69ull, 2, SyntaxKind::InKeyword }, // in
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x676775626564ull, 0x726567677562ull, 8, SyntaxKind::DebuggerKeyword }, // debugger
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x686374697773ull, 0x686374697773ull, 6, SyntaxKind::SwitchKeyword }, // switch
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x6c6f626d7973ull, 0x6c6f626d7973ull, 6, SyntaxKind::SymbolKeyword }, // symbol
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x746e69676962ull, 0x746e69676962ull, 6, SyntaxKind::BigIntKeyword }, // bigint
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x696665646e75ull, 0x64656e696665ull, 9, SyntaxKind::UndefinedKeyword }, // undefined
    { 0x6e69746e6f63ull, 0x65756e69746eull, 8, SyntaxKind::ContinueKeyword }, // continue
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x69727265766full, 0x656469727265ull, 8, SyntaxKind::OverrideKeyword }, // override
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x000000007361ull, 0x000000007361ull, 2, SyntaxKind::AsKeyword }, // as
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x7073656d616eull, 0x656361707365ull, 9, SyntaxKind::NamespaceKeyword }, // namespace
    { 0x667265746e69ull, 0x656361667265ull, 9, SyntaxKind::InterfaceKeyword }, // interface
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x6d656c706d69ull, 0x73746e656d65ull, 10, SyntaxKind::ImplementsKeyword }, // implements
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x6e7275746572ull, 0x6e7275746572ull, 6, SyntaxKind::ReturnKeyword }, // return
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x6e6174736e69ull, 0x666f65636e61ull, 10, SyntaxKind::InstanceOfKeyword }, // instanceof
    { 0x7463656a626full, 0x7463656a626full, 6, SyntaxKind::ObjectKeyword }, // object
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x72616c636564ull, 0x6572616c6365ull, 7, SyntaxKind::DeclareKeyword }, // declare
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x00636e797361ull, 0x00636e797361ull, 5, SyntaxKind::AsyncKeyword }, // async
    { 0x000073696874ull, 0x000073696874ull, 4, SyntaxKind::ThisKeyword }, // this
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x6c7561666564ull, 0x746c75616665ull, 7, SyntaxKind::DefaultKeyword }, // default
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x00000074656cull, 0x00000074656cull, 3, SyntaxKind::LetKeyword }, // let
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x676e69727473ull, 0x676e69727473ull, 6, SyntaxKind::StringKeyword }, // string
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x006863746163ull, 0x006863746163ull, 5, SyntaxKind::CatchKeyword }, // catch
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x6e6f64616572ull, 0x796c6e6f6461ull, 8, SyntaxKind::ReadonlyKeyword }, // readonly
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x00776f726874ull, 0x00776f726874ull, 5, SyntaxKind::ThrowKeyword }, // throw
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x000065757274ull, 0x000065757274ull, 4, SyntaxKind::TrueKeyword }, // true
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x006b61657262ull, 0x006b61657262ull, 5, SyntaxKind::BreakKeyword }, // break
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x00006c6c756eull, 0x00006c6c756eull, 4, SyntaxKind::NullKeyword }, // null
    { 0x74726f707865ull, 0x74726f707865ull, 6, SyntaxKind::ExportKeyword }, // export
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x000065707974ull, 0x000065707974ull, 4, SyntaxKind::TypeKeyword }, // type
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x00666f79656bull, 0x00666f79656bull, 5, SyntaxKind::KeyOfKeyword }, // keyof
    { 0, 0, 0, SyntaxKind::Unknown },
    { 0x00006d6f7266ull, 0x00006d6f7266ull, 4, SyntaxKind::FromKeyword }, // from
};

}
}
