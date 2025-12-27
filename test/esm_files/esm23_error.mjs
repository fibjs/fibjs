// This file has a syntax error to test that module instantiation failure
// does not crash when import.meta is used
export var test = import.meta;
syntax error here  // This will cause instantiation to fail
