var db = require('db');
var assert = require('assert');

var rdb = db.open("redis://127.0.0.1");

rdb.command("set", "test", "aaa");
rdb.command("set", "test1", "aaa");
console.log(rdb.command("get", "test"));
console.log(rdb.command("exists", "test"));
console.log(rdb.command("keys", "*"));