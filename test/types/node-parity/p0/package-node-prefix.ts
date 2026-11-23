// P0: the `node:` prefixed specifiers and default imports.
import fs from 'node:fs';
import * as fsp from 'node:fs/promises';
import { Readable } from 'node:stream';
import { setTimeout as delay } from 'node:timers/promises';
import assert from 'node:assert/strict';

const stats: fs.Stats = fs.statSync(__filename);
const statPromise = fsp.stat(__filename);
const readable: Readable = new Readable();
const delayed: Promise<void> = delay(1);

assert.ok(true);
export { stats, statPromise, readable, delayed };
