// This file tries to import a non-existent module
// Used to test that error message contains the correct file path
import { something } from './this_module_does_not_exist.mjs';

export var test = something;
