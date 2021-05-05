/// <reference no-default-lib="true" />

/// <reference lib="es5"/>
/// <reference lib="es6"/>
/// <reference lib="es7"/>
/// <reference lib="es2015"/>
/// <reference lib="es2016"/>
/// <reference lib="es2017"/>
/// <reference lib="es2018"/>
/// <reference lib="es2019"/>

/// <reference path="./dts/bridge.d.ts" />

import _Global = require('global');

// declare global {
// } /** end of `declare global` */

declare var exports: {
    [k: string]: any
};
declare var module: {
    exports: typeof exports;
};

declare const Buffer: typeof _Global.Buffer
declare const console: typeof _Global.console
declare const process: typeof _Global.process
declare const Master: typeof _Global.Master
declare const global: typeof _Global.global
declare const run: typeof _Global.run
declare var require: typeof _Global.require & {
    main: any
}
declare const argv: typeof _Global.argv
declare const __filename: typeof _Global.__filename
declare const __dirname: typeof _Global.__dirname
declare const setTimeout: typeof _Global.setTimeout
declare const clearTimeout: typeof _Global.clearTimeout
declare const setInterval: typeof _Global.setInterval
declare const clearInterval: typeof _Global.clearInterval
declare const setHrInterval: typeof _Global.setHrInterval
declare const clearHrInterval: typeof _Global.clearHrInterval
declare const setImmediate: typeof _Global.setImmediate
declare const clearImmediate: typeof _Global.clearImmediate
declare const GC: typeof _Global.GC

declare module '@fibjs/types' {
}