// P0: CommonJS import forms and the `fs/promises` module graph.
import fs = require('fs');
import fsp = require('fs/promises');

const stats: fs.Stats = fs.statSync(__filename);
const statPromise: Promise<fs.Stats> = fsp.stat(__filename);

module.exports = { stats, statPromise };
