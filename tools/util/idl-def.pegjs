
{
  // `Function(...)` is a typing-only refinement of a `Function` argument: the
  // type name stays 'Function' (the C++ generators look it up by name) and the
  // inline shape travels in the side-car `callback` field, consumed by the
  // d.ts / docs generators only (see tools/util/ir.d.ts).
  function typeName(t) {
    return (t !== null && typeof t === 'object' && !Array.isArray(t)) ? 'Function' : t;
  }

  function callbackOf(t) {
    return (t !== null && typeof t === 'object' && !Array.isArray(t)) ? t.callback : null;
  }

  // The call operator (`operator(...)`, see the rule below) is a plain method
  // member named `operator` at the IR level: gen_code maps the name to the
  // C++ `_function` call stub and gen_dts to the callable-module (`export =`)
  // and call-signature forms. The historical `Function(...)` spelling is
  // retired - it collided with the `Function` type name (callback shapes).
}

interface = head:declareHead
  body:interfaceBody _* ";" _* {
    return {
      declare: head,
      members: body
    }
  }

declareHead
  = interfaceHead / moduleHead

interfaceHead
  = comments:_* type:InterfaceToken _* 
    name:Identifier _* 
    extend:(interfaceEntend)? {
    return {
      comments: comments.join(""),
      type: type,
      name: name,
      extend: extend || "object"
    }
  }

moduleHead
  = comments:_* type:ModuleToken _* 
    name:Identifier _*
    extend:(interfaceEntend)? {
    return {
      comments: comments.join(""),
      type: type,
      name: name,
      extend: extend || "object",
      module: true
    }
  }

interfaceEntend
  = ":" _* extend:Identifier _* {
    return extend;
  }

interfaceBody
  = "{" members:(constMember / prop / object / object1 / eventDef / callOperator / method / operator)* _* "}" {
    return members;
  }

constMember
  = comments:_* deprecated:deprecatedToken? _* constMode:constToken _* symbol:Identifier _+ name:Identifier def:defValue? _* ";" {
    return {
      memType: "const",
      comments: comments.join(""),
      deprecated: deprecated,
      const: constMode,
      symbol: symbol,
      name: name,
      default: def
    };
  }
  / comments:_* deprecated:deprecatedToken? _* constMode:constToken _* name:Identifier? def:defValue? _* ";" {
    return {
      memType: "const",
      comments: comments.join(""),
      deprecated: deprecated,
      const: constMode,
      symbol: '',
      name: name,
      default: def
    };
  }

prop
  = comments:_* deprecated:deprecatedToken? _* staticMode:staticToken? _* readonly:readonlyToken? _* type:type _* isarray:("[" _* "]")? _* symbol:"@"? name:Identifier _* ";" {
    return {
      memType: "prop",
      comments: comments.join(""),
      deprecated: deprecated,
      static: staticMode,
      readonly: readonly,
      symbol: symbol ? '@' : '',
      name: name,
      type: type,
      isarray: isarray
    };
  }

operator
  = comments:_* deprecated:deprecatedToken? _* readonly:readonlyToken? _* type:Identifier _* name:Identifier _* "[" _* index:StringToken? _* "]" _* ";" {
    return {
      memType: "operator",
      comments: comments.join(""),
      deprecated: deprecated,
      readonly: readonly,
      symbol: '',
      name: index ? "[String]" : "[]",
      type: type,
      index: index
    };
  }

eventDef
  = comments:_* deprecated:deprecatedToken? _* eventToken _* name:Identifier _* "(" params:params? _* ")" _* ";" {
    return {
      memType: "event",
      comments: comments.join(""),
      deprecated: deprecated,
      name: name,
      params: params
    };
  }

object
  = comments:_* _* deprecated:deprecatedToken? _* staticMode:staticToken _* name:Identifier _* "=" _* type:Identifier _*  ";" {
    return {
      memType: "object",
      comments: comments.join(""),
      deprecated: deprecated,
      symbol: '',
      name: name,
      type: type,
      newable: true
    };
  }

object1
  = comments:_* _* deprecated:deprecatedToken? _* staticMode:staticToken _* type:Identifier _*  ";" {
    return {
      memType: "object",
      comments: comments.join(""),
      deprecated: deprecated,
      symbol: '',
      name: type,
      type: type
    };
  }

// The call operator (C++ `operator()`, WebIDL `legacycaller`): the module
// object is callable (`test(...)`, `assert(...)`, `describe(...)`) or the
// instances of the interface are (`util.debuglog(section)(msg)`). The optional
// leading type is the operator's return value, omitting it means void. The
// member name `operator` is what gen_code keys the C++ `_function` call stub
// on; keep one spelling - the retired `Function(...)` form registered as a
// plain method instead.
callOperator
  = comments:_* _* deprecated:deprecatedToken? _* staticMode:staticToken? _* "operator" _* "(" params:params? _* ")" _* ";" {
    return {
      memType: "method",
      comments: comments.join(""),
      deprecated: deprecated,
      static: staticMode,
      async: null,
      symbol: '',
      name: "operator",
      type: null,
      params: params
    };
  }
  / comments:_* _* deprecated:deprecatedToken? _* staticMode:staticToken? _* type:extType _* "operator" _* "(" params:params? _* ")" _* ";" {
    var mem = {
      memType: "method",
      comments: comments.join(""),
      deprecated: deprecated,
      static: staticMode,
      async: null,
      symbol: '',
      name: "operator",
      type: typeName(type),
      params: params
    };
    var callback = callbackOf(type);
    if (callback)
      mem.callback = callback;
    return mem;
  }

method
  = comments:_* _* deprecated:deprecatedToken? _* staticMode:staticToken? _* symbol:"@"? name:Identifier _* "(" params:params? _* ")" _* async:async_type? ";" {
    return {
      memType: "method",
      comments: comments.join(""),
      deprecated: deprecated,
      static: staticMode,
      async: async,
      symbol: symbol ? '@' : '',
      name: name,
      type: null,
      params: params
    };
  }
  / comments:_* _* deprecated:deprecatedToken? _* staticMode:staticToken? _* type:extType _* isarray:("[" _* "]")? _* symbol:"@"? name:Identifier _* "(" params:params? _* ")" _* async:async_type? ";" {
    var mem = {
      memType: "method",
      comments: comments.join(""),
      deprecated: deprecated,
      static: staticMode,
      async: async,
      symbol: symbol ? '@' : '',
      name: name,
      type: typeName(type),
      isarray: isarray,
      params: params
    };
    var callback = callbackOf(type);
    if (callback)
      mem.callback = callback;
    return mem;
  }

async_type
  = asyncToken
  / promiseToken

params
  = first:param nexts:nextparam* {
     return [first].concat(nexts);
  }

nextparam
  = "," param:param {
    return param;
  }

param
  = paramitem
  / paramopt

paramopt
  = _* "..." _* name:Identifier? {
    return name ? 
    {
      type: "...",
      name: name,
      default: null
    } : {
      type: null,
      name: "...",
      default: null
    }
  }

paramitem
  = _* type:paramType _* name:Identifier _* isarray:("[" _* "]")? def:def? {
    var param = {
      type: typeName(type),
      isarray: isarray,
      name: name,
      default:def
    };
    var callback = callbackOf(type);
    if (callback)
      param.callback = callback;
    return param;
  }

type
  = IteratorType
  / Identifier
  / struct

// `Buffer|String` / `Buffer|KeyObject|Object|String`: a parameter-position
// union. The alternatives are the runtime conversion's preference order (see
// plans/idl-union-types-2026-10-02.md); the C++ side receives one
// `std::variant` (see gen_code). Alternatives are named types or
// `Iterator<T>`; struct, Function, `...`, a single `[]` and string literals
// are not alternatives, and a callback shape is never a union member.
paramType
  = UnionType
  / extType

UnionType
  = first:unionUnit rest:unionTail+ {
      return [first].concat(rest).join('|');
    }

unionTail
  = _* "|" _* unit:unionUnit {
      return unit;
    }

unionUnit
  = IteratorType
  / Identifier

// `extType` is the only way to reach `CallbackType`: it is used by `paramitem`
// and by the method return position, so `prop` (which keeps `type`), `operator`
// and the struct fields (which use `Identifier`) can never carry a shape.
extType
  = CallbackType
  / type

CallbackType
  = "Function" _* "(" params:callbackParams? _* ")" ret:callbackRet? {
      return { callback: { params: params || [], ret: ret || null } };
    }

callbackParams
  = first:callbackParam nexts:nextCallbackParam* {
      return [first].concat(nexts);
    }

nextCallbackParam
  = "," param:callbackParam {
      return param;
    }

callbackParam
  = _* "..." _* name:Identifier? _* {
      return { type: "...", name: name || "...", default: null };
    }
  / _* type:paramType _* name:Identifier _* isarray:("[" _* "]")? _* {
      var param = {
        type: typeName(type),
        isarray: isarray,
        name: name,
        default: null
      };
      var callback = callbackOf(type);
      if (callback)
        param.callback = callback;
      return param;
    }

callbackRet
  = _* "=>" _* type:extType {
      return typeName(type);
    }

IteratorType
  = "Iterator" _* "<" _* arg:Identifier _* ">" {
      return 'Iterator<' + arg + '>';
    }

struct
  = "(" items:items? _* ")" {
    return items
  }

items
  = first:itemtype nexts:nextitem* {
     return [first].concat(nexts);
  }

nextitem
  = "," itemtype:itemtype {
    return itemtype;
  }

itemtype
  = _* type:Identifier _* name:Identifier def:def? {
    return {
      type: type,
      name: name,
      default:def
    }
  }


def
  = defValue
  / defConst

defConst
  =  _* "=" _* m:Identifier c:("." Identifier)? {
    return {
      const: c ? [m,c.join("").slice(1)] : m
    };
  }

defValue
  = _* "=" _* value:value {
    return {
      value: value
    };
  }

value
  = false
  / true
  / null
  / undefined
  / objectVal
  / arrayVal
  / oct
  / number
  / string

false = "false" { return 'false'; }
true  = "true"  { return 'true';  }
null  = "null"  { return 'NULL';  }
undefined = "undefined" { return 'v8::Undefined(isolate->m_isolate)';}

objectVal = "{" _* "}" { return "v8::Object::New(isolate->m_isolate)" }
arrayVal = "[" _* "]" { return "v8::Array::New(isolate->m_isolate)"; }

number "number"
  = minus? int frac? exp? { return text(); }

oct "oct"
  = zero OCTDIG+ { return text(); }

decimal_point = "."
digit1_9      = [1-9]
e             = [eE]
exp           = e (minus / plus)? DIGIT+
frac          = decimal_point DIGIT+
int           = zero / (digit1_9 DIGIT*)
minus         = "-"
plus          = "+"
zero          = "0"

string "string"
  = quotation_mark chars:char* quotation_mark { return '"' + chars.join("") + '"'; }

char
  = unescaped
  / escape
    sequence:(
        '"'
      / "\\"
      / "/"
      / "b" { return "\b"; }
      / "f" { return "\f"; }
      / "n" { return "\n"; }
      / "r" { return "\r"; }
      / "t" { return "\t"; }
      / "u" digits:$(HEXDIG HEXDIG HEXDIG HEXDIG) {
          return String.fromCharCode(parseInt(digits, 16));
        }
    )
    { return sequence; }

escape         = "\\"
quotation_mark = '"'
unescaped      = [^\0-\x1F\x22\x5C]

DIGIT  = [0-9]
OCTDIG = [0-7]
HEXDIG = [0-9a-f]i

Identifier "identifier"
  = start:IdentifierStart chars:IdentifierChar* {
     return start + chars.join("");
  }

IdentifierStart
  = [_a-z]i

IdentifierChar
  = [_a-z0-9-]i
_
  = (WhiteSpace / LineTerminator / Comment)

Comment "comment"
  = MultiLineComment
  / SingleLineComment

MultiLineComment
  = "/*" comments:(!"*/" SourceCharacter)* "*/" {
    return comments.map(i => i[1]).join("");
  }

MultiLineCommentNoLineTerminator
  = "/*" comments:(!("*/" / LineTerminator) SourceCharacter)* "*/" {
    return comments.map(i => i[1]).join("");
  }

SingleLineComment
  = "//" comments:(!LineTerminator SourceCharacter)* {
    return comments.map(i => i[1]).join("");
  }

WhiteSpace
  = [\t ] {
    return '';
  }

LineTerminator
  = [\n\r] {
    return '';
  }

SourceCharacter
  = .

/* Tokens */

ModuleToken     = "module"
InterfaceToken  = "interface"
StringToken     = "String"
staticToken     = "static"
eventToken      = "event"
deprecatedToken = "deprecated"
readonlyToken   = "readonly"
asyncToken      = "async"
promiseToken    = "promise"
constToken      = "const"

