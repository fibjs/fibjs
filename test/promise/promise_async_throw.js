// fs.promises methods should return rejected promise on bad args,
// not throw synchronously
var fs = require('fs');

var p = fs.promises.readFile();
if (p instanceof Promise)
    p.catch(e => console.log("rejected: " + e.message));
else
    console.log("not a promise");
