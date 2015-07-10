var path = require('path'),
	process = require('process');
var executelist = path.fullpath('.').split('/');
var argvlist = process.argv[1].split('/');

argvlist.forEach(function(item) {
	if ('..' === item)
		executelist.pop();
	else if ('.' !== item)
		executelist.push(item);
})
executelist.pop();

module.exports = {
	basedir: executelist.join('/')
}