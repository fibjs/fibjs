// P0: every `.promises` module must be reachable from the package entry.
// Side-effect imports are enough: they fail with TS2307 when the module was
// written to disk but left out of the bridge reference graph.
import 'fs/promises';
import 'dns/promises';
import 'net/promises';
import 'zlib/promises';
import 'crypto/promises';
import 'http/promises';
import 'tls/promises';

export {};
