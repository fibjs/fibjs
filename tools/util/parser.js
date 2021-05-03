var peg = require("pegjs");
var fs = require("fs");
var path = require("path");

var _ppegjs = path.resolve(__dirname, './idl-def.pegjs')

var grammar = fs.readTextFile(_ppegjs);

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

/**
 * @param {string} baseFolder 
 * @param {Record<string, import('../../idl/ir').IIDLDefinition>} defs 
 * @returns {Record<string, import('../../idl/ir').IIDLDefinition>} 
 */
module.exports = function (baseFolder, defs) {
  var defs1 = {};
  var collect = {};

  fs.readdir(baseFolder).sort().forEach(f => {
    if (f === 'collect.json') {
      f = path.join(baseFolder, f);
      collect = JSON.parse(fs.readTextFile(f));
    } else if (path.extname(f) == '.idl') {
      f = path.join(baseFolder, f);
      var def = parser.parse(fs.readTextFile(f));

      def.declare.doc = parser_comment(def.declare.comments);
      for (var m in def.members)
        def.members[m].doc = parser_comment(def.members[m].comments);

      defs1[def.declare.name] = def;
    }
  });

  var defs2 = {};
  for (var g in collect) {
    collect[g].forEach(n => {
      var def = defs1[n];
      def.collect = g;
      defs2[n] = def;
      delete defs1[n];
    });
  }

  for (var n in defs1) {
    defs2[n] = defs1[n];
  }

  if (defs) {
    for (var n in defs) {
      defs[n].__skip = true;
      defs2[n] = defs[n];
    }
  }

  if (defs2['object'] && defs2['object'].declare) {
    delete defs2['object'].declare.extend;
  }

  return defs2;
};