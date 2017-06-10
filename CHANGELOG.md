## 2017-06-10, Version v0.4.0 @ngot

* **feature** :
  * wasm: Add wasm extension demo.Now you can use WebAssembly to write extention of fibjs!
  * util: format support TypedArray.
  * module:
    - support auto exec selfzip file. Now you can zip your application to a executable file and distribute it easily!
    - add --noroot options to disable application root directory. Application root directory will be remove in 0.5.0, so please pay attention to this.
    - add require.resovle
    - implement .ext loader.
  * zlip: date_t support getdate.
  * vm: Sandbox.run support extension search and package.json.
  * fs: add access, accessSync, appendFile, appendFileSync, chown, chownSync, lchmod, lchmodSync, lchown, lchownSync, link, linkSync, lstat, lstatSync, readlink, readlinkSync, realpath, realpathSync, symlink, symlinkSync, truncate, truncateSync, F_OK, R_OK, W_OK, X_OK & constants
  * util: util.compile support bash style comment.
  * path: 
    - support path.isAbsolute.
    - enable path.win32 & path.posix both works on win32 & posix
  * os: 
    - add os.homedir()
    - add os.release()
    - add os.endianness()
    - add os.userInfo()
  * events: 
    now it is possible create EventEmit object just from `require('events')`.
  * timers: add timers module

* **breakchange** :
  * os: normalize execPath.
  * vm: disable relative path in sanbox.run.
  * global: move global.sync to util.sync.
  * os: replace os.version with os.release()
  * os: change os.type to os.type()
  * os: change os.hostname to os.hostname()
  * os: rename os.CPUInfo() to os.cpus() & rename os.CPUs() to os.cpuNumbers()
  * os: change os.arch to os.arch()
  * os: rename os.networkInfo() to os.networkInterfaces
  * core: reorder module wrap arguments.
  * gui: disable drag file to gui webbrowser.

* **bugfix** :
  * process: fix process on Linux, not throw when process.run failed.
  * core: fix GLIBC_2.7 not found. disable eventfd.
  * gui: fix gui windows visible&maxmize bug.
  * gui: fix maximize mode of gui window not worked in shortcut.
  * gui: fix windows gui input control ctrl-v delete error.
  * module: change path .zip?/file to .zip$/file, fix windows gui url error.
* **internal** :
  * upgrade V8 to 6.1.64

### Commits

* [[`4cd3e6c7d4`](https://github.com/fibjs/fibjs/commit/4cd3e6c7d4)] - **wasm, feat**: add wasm test (ngot) 
* [[`80c1837f8b`](https://github.com/fibjs/fibjs/commit/80c1837f8b)] - **util, feat**: format support TypedArray. (xicilion) 
* [[`e9831314eb`](https://github.com/fibjs/fibjs/commit/e9831314eb)] - **module, refactor**: not register id into module cache. (xicilion) 
* [[`efec83bfc0`](https://github.com/fibjs/fibjs/commit/efec83bfc0)] - **module, chore**: check module id. (xicilion) 
* [[`032835643b`](https://github.com/fibjs/fibjs/commit/032835643b)] - **test, chore**: move temp file to test folder. (xicilion) 
* [[`92fee00865`](https://github.com/fibjs/fibjs/commit/92fee00865)] - **module, feat**: auto exec selfzip file. (xicilion) 
* [[`f11f666176`](https://github.com/fibjs/fibjs/commit/f11f666176)] - **os, break**: normalize execPath. (xicilion) 
* [[`bff9a9f33d`](https://github.com/fibjs/fibjs/commit/bff9a9f33d)] - **process, fixbug**: process on Linux, not throw when process.run failed. (xicilion) 
* [[`a6421df39a`](https://github.com/fibjs/fibjs/commit/a6421df39a)] - **core, fixbug**: GLIBC_2.7 not found. disable eventfd. (xicilion) 
* [[`dd437b2831`](https://github.com/fibjs/fibjs/commit/dd437b2831)] - **module, break**: add --noroot options to disable application root directory. (xicilion) 
* [[`ca3951c1b2`](https://github.com/fibjs/fibjs/commit/ca3951c1b2)] - **test, fixbug**: fix module_test, disable absolute identifiers test. (xicilion) 
* [[`33f800ff08`](https://github.com/fibjs/fibjs/commit/33f800ff08)] - **module, refactor**: reuse variable. (xicilion) 
* [[`6e6ee02be4`](https://github.com/fibjs/fibjs/commit/6e6ee02be4)] - **module, refactor**: Organize source code. (xicilion) 
* [[`be1a651d8a`](https://github.com/fibjs/fibjs/commit/be1a651d8a)] - **module, feat**: implement require.resovle (xicilion) 
* [[`be71bb05ff`](https://github.com/fibjs/fibjs/commit/be71bb05ff)] - **module, fixbug**: normalize id, fix error on windows. (xicilion) 
* [[`7ac814c7dc`](https://github.com/fibjs/fibjs/commit/7ac814c7dc)] - **module, feat**: implement requireModule. (xicilion) 
* [[`9bfea8d064`](https://github.com/fibjs/fibjs/commit/9bfea8d064)] - **module, refactor**: use ZipFile.date as the date of zip virtual file. (xicilion) 
* [[`693b015c01`](https://github.com/fibjs/fibjs/commit/693b015c01)] - **ZipFile**: add date test case. (xicilion) 
* [[`01cd70605f`](https://github.com/fibjs/fibjs/commit/01cd70605f)] - **ZipFile**: support date while zip a file. (xicilion) 
* [[`4f6330cda4`](https://github.com/fibjs/fibjs/commit/4f6330cda4)] - **zlip, feat**: date_t support getdate. (xicilion) 
* [[`fe52e70f6f`](https://github.com/fibjs/fibjs/commit/fe52e70f6f)] - **fs, feat**: enable zip cache timeout. (xicilion) 
* [[`e85d504ed1`](https://github.com/fibjs/fibjs/commit/e85d504ed1)] - **vm, feat**: implement requireFile, use locateFile to load module file. (xicilion) 
* [[`ac68fc8432`](https://github.com/fibjs/fibjs/commit/ac68fc8432)] - **vm, refactor**: split source code of SandBox. (xicilion) 
* [[`c5351420cb`](https://github.com/fibjs/fibjs/commit/c5351420cb)] - **test, fixbug**: fix websocket test on windows. (xicilion) 
* [[`bf49dcaf5c`](https://github.com/fibjs/fibjs/commit/bf49dcaf5c)] - **vm, feat**: Sandbox.run support extension search and package.json. (xicilion) 
* [[`d87bcc6960`](https://github.com/fibjs/fibjs/commit/d87bcc6960)] - **vm, break**: disable relative path in sanbox.run. (xicilion) 
* [[`e3fa7060de`](https://github.com/fibjs/fibjs/commit/e3fa7060de)] - **vm, refactor**: share script cache in isolate. (xicilion) 
* [[`a155c2133c`](https://github.com/fibjs/fibjs/commit/a155c2133c)] - **vm, refactor**: use LruCache to cache script file. (xicilion) 
* [[`391b467618`](https://github.com/fibjs/fibjs/commit/391b467618)] - **vm, feat**: enable script file cache. (xicilion) 
* [[`c5ae89255c`](https://github.com/fibjs/fibjs/commit/c5ae89255c)] - **gui, fixbug**: gui windows visible&maxmize bug. (xicilion) 
* [[`f56ff6185a`](https://github.com/fibjs/fibjs/commit/f56ff6185a)] - **gui, fixbug**: maximize mode of gui window not worked in shortcut. (xicilion) 
* [[`e759bf6270`](https://github.com/fibjs/fibjs/commit/e759bf6270)] - **fs, chore**: set fs.constants readonly attribute (asionius) 
* [[`2e9b597470`](https://github.com/fibjs/fibjs/commit/2e9b597470)] - **fs, chore**: remove method access return value (asionius) 
* [[`02a9f33024`](https://github.com/fibjs/fibjs/commit/02a9f33024)] - **fs, fixbug**: fix windows compile error (asionius) 
* [[`aa34b7d23f`](https://github.com/fibjs/fibjs/commit/aa34b7d23f)] - **fs, feat**: some fs apis to compat to node fs api (asionius) 
* [[`9a864d3773`](https://github.com/fibjs/fibjs/commit/9a864d3773)] - **internal, upgrade**: upgrade v8 to 6.1.64 (xicilion) 
* [[`b0e003bcee`](https://github.com/fibjs/fibjs/commit/b0e003bcee)] - **iconv, chore**: delete unused variable. (xicilion) 
* [[`032ccc2a97`](https://github.com/fibjs/fibjs/commit/032ccc2a97)] - **vm, feat**: implement .ext loader. (xicilion) 
* [[`29717ea64e`](https://github.com/fibjs/fibjs/commit/29717ea64e)] - **os, chore**: simplify os.usereInfo (ngot) 
* [[`4ce4e3ee73`](https://github.com/fibjs/fibjs/commit/4ce4e3ee73)] - **os, fixbug**: fix crash error (ngot) 
* [[`e2b2454fa2`](https://github.com/fibjs/fibjs/commit/e2b2454fa2)] - **os, refactor**: remove useless userInfo code (ngot) 
* [[`d941e05375`](https://github.com/fibjs/fibjs/commit/d941e05375)] - **test, remove**: useless code (ngot) 
* [[`bca9d01961`](https://github.com/fibjs/fibjs/commit/bca9d01961)] - **Buffer, chore**: format (ngot) 
* [[`f8732d14a7`](https://github.com/fibjs/fibjs/commit/f8732d14a7)] - **encoding, feat**: iconv.encode & iconv.dcode support binary encoding (ngot) 
* [[`dc4e7dfca9`](https://github.com/fibjs/fibjs/commit/dc4e7dfca9)] - **encoding, revert**: remove iconv.encode & iconv.dcode support base64 & hex (ngot) 
* [[`1a833967b8`](https://github.com/fibjs/fibjs/commit/1a833967b8)] - **encoding, feat**: iconv.encode & iconv.dcode support base64 & hex (ngot) 
* [[`62e1d88cf2`](https://github.com/fibjs/fibjs/commit/62e1d88cf2)] - **os, refactor**: simplify & better performance (ngot) 
* [[`f9a8f140dc`](https://github.com/fibjs/fibjs/commit/f9a8f140dc)] - **encoding, feat**: add iconv::decode(String) & hex.encode(String) & base64.encode(String) (ngot) 
* [[`00ef954745`](https://github.com/fibjs/fibjs/commit/00ef954745)] - **os,fixbug**: fix build error (ngot) 
* [[`303f42d958`](https://github.com/fibjs/fibjs/commit/303f42d958)] - **os, feat**: implement os.userInfo() encoding options (ngot) 
* [[`d394a179d5`](https://github.com/fibjs/fibjs/commit/d394a179d5)] - **os, feat**: impement os.userInfo on windows (ngot) 
* [[`016b535735`](https://github.com/fibjs/fibjs/commit/016b535735)] - **os, feat**: add os.userInfo() (ngot) 
* [[`61fc723cfd`](https://github.com/fibjs/fibjs/commit/61fc723cfd)] - **core, chore**: remove fibjs namespace. (xicilion) 
* [[`dd0f99f062`](https://github.com/fibjs/fibjs/commit/dd0f99f062)] - **vm, refactor**: rewrite sanbox.run. (xicilion) 
* [[`ec616ea050`](https://github.com/fibjs/fibjs/commit/ec616ea050)] - **vm, refactor**: rewrite addScript. (xicilion) 
* [[`c07fd32936`](https://github.com/fibjs/fibjs/commit/c07fd32936)] - **util, feat**: util.compile support bash style comment. (xicilion) 
* [[`71f5916562`](https://github.com/fibjs/fibjs/commit/71f5916562)] - **global, break**: move global.sync to util.sync. (xicilion) 
* [[`6bd3ff3f7e`](https://github.com/fibjs/fibjs/commit/6bd3ff3f7e)] - **path, feat**: support path.isAbsolute. (xicilion) 
* [[`7c5084af38`](https://github.com/fibjs/fibjs/commit/7c5084af38)] - **path, refactor**: use Runtime::setError to report error. (xicilion) 
* [[`2f1b2a4aef`](https://github.com/fibjs/fibjs/commit/2f1b2a4aef)] - **test, fixbug**: update arm/arm64 jsc test file. (xicilion) 
* [[`5532d31ec4`](https://github.com/fibjs/fibjs/commit/5532d31ec4)] - **internal, upgrade**: upgrade v8 to 6.1.24 (xicilion) 
* [[`73088ef4ec`](https://github.com/fibjs/fibjs/commit/73088ef4ec)] - **core, chore**: remove docs folder. (xicilion) 
* [[`cb7800f342`](https://github.com/fibjs/fibjs/commit/cb7800f342)] - **core, chore**: remove submodule docs. (xicilion) 
* [[`6ac737f3e9`](https://github.com/fibjs/fibjs/commit/6ac737f3e9)] - **path, refactor**: rewrite path/path.posix/path.win32. (xicilion) 
* [[`05d344448f`](https://github.com/fibjs/fibjs/commit/05d344448f)] - **path, fixbug**: fix os_win GetVersion2 compile error. (xicilion) 
* [[`049ebb6ff7`](https://github.com/fibjs/fibjs/commit/049ebb6ff7)] - **core, feat**: support module renaming. (xicilion) 
* [[`8dd5d5d46c`](https://github.com/fibjs/fibjs/commit/8dd5d5d46c)] - **os, break**: replace os.version with os.release() (ngot) 
* [[`b4220d94f5`](https://github.com/fibjs/fibjs/commit/b4220d94f5)] - **os, chore**: update comment (ngot) 
* [[`0381452de3`](https://github.com/fibjs/fibjs/commit/0381452de3)] - **os, fixbug**: fix error check (ngot) 
* [[`8b08c7e608`](https://github.com/fibjs/fibjs/commit/8b08c7e608)] - **os, fixbug**: throw detail error & mem leak (ngot) 
* [[`67b594cff9`](https://github.com/fibjs/fibjs/commit/67b594cff9)] - **test, fixbug**: fix vm test on windows (ngot) 
* [[`d2e908c89e`](https://github.com/fibjs/fibjs/commit/d2e908c89e)] - **test, feat**: add os.homedir() test case (ngot) 
* [[`955f6e9ab2`](https://github.com/fibjs/fibjs/commit/955f6e9ab2)] - **os, feat**: implememt os.homedir() on windows (ngot) 
* [[`9196a756f5`](https://github.com/fibjs/fibjs/commit/9196a756f5)] - **os, feat**: add os.homedir() (ngot) 
* [[`d80e88a0c1`](https://github.com/fibjs/fibjs/commit/d80e88a0c1)] - **test, feat**: add os.release test case (ngot) 
* [[`c40e82df7a`](https://github.com/fibjs/fibjs/commit/c40e82df7a)] - **os, feat**: add os.release() (ngot) 
* [[`623bb542a8`](https://github.com/fibjs/fibjs/commit/623bb542a8)] - **os, feat**: add os.endianness() (ngot) 
* [[`378d0a0fae`](https://github.com/fibjs/fibjs/commit/378d0a0fae)] - **os, break**: change os.type to os.type() (ngot) 
* [[`bf34081629`](https://github.com/fibjs/fibjs/commit/bf34081629)] - **os, break**: change os.hostname to os.hostname() (ngot) 
* [[`e2b8983df8`](https://github.com/fibjs/fibjs/commit/e2b8983df8)] - **os, break**: rename os.CPUInfo() to os.cpus() & rename os.CPUs() to os.cpuNumbers() (ngot) 
* [[`74154a42fb`](https://github.com/fibjs/fibjs/commit/74154a42fb)] - **os, break**: change os.arch to os.arch() (ngot) 
* [[`f618afa24f`](https://github.com/fibjs/fibjs/commit/f618afa24f)] - **os, break**: rename os.networkInfo() to os.networkInterfaces (ngot) 
* [[`06fe572b72`](https://github.com/fibjs/fibjs/commit/06fe572b72)] - **test, chore**: fix test on windows (ngot) 
* [[`1fafaad796`](https://github.com/fibjs/fibjs/commit/1fafaad796)] - **test, chore**: ignore single test runner (ngot) 
* [[`2145bfdbc0`](https://github.com/fibjs/fibjs/commit/2145bfdbc0)] - **test, feat**: add path.win32 & path.posix test case (ngot) 
* [[`03820c7680`](https://github.com/fibjs/fibjs/commit/03820c7680)] - **path, feat**: enable path.win32 & path.posix both works on win32 & posix (ngot) 
* [[`d97c82eb41`](https://github.com/fibjs/fibjs/commit/d97c82eb41)] - **path, fixbug**: fix posix path sep (ngot) 
* [[`c3e3de463e`](https://github.com/fibjs/fibjs/commit/c3e3de463e)] - **core, fixbug**: fix build error (ngot) 
* [[`35e715ab48`](https://github.com/fibjs/fibjs/commit/35e715ab48)] - **core, chore**: update vcxproj (ngot) 
* [[`3b8ba008df`](https://github.com/fibjs/fibjs/commit/3b8ba008df)] - **path, refactor**: simplify path implementation (ngot) 
* [[`78afbfb026`](https://github.com/fibjs/fibjs/commit/78afbfb026)] - **path, feat**: more compatibility to Node.js path module (ngot) 
* [[`524a241146`](https://github.com/fibjs/fibjs/commit/524a241146)] - **events, chore**: remove unuseful code. (xicilion) 
* [[`b7033a6f9d`](https://github.com/fibjs/fibjs/commit/b7033a6f9d)] - **events, feat**: more compatibility to Node.js events module (xicilion) 
* [[`7c82f03f23`](https://github.com/fibjs/fibjs/commit/7c82f03f23)] - **core, break**: reorder module wrap arguments. (xicilion) 
* [[`9b5e96e3a8`](https://github.com/fibjs/fibjs/commit/9b5e96e3a8)] - **test, fixbug**: enable ignored test case. (xicilion) 
* [[`278d44c7ee`](https://github.com/fibjs/fibjs/commit/278d44c7ee)] - **test, chore**: remove single tester (ngot) 
* [[`f238e16207`](https://github.com/fibjs/fibjs/commit/f238e16207)] - **timers, feat**: add timers module (ngot) 
* [[`c5cd73a116`](https://github.com/fibjs/fibjs/commit/c5cd73a116)] - **gui, break**: disable drag file to gui webbrowser. (xicilion) 
* [[`aabebe6f6b`](https://github.com/fibjs/fibjs/commit/aabebe6f6b)] - **gui, fixbug**: windows gui input control ctrl-v delete error. (xicilion) 
* [[`58c368087f`](https://github.com/fibjs/fibjs/commit/58c368087f)] - **test, chore**: support relative path. (xicilion) 
* [[`46e30cad9a`](https://github.com/fibjs/fibjs/commit/46e30cad9a)] - **doc, chore**: update download link (ngot) 
* [[`37ffd771c1`](https://github.com/fibjs/fibjs/commit/37ffd771c1)] - **module, fixbug**: change path .zip?/file to .zip$/file, fix windows gui url error. (xicilion) 
* [[`5adb9780cf`](https://github.com/fibjs/fibjs/commit/5adb9780cf)] - **ci, fixbug**: ci test (#220) (ngot) 
* [[`2044869df2`](https://github.com/fibjs/fibjs/commit/2044869df2)] - **doc, chore**: update downloads url (ngot) 
* [[`087194a32b`](https://github.com/fibjs/fibjs/commit/087194a32b)] - **fs, feat**: cache *.zip?/ file data. (xicilion) 

## 2017-05-20, Version v0.3.1 @ngot

* **bugfix** :
  * util.compile function export error.
  * fix Timer.clear release error.
* **feature** :
  * HttpClient: enable parallel query
* **internal** :
  * upgrade V8 to 6.0.252

### Commits

* [[`02426d5371`](https://github.com/fibjs/fibjs/commit/02426d5371)] - **ci, feat**: add ssh deploy (#218) (ngot)
* [[`59d743dacc`](https://github.com/fibjs/fibjs/commit/59d743dacc)] - **util, bugfix**: util.compile function export error. (xicilion)
* [[`51af1f60d7`](https://github.com/fibjs/fibjs/commit/51af1f60d7)] - **ci, fix**: osx lost xz deps (ngot)
* [[`0c055cd114`](https://github.com/fibjs/fibjs/commit/0c055cd114)] - **ci, fix**: osx build (ngot)
* [[`82b0aad977`](https://github.com/fibjs/fibjs/commit/82b0aad977)] - **ci, refacor**: rename gui (ngot)
* [[`64b5d53a81`](https://github.com/fibjs/fibjs/commit/64b5d53a81)] - **ci, fix**: add env param (ngot)
* [[`0edf3dcdb9`](https://github.com/fibjs/fibjs/commit/0edf3dcdb9)] - **ci, refactor**: only ci env run the xz compress (ngot)
* [[`4529e4ba0d`](https://github.com/fibjs/fibjs/commit/4529e4ba0d)] - **ci, chore**:output build step message. (xicilion)
* [[`a4e84fd00f`](https://github.com/fibjs/fibjs/commit/a4e84fd00f)] - **http, test**:HttpClient parallel query test. (xicilion)
* [[`53946382d9`](https://github.com/fibjs/fibjs/commit/53946382d9)] - **http, feat**: enable parallel query (xicilion)
* [[`7285222e6e`](https://github.com/fibjs/fibjs/commit/7285222e6e)] - **ci, feat**: enable overwrite when build failed (ngot)
* [[`ff0db46997`](https://github.com/fibjs/fibjs/commit/ff0db46997)] - **ci, fix**: windows x86 cab release (ngot)
* [[`f339104932`](https://github.com/fibjs/fibjs/commit/f339104932)] - **ci, feat**: windows cab files (ngot)
* [[`9bcc013036`](https://github.com/fibjs/fibjs/commit/9bcc013036)] - **ci, feat**: release compress files (ngot)
* [[`3a05b3199b`](https://github.com/fibjs/fibjs/commit/3a05b3199b)] - **deps, feat**: upgrade to v8::Global interface. (xicilion)
* [[`d29ca964f3`](https://github.com/fibjs/fibjs/commit/d29ca964f3)] - **internal, deps**: upgrade v8 to 6.0.252 (xicilion)
* [[`2c6b793304`](https://github.com/fibjs/fibjs/commit/2c6b793304)] - **test, bugfix**: Wait for the timer to release (xicilion)
* [[`0903f95dff`](https://github.com/fibjs/fibjs/commit/0903f95dff)] - **test, refactor**: rewrite GC test case. (xicilion)
* [[`82f0041590`](https://github.com/fibjs/fibjs/commit/82f0041590)] - **timer, bugfix**: fix Timer.clear release error. (xicilion)
* [[`29a8389ca6`](https://github.com/fibjs/fibjs/commit/29a8389ca6)] - **internal, deps**: upgrade v8 (xicilion)
* [[`6168c1ecf2`](https://github.com/fibjs/fibjs/commit/6168c1ecf2)] - **doc, feat**: add gitter badge (ngot)
* [[`3046566b2b`](https://github.com/fibjs/fibjs/commit/3046566b2b)] - **chore, badge**: change build image (#208) (ngot)

## 2017-05-13, Version v0.3.0 @ngot

Good news, we will land the ES6 Module support in the 1.0.0. Before that, we still have a lot of work to do.
### Notable Changes

v0.3.0 is a pre-release update version of fibjs. There are many exciting features in this release such as multi-thread Worker Mode, built-in WebSocket, arm & mip arch support, better compatibility to npm. We recommend you to try these new features. 

* **module**:
  * support `node_modules` directory.
  * remove `.module` directory support.
  * remove `__sbname` support.
  * add `util.compile` method. JavaScript code can be built into ByteCode with `.jsc` extension and it can be required like `require('mod.jsc')`. This is very useful to ship production and protect you code.
  * support virtual zip path. You can pacck you code into a zip file and just require it like this: `require('./test.zip?/b.js'))`
  * syntax check after load error to speed the require.
  * SandBox.require adding base argv，set the start query dir
  * package.json support directory.
  * sandbox add --approot to set the run path.
  * remove the sandbox name from the source path
* **mq**:
  * remove mq.jsHandler
* **rpc**:
  * remove rpc module. Please use coroutine.Worker instead.
* **events**:
  * remove Trigger Object，use events.EventEmitter instead.
* **http**:
  * remove Message.result property
  * add HttpMessage.socket
  * add httpClient
* **coroutine**:
  * add multi-thread Worker support.
* **process**:
  * process extends EventEmitter.
  * add process.nextTick
* **ws**:
  * add WebSocket module.
* **querystring**:
  * add querystring module.
* **zmq**:
  * add zmq module.
* **gui**:
  * add gui module on windows in purpose to support webview.
* **db**:
  * add MSSQL support
### Commits

* [[`233254d`](https://github.com/fibjs/fibjs/commit/233254d)] - **ws, fix**: fix the close error. (xicilion)
* [[`66e4367`](https://github.com/fibjs/fibjs/commit/66e4367)] - **http, feat**: add HttpMessage.socket (xicilion)
* [[`15a430e`](https://github.com/fibjs/fibjs/commit/15a430e)] - **module, remove**: remove .module support (xicilion)
* [[`1ca59b2`](https://github.com/fibjs/fibjs/commit/1ca59b2)] - **module, remove**: remove  Sandbox.compile, moved to util.compile (xicilion)
* [[`f1b89d3`](https://github.com/fibjs/fibjs/commit/f1b89d3)] - **http, refactor**: reuse request.cookies (xicilion)
* [[`3624e39`](https://github.com/fibjs/fibjs/commit/3624e39)] - **mq, remove**: remove mq.jsHandler (xicilion)
* [[`b7d5efd`](https://github.com/fibjs/fibjs/commit/b7d5efd)] - **rpc, remove**: remove rpc module, use coroutine.Worker instead. (xicilion)
* [[`059e771`](https://github.com/fibjs/fibjs/commit/059e771)] - **http, refactor**: reuse request，avoid too many objects created (xicilion)
* [[`298530f`](https://github.com/fibjs/fibjs/commit/298530f)] - **coroutine, feat**: implement native object passing (xicilion)
* [[`4436f62`](https://github.com/fibjs/fibjs/commit/4436f62)] - **coroutine, feat**: implement Worker message passing (xicilion)
* [[`ddc13b8`](https://github.com/fibjs/fibjs/commit/ddc13b8)] - **coroutine, feat**: implement Worker basic message passing，onopen event. (xicilion)
* [[`326a677`](https://github.com/fibjs/fibjs/commit/326a677)] - **os, feat**: add mac address & mask (ngot)
* [[`0796c51`](https://github.com/fibjs/fibjs/commit/0796c51)] - **mq, remove**: remove mq.await, please use sync instead. (xicilion)
* [[`a1219dc`](https://github.com/fibjs/fibjs/commit/a1219dc)] - **module, refactor**: sandbox.run support different mod，add compile argv，compile different target. (xicilion)
* [[`642ddcb`](https://github.com/fibjs/fibjs/commit/642ddcb)] - **zip, fix**: ZipFile  add entry check (xicilion)
* [[`3773f1a`](https://github.com/fibjs/fibjs/commit/3773f1a)] - **coroutine, feat**: add Worker (xicilion)
* [[`7e9db7d`](https://github.com/fibjs/fibjs/commit/7e9db7d)] - **net, fix**: fix remotePort ntohs remotePort (#200) (Hengfei)
* [[`61bd021`](https://github.com/fibjs/fibjs/commit/61bd021)] - **global, fix**: fix repl error (xicilion)
* [[`c5a4786`](https://github.com/fibjs/fibjs/commit/c5a4786)] - **internal, deps**: upgrade SQLlite (xicilion)
* [[`108a180`](https://github.com/fibjs/fibjs/commit/108a180)] - **internal, deps**: upgrade libgd (xicilion)
* [[`d18f338`](https://github.com/fibjs/fibjs/commit/d18f338)] - **core, remove**: remove ppc arch support. (xicilion)
* [[`a0f78f2`](https://github.com/fibjs/fibjs/commit/a0f78f2)] - **util, feat**: LruCache support modifies timeout when invoked. (xicilion)
* [[`0be9fc3`](https://github.com/fibjs/fibjs/commit/0be9fc3)] - **db, fix**: redis pubsub avoid reentry. (xicilion)
* [[`3911558`](https://github.com/fibjs/fibjs/commit/3911558)] - **ws, refactor**: dalay web socket at background read，avoid miss event. (xicilion)
* [[`9437cfc`](https://github.com/fibjs/fibjs/commit/9437cfc)] - **fs, fix**: fix file.stat on windows (ngot)
* [[`3480a0c`](https://github.com/fibjs/fibjs/commit/3480a0c)] - **ssl, feat**: add SslSocket.stream，to query the socket (xicilion)
* [[`cbd4480`](https://github.com/fibjs/fibjs/commit/cbd4480)] - **collection, feat**: add List.lastIndexOf (onc3young)
* [[`17393ee`](https://github.com/fibjs/fibjs/commit/17393ee)] - **http, remove**: remove Message.result (xicilion)
* [[`f72bcb7`](https://github.com/fibjs/fibjs/commit/f72bcb7)] - **rpc, remove**: remove rpc.json (xicilion)
* [[`da0d86f`](https://github.com/fibjs/fibjs/commit/da0d86f)] - **ws, fix**: fix error throw when no error handler. (xicilion)
* [[`214ed13`](https://github.com/fibjs/fibjs/commit/214ed13)] - **collection, feat**: collection.List add indexOf. (zmq)
* [[`000b41f`](https://github.com/fibjs/fibjs/commit/000b41f)] - **gui, refactor**: change WebView event args (xicilion)
* [[`b9d0cce`](https://github.com/fibjs/fibjs/commit/b9d0cce)] - **ws, refactor**: rename WebSocketEvent to EventInfo (xicilion)
* [[`9eaaf7d`](https://github.com/fibjs/fibjs/commit/9eaaf7d)] - **http, feat**: Message add type and data property (xicilion)
* [[`2baf90e`](https://github.com/fibjs/fibjs/commit/2baf90e)] - **util, fix**: fix LruCache logic (xicilion)
* [[`a5582a5`](https://github.com/fibjs/fibjs/commit/a5582a5)] - **mq, refactor**: jsHandler remove method map support, only support [] and {}. (xicilion)
* [[`1df7977`](https://github.com/fibjs/fibjs/commit/1df7977)] - **mq, feat**: add mq.await  (xicilion)
* [[`c796fe9`](https://github.com/fibjs/fibjs/commit/c796fe9)] - **gui, feat**: add edge CONST (ngot)
* [[`b9ceb7f`](https://github.com/fibjs/fibjs/commit/b9ceb7f)] - **util, refactor**: LRUCache updater return undefined not cache. (xicilion)
* [[`7bf0a8b`](https://github.com/fibjs/fibjs/commit/7bf0a8b)] - **net, fix**: fix localPort double ntohs port (ngot)
* [[`30dd890`](https://github.com/fibjs/fibjs/commit/30dd890)] - **registry, fix**: fix a logic error. (xicilion)
* [[`2301a94`](https://github.com/fibjs/fibjs/commit/2301a94)] - **gui, feat**: add gui.setVersion to chose the webview version. (xicilion)
* [[`325b498`](https://github.com/fibjs/fibjs/commit/325b498)] - **collection. feat**: add Map.set (xicilion)
* [[`26a8c1b`](https://github.com/fibjs/fibjs/commit/26a8c1b)] - **crypto, feat**: modify pbkdf2 ，compat nodejs (xicilion)
* [[`b61b9d9`](https://github.com/fibjs/fibjs/commit/b61b9d9)] - **http, feat**: Add Message.end to terminate chain transaction. (xicilion)
* [[`232657c`](https://github.com/fibjs/fibjs/commit/232657c)] - **http, refactor**: reuse Message.response，refactor httpHandler creates new instance by every request. (xicilion)
* [[`bac5eb3`](https://github.com/fibjs/fibjs/commit/bac5eb3)] - **module, refactor**: syntax check after load error to speed the require. (xicilion)
* [[`47ee1ae`](https://github.com/fibjs/fibjs/commit/47ee1ae)] - **process, feat**: process extends EventEmitter (xicilion)
* [[`538f16f`](https://github.com/fibjs/fibjs/commit/538f16f)] - **ws, feat**: complete error handles logic. (xicilion)
* [[`f5505cf`](https://github.com/fibjs/fibjs/commit/f5505cf)] - **db, fix**: fix a SQLite backup error. (xicilion)
* [[`a7fb229`](https://github.com/fibjs/fibjs/commit/a7fb229)] - **ws, feat**: add WebSocketEvent for Object Event. (xicilion)
* [[`8083561`](https://github.com/fibjs/fibjs/commit/8083561)] - **registry, feat**: add registry module to operate the Windows registry. (xicilion)
* [[`2735db2`](https://github.com/fibjs/fibjs/commit/2735db2)] - **path, fix**: fix path.basename error. (xicilion)
* [[`6fad8ac`](https://github.com/fibjs/fibjs/commit/6fad8ac)] - **ws, feat**: complete close logic. (xicilion)
* [[`4113654`](https://github.com/fibjs/fibjs/commit/4113654)] - **ws, feat**: add close & response protocol (xicilion)
* [[`77ae331`](https://github.com/fibjs/fibjs/commit/77ae331)] - **ws, refactor**: WebSocket.close default code to 1000 (xicilion)
* [[`1568ee0`](https://github.com/fibjs/fibjs/commit/1568ee0)] - **io, refactor**: bridge use async post start worker flow. (xicilion)
* [[`267defa`](https://github.com/fibjs/fibjs/commit/267defa)] - **ws, feat**: add ws.upgrade，implement the Event-Driver ws server. (xicilion)
* [[`ce3bcbb`](https://github.com/fibjs/fibjs/commit/ce3bcbb)] - **querystring, feat**: add querystring module. (xicilion)
* [[`b5acc55`](https://github.com/fibjs/fibjs/commit/b5acc55)] - **gui, feat**: Webview add setHtml & print. (xicilion)
* [[`9155774`](https://github.com/fibjs/fibjs/commit/9155774)] - **gui, refactor**: Webview set the init window size. (xicilion)
* [[`344a76b`](https://github.com/fibjs/fibjs/commit/344a76b)] - **gui, feat**: Webview adds visible property to control the window hide or show. (xicilion)
* [[`f560347`](https://github.com/fibjs/fibjs/commit/f560347)] - **gui, feat**: Webview add the onload event (xicilion)
* [[`11e7f01`](https://github.com/fibjs/fibjs/commit/11e7f01)] - **ws, feat**: complete send/onmessage & more test case (xicilion)
* [[`5f01c29`](https://github.com/fibjs/fibjs/commit/5f01c29)] - **ws, feat**: implement WebSocket (xicilion)
* [[`05c865e`](https://github.com/fibjs/fibjs/commit/05c865e)] - **events, feat**: implement events.EventEmitter.call (xicilion)
* [[`e56a75c`](https://github.com/fibjs/fibjs/commit/e56a75c)] - **fs, fix**: fix fs.stat mod logic error. (xicilion)
* [[`8206d37`](https://github.com/fibjs/fibjs/commit/8206d37)] - **fs, fix**: fix the error Linux fs.open never throw the error. (xicilion)
* [[`4d96ebd`](https://github.com/fibjs/fibjs/commit/4d96ebd)] - **ws, refactor**: rename websocket to ws (xicilion)
* [[`e631090`](https://github.com/fibjs/fibjs/commit/e631090)] - **db, refactor**: redis change onxxxx to property. (xicilion)
* [[`b76d1a9`](https://github.com/fibjs/fibjs/commit/b76d1a9)] - **events, remove**: remove Trigger Object，use EventEmitter instead. (xicilion)
* [[`9b76ee6`](https://github.com/fibjs/fibjs/commit/9b76ee6)] - **module, remove**: remove __sbname support (xicilion)
* [[`5b8f29a`](https://github.com/fibjs/fibjs/commit/5b8f29a)] - **util, feat**: info add zmq version (xicilion)
* [[`995e54a`](https://github.com/fibjs/fibjs/commit/995e54a)] - **zmq, feat**: add zmq module (xicilion)
* [[`40cdcdd`](https://github.com/fibjs/fibjs/commit/40cdcdd)] - **collection, feat**: add List.reduce (xicilion)
* [[`815e2d9`](https://github.com/fibjs/fibjs/commit/815e2d9)] - **uuid, feat**: add uuid.snowflake get the unique global id. (xicilion)
* [[`b892c3d`](https://github.com/fibjs/fibjs/commit/b892c3d)] - **os, feat**: support os.tmpdir. (xicilion)
* [[`24f4fdb`](https://github.com/fibjs/fibjs/commit/24f4fdb)] - **assert, feat**: assert support invoked a s a function. (xicilion)
* [[`5b2eb19`](https://github.com/fibjs/fibjs/commit/5b2eb19)] - **path, feat**: add path.resolve. (ngot)
* [[`da1656c`](https://github.com/fibjs/fibjs/commit/da1656c)] - **process, feat**: add process.umask() (ngot)
* [[`d1a8bcd`](https://github.com/fibjs/fibjs/commit/d1a8bcd)] - **coroutine, feat**: lock adds count to query task info. (xicilion)
* [[`ff3c494`](https://github.com/fibjs/fibjs/commit/ff3c494)] - **process, feat**: add process.uptime (ngot)
* [[`8134e68`](https://github.com/fibjs/fibjs/commit/8134e68)] - **db, fix**: mongodb change mongodb m_conn struct，avoid out of bounds errors. (xicilion)
* [[`1584c81`](https://github.com/fibjs/fibjs/commit/1584c81)] - **core, refactor**: exit wait all the async invoke & timer. (xicilion)
* [[`b3023ca`](https://github.com/fibjs/fibjs/commit/b3023ca)] - **coroutine, refactor**: coroutine.sleep use async way. (xicilion)
* [[`7c9e1b7`](https://github.com/fibjs/fibjs/commit/7c9e1b7)] - **core, refactor**: exit before all the fibers exit. (xicilion)
* [[`88e43a5`](https://github.com/fibjs/fibjs/commit/88e43a5)] - **module, fix**: fix .js dir can't be loaded error. (xicilion)
* [[`0e86ed9`](https://github.com/fibjs/fibjs/commit/0e86ed9)] - **console, refactor**: change console flush policy. (xicilion)
* [[`8b76da5`](https://github.com/fibjs/fibjs/commit/8b76da5)] - **fs, refactor**: POSIX init umask to 0 (xicilion)
* [[`93bf249`](https://github.com/fibjs/fibjs/commit/93bf249)] - **fs, feat**: more Sync API. (xicilion)
* [[`0b094f0`](https://github.com/fibjs/fibjs/commit/0b094f0)] - **fs, refactor**: change windows readdir. (xicilion)
* [[`013ac91`](https://github.com/fibjs/fibjs/commit/013ac91)] - **fs, refactor**: change the fs.readdir result，add readdirSync. (xicilion)
* [[`edcc5ca`](https://github.com/fibjs/fibjs/commit/edcc5ca)] - **collection, feat**: List support sort (xicilion)
* [[`d332d58`](https://github.com/fibjs/fibjs/commit/d332d58)] - **collection, feat**: List support callback args (xicilion)
* [[`95663b8`](https://github.com/fibjs/fibjs/commit/95663b8)] - **url, refactor**: Url property can be modified. (xicilion)
* [[`722fff1`](https://github.com/fibjs/fibjs/commit/722fff1)] - **db, refactor**: SQLite use the Object async lock instead of it's custom lock. (xicilion)
* [[`5f951c1`](https://github.com/fibjs/fibjs/commit/5f951c1)] - **core, feat**: callback supports async lock. (xicilion)
* [[`a48fada`](https://github.com/fibjs/fibjs/commit/a48fada)] - **global, refactor**: sync throw error to outside. (xicilion)
* [[`66b0e0b`](https://github.com/fibjs/fibjs/commit/66b0e0b)] - **db, fix**: SQLite execute add lock avoid a crash. (xicilion)
* [[`72a058f`](https://github.com/fibjs/fibjs/commit/72a058f)] - **db, refactor**: DBResult fields，insertId，affected return the default value without throw error. (xicilion)
* [[`d55ae30`](https://github.com/fibjs/fibjs/commit/d55ae30)] - **module, feat**: SandBox.require adding base argv，set the start query dir. (xicilion)
* [[`7f72bea`](https://github.com/fibjs/fibjs/commit/7f72bea)] - **process, feat**: add process.nextTick (xicilion)
* [[`8909cdc`](https://github.com/fibjs/fibjs/commit/8909cdc)] - **global, fix**: fix sync when err is null the mistake throw. (xicilion)
* [[`6999331`](https://github.com/fibjs/fibjs/commit/6999331)] - **url, feat**: url.parse add parseQueryString， (xicilion)
* [[`a443e76`](https://github.com/fibjs/fibjs/commit/a443e76)] - **db, feat**: SQLite add DBConnection type to detect the current connection. (xicilion)
* [[`4d2a6f1`](https://github.com/fibjs/fibjs/commit/4d2a6f1)] - **process, feat**: add process.version (ngot)
* [[`423c1ea`](https://github.com/fibjs/fibjs/commit/423c1ea)] - **net, refactor**: restore the net.UrlObject to net.Url (xicilion)
* [[`99706bd`](https://github.com/fibjs/fibjs/commit/99706bd)] - **events, feat**: add EventEmitter (xicilion)
* [[`508347b`](https://github.com/fibjs/fibjs/commit/508347b)] - **buffer, feat**: add buffer module. (xicilion)
* [[`d545164`](https://github.com/fibjs/fibjs/commit/d545164)] - **util, feat**: add util.inherits (xicilion)
* [[`d2d5cea`](https://github.com/fibjs/fibjs/commit/d2d5cea)] - **net, refactor**: change socket.bind background thread. (xicilion)
* [[`32d0d28`](https://github.com/fibjs/fibjs/commit/32d0d28)] - **process, feat**: add process global & some method. (xicilion)
* [[`f1ebb86`](https://github.com/fibjs/fibjs/commit/f1ebb86)] - **util, fix**: fix format error. (xicilion)
* [[`de5e5b3`](https://github.com/fibjs/fibjs/commit/de5e5b3)] - **module, feat**: package.json support directory. (xicilion)
* [[`0a20b2d`](https://github.com/fibjs/fibjs/commit/0a20b2d)] - **console, fix**: fix osx ESC. (xicilion)
* [[`b72efaa`](https://github.com/fibjs/fibjs/commit/b72efaa)] - **http, feat**: httpClient add more method. (xicilion)
* [[`cb25286`](https://github.com/fibjs/fibjs/commit/cb25286)] - **core, fix**: release callback function，avoid memory leak. (xicilion)
* [[`750cdf0`](https://github.com/fibjs/fibjs/commit/750cdf0)] - **io, feat**: add io.bridge for stream forward both direction. (xicilion)
* [[`059bab9`](https://github.com/fibjs/fibjs/commit/059bab9)] - **io, feat**: add io.copyStream (xicilion)
* [[`b4e95fe`](https://github.com/fibjs/fibjs/commit/b4e95fe)] - **http, feat**: httpClient add default userAgent. (xicilion)
* [[`7d61d13`](https://github.com/fibjs/fibjs/commit/7d61d13)] - **url: Add**:url module. (xicilion)
* [[`50a5d35`](https://github.com/fibjs/fibjs/commit/50a5d35)] - **net, refactor**: rename net.Url to net.UrlObject (xicilion)
* [[`18591b7`](https://github.com/fibjs/fibjs/commit/18591b7)] - **net, feat**: Socket support UDP. (xicilion)
* [[`7122959`](https://github.com/fibjs/fibjs/commit/7122959)] - **global, feat**: add global，compat Node.js (xicilion)
* [[`7fe69e6`](https://github.com/fibjs/fibjs/commit/7fe69e6)] - **db, feat**: SQLite add SQLITE_BUSY support. (xicilion)
* [[`cbde0fb`](https://github.com/fibjs/fibjs/commit/cbde0fb)] - **db, fix**: sqlite prepare retry，avoid error. (xicilion)
* [[`6f3e634`](https://github.com/fibjs/fibjs/commit/6f3e634)] - **coroutine, feat**: parallel add a new mode. (xicilion)
* [[`30b8a87`](https://github.com/fibjs/fibjs/commit/30b8a87)] - **mq, refactor**: adjust Routing match sequeue，add put method. (xicilion)
* [[`5e22575`](https://github.com/fibjs/fibjs/commit/5e22575)] - **http, fix**: fix httphandler zip error. (xicilion)
* [[`9fd0ce4`](https://github.com/fibjs/fibjs/commit/9fd0ce4)] - **gui, feat**: webview support DPI. (xicilion)
* [[`1ef9a62`](https://github.com/fibjs/fibjs/commit/1ef9a62)] - **gui, feat**: webview on HD display auto scale. (xicilion)
* [[`05c9357`](https://github.com/fibjs/fibjs/commit/05c9357)] - **module: support**:node_modules (xicilion)
* [[`27d092c`](https://github.com/fibjs/fibjs/commit/27d092c)] - **module, fix**: fix when require folder/index error can't be throw error. (xicilion)
* [[`4b5b358`](https://github.com/fibjs/fibjs/commit/4b5b358)] - **core fix**: fix socket error canceled on Window XP. (xicilion)
* [[`43832a7`](https://github.com/fibjs/fibjs/commit/43832a7)] - **fs, feat**: fs.open cache the zip file, speed require. (xicilion)
* [[`d6c4b71`](https://github.com/fibjs/fibjs/commit/d6c4b71)] - **fs, fix**: fix fs.open zip logic error on windows. (xicilion)
* [[`e1a0fdf`](https://github.com/fibjs/fibjs/commit/e1a0fdf)] - **encodeing, fix**: fix utf16 encoding error. (xicilion)
* [[`c30506c`](https://github.com/fibjs/fibjs/commit/c30506c)] - **gui, feat**: webview handle PARSE_SECURITY_DOMAIN，support xmlhttp make a request. (xicilion)
* [[`cb8df8b`](https://github.com/fibjs/fibjs/commit/cb8df8b)] - **fs, fix**:  fix fs.stat & File.stat error on window xp. (xicilion)
* [[`a941e5f`](https://github.com/fibjs/fibjs/commit/a941e5f)] - **module, feat**: sandbox add --approot to set the run path. (xicilion)
* [[`e192bf0`](https://github.com/fibjs/fibjs/commit/e192bf0)] - **gui, refactor**: webview close the javascript error report. (xicilion)
* [[`9bf89dd`](https://github.com/fibjs/fibjs/commit/9bf89dd)] - **gui, refactor**: webview disable the right click menu. (xicilion)
* [[`85b78fb`](https://github.com/fibjs/fibjs/commit/85b78fb)] - **gui, feat**: add gui.open window option. (xicilion)
* [[`7373d75`](https://github.com/fibjs/fibjs/commit/7373d75)] - **gui, feat**: add gui.open and move , size event. (xicilion)
* [[`530f80d`](https://github.com/fibjs/fibjs/commit/530f80d)] - **gui, feat**: webview add postMessage/onmessage. (xicilion)
* [[`edeb50e`](https://github.com/fibjs/fibjs/commit/edeb50e)] - **gui, feat**: WebView add wait and move load function logic to open. (xicilion)
* [[`71d2b57`](https://github.com/fibjs/fibjs/commit/71d2b57)] - **gui, fix**: fix the relative path error on Window XP. (xicilion)
* [[`21e3dab`](https://github.com/fibjs/fibjs/commit/21e3dab)] - **gui, feat**: support fs path. (xicilion)
* [[`d6f9e30`](https://github.com/fibjs/fibjs/commit/d6f9e30)] - **net, feat**: add info, get the result of os.networkInfo. (xicilion)
* [[`a811253`](https://github.com/fibjs/fibjs/commit/a811253)] - **os, feat**: add printerInfo get the printer info. (xicilion)
* [[`47ed32e`](https://github.com/fibjs/fibjs/commit/47ed32e)] - **gui, feat**: set windows to HD. (xicilion)
* [[`86282fd`](https://github.com/fibjs/fibjs/commit/86282fd)] - **gui, feat**: add webview event Handler. (xicilion)
* [[`13d0451`](https://github.com/fibjs/fibjs/commit/13d0451)] - **gui, fix**: fix webview open page very slow and the printer problem. (xicilion)
* [[`443d8e0`](https://github.com/fibjs/fibjs/commit/443d8e0)] - **gui, refactor**: remove the 3D border. (xicilion)
* [[`b57767b`](https://github.com/fibjs/fibjs/commit/b57767b)] - **db, refactor**: MySQL set command timeout 0. (xicilion)
* [[`9f7b01a`](https://github.com/fibjs/fibjs/commit/9f7b01a)] - **db, refactor**: when SQL query return null, length is 0, not throw an error. (xicilion)
* [[`6c05f87`](https://github.com/fibjs/fibjs/commit/6c05f87)] - **gui, feat**: add gui.open. (xicilion)
* [[`1936d6f`](https://github.com/fibjs/fibjs/commit/1936d6f)] - **db, feat**: MySQL support affected (xicilion)
* [[`4823ac6`](https://github.com/fibjs/fibjs/commit/4823ac6)] - **gui, feat**: webview support close function. (xicilion)
* [[`ed14cb1`](https://github.com/fibjs/fibjs/commit/ed14cb1)] - **module, feat**: require supprt .zip? extension. (xicilion)
* [[`db8e099`](https://github.com/fibjs/fibjs/commit/db8e099)] - **fs, feat**: fs.open support virtual zip path. (xicilion)
* [[`f32d777`](https://github.com/fibjs/fibjs/commit/f32d777)] - **fs, refactor**:  fs.open return SeekableStream type. (xicilion)
* [[`e2b3f4b`](https://github.com/fibjs/fibjs/commit/e2b3f4b)] - **gui, feat**: add gui mode，add windows mode entry. (xicilion)
* [[`11d999e`](https://github.com/fibjs/fibjs/commit/11d999e)] - **gui, feat**: webview logo (xicilion)
* [[`9f6f9bf`](https://github.com/fibjs/fibjs/commit/9f6f9bf)] - **module, remove**: remove the sandbox name from the source path. (xicilion)
* [[`1f09d6f`](https://github.com/fibjs/fibjs/commit/1f09d6f)] - **fs, refactor**: fs.readFile return Buffer Object and add fs.readTextFile for txt read. (xicilion)
* [[`ee14ea1`](https://github.com/fibjs/fibjs/commit/ee14ea1)] - **encoding, fix**: fix sometimes parse error. (xicilion)
* [[`9682aa9`](https://github.com/fibjs/fibjs/commit/9682aa9)] - **console, fix**: fix big string output error on Window XP. (xicilion)
* [[`6dfdc20`](https://github.com/fibjs/fibjs/commit/6dfdc20)] - **db, feat**: MySQL support null & decimal. (xicilion)
* [[`ee4930e`](https://github.com/fibjs/fibjs/commit/ee4930e)] - **db, feat**: add MSSQL support. (xicilion)
* [[`b1c68ef`](https://github.com/fibjs/fibjs/commit/b1c68ef)] - **db, refactor**: SQLite default to wal mode. (xicilion)
* [[`62af5f0`](https://github.com/fibjs/fibjs/commit/62af5f0)] - **os, feat**: Add Service Object on Windows. (xicilion)
* [[`0e24bc9`](https://github.com/fibjs/fibjs/commit/0e24bc9)] - **http, fix**: fix the message starts with '0' cut off when transferred by chunk. (xicilion)
* [[`340fcd9`](https://github.com/fibjs/fibjs/commit/340fcd9)] - **mq, feat**: Routing add all/get/post (xicilion)
* [[`116d751`](https://github.com/fibjs/fibjs/commit/116d751)] - **console, feat**: support simulates the mouse. (xicilion)
* [[`123869f`](https://github.com/fibjs/fibjs/commit/123869f)] - **fs, feat**: add copy function. (xicilion)
* [[`2fa93ab`](https://github.com/fibjs/fibjs/commit/2fa93ab)] - **console, fix**: fix the fn keyboard support. (xicilion)
* [[`a4cf360`](https://github.com/fibjs/fibjs/commit/a4cf360)] - **gui, refactor**: rename hasWindow to findWindow & return the rect on Windows. (xicilion)
* [[`7812ca9`](https://github.com/fibjs/fibjs/commit/7812ca9)] - **gui, fix**: hasWindow ignore the hidden window. (xicilion)
* [[`9ea564f`](https://github.com/fibjs/fibjs/commit/9ea564f)] - **console, feat**: support simulates keyboard input for an automated task. (xicilion)
* [[`19885a3`](https://github.com/fibjs/fibjs/commit/19885a3)] - **SubProcess, feat**: add hasWindow. (xicilion)
* [[`201218f`](https://github.com/fibjs/fibjs/commit/201218f)] - **gui, feat**: add Windows gui convert tool. (xicilion)
* [[`7f4aeea`](https://github.com/fibjs/fibjs/commit/7f4aeea)] - **gui, feat**: add gui module on windows in purpose to support webview. (xicilion)
* [[`80f1463`](https://github.com/fibjs/fibjs/commit/80f1463)] - **SubProcess, refactor**: use TerminateProcess to terminate process on Windows. (xicilion)
* [[`6ef637a`](https://github.com/fibjs/fibjs/commit/6ef637a)] - **SubProcess, refactor**: directly invoke the process on windows without cmd. (xicilion)
* [[`328d121`](https://github.com/fibjs/fibjs/commit/328d121)] - **smtp, feat**: add timeout & ssl connection support. (xicilion)
* [[`9e894ee`](https://github.com/fibjs/fibjs/commit/9e894ee)] - **ssl, feat**: ssl.connect support timeout (xicilion)
* [[`30c905e`](https://github.com/fibjs/fibjs/commit/30c905e)] - **module, feat**: Sandbox support compile method and require/addScript/run support jsc. (xicilion)
* [[`bd52b4f`](https://github.com/fibjs/fibjs/commit/bd52b4f)] - **fs, feat**: fs.writeFile support binary data. (xicilion)
* [[`137a9e0`](https://github.com/fibjs/fibjs/commit/137a9e0)] - **core, fix**: fix nonascii char on windows. (xicilion)
* [[`b07c2bc`](https://github.com/fibjs/fibjs/commit/b07c2bc)] - **console, feat**: add event log output on windows. (xicilion)
* [[`d470614`](https://github.com/fibjs/fibjs/commit/d470614)] - **console, feat**: add moveTo, hideCursor, showCursor, clear (xicilion)
* [[`0596f18`](https://github.com/fibjs/fibjs/commit/0596f18)] - **console, fix**: fix console.timeEnd interger overflow error (asionius)
* [[`09ed0fc`](https://github.com/fibjs/fibjs/commit/09ed0fc)] - **SubProcess, fix**: fix the timer create an error of SubProcess on windows. (asionius)
* [[`c0f17ec`](https://github.com/fibjs/fibjs/commit/c0f17ec)] - **console, feat**: add width，height for query or set the console size. (xicilion)
* [[`94cecb3`](https://github.com/fibjs/fibjs/commit/94cecb3)] - **http, feat**: add httpClient (asionius)
* [[`f259bcc`](https://github.com/fibjs/fibjs/commit/f259bcc)] - **core, feat**: support mips arch (xicilion)
* [[`f94b0ee`](https://github.com/fibjs/fibjs/commit/f94b0ee)] - **Socket, feat**: socket add timeout (xicilion)
* [[`ef9577d`](https://github.com/fibjs/fibjs/commit/ef9577d)] - **internal, deps**: upgrade gd (xicilion)
