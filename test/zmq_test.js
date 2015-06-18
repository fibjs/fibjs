console.log('zmq testing....');

var zmq = require('zmq');
var assert = require('assert');
var coroutine = require('coroutine');

function s1() {
	var s = new zmq.Socket(zmq.REP);
	s.bind("tcp://*:5555");
	assert.equal('Hello?', s.recv().toString());
	s.send(new Buffer('Hello!'));
	s.close();
}

coroutine.start(s1);

var c = new zmq.Socket(zmq.REQ);
c.connect('tcp://localhost:5555');
c.send(new Buffer('Hello?'));
assert.equal('Hello!', c.recv().toString());
c.close();