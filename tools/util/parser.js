var peg = require("pegjs");
var fs = require("fs");
var path = require("path");

var grammar = `
interface = head:declareHead
  body:interfaceBody _* ";" _* {
    return {
      declare: head,
      members:body
    }
  }

declareHead
  = interfaceHead / moduleHead

interfaceHead
  = comments:_* type:InterfaceToken _* 
    name:Identifier _* 
    extend:(interfaceEntend)?
    {
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
    extend:(interfaceEntend)?
    {
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
  = "{" members:(constMember / prop / object / object1 / method / operator)* _* "}" {
    return members;
  }

constMember
  = comments:_* deprecated:deprecatedToken? _* constMode:constToken _* name:Identifier? def:defValue? _* ";" {
    return {
      memType: "const",
      comments: comments.join(""),
      deprecated: deprecated,
      const: constMode,
      name: name,
      default: def
    };
  }

prop
  = comments:_* deprecated:deprecatedToken? _* staticMode:staticToken? _* readonly:readonlyToken? _* type:Identifier _* name:Identifier _* ";" {
    return {
      memType: "prop",
      comments: comments.join(""),
      deprecated: deprecated,
      static: staticMode,
      readonly: readonly,
      name: name,
      type: type
    };
  }

operator
  = comments:_* deprecated:deprecatedToken? _* readonly:readonlyToken? _* type:Identifier _* name:Identifier _* "[" _* index:StringToken? _* "]" _* ";" {
    return {
      memType: "operator",
      comments: comments.join(""),
      deprecated: deprecated,
      readonly: readonly,
      name: index ? "[String]" : "[]",
      type: type,
      index: index
    };
  }

object
  = comments:_* _* deprecated:deprecatedToken? _* staticMode:staticToken _* type:Identifier _* newToken _* name:Identifier "(" _* ")" _*  ";" {
    return {
      memType: "object",
      comments: comments.join(""),
      deprecated: deprecated,
      name: name,
      type: type
    };
  }

object1
  = comments:_* _* deprecated:deprecatedToken? _* staticMode:staticToken _* type:Identifier _*  ";" {
    return {
      memType: "object",
      comments: comments.join(""),
      deprecated: deprecated,
      name: type,
      type: type
    };
  }

method
  = comments:_* _* deprecated:deprecatedToken? _* staticMode:staticToken? _* type:Identifier? _* name:Identifier? _* "(" params:params? _* ")" _* async:asyncToken? ";" {
    return {
      memType: "method",
      comments: comments.join(""),
      deprecated: deprecated,
      static: staticMode,
      async: async,
      name: name ? name : type,
      type: name ? type : null,
      params: params
    };
  }

params
  = first:param nexts:nextparam* {
     return [first].concat(nexts);
  }

nextparam
  = "," param:param {
    return param;
  }

param
  = paramtype / paramopt

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

paramtype
  = _* type:Identifier _* name:Identifier def:def? {
    return {
      type: type,
      name: name,
      default:def
    }
  }

def
  = defValue / defConst

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
  / undefined
  / objectVal
  / arrayVal
  / oct
  / number
  / string

false = "false" { return 'false'; }
true  = "true"  { return 'true';  }
undefined = "undefined" { return 'v8::Undefined(isolate)';}

objectVal = "{" _* "}" { return "v8::Object::New(isolate)" }
arrayVal = "[" _* "]" { return "v8::Array::New(isolate)"; }

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
      / "\\\\"
      / "/"
      / "b" { return "\\b"; }
      / "f" { return "\\f"; }
      / "n" { return "\\n"; }
      / "r" { return "\\r"; }
      / "t" { return "\\t"; }
      / "u" digits:$(HEXDIG HEXDIG HEXDIG HEXDIG) {
          return String.fromCharCode(parseInt(digits, 16));
        }
    )
    { return sequence; }

escape         = "\\\\"
quotation_mark = '"'
unescaped      = [^\\0-\\x1F\\x22\\x5C]

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
  = [\\t ] {
    return '';
  }

LineTerminator
  = [\\n\\r] {
    return '';
  }

SourceCharacter
  = .

/* Tokens */

FunctionToken   = "Function"
IntegerToken    = "Integer"
NumberToken     = "Number"
ModuleToken     = "module"
InterfaceToken  = "interface"
StringToken     = "String"
JSObjectToken   = "Object"
JSValueToken    = "Value"
staticToken     = "static"
deprecatedToken = "deprecated"
readonlyToken   = "readonly"
asyncToken      = "async"
constToken      = "const"
newToken        = "new"
`;

var parser = peg.generate(grammar);

function parser_comment(comment) {
  var lines;
  var doc = {
    descript: '',
    detail: [],
    params: []
  };

  function check_padding() {
    var sp = 0;
    var s;

    lines.forEach(l => {
      var r = /^\s*/.exec(l)[0];
      if (r !== '') {
        if (sp === 0 || sp > r.length) {
          sp = r.length;
          s = r;
        }
      }
    });

    if (sp) {
      lines = lines.map(line => {
        if (line.substr(0, sp) === s)
          return line.substr(sp);
        return line;
      });
    }
  }

  function get_title() {
    lines = comment.split('\n');
    lines = lines.map(line => {
      return line.trimRight();
    });

    doc.descript = lines[0];

    lines = lines.slice(1);
    if (lines[0] == '')
      lines = lines.slice(1);
  }

  function get_docs() {
    var now = doc;
    lines.forEach(l => {
      var re = /^\s*@((param)\s+([^\s]+)|return)\s*(.*)/g;
      var r = re.exec(l);

      if (r) {
        if (r[1] === 'return') {
          doc["return"] = now = {
            descript: r[4],
            detail: []
          };
        } else if (r[2] === 'param') {
          doc.params.push(now = {
            name: r[3],
            descript: r[4],
            detail: []
          });
        }
      } else
        now.detail.push(l);
    });
  }

  comment = comment.replace(/^\s*!?\s*(@brief)?\s*/, '');
  comment = comment.replace(/\s*$/, '');

  if (comment == '')
    return doc;

  get_title();
  check_padding();
  get_docs();

  return doc;
}

module.exports = function (baseFolder) {
  var defs = {};

  fs.readdir(baseFolder).forEach(f => {
    if (path.extname(f) == '.idl') {
      f = path.join(baseFolder, f);
      var def = parser.parse(fs.readTextFile(f));
      defs[def.declare.name] = def;
    }
  });

  for (var n in defs) {
    defs[n].declare.doc = parser_comment(defs[n].declare.comments);
    for (var m in defs[n].members)
      defs[n].members[m].doc = parser_comment(defs[n].members[m].comments);
  }

  delete defs['object'].declare.extend;

  return defs;
};