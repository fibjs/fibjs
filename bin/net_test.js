/**
 * 
 */

var assert = require('assert');
var net = require('net');

var s = net.socket(net.AF_INET6, net.SOCK_STREAM);
s.connect('::1', 80);
