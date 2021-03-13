var path = require('path');
var child_process = require('child_process');
var parser = require('./util/parser');
var gen_docs = require('./util/gen_docs');

var baseFolder = path.join(__dirname, "../idl/zh-cn");
var docsFolder = path.join(__dirname, "../docs/docs/manual/");

var defs = parser(baseFolder);
gen_docs(defs, docsFolder);

process.chdir(path.join(__dirname, '../docs'));
child_process.run('npm', ['run', 'build']);