var path = require('path');
var parser = require('./util/parser');
var gen_docs = require('./util/gen_docs');

var baseFolder = path.join(__dirname, "../fibjs/include/ifs/");
var docsFolder = path.join(__dirname, "../docs/docs/manual/");

var defs = parser(baseFolder);
gen_docs(defs, docsFolder);

process.chdir(path.join(__dirname, '../docs'));
process.run('npm', ['run', 'build']);