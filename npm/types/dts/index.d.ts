/// <reference no-default-lib="true" />

/// <reference lib="es5"/>
/// <reference lib="es6"/>
/// <reference lib="es7"/>
/// <reference lib="es2015"/>
/// <reference lib="es2016"/>
/// <reference lib="es2017"/>
/// <reference lib="es2018"/>
/// <reference lib="es2019"/>

/// <reference path="./bridge.d.ts" />

import _Global = require('global');

declare global {
    var exports: {
        [k: string]: any
    };
    var module: {
        exports: typeof exports;
    };

    const Buffer: typeof _Global.Buffer
    const console: typeof _Global.console
    const process: typeof _Global.process
    const Master: typeof _Global.Master
    const global: typeof _Global.global
    const run: typeof _Global.run
    const require: typeof _Global.require
    const argv: typeof _Global.argv
    const __filename: typeof _Global.__filename
    const __dirname: typeof _Global.__dirname
    const setTimeout: typeof _Global.setTimeout
    const clearTimeout: typeof _Global.clearTimeout
    const setInterval: typeof _Global.setInterval
    const clearInterval: typeof _Global.clearInterval
    const setHrInterval: typeof _Global.setHrInterval
    const clearHrInterval: typeof _Global.clearHrInterval
    const setImmediate: typeof _Global.setImmediate
    const clearImmediate: typeof _Global.clearImmediate
    const GC: typeof _Global.GC
} /** end of `declare global` */