// Static import of a JSON module holding a primitive must bind `default` to
// that value (no named exports exist to read instead).
import num from './json_primitive_num.json' with { type: 'json' };
import str from './json_primitive_str.json' with { type: 'json' };
import nul from './json_primitive_null.json' with { type: 'json' };
import * as ns from './json_primitive_null.json' with { type: 'json' };

export const values = { num, str, nul, hasDefault: 'default' in ns };
