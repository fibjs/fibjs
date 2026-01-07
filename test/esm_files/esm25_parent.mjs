// Parent module that imports a child module
// The child module will try to import a non-existent module
import { something } from './esm25_child.mjs';

export var test = something;
