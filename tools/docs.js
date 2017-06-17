var parser = require('./util/parser');
var gen_docs = require('./util/gen_docs');

var baseFolder = "../fibjs/include/ifs/";
var docsFolder = "../docs/docs/manual/";

var defs = parser(baseFolder);
gen_docs(defs, docsFolder);

process.chdir('../docs');
process.run('npm', ['run', 'build']);