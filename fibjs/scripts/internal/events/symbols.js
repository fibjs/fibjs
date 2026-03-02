'use strict';

const primordials = require('internal/primordials');
const {
  Symbol,
} = primordials;

const kFirstEventParam = Symbol('nodejs.kFirstEventParam');

module.exports = {
  kFirstEventParam,
};
