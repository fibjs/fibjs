// Child module that tries to import a non-existent module
// Used to test that error message contains the correct file path in dependency tree
import { value } from './nonexistent_dep_module.mjs';

export var something = value;
