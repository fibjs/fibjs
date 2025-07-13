var path = require('path');
var child_process = require('child_process');
var parser = require('./util/parser');
var gen_docs = require('./util/gen_docs');

var baseFolder = path.join(__dirname, "../idl/zh-cn");
var docsFolder = path.join(__dirname, "../docs/docs/manual/");

console.log('🚀 Starting fibjs documentation generation...');

console.log('📖 Parsing IDL definitions...');
var defs = parser(baseFolder);

console.log('📚 Generating documentation...');
gen_docs(defs, docsFolder);

console.log('🏗️ Building documentation site...');
process.chdir(path.join(__dirname, '../docs'));
child_process.execFileSync('npm', ['run', 'build'], { stdio: 'inherit' });

console.log('🎉 Documentation generation completed successfully!');