// Test that dependency module's import.meta points to the correct file
import { meta as depMeta, value } from './esm22_dep.mjs';

export var parentMeta = import.meta;
export var childMeta = depMeta;
export var childValue = value;
