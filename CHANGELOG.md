## 2018-02-06, Version v0.22.0
* **feature** :
    * Buffer - support Buffer.join method.(xicilion)
    * core:
      - upgrade to new v8 api.(xicilion)
      - support require.main.(xicilion)
    * dgram:
      - implement method setBroadcast.(xicilion)
      - method send return data size.(xicilion)
      - add documents for dgram module.(xicilion)
      - add argument to event 'error'.(xicilion)
      - add dgram test case into main.js.(xicilion)
      - add test case for dgram.(xicilion)
      - implement send and message.(xicilion)
      - add dgram module and DgramSocket object.(xicilion)
    * dns - add dns.lookup.(xicilion)
    * module - not check param count in require.(xicilion)
    * option - simple package install tool.(xicilion)
    * options:
      - enable parallel scavenge.(xicilion)
      - support install package.(xicilion)
      - add init script.(xicilion)
      - generate embed scripts.(xicilion)
      - retry download when http error.(xicilion)
    * sqlite - change default synchronous mode to full.(xicilion)
    * timers:
      - allow null in clearTimeout.(xicilion)
      - support ref/unref in Timer object.(xicilion)
    * v8 - upgrade to v6.6.155.(xicilion)
    * ws - support ref/unref.(xicilion)
    * zlib - end inflate stream when data error.(xicilion)

* **bugfix** :
    * Buffer:
      - crash when Buffer.concat([])(xicilion)
      - new Buffer error when TypedArray offset is not zero.(xicilion)
    * dgram:
      - send error when addr is empty on windows.(xicilion)
      - test error on linux.(xicilion)
      - compile error on windows.(xicilion)
    * http - crash when HttpResponse.statusCode is too large.(xicilion)
    * option:
      - data error on some tgz file.(xicilion)
      - download error when '/' in package name.(xicilion)
    * options:
      - init error when package.json is not exists.(xicilion)
      - zero size file is not extracted.(xicilion)
    * path - compile warning in debug mode.(xicilion)
    * process - compile warning in debug mode.(xicilion)
    * stream - fix buffer type check.(xicilion)
    * util - app crash when clone a Map.(xicilion)

### Commits
* [[`be4e3b122f`](https://github.com/fibjs/fibjs/commit/be4e3b122f)] - **options, feat**: enable parallel scavenge.(xicilion)
* [[`a4ff03be02`](https://github.com/fibjs/fibjs/commit/a4ff03be02)] - **v8, feat**: upgrade to v6.6.155.(xicilion)
* [[`551bcfb693`](https://github.com/fibjs/fibjs/commit/551bcfb693)] - **util, bugfix**: app crash when clone a Map.(xicilion)
* [[`7d9222fbb0`](https://github.com/fibjs/fibjs/commit/7d9222fbb0)] - **core, feat**: upgrade to new v8 api.(xicilion)
* [[`3f12dfe7b6`](https://github.com/fibjs/fibjs/commit/3f12dfe7b6)] - **stream, bugfix**: fix buffer type check.(xicilion)
* [[`44a3e84013`](https://github.com/fibjs/fibjs/commit/44a3e84013)] - **ws, feat**: support ref/unref.(xicilion)
* [[`ff341edc49`](https://github.com/fibjs/fibjs/commit/ff341edc49)] - **dgram, feat**: implement method setBroadcast.(xicilion)
* [[`6f0ee079ae`](https://github.com/fibjs/fibjs/commit/6f0ee079ae)] - **module, feat**: not check param count in require.(xicilion)
* [[`a17b1835d5`](https://github.com/fibjs/fibjs/commit/a17b1835d5)] - **options, bugfix**: init error when package.json is not exists.(xicilion)
* [[`b9d53e2c08`](https://github.com/fibjs/fibjs/commit/b9d53e2c08)] - **dgram, feat**: method send return data size.(xicilion)
* [[`469d0d0e7c`](https://github.com/fibjs/fibjs/commit/469d0d0e7c)] - **core, feat**: support require.main.(xicilion)
* [[`44287c8558`](https://github.com/fibjs/fibjs/commit/44287c8558)] - **options, feat**: support install package.(xicilion)
* [[`a75a1b98d8`](https://github.com/fibjs/fibjs/commit/a75a1b98d8)] - **options, feat**: add init script.(xicilion)
* [[`4395512533`](https://github.com/fibjs/fibjs/commit/4395512533)] - **options, feat**: generate embed scripts.(xicilion)
* [[`416b7e1137`](https://github.com/fibjs/fibjs/commit/416b7e1137)] - **options, feat**: retry download when http error.(xicilion)
* [[`3e1bd98609`](https://github.com/fibjs/fibjs/commit/3e1bd98609)] - **options, bugfix**: zero size file is not extracted.(xicilion)
* [[`4625da4cbe`](https://github.com/fibjs/fibjs/commit/4625da4cbe)] - **dgram, feat**: add documents for dgram module.(xicilion)
* [[`1c81e1d84c`](https://github.com/fibjs/fibjs/commit/1c81e1d84c)] - **sqlite, feat**: change default synchronous mode to full.(xicilion)
* [[`ce138af65d`](https://github.com/fibjs/fibjs/commit/ce138af65d)] - **timers, feat**: allow null in clearTimeout.(xicilion)
* [[`781ab7427f`](https://github.com/fibjs/fibjs/commit/781ab7427f)] - **dgram, feat**: add argument to event 'error'.(xicilion)
* [[`ffc6cc69e3`](https://github.com/fibjs/fibjs/commit/ffc6cc69e3)] - **dgram, bugfix**: send error when addr is empty on windows.(xicilion)
* [[`8203c574b9`](https://github.com/fibjs/fibjs/commit/8203c574b9)] - **Buffer, feat**: support Buffer.join method.(xicilion)
* [[`d5e4bf1cb8`](https://github.com/fibjs/fibjs/commit/d5e4bf1cb8)] - **Buffer, bugfix**: crash when Buffer.concat([])(xicilion)
* [[`962c505310`](https://github.com/fibjs/fibjs/commit/962c505310)] - **dgram, bugfix**: test error on linux.(xicilion)
* [[`47abe8b7f6`](https://github.com/fibjs/fibjs/commit/47abe8b7f6)] - **dgram, feat**: add dgram test case into main.js.(xicilion)
* [[`4701be824b`](https://github.com/fibjs/fibjs/commit/4701be824b)] - **dgram, feat**: add test case for dgram.(xicilion)
* [[`dfd063c4ea`](https://github.com/fibjs/fibjs/commit/dfd063c4ea)] - **dgram, bugfix**: compile error on windows.(xicilion)
* [[`e4eb495b37`](https://github.com/fibjs/fibjs/commit/e4eb495b37)] - **dns, feat**: add dns.lookup.(xicilion)
* [[`8d54dd1aaa`](https://github.com/fibjs/fibjs/commit/8d54dd1aaa)] - **dgram, feat**: implement send and message.(xicilion)
* [[`1ae403550d`](https://github.com/fibjs/fibjs/commit/1ae403550d)] - **dgram, feat**: add dgram module and DgramSocket object.(xicilion)
* [[`45fcbb3bb3`](https://github.com/fibjs/fibjs/commit/45fcbb3bb3)] - **http, bugfix**: crash when HttpResponse.statusCode is too large.(xicilion)
* [[`868e455f2b`](https://github.com/fibjs/fibjs/commit/868e455f2b)] - **process, bugfix**: compile warning in debug mode.(xicilion)
* [[`20cf8c4f00`](https://github.com/fibjs/fibjs/commit/20cf8c4f00)] - **path, bugfix**: compile warning in debug mode.(xicilion)
* [[`64317ed84b`](https://github.com/fibjs/fibjs/commit/64317ed84b)] - **option, bugfix**: data error on some tgz file.(xicilion)
* [[`50a2673da0`](https://github.com/fibjs/fibjs/commit/50a2673da0)] - **zlib, feat**: end inflate stream when data error.(xicilion)
* [[`f3cedc796d`](https://github.com/fibjs/fibjs/commit/f3cedc796d)] - **option, bugfix**: download error when '/' in package name.(xicilion)
* [[`41db91f502`](https://github.com/fibjs/fibjs/commit/41db91f502)] - **option, feat**: simple package install tool.(xicilion)
* [[`7e046495d7`](https://github.com/fibjs/fibjs/commit/7e046495d7)] - **Buffer, bugfix**: new Buffer error when TypedArray offset is not zero.(xicilion)
* [[`94211628ec`](https://github.com/fibjs/fibjs/commit/94211628ec)] - **timers, feat**: support ref/unref in Timer object.(xicilion)

## 2017-12-28, Version v0.21.0
* **feature** :
    * core - disable code coverage for embed script module.(xicilion)
    * coroutine - use hrtime to implement sleep.(xicilion)
    * crypto - update root ca.(xicilion)
    * dns - add module dns, support dns.resolve.(xicilion)
    * expat - upgrade to v2.2.5(xicilion)
    * git - commit .jsc file as binary file.(xicilion)
    * repl - add 'Thown:' prefix in repl (#385)(Hengfei Zhuang)
    * stream - port script module stream from node.(xicilion)
    * util:
      - output Error object friendly.(xicilion)
      - support custom Promise object in util.syn.(xicilion)
    * v8:
      - upgrade to v6.5.108.(xicilion)
      - upgrade to v6.5.26.(xicilion)

* **bugfix** :
    * core:
      - refactor object manager, fix runtime error in debug mode.(xicilion)
      - fix compile waring in debug mode.(xicilion)
    * db - deadlock when multi fiber access in db.trans.(xicilion)
    * dns - fix ipv6 address error.(xicilion)
    * exlib - fix sleep time error.(xicilion)
    * test - compile new amd64 jsc test file.(xicilion)
    * v8:
      - fix compile error in debug mode on windows.(xicilion)
      - fix compile warning on windows.(xicilion)

* **refactor** :
    * core - move opt_tools script to sub folder.(xicilion)
    * mq - remove unused code.(xicilion)

* **others** :
    * change memcpy to memmove (#391)(zywww)
    * Merge branch 'master' into dev(xicilion)

### Commits
* [[`2e6be70555`](https://github.com/fibjs/fibjs/commit/2e6be70555)] - **v8, bugfix**: fix compile error in debug mode on windows.(xicilion)
* [[`2a1d13f2c4`](https://github.com/fibjs/fibjs/commit/2a1d13f2c4)] - **v8, feat**: upgrade to v6.5.108.(xicilion)
* [[`19763e6253`](https://github.com/fibjs/fibjs/commit/19763e6253)] - **dns, bugfix**: fix ipv6 address error.(xicilion)
* [[`02992c421b`](https://github.com/fibjs/fibjs/commit/02992c421b)] - **expat, feat**: upgrade to v2.2.5(xicilion)
* [[`4523884506`](https://github.com/fibjs/fibjs/commit/4523884506)] - **core, bugfix**: refactor object manager, fix runtime error in debug mode.(xicilion)
* [[`6de34423b4`](https://github.com/fibjs/fibjs/commit/6de34423b4)] - **mq, refactor**: remove unused code.(xicilion)
* [[`5b071a3987`](https://github.com/fibjs/fibjs/commit/5b071a3987)] - change memcpy to memmove (#391)(zywww)
* [[`5b694853b0`](https://github.com/fibjs/fibjs/commit/5b694853b0)] - **core, bugfix**: fix compile waring in debug mode.(xicilion)
* [[`3a8ba731d7`](https://github.com/fibjs/fibjs/commit/3a8ba731d7)] - **test, bugfix**: compile new amd64 jsc test file.(xicilion)
* [[`5d2d96baac`](https://github.com/fibjs/fibjs/commit/5d2d96baac)] - **git, feat**: commit .jsc file as binary file.(xicilion)
* [[`1288a3d8ed`](https://github.com/fibjs/fibjs/commit/1288a3d8ed)] - **crypto, feat**: update root ca.(xicilion)
* [[`97e25de6a3`](https://github.com/fibjs/fibjs/commit/97e25de6a3)] - **exlib, bugfix**: fix sleep time error.(xicilion)
* [[`6a105104b4`](https://github.com/fibjs/fibjs/commit/6a105104b4)] - **core, refactor**: move opt_tools script to sub folder.(xicilion)
* [[`4ded2bd6ca`](https://github.com/fibjs/fibjs/commit/4ded2bd6ca)] - **coroutine, feat**: use hrtime to implement sleep.(xicilion)
* [[`9abe80fe9d`](https://github.com/fibjs/fibjs/commit/9abe80fe9d)] - **core, feat**: disable code coverage for embed script module.(xicilion)
* [[`2e6e961dbb`](https://github.com/fibjs/fibjs/commit/2e6e961dbb)] - **util, feat**: output Error object friendly.(xicilion)
* [[`52b984cb66`](https://github.com/fibjs/fibjs/commit/52b984cb66)] - **dns, feat**: add module dns, support dns.resolve.(xicilion)
* [[`b85cc934d6`](https://github.com/fibjs/fibjs/commit/b85cc934d6)] - **stream, feat**: port script module stream from node.(xicilion)
* [[`29f5cb6fe4`](https://github.com/fibjs/fibjs/commit/29f5cb6fe4)] - **util, feat**: support custom Promise object in util.syn.(xicilion)
* [[`0c97518e05`](https://github.com/fibjs/fibjs/commit/0c97518e05)] - **v8, bugfix**: fix compile warning on windows.(xicilion)
* [[`782bd434fe`](https://github.com/fibjs/fibjs/commit/782bd434fe)] - **v8, feat**: upgrade to v6.5.26.(xicilion)
* [[`68a1cb5bc4`](https://github.com/fibjs/fibjs/commit/68a1cb5bc4)] - **db, bugfix**: deadlock when multi fiber access in db.trans.(xicilion)
* [[`4394b5e029`](https://github.com/fibjs/fibjs/commit/4394b5e029)] - Merge branch 'master' into dev(xicilion)
* [[`33f3c01fc4`](https://github.com/fibjs/fibjs/commit/33f3c01fc4)] - **repl, feat**: add 'Thown:' prefix in repl (#385)(Hengfei Zhuang)

## 2017-12-03, Version v0.20.0
* **feature** :
    * constants - support constants module.(xicilion)
    * coroutine - hold the process while at least one worker is working.(xicilion)
    * crypto - PKey constructor support parameters.(xicilion)
    * db:
      - support multi level array in db.format.(xicilion)
      - enable chinese fulltext search in SQLite.(xicilion)
    * event - support any type of custom field in EventInfo.(xicilion)
    * LruCache - support expire event.(xicilion)
    * mbedtls - upgrade to v2.6.1.(xicilion)
    * sandbox - support exports in main script.(xicilion)
    * test - add LruCache lock test.(xicilion)
    * tool - upgrade awesome sync tool.(xicilion)
    * v8:
      - update vender version.(xicilion)
      - upgrade v8 to v6.4.365.(xicilion)
      - upgrade to v6.4.309.(xicilion)
    * websocket - hold the process while websocket is not disconnected.(xicilion)

* **bugfix** :
    * core:
      - fix compile waring on windows.(xicilion)
      - fix compile error on windows.(xicilion)
    * LruCache - object locked in updater callback.(xicilion)
    * v8 - disable parallel_marking, fix crash in snapshot creator.(xicilion)

* **refactor** :
    * core:
      - uniform the two constructor of AsyncCallback.(xicilion)
      - support ref/unref in isolate, refactor the logic of holding process.(xicilion)
      - format JavaScript code.(xicilion)
    * mq - move Message.response to HttpRequest.response.(xicilion)

### Commits
* [[`a9a78dd7a6`](https://github.com/fibjs/fibjs/commit/a9a78dd7a6)] - **mq, refactor**: move Message.response to HttpRequest.response.(xicilion)
* [[`46dd85fec2`](https://github.com/fibjs/fibjs/commit/46dd85fec2)] - **LruCache, feat**: support expire event.(xicilion)
* [[`205cef37b7`](https://github.com/fibjs/fibjs/commit/205cef37b7)] - **event, feat**: support any type of custom field in EventInfo.(xicilion)
* [[`f2b6e1fe05`](https://github.com/fibjs/fibjs/commit/f2b6e1fe05)] - **test, feat**: add LruCache lock test.(xicilion)
* [[`8376932226`](https://github.com/fibjs/fibjs/commit/8376932226)] - **LruCache, bugfix**: object locked in updater callback.(xicilion)
* [[`66a518641b`](https://github.com/fibjs/fibjs/commit/66a518641b)] - **mbedtls, feat**: upgrade to v2.6.1.(xicilion)
* [[`d59b73839f`](https://github.com/fibjs/fibjs/commit/d59b73839f)] - **tool, feat**: upgrade awesome sync tool.(xicilion)
* [[`ac25be15bd`](https://github.com/fibjs/fibjs/commit/ac25be15bd)] - **core, bugfix**: fix compile waring on windows.(xicilion)
* [[`f6caafc9d4`](https://github.com/fibjs/fibjs/commit/f6caafc9d4)] - **core, bugfix**: fix compile error on windows.(xicilion)
* [[`5ac83f7b29`](https://github.com/fibjs/fibjs/commit/5ac83f7b29)] - **db, feat**: support multi level array in db.format.(xicilion)
* [[`e20a32ea10`](https://github.com/fibjs/fibjs/commit/e20a32ea10)] - **coroutine, feat**: hold the process while at least one worker is working.(xicilion)
* [[`38bdf89608`](https://github.com/fibjs/fibjs/commit/38bdf89608)] - **websocket, feat**: hold the process while websocket is not disconnected.(xicilion)
* [[`c73b24caaf`](https://github.com/fibjs/fibjs/commit/c73b24caaf)] - **core, refactor**: uniform the two constructor of AsyncCallback.(xicilion)
* [[`ff75686678`](https://github.com/fibjs/fibjs/commit/ff75686678)] - **core, refactor**: support ref/unref in isolate, refactor the logic of holding process.(xicilion)
* [[`b617bb877a`](https://github.com/fibjs/fibjs/commit/b617bb877a)] - **sandbox, feat**: support exports in main script.(xicilion)
* [[`9a240973b8`](https://github.com/fibjs/fibjs/commit/9a240973b8)] - **db, feat**: enable chinese fulltext search in SQLite.(xicilion)
* [[`7886dc4bac`](https://github.com/fibjs/fibjs/commit/7886dc4bac)] - **v8, bugfix**: disable parallel_marking, fix crash in snapshot creator.(xicilion)
* [[`62ca518a70`](https://github.com/fibjs/fibjs/commit/62ca518a70)] - **v8, feat**: update vender version.(xicilion)
* [[`aa2d8bbe86`](https://github.com/fibjs/fibjs/commit/aa2d8bbe86)] - **v8, feat**: upgrade v8 to v6.4.365.(xicilion)
* [[`8f0bc1f271`](https://github.com/fibjs/fibjs/commit/8f0bc1f271)] - **core, refactor**: format JavaScript code.(xicilion)
* [[`3f895d267b`](https://github.com/fibjs/fibjs/commit/3f895d267b)] - **v8, feat**: upgrade to v6.4.309.(xicilion)
* [[`076830eaa1`](https://github.com/fibjs/fibjs/commit/076830eaa1)] - **crypto, feat**: PKey constructor support parameters.(xicilion)
* [[`575deddaac`](https://github.com/fibjs/fibjs/commit/575deddaac)] - **constants, feat**: support constants module.(xicilion)

## 2017-11-15, Version v0.19.0
* **feature** :
    * build - update task.json to version 2.0.(xicilion)
    * core:
      - add full error type support (#381)(Hengfei Zhuang)
      - disable bigobj compile option on windows.(xicilion)
    * db - switch mysql to fiber worker.(xicilion)
    * path:
      - add path.toNamespacedPath  (#375)(Hengfei Zhuang)
      - add relative test case (not use).(xicilion)
      - add zero test case.(xicilion)
    * process:
      - use RegisterWaitForSingleObject to wait child process in real async mode on windows.(xicilion)
      - use ev to wait child process in real async mode.(xicilion)
      - only check exit code on linux.(xicilion)
    * repl - add all the builtin modules to the repl global (#377)(Hengfei Zhuang)

* **bugfix** :
    * core:
      - compile error on windows.(xicilion)
      - use posix_spawnp to check glibc_2.2, fix compile on i386.(xicilion)
      - use check_c_source_compiles to check glibc_2.2.5, fix alpine compile error.(xicilion)
    * fs - fix fs stat when path endsWith slash (#371)(Hengfei Zhuang)
    * gui - fix the gui testcase (#379)(Hengfei Zhuang)
    * http - parallel request in HttpClient will crash when response cookies.(xicilion)
    * path:
      - fix test case error on windows.(xicilion)
      - keep the disk no of the last path.(xicilion)
    * process:
      - SubProcess.wait return wrong status some times on linux.(xicilion)
      - process.wait not return while process is already exit.(xicilion)
      - compile error on windows i386.(xicilion)
      - process.run not throw error when exec file not exists on linux.(xicilion)
    * unzip - fix isZipFile error if filename is in unicode (#373)(王爱科)

* **refactor** :
    * core:
      - ThrowAssertionError use reference to avoid copy (#383)(Hengfei Zhuang)
      - more powerful error system (#378)(Hengfei Zhuang)
      - Refactor the asyncEv class.(xicilion)
    * net - remove no use argument in function AsyncIO::close.(xicilion)
    * path:
      - rewrite path.isAbsolute, more compatible with node.js(xicilion)
      - rewrite path.basename/extname/dirname, more compatible with node.js(xicilion)
      - rewrite path.resolve, more compatible with node.js (#374)(Hengfei Zhuang)
      - rewrite path.resolve, more compatible with node.js(xicilion)
      - rewrite path.join, more compatible with node.js(xicilion)
      - rewrite path.normalize, more compatible with node.js(xicilion)
    * process - use AsyncIO.close to close process pipe.(xicilion)
    * util - make inherits more compact with node.js (#380)(Hengfei Zhuang)

* **test** :
    * path - adjust path.basename test case (#376)(Hengfei Zhuang)

### Commits
* [[`7f5b2482fc`](https://github.com/fibjs/fibjs/commit/7f5b2482fc)] - Release v0.19.0(xicilion)
* [[`1aa840c663`](https://github.com/fibjs/fibjs/commit/1aa840c663)] - **process, bugfix**: SubProcess.wait return wrong status some times on linux.(xicilion)
* [[`3f19c1fac3`](https://github.com/fibjs/fibjs/commit/3f19c1fac3)] - **core, refactor**: ThrowAssertionError use reference to avoid copy (#383)(Hengfei Zhuang)
* [[`dbe3c74ebd`](https://github.com/fibjs/fibjs/commit/dbe3c74ebd)] - **core, feat**: add full error type support (#381)(Hengfei Zhuang)
* [[`89454b908a`](https://github.com/fibjs/fibjs/commit/89454b908a)] - **process, bugfix**: process.wait not return while process is already exit.(xicilion)
* [[`197375424c`](https://github.com/fibjs/fibjs/commit/197375424c)] - **util, refactor**: make inherits more compact with node.js (#380)(Hengfei Zhuang)
* [[`7b200e800e`](https://github.com/fibjs/fibjs/commit/7b200e800e)] - **gui, bugfix**: fix the gui testcase (#379)(Hengfei Zhuang)
* [[`b41dfd973e`](https://github.com/fibjs/fibjs/commit/b41dfd973e)] - **core, refactor**: more powerful error system (#378)(Hengfei Zhuang)
* [[`f5d12cdcfc`](https://github.com/fibjs/fibjs/commit/f5d12cdcfc)] - **build, feat**: update task.json to version 2.0.(xicilion)
* [[`4f7959a4af`](https://github.com/fibjs/fibjs/commit/4f7959a4af)] - **repl, feat**: add all the builtin modules to the repl global (#377)(Hengfei Zhuang)
* [[`c630e3b2b1`](https://github.com/fibjs/fibjs/commit/c630e3b2b1)] - **process, bugfix**: compile error on windows i386.(xicilion)
* [[`5f33537c67`](https://github.com/fibjs/fibjs/commit/5f33537c67)] - **db, feat**: switch mysql to fiber worker.(xicilion)
* [[`d89d4d96d6`](https://github.com/fibjs/fibjs/commit/d89d4d96d6)] - **process, feat**: use RegisterWaitForSingleObject to wait child process in real async mode on windows.(xicilion)
* [[`e71a1bf382`](https://github.com/fibjs/fibjs/commit/e71a1bf382)] - **net, refactor**: remove no use argument in function AsyncIO::close.(xicilion)
* [[`2dbe78cd29`](https://github.com/fibjs/fibjs/commit/2dbe78cd29)] - **process, feat**: use ev to wait child process in real async mode.(xicilion)
* [[`a4659645de`](https://github.com/fibjs/fibjs/commit/a4659645de)] - **core, refactor**: Refactor the asyncEv class.(xicilion)
* [[`b82e545c1c`](https://github.com/fibjs/fibjs/commit/b82e545c1c)] - **http, bugfix**: parallel request in HttpClient will crash when response cookies.(xicilion)
* [[`910b473727`](https://github.com/fibjs/fibjs/commit/910b473727)] - **process, refactor**: use AsyncIO.close to close process pipe.(xicilion)
* [[`8bff4df01c`](https://github.com/fibjs/fibjs/commit/8bff4df01c)] - **process, feat**: only check exit code on linux.(xicilion)
* [[`261ddfee1a`](https://github.com/fibjs/fibjs/commit/261ddfee1a)] - **process, bugfix**: process.run not throw error when exec file not exists on linux.(xicilion)
* [[`cb0ef916ba`](https://github.com/fibjs/fibjs/commit/cb0ef916ba)] - **core, bugfix**: compile error on windows.(xicilion)
* [[`ce343b409e`](https://github.com/fibjs/fibjs/commit/ce343b409e)] - **core, bugfix**: use posix_spawnp to check glibc_2.2, fix compile on i386.(xicilion)
* [[`c301b9085c`](https://github.com/fibjs/fibjs/commit/c301b9085c)] - **core, bugfix**: use check_c_source_compiles to check glibc_2.2.5, fix alpine compile error.(xicilion)
* [[`ad39cae7c9`](https://github.com/fibjs/fibjs/commit/ad39cae7c9)] - **path, test**: adjust path.basename test case (#376)(Hengfei Zhuang)
* [[`2bdd279eae`](https://github.com/fibjs/fibjs/commit/2bdd279eae)] - **fs, bugfix**: fix fs stat when path endsWith slash (#371)(Hengfei Zhuang)
* [[`e067af4a17`](https://github.com/fibjs/fibjs/commit/e067af4a17)] - **path, feat**: add path.toNamespacedPath  (#375)(Hengfei Zhuang)
* [[`fc7b7b974b`](https://github.com/fibjs/fibjs/commit/fc7b7b974b)] - **path, feat**: add relative test case (not use).(xicilion)
* [[`e5bb49aa20`](https://github.com/fibjs/fibjs/commit/e5bb49aa20)] - **unzip, bugfix**: fix isZipFile error if filename is in unicode (#373)(王爱科)
* [[`0f4adf5519`](https://github.com/fibjs/fibjs/commit/0f4adf5519)] - **path, bugfix**: fix test case error on windows.(xicilion)
* [[`c3ba036c2f`](https://github.com/fibjs/fibjs/commit/c3ba036c2f)] - **path, refactor**: rewrite path.isAbsolute, more compatible with node.js(xicilion)
* [[`780ba6be1e`](https://github.com/fibjs/fibjs/commit/780ba6be1e)] - **path, feat**: add zero test case.(xicilion)
* [[`eee418d9f4`](https://github.com/fibjs/fibjs/commit/eee418d9f4)] - **path, refactor**: rewrite path.basename/extname/dirname, more compatible with node.js(xicilion)
* [[`bf6a56159b`](https://github.com/fibjs/fibjs/commit/bf6a56159b)] - **path, refactor**: rewrite path.resolve, more compatible with node.js (#374)(Hengfei Zhuang)
* [[`402f7727d1`](https://github.com/fibjs/fibjs/commit/402f7727d1)] - **path, bugfix**: keep the disk no of the last path.(xicilion)
* [[`ddd6eaa1dd`](https://github.com/fibjs/fibjs/commit/ddd6eaa1dd)] - **path, refactor**: rewrite path.resolve, more compatible with node.js(xicilion)
* [[`4afd2903a2`](https://github.com/fibjs/fibjs/commit/4afd2903a2)] - **path, refactor**: rewrite path.join, more compatible with node.js(xicilion)
* [[`6c7aaa7c8c`](https://github.com/fibjs/fibjs/commit/6c7aaa7c8c)] - **path, refactor**: rewrite path.normalize, more compatible with node.js(xicilion)
* [[`887ac99e4b`](https://github.com/fibjs/fibjs/commit/887ac99e4b)] - **core, feat**: disable bigobj compile option on windows.(xicilion)

## 2017-11-3, Version v0.18.0

* **feature** :
    * core:
      - support --cov=filename to specify the cov filename (#368)(Hengfei Zhuang)
      - support --cov-process tool to generate code coverage analysis report.(xicilion)
      - support collecting code coverage information.(xicilion)
      - support v8 code coverage.(xicilion)
      - support OnEventEmit in Trigger.(xicilion)
    * http - check option items, throw unknown option.(xicilion)
    * process:
      - add test case of kill.(xicilion)
      - process.exit support return 0;(xicilion)
    * v8 - upgrade to 6.4.185.(xicilion)
    * websocket:
      - add come documents.(xicilion)
      - change readyState to CLOSED in js thread.(xicilion)
      - close a closed socket not throw error.(xicilion)
      - ws.upgrade add req as the second argument in callback.(xicilion)

* **breakchange** :
    * collection:
      - deprecated collection module and Map object.(xicilion)
      - deprecated  Queue.(xicilion)
      - deprecated List object.(xicilion)
    * console - deprecated TextColor object.(xicilion)
    * core - deprecated object.dispose(xicilion)
    * coroutine - deprecated BlockedQueue.(xicilion)
    * http - deprecated http.find.(xicilion)
    * mongodb - deprecated GridFS.(xicilion)
    * mq - deprecated mq.await.(xicilion)
    * websocket - deprecated ws.connect.(xicilion)
    * zip - deprecated ZipInfo object.(xicilion)

* **bugfix** :
    * assert - compile error on Linux.(xicilion)
    * core:
      - closesocket before canceling io operation.(xicilion)
      - ensure the coverage report could be generated successfully. (#369)(Hengfei Zhuang)
      - ensure the coverage report could be generated successfully. (#369)(Hengfei Zhuang)
      - compile error on Linux and windows.(xicilion)
    * docs - fix wrong “.(xicilion)
    * fs - compile error on FreeBSD.(xicilion)
    * gb - fix hsb2rgb algorithm error.(xicilion)
    * mq - params lost in asynchronous js handler.(xicilion)
    * SubProcess - SubProcess not allow reenter.(xicilion)
    * websocket - not compress control message.(xicilion)
    * WebView - compile error on windows.(xicilion)

* **refactor** :
    * core:
      - use JavaScript property to implement const.(xicilion)
      - add trace_tcp option.(xicilion)
      - remove equals from object to Buffer and Int64.(xicilion)
      - revert to the previous code (v8 fixed the deadlock error).(xicilion)
      - add options.h to define extern options variable.(xicilion)
    * events - save the wrapper function into data object to avoid using Callee.(xicilion)
    * websocket - remove nouse variable.(xicilion)
    * zlib - remove nouse variable.(xicilion)

* **others** :
    * Merge branch 'dev' of https - //github.com/fibjs/fibjs into dev(xicilion)
    * typo.(xicilion)
    * Merge branch 'master' into dev(响马)

* **chore** :
    * doc - remove unused file (#370)(Hengfei Zhuang)

### Commits
* [[`414adf4074`](https://github.com/fibjs/fibjs/commit/414adf4074)] - **v8, feat**: upgrade to 6.4.185.(xicilion)
* [[`5da1a51ad2`](https://github.com/fibjs/fibjs/commit/5da1a51ad2)] - **mq, break**: deprecated mq.await.(xicilion)
* [[`4fc8482a99`](https://github.com/fibjs/fibjs/commit/4fc8482a99)] - **mq, bugfix**: params lost in asynchronous js handler.(xicilion)
* [[`25c6e682e1`](https://github.com/fibjs/fibjs/commit/25c6e682e1)] - **Merge branch 'dev' of https**: //github.com/fibjs/fibjs into dev(xicilion)
* [[`7f3d5742d8`](https://github.com/fibjs/fibjs/commit/7f3d5742d8)] - **doc, chore**: remove unused file (#370)(Hengfei Zhuang)
* [[`6a2b51e03c`](https://github.com/fibjs/fibjs/commit/6a2b51e03c)] - **process, feat**: add test case of kill.(xicilion)
* [[`f551793d42`](https://github.com/fibjs/fibjs/commit/f551793d42)] - **events, refactor**: save the wrapper function into data object to avoid using Callee.(xicilion)
* [[`d860ada40f`](https://github.com/fibjs/fibjs/commit/d860ada40f)] - **core, refactor**: use JavaScript property to implement const.(xicilion)
* [[`00440c1632`](https://github.com/fibjs/fibjs/commit/00440c1632)] - **core, bugfix**: closesocket before canceling io operation.(xicilion)
* [[`98e4d2e06c`](https://github.com/fibjs/fibjs/commit/98e4d2e06c)] - **fs, bugfix**: compile error on FreeBSD.(xicilion)
* [[`5c42bd478a`](https://github.com/fibjs/fibjs/commit/5c42bd478a)] - **core, bugfix**: ensure the coverage report could be generated successfully. (#369)(Hengfei Zhuang)
* [[`7d5f6d3019`](https://github.com/fibjs/fibjs/commit/7d5f6d3019)] - **core, bugfix**: ensure the coverage report could be generated successfully. (#369)(Hengfei Zhuang)
* [[`95cf94caa8`](https://github.com/fibjs/fibjs/commit/95cf94caa8)] - **core, feat**: support --cov=filename to specify the cov filename (#368)(Hengfei Zhuang)
* [[`c8ac686ee9`](https://github.com/fibjs/fibjs/commit/c8ac686ee9)] - **gb, bugfix**: fix hsb2rgb algorithm error.(xicilion)
* [[`f4be9559eb`](https://github.com/fibjs/fibjs/commit/f4be9559eb)] - **core, refactor**: add trace_tcp option.(xicilion)
* [[`0fe6b96a54`](https://github.com/fibjs/fibjs/commit/0fe6b96a54)] - **http, break**: deprecated http.find.(xicilion)
* [[`a3e91bb949`](https://github.com/fibjs/fibjs/commit/a3e91bb949)] - **core, break**: deprecated object.dispose(xicilion)
* [[`a1ef84dd74`](https://github.com/fibjs/fibjs/commit/a1ef84dd74)] - **core, refactor**: remove equals from object to Buffer and Int64.(xicilion)
* [[`d8113504db`](https://github.com/fibjs/fibjs/commit/d8113504db)] - **http, feat**: check option items, throw unknown option.(xicilion)
* [[`b6f1aefc5f`](https://github.com/fibjs/fibjs/commit/b6f1aefc5f)] - **assert, bugfix**: compile error on Linux.(xicilion)
* [[`7c56f7f95b`](https://github.com/fibjs/fibjs/commit/7c56f7f95b)] - **WebView, bugfix**: compile error on windows.(xicilion)
* [[`e3df9ee921`](https://github.com/fibjs/fibjs/commit/e3df9ee921)] - **docs, bugfix**: fix wrong “.(xicilion)
* [[`2290e365d4`](https://github.com/fibjs/fibjs/commit/2290e365d4)] - **mongodb, break**: deprecated GridFS.(xicilion)
* [[`c445485c98`](https://github.com/fibjs/fibjs/commit/c445485c98)] - **zip, break**: deprecated ZipInfo object.(xicilion)
* [[`590b76a47f`](https://github.com/fibjs/fibjs/commit/590b76a47f)] - **collection, break**: deprecated collection module and Map object.(xicilion)
* [[`1f62211b25`](https://github.com/fibjs/fibjs/commit/1f62211b25)] - **console, break**: deprecated TextColor object.(xicilion)
* [[`3e13010e35`](https://github.com/fibjs/fibjs/commit/3e13010e35)] - **core, bugfix**: compile error on Linux and windows.(xicilion)
* [[`0ef8e161b6`](https://github.com/fibjs/fibjs/commit/0ef8e161b6)] - **collection, break**: deprecated  Queue.(xicilion)
* [[`7b8797fbf6`](https://github.com/fibjs/fibjs/commit/7b8797fbf6)] - **coroutine, break**: deprecated BlockedQueue.(xicilion)
* [[`aaa56a6bd9`](https://github.com/fibjs/fibjs/commit/aaa56a6bd9)] - **collection, break**: deprecated List object.(xicilion)
* [[`bfe47e8bed`](https://github.com/fibjs/fibjs/commit/bfe47e8bed)] - **SubProcess, bugfix**: SubProcess not allow reenter.(xicilion)
* [[`b4cd3c1bfd`](https://github.com/fibjs/fibjs/commit/b4cd3c1bfd)] - **core, refactor**: revert to the previous code (v8 fixed the deadlock error).(xicilion)
* [[`6ddc436f19`](https://github.com/fibjs/fibjs/commit/6ddc436f19)] - **core, feat**: support --cov-process tool to generate code coverage analysis report.(xicilion)
* [[`e83d0b1a09`](https://github.com/fibjs/fibjs/commit/e83d0b1a09)] - typo.(xicilion)
* [[`b385d6939f`](https://github.com/fibjs/fibjs/commit/b385d6939f)] - **core, feat**: support collecting code coverage information.(xicilion)
* [[`600764b2da`](https://github.com/fibjs/fibjs/commit/600764b2da)] - **core, refactor**: add options.h to define extern options variable.(xicilion)
* [[`c4be76a406`](https://github.com/fibjs/fibjs/commit/c4be76a406)] - **process, feat**: process.exit support return 0;(xicilion)
* [[`04f0b5dcb2`](https://github.com/fibjs/fibjs/commit/04f0b5dcb2)] - **core, feat**: support v8 code coverage.(xicilion)
* [[`fce9a81732`](https://github.com/fibjs/fibjs/commit/fce9a81732)] - **zlib, refactor**: remove nouse variable.(xicilion)
* [[`a36cdd9e69`](https://github.com/fibjs/fibjs/commit/a36cdd9e69)] - **websocket, feat**: add come documents.(xicilion)
* [[`5e90fb993b`](https://github.com/fibjs/fibjs/commit/5e90fb993b)] - **websocket, break**: deprecated ws.connect.(xicilion)
* [[`44e7657727`](https://github.com/fibjs/fibjs/commit/44e7657727)] - **websocket, feat**: change readyState to CLOSED in js thread.(xicilion)
* [[`3240d9dd37`](https://github.com/fibjs/fibjs/commit/3240d9dd37)] - **core, feat**: support OnEventEmit in Trigger.(xicilion)
* [[`1f02198883`](https://github.com/fibjs/fibjs/commit/1f02198883)] - **websocket, refactor**: remove nouse variable.(xicilion)
* [[`8bdf4ccb04`](https://github.com/fibjs/fibjs/commit/8bdf4ccb04)] - **websocket, bugfix**: not compress control message.(xicilion)
* [[`e033057f5c`](https://github.com/fibjs/fibjs/commit/e033057f5c)] - **websocket, feat**: close a closed socket not throw error.(xicilion)
* [[`353d9a7490`](https://github.com/fibjs/fibjs/commit/353d9a7490)] - **websocket, feat**: ws.upgrade add req as the second argument in callback.(xicilion)
* [[`0a6acf69f3`](https://github.com/fibjs/fibjs/commit/0a6acf69f3)] - Merge branch 'master' into dev(响马)

## 2017-10-27, Version v0.17.0

* **feature** :
    * ci - test --prof.(xicilion)
    * core:
      - support async locker in state machine.(响马)
      - save the status of isolate.(xicilion)
      - exit application directly while v8 is not alive.(响马)
    * docs:
      - change diagram style.(响马)
      - add process signal docs.(xicilion)
    * EventEmitter - save method name in emit.(xicilion)
    * Fiber:
      - not trace internal function in stack.(xicilion)
      - support Fiber.id.(响马)
    * process:
      - interrupt isolate in async mode.(xicilion)
      - support signal.(xicilion)
      - support Event exit.(xicilion)
      - support Event beforeExit.(xicilion)
      - support process.exitCode.(xicilion)
    * profiler:
      - support command line option: --prof and --prof-interval.(xicilion)
      - not log empty sample.(xicilion)
      - profiler.start(name, -1) will profile for ever.(xicilion)
      - support process tool.(xicilion)
    * test:
      - test.run set result to process.exitCode.(xicilion)
      - change the test mode according to the startup mode.(xicilion)
    * util:
      - add underscore test case.(响马)
      - save sync function name to fiber stack.(xicilion)
    * v8 - upgrade to v6.4.146.(xicilion)
    * websocket:
      - support context takeover.(响马)
      - lock the Buffer individually to send the buffer as soon as possible.(xicilion)
      - Send multiple websocket packets together.(响马)
      - use locker to keep the message sequence.(响马)
      - new WebSocket support compress.(响马)
      - support compress handshake in ws.upgrade.(响马)
      - support compress in WebSocketMessage.sendTo.(响马)
      - WebSocketMessage.readFrom support compress message.(xicilion)
    * zlib - support zlib stream.(xicilion)

* **breakchange** :
    * build - remove support of vs2015.(xicilion)
    * websocket:
      - delete nouse test case.(xicilion)
      - deprecated WebSocketHandler.(xicilion)

* **bugfix** :
    * ci:
      - fix windows build (#367)(Hengfei Zhuang)
      - fix windows build (#366)(Hengfei Zhuang)
    * fs:
      - fix crash when read empty file with codec use fs.readFile.(响马)
      - change fs.copy to async mode.(xicilion)
    * MemoryStream - fix MemoryStrem.seek error on windows.(xicilion)
    * net - safely close socket to repair socket reuse errors.(xicilion)
    * process - ctrl_c will cause the program to crash during startup.(响马)
    * profiler - log file parse error on windows.(xicilion)
    * socket - closing a socket twice will cause a crash.(响马)
    * test:
      - change tcp port.(xicilion)
      - clear root ca after test.(xicilion)
    * timers - fix v8 deadlock in setHrInterval.(xicilion)
    * websocket - end async recv worker after close stream.(响马)
    * zlib:
      - compile error on windows.(xicilion)
      - compile warning in debug mode.(响马)
      - asynchronous operation may cause access to illegal memory.(响马)

* **refactor** :
    * core:
      - define v8 api in v8_api.h.(xicilion)
      - AsyncEvent base on Task_base.(响马)
      - call interrupt callback function directly if possible.(响马)
      - move RequestInterrupt into isolate.(xicilion)
      - optimize the startup process.(xicilion)
    * io - move method flush to Interface Stream.(响马)
    * net - remove unused event from libev.(响马)
    * timers - reuse variable isolate.(xicilion)
    * zlib - move ZlibStream to include file.(xicilion)

* **doc** :
    * gui - add gui english doc (#365)(Hengfei Zhuang)

### Commits
* [[`0260298dd4`](https://github.com/fibjs/fibjs/commit/0260298dd4)] - **core, refactor**: define v8 api in v8_api.h.(xicilion)
* [[`bfdedd0835`](https://github.com/fibjs/fibjs/commit/bfdedd0835)] - **v8, feat**: upgrade to v6.4.146.(xicilion)
* [[`0334b8a375`](https://github.com/fibjs/fibjs/commit/0334b8a375)] - **zlib, bugfix**: compile error on windows.(xicilion)
* [[`e96f23e9fa`](https://github.com/fibjs/fibjs/commit/e96f23e9fa)] - **websocket, feat**: support context takeover.(响马)
* [[`8b31858bba`](https://github.com/fibjs/fibjs/commit/8b31858bba)] - **ci, bugfix**: fix windows build (#367)(Hengfei Zhuang)
* [[`7bbbb143ec`](https://github.com/fibjs/fibjs/commit/7bbbb143ec)] - **zlib, refactor**: move ZlibStream to include file.(xicilion)
* [[`49aa9975c6`](https://github.com/fibjs/fibjs/commit/49aa9975c6)] - **zlib, feat**: support zlib stream.(xicilion)
* [[`feac71466c`](https://github.com/fibjs/fibjs/commit/feac71466c)] - **io, refactor**: move method flush to Interface Stream.(响马)
* [[`666ae9f38d`](https://github.com/fibjs/fibjs/commit/666ae9f38d)] - **net, refactor**: remove unused event from libev.(响马)
* [[`f0459a34b8`](https://github.com/fibjs/fibjs/commit/f0459a34b8)] - **socket, bugfix**: closing a socket twice will cause a crash.(响马)
* [[`64e9abaf99`](https://github.com/fibjs/fibjs/commit/64e9abaf99)] - **net, bugfix**: safely close socket to repair socket reuse errors.(xicilion)
* [[`2986202089`](https://github.com/fibjs/fibjs/commit/2986202089)] - **websocket, feat**: lock the Buffer individually to send the buffer as soon as possible.(xicilion)
* [[`dbff1aa286`](https://github.com/fibjs/fibjs/commit/dbff1aa286)] - **websocket, feat**: Send multiple websocket packets together.(响马)
* [[`d69b32c6ec`](https://github.com/fibjs/fibjs/commit/d69b32c6ec)] - **websocket, feat**: use locker to keep the message sequence.(响马)
* [[`344f0ae6aa`](https://github.com/fibjs/fibjs/commit/344f0ae6aa)] - **process, bugfix**: ctrl_c will cause the program to crash during startup.(响马)
* [[`418d2a8dc5`](https://github.com/fibjs/fibjs/commit/418d2a8dc5)] - **websocket, bugfix**: end async recv worker after close stream.(响马)
* [[`3637d39a95`](https://github.com/fibjs/fibjs/commit/3637d39a95)] - **zlib, bugfix**: compile warning in debug mode.(响马)
* [[`0fd19ee721`](https://github.com/fibjs/fibjs/commit/0fd19ee721)] - **MemoryStream, bugfix**: fix MemoryStrem.seek error on windows.(xicilion)
* [[`6563d2c665`](https://github.com/fibjs/fibjs/commit/6563d2c665)] - **gui, doc**: add gui english doc (#365)(Hengfei Zhuang)
* [[`a24b4b404e`](https://github.com/fibjs/fibjs/commit/a24b4b404e)] - **ci, bugfix**: fix windows build (#366)(Hengfei Zhuang)
* [[`ec8362f662`](https://github.com/fibjs/fibjs/commit/ec8362f662)] - **build, break**: remove support of vs2015.(xicilion)
* [[`42b69ff4c2`](https://github.com/fibjs/fibjs/commit/42b69ff4c2)] - **test, feat**: test.run set result to process.exitCode.(xicilion)
* [[`77e64585d1`](https://github.com/fibjs/fibjs/commit/77e64585d1)] - **docs, feat**: change diagram style.(响马)
* [[`6147c17aaf`](https://github.com/fibjs/fibjs/commit/6147c17aaf)] - **core, feat**: support async locker in state machine.(响马)
* [[`51e88700ad`](https://github.com/fibjs/fibjs/commit/51e88700ad)] - **core, refactor**: AsyncEvent base on Task_base.(响马)
* [[`39ed71a85c`](https://github.com/fibjs/fibjs/commit/39ed71a85c)] - **fs, bugfix**: fix crash when read empty file with codec use fs.readFile.(响马)
* [[`07e705e8e0`](https://github.com/fibjs/fibjs/commit/07e705e8e0)] - **zlib, bugfix**: asynchronous operation may cause access to illegal memory.(响马)
* [[`122bc6dbb2`](https://github.com/fibjs/fibjs/commit/122bc6dbb2)] - **websocket, feat**: new WebSocket support compress.(响马)
* [[`76efe05aa1`](https://github.com/fibjs/fibjs/commit/76efe05aa1)] - **websocket, feat**: support compress handshake in ws.upgrade.(响马)
* [[`e2662049c5`](https://github.com/fibjs/fibjs/commit/e2662049c5)] - **websocket, feat**: support compress in WebSocketMessage.sendTo.(响马)
* [[`2e17b82fad`](https://github.com/fibjs/fibjs/commit/2e17b82fad)] - **websocket, feat**: WebSocketMessage.readFrom support compress message.(xicilion)
* [[`4fe56e695a`](https://github.com/fibjs/fibjs/commit/4fe56e695a)] - **websocket, break**: delete nouse test case.(xicilion)
* [[`c39d7037f0`](https://github.com/fibjs/fibjs/commit/c39d7037f0)] - **websocket, break**: deprecated WebSocketHandler.(xicilion)
* [[`8d00946b50`](https://github.com/fibjs/fibjs/commit/8d00946b50)] - **test, bugfix**: change tcp port.(xicilion)
* [[`6c625f5446`](https://github.com/fibjs/fibjs/commit/6c625f5446)] - **util, feat**: add underscore test case.(响马)
* [[`ff90700a11`](https://github.com/fibjs/fibjs/commit/ff90700a11)] - **ci, feat**: test --prof.(xicilion)
* [[`138b59b221`](https://github.com/fibjs/fibjs/commit/138b59b221)] - **profiler, bugfix**: log file parse error on windows.(xicilion)
* [[`9f6bad2138`](https://github.com/fibjs/fibjs/commit/9f6bad2138)] - **profiler, feat**: support command line option: --prof and --prof-interval.(xicilion)
* [[`21425c9eae`](https://github.com/fibjs/fibjs/commit/21425c9eae)] - **profiler, feat**: not log empty sample.(xicilion)
* [[`7b42a50f6b`](https://github.com/fibjs/fibjs/commit/7b42a50f6b)] - **profiler, feat**: profiler.start(name, -1) will profile for ever.(xicilion)
* [[`c79d4bd26f`](https://github.com/fibjs/fibjs/commit/c79d4bd26f)] - **docs, feat**: add process signal docs.(xicilion)
* [[`d80d83060f`](https://github.com/fibjs/fibjs/commit/d80d83060f)] - **test, feat**: change the test mode according to the startup mode.(xicilion)
* [[`106639b788`](https://github.com/fibjs/fibjs/commit/106639b788)] - **test, bugfix**: clear root ca after test.(xicilion)
* [[`6253b8a13d`](https://github.com/fibjs/fibjs/commit/6253b8a13d)] - **fs, bugfix**: change fs.copy to async mode.(xicilion)
* [[`880f894dc9`](https://github.com/fibjs/fibjs/commit/880f894dc9)] - **process, feat**: interrupt isolate in async mode.(xicilion)
* [[`4520bb3a85`](https://github.com/fibjs/fibjs/commit/4520bb3a85)] - **timers, bugfix**: fix v8 deadlock in setHrInterval.(xicilion)
* [[`838295b4be`](https://github.com/fibjs/fibjs/commit/838295b4be)] - **core, feat**: save the status of isolate.(xicilion)
* [[`5b2c83f57c`](https://github.com/fibjs/fibjs/commit/5b2c83f57c)] - **core, feat**: exit application directly while v8 is not alive.(响马)
* [[`4ec77e3241`](https://github.com/fibjs/fibjs/commit/4ec77e3241)] - **core, refactor**: call interrupt callback function directly if possible.(响马)
* [[`c62738774c`](https://github.com/fibjs/fibjs/commit/c62738774c)] - **process, feat**: support signal.(xicilion)
* [[`8d6271866d`](https://github.com/fibjs/fibjs/commit/8d6271866d)] - **EventEmitter, feat**: save method name in emit.(xicilion)
* [[`dc1089eefe`](https://github.com/fibjs/fibjs/commit/dc1089eefe)] - **Fiber, feat**: not trace internal function in stack.(xicilion)
* [[`dd0c0611df`](https://github.com/fibjs/fibjs/commit/dd0c0611df)] - **process, feat**: support Event exit.(xicilion)
* [[`08cf7066e4`](https://github.com/fibjs/fibjs/commit/08cf7066e4)] - **process, feat**: support Event beforeExit.(xicilion)
* [[`b6ec6558ef`](https://github.com/fibjs/fibjs/commit/b6ec6558ef)] - **process, feat**: support process.exitCode.(xicilion)
* [[`7c9a6cba3d`](https://github.com/fibjs/fibjs/commit/7c9a6cba3d)] - **core, refactor**: move RequestInterrupt into isolate.(xicilion)
* [[`eb4d6bce05`](https://github.com/fibjs/fibjs/commit/eb4d6bce05)] - **profiler, feat**: support process tool.(xicilion)
* [[`159b2275f2`](https://github.com/fibjs/fibjs/commit/159b2275f2)] - **core, refactor**: optimize the startup process.(xicilion)
* [[`39145217bc`](https://github.com/fibjs/fibjs/commit/39145217bc)] - **util, feat**: save sync function name to fiber stack.(xicilion)
* [[`c3092d0476`](https://github.com/fibjs/fibjs/commit/c3092d0476)] - **Fiber, feat**: support Fiber.id.(响马)
* [[`41e402b5d2`](https://github.com/fibjs/fibjs/commit/41e402b5d2)] - **timers, refactor**: reuse variable isolate.(xicilion)

## 2017-10-14, Version v0.16.0

* **feature** :
    * core:
      - save method name before lock the object, support recursive native call.(xicilion)
      - treat NaN as zero in argumets when require number (#362)(Hengfei Zhuang)
      - use exlib::string::Buffer as ArrayBuffer allocator.(xicilion)
    * docs - sort .idl file before parse.(响马)
    * Fiber:
      - include native function name in Fiber.stack.(xicilion)
      - support Fiber.stack to trace JavaScript stack frame.(xicilion)
    * fs - access string data safely.(xicilion)
    * gd - change Image.affine to async to free v8 runtime.(xicilion)
    * http:
      - request support query in opts.(响马)
      - reset body when store json data.(xicilion)
    * profiler - support cpu profiler.(xicilion)
    * test:
      - add jsc async callback test case.(响马)
      - use Function.length to check callback function.(响马)
    * timers:
      - support property stopped to detect if the Timer has stopped.(xicilion)
      - Use => rewrite blank function.(xicilion)
      - support setHrInterval/clearHrInternal.(xicilion)
      - support param (#358)(Hengfei Zhuang)
    * tools - changelog script support type alias.(xicilion)
    * util - remove nouse variable.(xicilion)
    * v8 - upgrade to v6.3.296.(xicilion)

* **bugfix** :
    * ci - lock the ruby version to 2.4 to fix homebrew error (#364)(Hengfei Zhuang)
    * docs - fix process.stdout document.(xicilion)
    * Fiber - Fiber.stack not break line.(xicilion)
    * SQLite - use coroutine.sleep release worker thread.(xicilion)
    * timers:
      - setHrInterval delay sometimes.(xicilion)
      - fix wrong timeout (#359)(Hengfei Zhuang)
    * Url - compile in debug mode.(xicilion)
    * url - compile warning on windows.(xicilion)
    * vm - fix the sandbox.require base dir (#361)(Hengfei Zhuang)

* **refactor** :
    * Buffer - If possible, use ArrayBuffer to create Buffer.(xicilion)
    * core - use same api to trace stack.(xicilion)
    * coroutine - sleep with 0ms return directly.(xicilion)
    * encoding - rewrite json.encode use new v8 json api.(xicilion)
    * exlib - split source code.(xicilion)
    * SQLite - Optimize the sleep algorithm when the database is busy.(xicilion)
    * timers - use v8 internal api to invoke interrupt callback directly.(xicilion)

### Commits
* [[`3b925b7de6`](https://github.com/fibjs/fibjs/commit/3b925b7de6)] - **core, refactor**: use same api to trace stack.(xicilion)
* [[`48bbcc6699`](https://github.com/fibjs/fibjs/commit/48bbcc6699)] - **v8, feat**: upgrade to v6.3.296.(xicilion)
* [[`9c82a02f28`](https://github.com/fibjs/fibjs/commit/9c82a02f28)] - **timers, refactor**: use v8 internal api to invoke interrupt callback directly.(xicilion)
* [[`6155b8dbee`](https://github.com/fibjs/fibjs/commit/6155b8dbee)] - **Fiber, bugfix**: Fiber.stack not break line.(xicilion)
* [[`4673b38448`](https://github.com/fibjs/fibjs/commit/4673b38448)] - **timers, feat**: support property stopped to detect if the Timer has stopped.(xicilion)
* [[`89bf1893a3`](https://github.com/fibjs/fibjs/commit/89bf1893a3)] - **core, feat**: save method name before lock the object, support recursive native call.(xicilion)
* [[`d2550f5bc6`](https://github.com/fibjs/fibjs/commit/d2550f5bc6)] - **Url, bugfix**: compile in debug mode.(xicilion)
* [[`e09d3b56ba`](https://github.com/fibjs/fibjs/commit/e09d3b56ba)] - **util, feat**: remove nouse variable.(xicilion)
* [[`2ec5980ab5`](https://github.com/fibjs/fibjs/commit/2ec5980ab5)] - **profiler, feat**: support cpu profiler.(xicilion)
* [[`7443033c29`](https://github.com/fibjs/fibjs/commit/7443033c29)] - **SQLite, refactor**: Optimize the sleep algorithm when the database is busy.(xicilion)
* [[`dbd3c63fd7`](https://github.com/fibjs/fibjs/commit/dbd3c63fd7)] - **coroutine, refactor**: sleep with 0ms return directly.(xicilion)
* [[`5a5e7658a3`](https://github.com/fibjs/fibjs/commit/5a5e7658a3)] - **timers, feat**: Use => rewrite blank function.(xicilion)
* [[`d1c64dc5bf`](https://github.com/fibjs/fibjs/commit/d1c64dc5bf)] - **Fiber, feat**: include native function name in Fiber.stack.(xicilion)
* [[`612ac63127`](https://github.com/fibjs/fibjs/commit/612ac63127)] - **tools, feat**: changelog script support type alias.(xicilion)
* [[`125bcd348b`](https://github.com/fibjs/fibjs/commit/125bcd348b)] - **ci, fixbug**: lock the ruby version to 2.4 to fix homebrew error (#364)(Hengfei Zhuang)
* [[`7ac34e97c0`](https://github.com/fibjs/fibjs/commit/7ac34e97c0)] - **url, bugfix**: compile warning on windows.(xicilion)
* [[`921f9d47fa`](https://github.com/fibjs/fibjs/commit/921f9d47fa)] - **SQLite, bugfix**: use coroutine.sleep release worker thread.(xicilion)
* [[`cc3a032a07`](https://github.com/fibjs/fibjs/commit/cc3a032a07)] - **gd, feat**: change Image.affine to async to free v8 runtime.(xicilion)
* [[`b0fba8d60e`](https://github.com/fibjs/fibjs/commit/b0fba8d60e)] - **timers, bugfix**: setHrInterval delay sometimes.(xicilion)
* [[`75d8c55d74`](https://github.com/fibjs/fibjs/commit/75d8c55d74)] - **docs, feat**: sort .idl file before parse.(响马)
* [[`751d1f9269`](https://github.com/fibjs/fibjs/commit/751d1f9269)] - **http, feat**: request support query in opts.(响马)
* [[`1fa3bbf1c9`](https://github.com/fibjs/fibjs/commit/1fa3bbf1c9)] - **test, feat**: add jsc async callback test case.(响马)
* [[`82aa0f4e2f`](https://github.com/fibjs/fibjs/commit/82aa0f4e2f)] - **test, feat**: use Function.length to check callback function.(响马)
* [[`0bf90f4ced`](https://github.com/fibjs/fibjs/commit/0bf90f4ced)] - **http, feat**: reset body when store json data.(xicilion)
* [[`c0d4be1fa1`](https://github.com/fibjs/fibjs/commit/c0d4be1fa1)] - **core, feat**: treat NaN as zero in argumets when require number (#362)(Hengfei Zhuang)
* [[`3a92d0e6db`](https://github.com/fibjs/fibjs/commit/3a92d0e6db)] - **encoding, refactor**: rewrite json.encode use new v8 json api.(xicilion)
* [[`94e0ee98fc`](https://github.com/fibjs/fibjs/commit/94e0ee98fc)] - **fs, feat**: access string data safely.(xicilion)
* [[`d04963c97a`](https://github.com/fibjs/fibjs/commit/d04963c97a)] - **core, feat**: use exlib::string::Buffer as ArrayBuffer allocator.(xicilion)
* [[`3312d9d567`](https://github.com/fibjs/fibjs/commit/3312d9d567)] - **Buffer, refactor**: If possible, use ArrayBuffer to create Buffer.(xicilion)
* [[`efbfdeb65e`](https://github.com/fibjs/fibjs/commit/efbfdeb65e)] - **vm, fixbug**: fix the sandbox.require base dir (#361)(Hengfei Zhuang)
* [[`b5cb1cb732`](https://github.com/fibjs/fibjs/commit/b5cb1cb732)] - **timers, feat**: support setHrInterval/clearHrInternal.(xicilion)
* [[`ebcb904574`](https://github.com/fibjs/fibjs/commit/ebcb904574)] - **timers, fixbug**: fix wrong timeout (#359)(Hengfei Zhuang)
* [[`7be90a7a4f`](https://github.com/fibjs/fibjs/commit/7be90a7a4f)] - **Fiber, feat**: support Fiber.stack to trace JavaScript stack frame.(xicilion)
* [[`aa55e2bb6f`](https://github.com/fibjs/fibjs/commit/aa55e2bb6f)] - **exlib, refactor**: split source code.(xicilion)
* [[`0281ce9aad`](https://github.com/fibjs/fibjs/commit/0281ce9aad)] - **timers, feat**: support param (#358)(Hengfei Zhuang)
* [[`3a2ca55c09`](https://github.com/fibjs/fibjs/commit/3a2ca55c09)] - **docs, bugfix**: fix process.stdout document.(xicilion)

## 2017-09-30, Version v0.15.0

* **feature** :
    * docs:
      - remove github token.(xicilion)
      - add awesome sync tool.(xicilion)
    * http:
      - request support json any type.(xicilion)
      - HttpResponse parse statusMessage.(xicilion)
      - HttpResopnse support any statusCode.(xicilion)
      - support find method.(xicilion)
      - support statusCode, statusMessage, writeHeaders.(xicilion)
    * process:
      - support process.versions.(xicilion)
      - support process.stdin, process.stdout, process.stderr.(xicilion)
    * timer - support beyond 2^31 delay (#357)(Hengfei Zhuang)
    * util:
      - support util.inspect.(xicilion)
      - support util._extend.(xicilion)
    * v8 - upgrade to v6.3.228.(xicilion)

* **breakchange** :
    * http - deprecated property crossDomain, add method enableCrossOrigin.(xicilion)

* **bugfix** :
    * core - fix compile warning on windows.(xicilion)
    * http:
      - compile warning on windows.(xicilion)
      - HttpResponse.statusMessage encode error.(xicilion)
    * timer - compile warning on windows.(xicilion)
    * websocket - compatible with new http api.(xicilion)

* **refactor** :
    * core - use WriteUtf8 to get better performance (#356)(Hengfei Zhuang)
    * http:
      - use opts to pass body and headers, support urlencode and json encode.(xicilion)
      - use Object to pass headers.(xicilion)
    * idlc - use GetArgumentValue to convert property.(xicilion)
    * path - use getArgumentValue.(xicilion)
    * Variant - use GetArgumentValue.(xicilion)

### Commits
* [[`4167a08687`](https://github.com/fibjs/fibjs/commit/4167a08687)] - **v8, feat**: upgrade to v6.3.228.(xicilion)
* [[`55d21b5045`](https://github.com/fibjs/fibjs/commit/55d21b5045)] - **timer, bugfix**: compile warning on windows.(xicilion)
* [[`875654e4f0`](https://github.com/fibjs/fibjs/commit/875654e4f0)] - **timer, feat**: support beyond 2^31 delay (#357)(Hengfei Zhuang)
* [[`bab35312b3`](https://github.com/fibjs/fibjs/commit/bab35312b3)] - **http, bugfix**: compile warning on windows.(xicilion)
* [[`881a7c7636`](https://github.com/fibjs/fibjs/commit/881a7c7636)] - **http, feat**: request support json any type.(xicilion)
* [[`c258ae4129`](https://github.com/fibjs/fibjs/commit/c258ae4129)] - **websocket, bugfix**: compatible with new http api.(xicilion)
* [[`c66498fe7a`](https://github.com/fibjs/fibjs/commit/c66498fe7a)] - **http, refactor**: use opts to pass body and headers, support urlencode and json encode.(xicilion)
* [[`8538dc249d`](https://github.com/fibjs/fibjs/commit/8538dc249d)] - **http, feat**: HttpResponse parse statusMessage.(xicilion)
* [[`f8e5511b2a`](https://github.com/fibjs/fibjs/commit/f8e5511b2a)] - **http, feat**: HttpResopnse support any statusCode.(xicilion)
* [[`91915c0842`](https://github.com/fibjs/fibjs/commit/91915c0842)] - **http, bugfix**: HttpResponse.statusMessage encode error.(xicilion)
* [[`b7f14b1288`](https://github.com/fibjs/fibjs/commit/b7f14b1288)] - **http, feat**: support find method.(xicilion)
* [[`1429a5f7df`](https://github.com/fibjs/fibjs/commit/1429a5f7df)] - **http, feat**: support statusCode, statusMessage, writeHeaders.(xicilion)
* [[`731c1fdb9d`](https://github.com/fibjs/fibjs/commit/731c1fdb9d)] - **http, refactor**: use Object to pass headers.(xicilion)
* [[`a3cfcc5e89`](https://github.com/fibjs/fibjs/commit/a3cfcc5e89)] - **docs, feat**: remove github token.(xicilion)
* [[`ec4958dfc9`](https://github.com/fibjs/fibjs/commit/ec4958dfc9)] - **docs, feat**: add awesome sync tool.(xicilion)
* [[`d5d1196ac9`](https://github.com/fibjs/fibjs/commit/d5d1196ac9)] - **http, break**: deprecated property crossDomain, add method enableCrossOrigin.(xicilion)
* [[`ca05229cb2`](https://github.com/fibjs/fibjs/commit/ca05229cb2)] - **util, feat**: support util.inspect.(xicilion)
* [[`d93ca533c7`](https://github.com/fibjs/fibjs/commit/d93ca533c7)] - **util, feat**: support util._extend.(xicilion)
* [[`f97d881da0`](https://github.com/fibjs/fibjs/commit/f97d881da0)] - **process, feat**: support process.versions.(xicilion)
* [[`b3f68688b0`](https://github.com/fibjs/fibjs/commit/b3f68688b0)] - **process, feat**: support process.stdin, process.stdout, process.stderr.(xicilion)
* [[`8085d429da`](https://github.com/fibjs/fibjs/commit/8085d429da)] - **Variant, refactor**: use GetArgumentValue.(xicilion)
* [[`ad9f7dbc0d`](https://github.com/fibjs/fibjs/commit/ad9f7dbc0d)] - **path, refactor**: use getArgumentValue.(xicilion)
* [[`7765c2402a`](https://github.com/fibjs/fibjs/commit/7765c2402a)] - **idlc, refactor**: use GetArgumentValue to convert property.(xicilion)
* [[`8cc030841e`](https://github.com/fibjs/fibjs/commit/8cc030841e)] - **core, bugfix**: fix compile warning on windows.(xicilion)
* [[`4ce374f000`](https://github.com/fibjs/fibjs/commit/4ce374f000)] - **core, refactor**: use WriteUtf8 to get better performance (#356)(Hengfei Zhuang)

## 2017-09-21, Version v0.14.0

* **feature** :
    * punycode - support toUnicode and toASCII methods (#351)(王爱科)
    * v8 - upgrade to v6.3.179.(响马)

* **bugfix** :
    * LruCache - data error when update element in updater function.(xicilion)

* **refactor** :
    * LruCache - use result of insert to check the element in map.(响马)
    * Service - modify service control function to static function.(xicilion)
    * url - standardize url.parse behavior (#355)(王爱科)

### Commits
* [[`06b98f80f2`](https://github.com/fibjs/fibjs/commit/06b98f80f2)] - **url, refactor**: standardize url.parse behavior (#355)(王爱科)
* [[`0018f2d86c`](https://github.com/fibjs/fibjs/commit/0018f2d86c)] - **v8, feat**: upgrade to v6.3.179.(响马)
* [[`2de4dbae9f`](https://github.com/fibjs/fibjs/commit/2de4dbae9f)] - **Service, refactor**: modify service control function to static function.(xicilion)
* [[`a286b7be6f`](https://github.com/fibjs/fibjs/commit/a286b7be6f)] - **LruCache, refactor**: use result of insert to check the element in map.(响马)
* [[`d27eb0099a`](https://github.com/fibjs/fibjs/commit/d27eb0099a)] - **LruCache, bugfix**: data error when update element in updater function.(xicilion)
* [[`a4b70743a9`](https://github.com/fibjs/fibjs/commit/a4b70743a9)] - **punycode, feat**: support toUnicode and toASCII methods (#351)(王爱科)

## 2017-09-12, Version v0.13.0

* **feature** :
    * Buffer:
      - Buffer.write* return new offset, compatible with node.js.(响马)
      - add the lost desc (#342)(Hengfei Zhuang)
    * core:
      - support tuple return type.(xicilion)
      - support ucs2 string convert.(响马)
      - SimpleObject support Array.(响马)
      - Implement SimpleObject to build simple JavaScript Object from native.(响马)
      - not ignore nan in arguments.(响马)
    * crypto - support crypto.createHmac.(响马)
    * db:
      - query result NULL is converted to null in MySQL and SQLite (#334)(blingz)
      - undefined / null are converted to NULL (#333)(blingz)
    * docs - add a release step.(响马)
    * http - agent in custum headers will replace default agent.(响马)
    * HttpCookie - path=/ as default value in cookie (#332)(blingz)
    * iconv:
      - support ucs4 codec.(响马)
      - support ucs2, utf16le codec.(响马)
    * idlc - support NObject/NArray type.(响马)
    * Message:
      - method json check content-type charset.(xicilion)
      - method json support decode body.(xicilion)
      - support method json.(xicilion)
    * punycode - support native module punycode.(响马)
    * string_decoder - add string_decoder module (#340)(Hengfei Zhuang)
    * test:
      - add v8 crash test case.(xicilion)
      - support this in test function.(响马)
    * v8:
      - upgrade v8 to v6.3.92.(响马)
      - upgrade to v6.2.377.(响马)
    * vs - generate vc proj file.(响马)

* **breakchange** :
    * Buffer - compat with node toString (#338)(Hengfei Zhuang)
    * core - deprecated mem_dump in debug.mode.(xicilion)
    * Int64 - deprecated valueOf.(响马)
    * net - deprecated method net.conect with host/port arguments.(响马)
    * object - deprecated valueOf.(响马)
    * re - deprecated module re.(响马)

* **bugfix** :
    * Buffer:
      - Buffer.write codec string size error.(xicilion)
      - fix the buffer test (#339)(Hengfei Zhuang)
    * core:
      - compile warning on windows.(xicilion)
      - disable inlining in TurboFan, fix crash on multi fiber.(xicilion)
      - disable parallel scavenge, fix gc deadlock.(响马)
      - compile warning on windows debug.(响马)
      - bugfix: method count error in class info of  object.(响马)
    * db - fix typo.(响马)
    * HttpResponse - response header missing after method redirect.(响马)
    * iconv:
      - multi char utf-16 decode error.(xicilion)
      - unicode 0xd800 convert error.(响马)
    * io - compile warning on linux debug mode.(响马)
    * Message - method json error when content-type has charset.(xicilion)
    * os:
      - compile error on osx.(xicilion)
      - compile warning on linux debug mode.(响马)
    * punycode:
      - compile warning on windows x86.(响马)
      - compile error on linux.(响马)
    * sandbox - jsc test file load error.(xicilion)
    * SandBox:
      - object type argument error in standalone global.(xicilion)
      - delete default console object in global.(xicilion)
      - function member is missing in standalone global.(xicilion)
    * string_decoder:
      - fix compile warning on windows (#343)(Hengfei Zhuang)
      - fix encoding error on windows (#341)(Hengfei Zhuang)
    * test - callback hungup when execute return error.(响马)

* **refactor** :
    * core:
      - refactor the GC system of native object.(xicilion)
      - use NObject/NArray return SimpleObject.(响马)
      - use new NewString convert string to v8 Value, fix v8 utf8 issue.(响马)
      - rename utf8 api, support wchar32.(响马)
      - use valueOf to get the wrap of native object.(响马)
    * db - use simpleObject to return the query result of execute.(响马)
    * http - deprecated cross domain response for flash.(xicilion)
    * idlc - not refer object_base.(响马)
    * net - use SimpleObject to build the result of method recvfrom.(响马)
    * timer - hold the handler function until the timer ends.(xicilion)
    * util - method format check native object first.(响马)
    * Variant - use valueOf to convert object_base to JavaScript Value.(响马)

### Commits
* [[`dc5d816ece`](https://github.com/fibjs/fibjs/commit/dc5d816ece)] - **test, feat**: add v8 crash test case.(xicilion)
* [[`9dc6d62633`](https://github.com/fibjs/fibjs/commit/9dc6d62633)] - **v8, feat**: upgrade v8 to v6.3.92.(响马)
* [[`3c9e67b38a`](https://github.com/fibjs/fibjs/commit/3c9e67b38a)] - **http, refactor**: deprecated cross domain response for flash.(xicilion)
* [[`f46945b746`](https://github.com/fibjs/fibjs/commit/f46945b746)] - **core, refactor**: refactor the GC system of native object.(xicilion)
* [[`b59aeee7f3`](https://github.com/fibjs/fibjs/commit/b59aeee7f3)] - **timer, refactor**: hold the handler function until the timer ends.(xicilion)
* [[`5ca1044e6c`](https://github.com/fibjs/fibjs/commit/5ca1044e6c)] - **core, break**: deprecated mem_dump in debug.mode.(xicilion)
* [[`f4223f190e`](https://github.com/fibjs/fibjs/commit/f4223f190e)] - **core, bugfix**: compile warning on windows.(xicilion)
* [[`5936b4ed31`](https://github.com/fibjs/fibjs/commit/5936b4ed31)] - **core, bugfix**: disable inlining in TurboFan, fix crash on multi fiber.(xicilion)
* [[`ee7d899107`](https://github.com/fibjs/fibjs/commit/ee7d899107)] - **os, bugfix**: compile error on osx.(xicilion)
* [[`ffdf5d332b`](https://github.com/fibjs/fibjs/commit/ffdf5d332b)] - **os, bugfix**: compile warning on linux debug mode.(响马)
* [[`019f74dceb`](https://github.com/fibjs/fibjs/commit/019f74dceb)] - **io, bugfix**: compile warning on linux debug mode.(响马)
* [[`d3267297d2`](https://github.com/fibjs/fibjs/commit/d3267297d2)] - **net, break**: deprecated method net.conect with host/port arguments.(响马)
* [[`e1a52c83d6`](https://github.com/fibjs/fibjs/commit/e1a52c83d6)] - **HttpResponse, bugfix**: response header missing after method redirect.(响马)
* [[`8ef70c5273`](https://github.com/fibjs/fibjs/commit/8ef70c5273)] - **core, feat**: support tuple return type.(xicilion)
* [[`8035a3e56f`](https://github.com/fibjs/fibjs/commit/8035a3e56f)] - **core, refactor**: use NObject/NArray return SimpleObject.(响马)
* [[`2dc326c688`](https://github.com/fibjs/fibjs/commit/2dc326c688)] - **idlc, feat**: support NObject/NArray type.(响马)
* [[`72e1d1e23d`](https://github.com/fibjs/fibjs/commit/72e1d1e23d)] - **SandBox, bugfix**: object type argument error in standalone global.(xicilion)
* [[`a2919a9d50`](https://github.com/fibjs/fibjs/commit/a2919a9d50)] - **SandBox, bugfix**: delete default console object in global.(xicilion)
* [[`def11fc0cf`](https://github.com/fibjs/fibjs/commit/def11fc0cf)] - **SandBox, bugfix**: function member is missing in standalone global.(xicilion)
* [[`84a31dda38`](https://github.com/fibjs/fibjs/commit/84a31dda38)] - Merge remote-tracking branch 'origin/dev' into dev(xicilion)
* [[`a043b439ad`](https://github.com/fibjs/fibjs/commit/a043b439ad)] - **string_decoder, bugfix**: fix compile warning on windows (#343)(Hengfei Zhuang)
* [[`4287d0d71f`](https://github.com/fibjs/fibjs/commit/4287d0d71f)] - **string_decoder, doc**: add the lost desc (#342)(Hengfei Zhuang)
* [[`a459d4f647`](https://github.com/fibjs/fibjs/commit/a459d4f647)] - **string_decoder, bugfix**: fix encoding error on windows (#341)(Hengfei Zhuang)
* [[`5d704e918b`](https://github.com/fibjs/fibjs/commit/5d704e918b)] - **Message, feat**: method json check content-type charset.(xicilion)
* [[`fa6420eaa2`](https://github.com/fibjs/fibjs/commit/fa6420eaa2)] - **Message, bugfix**: method json error when content-type has charset.(xicilion)
* [[`f3b6e61572`](https://github.com/fibjs/fibjs/commit/f3b6e61572)] - **Message, feat**: method json support decode body.(xicilion)
* [[`abef2fba5e`](https://github.com/fibjs/fibjs/commit/abef2fba5e)] - **string_decoder, feat**: add string_decoder module (#340)(Hengfei Zhuang)
* [[`c4a7069e59`](https://github.com/fibjs/fibjs/commit/c4a7069e59)] - **Message, feat**: support method json.(xicilion)
* [[`456cd3e5e2`](https://github.com/fibjs/fibjs/commit/456cd3e5e2)] - **http, feat**: agent in custum headers will replace default agent.(响马)
* [[`81d5221f09`](https://github.com/fibjs/fibjs/commit/81d5221f09)] - **docs, feat**: add a release step.(响马)
* [[`19bc86ead3`](https://github.com/fibjs/fibjs/commit/19bc86ead3)] - **Buffer, test**: fix the buffer test (#339)(Hengfei Zhuang)
* [[`c2c1f98878`](https://github.com/fibjs/fibjs/commit/c2c1f98878)] - **Buffer, break**: compat with node toString (#338)(Hengfei Zhuang)
* [[`3fe7074cc5`](https://github.com/fibjs/fibjs/commit/3fe7074cc5)] - **Buffer, bugfix**: Buffer.write codec string size error.(xicilion)
* [[`814e3a5f1d`](https://github.com/fibjs/fibjs/commit/814e3a5f1d)] - dev v0.13.0-dev(xicilion)
* [[`c7a6d2565c`](https://github.com/fibjs/fibjs/commit/c7a6d2565c)] - Release v0.12.0(xicilion)
* [[`8d729e1a9d`](https://github.com/fibjs/fibjs/commit/8d729e1a9d)] - **sandbox, bugfix**: jsc test file load error.(xicilion)
* [[`7ffc201fae`](https://github.com/fibjs/fibjs/commit/7ffc201fae)] - **iconv, bugfix**: multi char utf-16 decode error.(xicilion)
* [[`29f63c319e`](https://github.com/fibjs/fibjs/commit/29f63c319e)] - **punycode, bugfix**: compile warning on windows x86.(响马)
* [[`5e24e08983`](https://github.com/fibjs/fibjs/commit/5e24e08983)] - **v8, feat**: upgrade to v6.2.377.(响马)
* [[`dc9d0afa70`](https://github.com/fibjs/fibjs/commit/dc9d0afa70)] - **iconv, feat**: support ucs4 codec.(响马)
* [[`195314e86c`](https://github.com/fibjs/fibjs/commit/195314e86c)] - **iconv, bugfix**: unicode 0xd800 convert error.(响马)
* [[`e359be21f5`](https://github.com/fibjs/fibjs/commit/e359be21f5)] - **core, refactor**: use new NewString convert string to v8 Value, fix v8 utf8 issue.(响马)
* [[`b0f49372f2`](https://github.com/fibjs/fibjs/commit/b0f49372f2)] - **iconv, feat**: support ucs2, utf16le codec.(响马)
* [[`d235fc204f`](https://github.com/fibjs/fibjs/commit/d235fc204f)] - **re, break**: deprecated module re.(响马)
* [[`6a02c5201d`](https://github.com/fibjs/fibjs/commit/6a02c5201d)] - **core, bugfix**: disable parallel scavenge, fix gc deadlock.(响马)
* [[`4ff74b2b22`](https://github.com/fibjs/fibjs/commit/4ff74b2b22)] - **punycode, bugfix**: compile error on linux.(响马)
* [[`f2fb96a312`](https://github.com/fibjs/fibjs/commit/f2fb96a312)] - **docs**: remove old docs.(响马)
* [[`d53df147e1`](https://github.com/fibjs/fibjs/commit/d53df147e1)] - **punycode, feat**: support native module punycode.(响马)
* [[`f52f2dedc3`](https://github.com/fibjs/fibjs/commit/f52f2dedc3)] - **util, refactor**: method format check native object first.(响马)
* [[`d3587e7fe0`](https://github.com/fibjs/fibjs/commit/d3587e7fe0)] - **db, feat**: query result NULL is converted to null in MySQL and SQLite (#334)(blingz)
* [[`c1a79a0004`](https://github.com/fibjs/fibjs/commit/c1a79a0004)] - **db, feat**: undefined / null are converted to NULL (#333)(blingz)
* [[`51f58f3578`](https://github.com/fibjs/fibjs/commit/51f58f3578)] - **core, feat**: support ucs2 string convert.(响马)
* [[`350a328ca0`](https://github.com/fibjs/fibjs/commit/350a328ca0)] - **core, refactor**: rename utf8 api, support wchar32.(响马)
* [[`82a9924bc4`](https://github.com/fibjs/fibjs/commit/82a9924bc4)] - **bugfix**: compile warning in debug mode.(响马)
* [[`8f9d6450f9`](https://github.com/fibjs/fibjs/commit/8f9d6450f9)] - **db, bugfix**: fix typo.(响马)
* [[`5b67d542a9`](https://github.com/fibjs/fibjs/commit/5b67d542a9)] - **db, refactor**: use simpleObject to return the query result of execute.(响马)
* [[`e5a98ede0d`](https://github.com/fibjs/fibjs/commit/e5a98ede0d)] - **core, feat**: SimpleObject support Array.(响马)
* [[`ee9b215c57`](https://github.com/fibjs/fibjs/commit/ee9b215c57)] - **vs, feat**: generate vc proj file.(响马)
* [[`9f76fa8627`](https://github.com/fibjs/fibjs/commit/9f76fa8627)] - **core**: bugfix: method count error in class info of  object.(响马)
* [[`97cfa51b53`](https://github.com/fibjs/fibjs/commit/97cfa51b53)] - **core, bugfix**: compile warning on windows debug.(响马)
* [[`ce2963d1f1`](https://github.com/fibjs/fibjs/commit/ce2963d1f1)] - **test, bugfix**: callback hungup when execute return error.(响马)
* [[`9ef7ae593d`](https://github.com/fibjs/fibjs/commit/9ef7ae593d)] - **net, refactor**: use SimpleObject to build the result of method recvfrom.(响马)
* [[`354f580471`](https://github.com/fibjs/fibjs/commit/354f580471)] - **idlc, refactor**: not refer object_base.(响马)
* [[`731e7de9da`](https://github.com/fibjs/fibjs/commit/731e7de9da)] - **Variant, refactor**: use valueOf to convert object_base to JavaScript Value.(响马)
* [[`62698ce857`](https://github.com/fibjs/fibjs/commit/62698ce857)] - **core, feat**: Implement SimpleObject to build simple JavaScript Object from native.(响马)
* [[`789d30f24a`](https://github.com/fibjs/fibjs/commit/789d30f24a)] - **core, refactor**: use valueOf to get the wrap of native object.(响马)
* [[`def3f5fab6`](https://github.com/fibjs/fibjs/commit/def3f5fab6)] - **object, break**: deprecated valueOf.(响马)
* [[`43c42740cf`](https://github.com/fibjs/fibjs/commit/43c42740cf)] - **Int64, break**: deprecated valueOf.(响马)
* [[`95c2e5aaa6`](https://github.com/fibjs/fibjs/commit/95c2e5aaa6)] - **test, feat**: support this in test function.(响马)
* [[`1f43374a7c`](https://github.com/fibjs/fibjs/commit/1f43374a7c)] - **crypto, feat**: support crypto.createHmac.(响马)
* [[`754a0ffe7f`](https://github.com/fibjs/fibjs/commit/754a0ffe7f)] - delete unused file.(响马)
* [[`9424db39a3`](https://github.com/fibjs/fibjs/commit/9424db39a3)] - **core, feat**: not ignore nan in arguments.(响马)
* [[`0f3f0ee885`](https://github.com/fibjs/fibjs/commit/0f3f0ee885)] - **Buffer, feat**: Buffer.write* return new offset, compatible with node.js.(响马)
* [[`f990110958`](https://github.com/fibjs/fibjs/commit/f990110958)] - **HttpCookie, feat**: path=/ as default value in cookie (#332)(blingz)

## 2017-08-27, Version v0.12.0

* **feature** :
    * Buffer - Buffer.write* return new offset, compatible with node.js.(响马)
    * core:
      - support ucs2 string convert.(响马)
      - SimpleObject support Array.(响马)
      - Implement SimpleObject to build simple JavaScript Object from native.(响马)
      - not ignore nan in arguments.(响马)
    * crypto - support crypto.createHmac.(响马)
    * db:
      - query result NULL is converted to null in MySQL and SQLite (#334)(blingz)
      - undefined / null are converted to NULL (#333)(blingz)
    * HttpCookie - path=/ as default value in cookie (#332)(blingz)
    * iconv:
      - support ucs4 codec.(响马)
      - support ucs2, utf16le codec.(响马)
    * punycode - support native module punycode.(响马)
    * test - support this in test function.(响马)
    * v8 - upgrade to v6.2.377.(响马)
    * vs - generate vc proj file.(响马)

* **breakchange** :
    * Int64 - deprecated valueOf.(响马)
    * object - deprecated valueOf.(响马)
    * re - deprecated module re.(响马)

* **bugfix** :
    * core:
      - disable parallel scavenge, fix gc deadlock.(响马)
      - compile warning on windows debug.(响马)
      - method count error in class info of  object.(响马)
    * db - fix typo.(响马)
    * iconv:
      - multi char utf-16 decode error.(xicilion)
      - unicode 0xd800 convert error.(响马)
    * punycode:
      - compile warning on windows x86.(响马)
      - compile error on linux.(响马)
    * sandbox - jsc test file load error.(xicilion)
    * test - callback hungup when execute return error.(响马)

* **refactor** :
    * core:
      - use new NewString convert string to v8 Value, fix v8 utf8 issue.(响马)
      - rename utf8 api, support wchar32.(响马)
      - use valueOf to get the wrap of native object.(响马)
    * db - use simpleObject to return the query result of execute.(响马)
    * idlc - not refer object_base.(响马)
    * net - use SimpleObject to build the result of method recvfrom.(响马)
    * util - method format check native object first.(响马)
    * Variant - use valueOf to convert object_base to JavaScript Value.(响马)

### Commits
* [[`8d729e1a9d`](https://github.com/fibjs/fibjs/commit/8d729e1a9d)] - **sandbox, bugfix**: jsc test file load error.(xicilion)
* [[`7ffc201fae`](https://github.com/fibjs/fibjs/commit/7ffc201fae)] - **iconv, bugfix**: multi char utf-16 decode error.(xicilion)
* [[`29f63c319e`](https://github.com/fibjs/fibjs/commit/29f63c319e)] - **punycode, bugfix**: compile warning on windows x86.(响马)
* [[`5e24e08983`](https://github.com/fibjs/fibjs/commit/5e24e08983)] - **v8, feat**: upgrade to v6.2.377.(响马)
* [[`dc9d0afa70`](https://github.com/fibjs/fibjs/commit/dc9d0afa70)] - **iconv, feat**: support ucs4 codec.(响马)
* [[`195314e86c`](https://github.com/fibjs/fibjs/commit/195314e86c)] - **iconv, bugfix**: unicode 0xd800 convert error.(响马)
* [[`e359be21f5`](https://github.com/fibjs/fibjs/commit/e359be21f5)] - **core, refactor**: use new NewString convert string to v8 Value, fix v8 utf8 issue.(响马)
* [[`b0f49372f2`](https://github.com/fibjs/fibjs/commit/b0f49372f2)] - **iconv, feat**: support ucs2, utf16le codec.(响马)
* [[`d235fc204f`](https://github.com/fibjs/fibjs/commit/d235fc204f)] - **re, break**: deprecated module re.(响马)
* [[`6a02c5201d`](https://github.com/fibjs/fibjs/commit/6a02c5201d)] - **core, bugfix**: disable parallel scavenge, fix gc deadlock.(响马)
* [[`4ff74b2b22`](https://github.com/fibjs/fibjs/commit/4ff74b2b22)] - **punycode, bugfix**: compile error on linux.(响马)
* [[`f2fb96a312`](https://github.com/fibjs/fibjs/commit/f2fb96a312)] - **docs**: remove old docs.(响马)
* [[`d53df147e1`](https://github.com/fibjs/fibjs/commit/d53df147e1)] - **punycode, feat**: support native module punycode.(响马)
* [[`f52f2dedc3`](https://github.com/fibjs/fibjs/commit/f52f2dedc3)] - **util, refactor**: method format check native object first.(响马)
* [[`d3587e7fe0`](https://github.com/fibjs/fibjs/commit/d3587e7fe0)] - **db, feat**: query result NULL is converted to null in MySQL and SQLite (#334)(blingz)
* [[`c1a79a0004`](https://github.com/fibjs/fibjs/commit/c1a79a0004)] - **db, feat**: undefined / null are converted to NULL (#333)(blingz)
* [[`51f58f3578`](https://github.com/fibjs/fibjs/commit/51f58f3578)] - **core, feat**: support ucs2 string convert.(响马)
* [[`350a328ca0`](https://github.com/fibjs/fibjs/commit/350a328ca0)] - **core, refactor**: rename utf8 api, support wchar32.(响马)
* [[`82a9924bc4`](https://github.com/fibjs/fibjs/commit/82a9924bc4)] - **bugfix**: compile warning in debug mode.(响马)
* [[`8f9d6450f9`](https://github.com/fibjs/fibjs/commit/8f9d6450f9)] - **db, bugfix**: fix typo.(响马)
* [[`5b67d542a9`](https://github.com/fibjs/fibjs/commit/5b67d542a9)] - **db, refactor**: use simpleObject to return the query result of execute.(响马)
* [[`e5a98ede0d`](https://github.com/fibjs/fibjs/commit/e5a98ede0d)] - **core, feat**: SimpleObject support Array.(响马)
* [[`ee9b215c57`](https://github.com/fibjs/fibjs/commit/ee9b215c57)] - **vs, feat**: generate vc proj file.(响马)
* [[`9f76fa8627`](https://github.com/fibjs/fibjs/commit/9f76fa8627)] - **core**: bugfix: method count error in class info of  object.(响马)
* [[`97cfa51b53`](https://github.com/fibjs/fibjs/commit/97cfa51b53)] - **core, bugfix**: compile warning on windows debug.(响马)
* [[`ce2963d1f1`](https://github.com/fibjs/fibjs/commit/ce2963d1f1)] - **test, bugfix**: callback hungup when execute return error.(响马)
* [[`9ef7ae593d`](https://github.com/fibjs/fibjs/commit/9ef7ae593d)] - **net, refactor**: use SimpleObject to build the result of method recvfrom.(响马)
* [[`354f580471`](https://github.com/fibjs/fibjs/commit/354f580471)] - **idlc, refactor**: not refer object_base.(响马)
* [[`731e7de9da`](https://github.com/fibjs/fibjs/commit/731e7de9da)] - **Variant, refactor**: use valueOf to convert object_base to JavaScript Value.(响马)
* [[`62698ce857`](https://github.com/fibjs/fibjs/commit/62698ce857)] - **core, feat**: Implement SimpleObject to build simple JavaScript Object from native.(响马)
* [[`789d30f24a`](https://github.com/fibjs/fibjs/commit/789d30f24a)] - **core, refactor**: use valueOf to get the wrap of native object.(响马)
* [[`def3f5fab6`](https://github.com/fibjs/fibjs/commit/def3f5fab6)] - **object, break**: deprecated valueOf.(响马)
* [[`43c42740cf`](https://github.com/fibjs/fibjs/commit/43c42740cf)] - **Int64, break**: deprecated valueOf.(响马)
* [[`95c2e5aaa6`](https://github.com/fibjs/fibjs/commit/95c2e5aaa6)] - **test, feat**: support this in test function.(响马)
* [[`1f43374a7c`](https://github.com/fibjs/fibjs/commit/1f43374a7c)] - **crypto, feat**: support crypto.createHmac.(响马)
* [[`754a0ffe7f`](https://github.com/fibjs/fibjs/commit/754a0ffe7f)] - delete unused file.(响马)
* [[`9424db39a3`](https://github.com/fibjs/fibjs/commit/9424db39a3)] - **core, feat**: not ignore nan in arguments.(响马)
* [[`0f3f0ee885`](https://github.com/fibjs/fibjs/commit/0f3f0ee885)] - **Buffer, feat**: Buffer.write* return new offset, compatible with node.js.(响马)
* [[`f990110958`](https://github.com/fibjs/fibjs/commit/f990110958)] - **HttpCookie, feat**: path=/ as default value in cookie (#332)(blingz)

## 2017-08-19, Version v0.11.0

* **feature** :
    * Buffer:
      - add Buffer.entries() (#326)(Hengfei Zhuang)
      - remove Buffer.Compare.(响马)
      - support Buffer.Compare and Buffer.isEncoding (#322)(tapir)
      - support new Buffer(DataView);(响马)
      - Buffer.from support byteOffset and length.(响马)
      - add keys and values method in Buffer Class. (#312)(tapir)
      - support Buffer.reverse.(响马)
      - support Buffer.allocUnsafe and  Buffer.allocUnsafeSlow method(响马)
      - support Buffer.alloc method. (#319)(tapir)
      - support Buffer.byteLength method. (#317)(tapir)
      - support require('buffer').Buffer.(响马)
    * build - disable parallel compile in Windows Debug mode.(响马)
    * ci - upgrade xcode version.(响马)
    * console - in Windows gui mode, call console.log popup console.(响马)
    * core:
      - ignore nan arguments.(响马)
      - continue js default process when _named_setter return NULL.(响马)
      - enable modify the built-in object in strict mode (#311)(Hengfei Zhuang)
    * crypto - support crypto.createHash.(响马)
    * db - allow modify the propertis of DBRow.(响马)
    * Digest - Digest.update return Digest object itself.(响马)
    * doc - warning about postMessage may lose messages.(响马)
    * docs - fix doc error.(响马)
    * idlc - support ...args syntax, Use Array to pass variable-length arguments.(响马)
    * Int64 - add multi & div (#309)(Hengfei Zhuang)
    * list - return null when index out of range.(响马)
    * mq - add 2 cases about memory leak.(响马)
    * PKey:
      - PKey.verify support algo.(响马)
      - importKey docs.(响马)
      - support import/export jwe format json.(响马)
    * process - use SIGINT to stop child process.(响马)
    * Routing:
      - sub routing document.(响马)
      - support sub routing.(响马)
      - support method chaining.(响马)
      - support express compatible path template.(响马)
    * TcpServer:
      - hold the server until all connection closed.(响马)
      - server holder not wrap js object.(响马)
    * test - add async execute test.(响马)
    * tools - automatically generate changelog.(响马)
    * v8 - upgrade to v6.2.219.(响马)
    * WebSocket - add sample code.(响马)
    * WebView:
      - document new api.(响马)
      - async open webview in gui.open.(响马)
      - add test case.(响马)
      - disable console.log when debug option set to false. the debug option defaults to true.(响马)
      - redirect the webview console to the fibjs log system.(响马)
      - set the default version to the highest version.(响马)
      - log web script error to console.error.(响马)
      - support window.close.(响马)

* **breakchange** :
    * Buffer - compact with Uint8Array setter (#330)(Hengfei Zhuang)
    * core:
      - compact with node index getter (#329)(Hengfei Zhuang)
      - not catch ctrl-c.(响马)
    * Routing:
      - remove method Routing.append(method, map);(响马)
      - msg.value is not modified when no child is matched.(响马)
    * WebView - emit onclose event instead of onmessage('close').(响马)

* **bugfix** :
    * Buffer:
      - compile warning on windows.(响马)
      - unused variable.(响马)
      - fix idl error.(响马)
    * core - do not skip undefined args in ...args mode.(响马)
    * event - app crash when emit an event on a no-js object.(响马)
    * fiber - compile error in debug mode.(响马)
    * HttpRequest - the cookie is not processed when the request object is reused.(响马)
    * mongo - lock the database when the cursor is destroyed asynchronously.(响马)
    * mssql - fix memory leak in mssql.execute (#310)(onceyoung)
    * PKey - compile warning on windows.(响马)
    * redis - fix test case error.(响马)
    * sanbox - fix error when require('.') and require('..').(响马)
    * SandBox - require('..') is not equal require('../') on Windows XP;(响马)
    * TcpServer - hold the js object during the execution of the server.run method.(响马)
    * test - restore env in windows, fix WebView test crash.(响马)
    * v8 - enable bigobj, fix compile error on windows debug mode.(响马)
    * WebSocket - on('message', func) not emit in ws.upgrade.(响马)
    * WebView:
      - console not inject in IE8.(响马)
      - remove unused output.(响马)
      - app crash when error source is null.(响马)
      - app crash when close an alert dialog box.(响马)
      - html document is not ready when the NavigateComplete2 event fired.(响马)
      - not trigger 'load' event when iframe loaded.(响马)

* **refactor** :
    * Buffer:
      - use ByteLength function to get length.(响马)
      - use the Buffer constructor conversion type.(响马)
      - format code.(响马)
    * console - switch to ...args syntax.(响马)
    * core:
      - not ref/unref when obj pointer not changed.(响马)
      - rewrite naked_ptr, change the pointer access mode.(响马)
      - declare OptArgs to process ...args.(响马)
      - use std::vector to transfer ...args.(响马)
    * coroutine - switch to ...args syntax.(响马)
    * db:
      - change execute(fmt, ...args) to real async mode.(响马)
      - switch to ...args syntax.(响马)
    * event - switch to ...args syntax.(响马)
    * events - support onEventTrigger virtual method.(响马)
    * http:
      - store the error handler separately.(响马)
      - change http.request to real async mode.(响马)
    * List - switch to ...args syntax.(响马)
    * mongo - remove useless code (#325)(Hengfei Zhuang)
    * mq:
      - use getArgumentValue to convert handler from value.(响马)
      - use obj_ptr to save handler poinger.(响马)
    * path - switch to ...args syntax.(响马)
    * redis - switch to ...args syntax.(响马)
    * SandBox - Unify the code used to detect relative paths.(响马)
    * util:
      - switch to ...args syntax.(响马)
      - util.extend switch to ...args syntax.(响马)

### Commits
* [[`fce451eb6d`](https://github.com/fibjs/fibjs/commit/fce451eb6d)] - **Buffer, break**: compact with Uint8Array setter (#330)(Hengfei Zhuang)
* [[`9d65e24eae`](https://github.com/fibjs/fibjs/commit/9d65e24eae)] - **core, break**: compact with node index getter (#329)(Hengfei Zhuang)
* [[`ba4c6197db`](https://github.com/fibjs/fibjs/commit/ba4c6197db)] - **tools, feat**: automatically generate changelog.(响马)
* [[`b1234a74ad`](https://github.com/fibjs/fibjs/commit/b1234a74ad)] - **Routing, feat**: sub routing document.(响马)
* [[`bc84cc78d9`](https://github.com/fibjs/fibjs/commit/bc84cc78d9)] - **Routing, feat**: support sub routing.(响马)
* [[`37da2d21ba`](https://github.com/fibjs/fibjs/commit/37da2d21ba)] - **Buffer, feat**: add Buffer.entries() (#326)(Hengfei Zhuang)
* [[`6d13eafe60`](https://github.com/fibjs/fibjs/commit/6d13eafe60)] - **Routing, break**: remove method Routing.append(method, map);(响马)
* [[`0e47076fc4`](https://github.com/fibjs/fibjs/commit/0e47076fc4)] - **mongo, refactor**: remove useless code (#325)(Hengfei Zhuang)
* [[`8caa3366e8`](https://github.com/fibjs/fibjs/commit/8caa3366e8)] - **mq, refactor**: use getArgumentValue to convert handler from value.(响马)
* [[`78c458c9df`](https://github.com/fibjs/fibjs/commit/78c458c9df)] - **WebSocket, feat**: add sample code.(响马)
* [[`2855a51c00`](https://github.com/fibjs/fibjs/commit/2855a51c00)] - **WebSocket, bugfix**: on('message', func) not emit in ws.upgrade.(响马)
* [[`88a8b021fc`](https://github.com/fibjs/fibjs/commit/88a8b021fc)] - **events, refactor**: support onEventTrigger virtual method.(响马)
* [[`43759997be`](https://github.com/fibjs/fibjs/commit/43759997be)] - **docs, feat**: fix doc error.(响马)
* [[`25cdcee4f3`](https://github.com/fibjs/fibjs/commit/25cdcee4f3)] - **Routing, feat**: support method chaining.(响马)
* [[`79d9856a0c`](https://github.com/fibjs/fibjs/commit/79d9856a0c)] - **WebView, feat**: document new api.(响马)
* [[`174a39d39b`](https://github.com/fibjs/fibjs/commit/174a39d39b)] - **Buffer, feat**: remove Buffer.Compare.(响马)
* [[`cc2fd7be17`](https://github.com/fibjs/fibjs/commit/cc2fd7be17)] - **ci, feat**: upgrade xcode version.(响马)
* [[`be9b9830aa`](https://github.com/fibjs/fibjs/commit/be9b9830aa)] - **Buffer, feat**: support Buffer.Compare and Buffer.isEncoding (#322)(tapir)
* [[`87eb352358`](https://github.com/fibjs/fibjs/commit/87eb352358)] - **Buffer, bugfix**: compile warning on windows.(响马)
* [[`8759232b1c`](https://github.com/fibjs/fibjs/commit/8759232b1c)] - **v8, feat**: upgrade to v6.2.219.(响马)
* [[`92d0c6a6f6`](https://github.com/fibjs/fibjs/commit/92d0c6a6f6)] - **Buffer, refactor**: use ByteLength function to get length.(响马)
* [[`b5d66ec92e`](https://github.com/fibjs/fibjs/commit/b5d66ec92e)] - **Buffer, refactor**: use the Buffer constructor conversion type.(响马)
* [[`0350d36e18`](https://github.com/fibjs/fibjs/commit/0350d36e18)] - **Buffer, feat**: support new Buffer(DataView);(响马)
* [[`e90a53e704`](https://github.com/fibjs/fibjs/commit/e90a53e704)] - **HttpRequest, bugfix**: the cookie is not processed when the request object is reused.(响马)
* [[`4240334ddd`](https://github.com/fibjs/fibjs/commit/4240334ddd)] - **Buffer, feat**: Buffer.from support byteOffset and length.(响马)
* [[`cd5ca14d76`](https://github.com/fibjs/fibjs/commit/cd5ca14d76)] - **Buffer, feat**: support Buffer.reverse.(响马)
* [[`6e9f705482`](https://github.com/fibjs/fibjs/commit/6e9f705482)] - **core, feat**: ignore nan arguments.(响马)
* [[`353d9ca9e4`](https://github.com/fibjs/fibjs/commit/353d9ca9e4)] - **crypto, feat**: support crypto.createHash.(响马)
* [[`91ce2f2220`](https://github.com/fibjs/fibjs/commit/91ce2f2220)] - **Digest, feat**: Digest.update return Digest object itself.(响马)
* [[`afc7d34c29`](https://github.com/fibjs/fibjs/commit/afc7d34c29)] - **Buffer, feat**: support Buffer.allocUnsafe and  Buffer.allocUnsafeSlow method(响马)
* [[`bcefc365de`](https://github.com/fibjs/fibjs/commit/bcefc365de)] - **mq, refactor**: use obj_ptr to save handler poinger.(响马)
* [[`6de04d0d93`](https://github.com/fibjs/fibjs/commit/6de04d0d93)] - **mq, feat**: add 2 cases about memory leak.(响马)
* [[`7d93938bf9`](https://github.com/fibjs/fibjs/commit/7d93938bf9)] - **TcpServer, feat**: hold the server until all connection closed.(响马)
* [[`53cd55304d`](https://github.com/fibjs/fibjs/commit/53cd55304d)] - **docs**: fix param error.(响马)
* [[`4abd7caf3c`](https://github.com/fibjs/fibjs/commit/4abd7caf3c)] - **Buffer, feat**: support Buffer.alloc method. (#319)(tapir)
* [[`938b7003df`](https://github.com/fibjs/fibjs/commit/938b7003df)] - **TcpServer, feat**: server holder not wrap js object.(响马)
* [[`1d807d52a4`](https://github.com/fibjs/fibjs/commit/1d807d52a4)] - **PKey, feat**: PKey.verify support algo.(响马)
* [[`c472d32407`](https://github.com/fibjs/fibjs/commit/c472d32407)] - **TcpServer, bugfix**: hold the js object during the execution of the server.run method.(响马)
* [[`4d57776396`](https://github.com/fibjs/fibjs/commit/4d57776396)] - **http, refactor**: store the error handler separately.(响马)
* [[`444d00e486`](https://github.com/fibjs/fibjs/commit/444d00e486)] - **core, refactor**: not ref/unref when obj pointer not changed.(响马)
* [[`88b760b636`](https://github.com/fibjs/fibjs/commit/88b760b636)] - **vender**: update vender version.(响马)
* [[`96f6a34f4d`](https://github.com/fibjs/fibjs/commit/96f6a34f4d)] - **Buffer, feat**: support Buffer.byteLength method. (#317)(tapir)
* [[`53636a9347`](https://github.com/fibjs/fibjs/commit/53636a9347)] - **core, refactor**: rewrite naked_ptr, change the pointer access mode.(响马)
* [[`ffd9dc4673`](https://github.com/fibjs/fibjs/commit/ffd9dc4673)] - **WebView, feat**: async open webview in gui.open.(响马)
* [[`b1828ada4e`](https://github.com/fibjs/fibjs/commit/b1828ada4e)] - **SandBox, bugfix**: require('..') is not equal require('../') on Windows XP;(响马)
* [[`c977e4849d`](https://github.com/fibjs/fibjs/commit/c977e4849d)] - **SandBox, refactor**: Unify the code used to detect relative paths.(响马)
* [[`7474b3f275`](https://github.com/fibjs/fibjs/commit/7474b3f275)] - **test, bugfix**: restore env in windows, fix WebView test crash.(响马)
* [[`7dff9943f1`](https://github.com/fibjs/fibjs/commit/7dff9943f1)] - **WebView, feat**: add test case.(响马)
* [[`63e8f87198`](https://github.com/fibjs/fibjs/commit/63e8f87198)] - **WebView, bugfix**: console not inject in IE8.(响马)
* [[`99a3d14160`](https://github.com/fibjs/fibjs/commit/99a3d14160)] - **WebView, feat**: disable console.log when debug option set to false. the debug option defaults to true.(响马)
* [[`6959f5674e`](https://github.com/fibjs/fibjs/commit/6959f5674e)] - **WebView, bugfix**: remove unused output.(响马)
* [[`2e2fcfb5dd`](https://github.com/fibjs/fibjs/commit/2e2fcfb5dd)] - **WebView, feat**: redirect the webview console to the fibjs log system.(响马)
* [[`ad37384c40`](https://github.com/fibjs/fibjs/commit/ad37384c40)] - **event, bugfix**: app crash when emit an event on a no-js object.(响马)
* [[`be85001b58`](https://github.com/fibjs/fibjs/commit/be85001b58)] - **WebView, bugfix**: app crash when error source is null.(响马)
* [[`ba52c7830f`](https://github.com/fibjs/fibjs/commit/ba52c7830f)] - **WebView, feat**: set the default version to the highest version.(响马)
* [[`c6a06404ab`](https://github.com/fibjs/fibjs/commit/c6a06404ab)] - **build, feat**: disable parallel compile in Windows Debug mode.(响马)
* [[`74ffaea233`](https://github.com/fibjs/fibjs/commit/74ffaea233)] - **WebView, bugfix**: app crash when close an alert dialog box.(响马)
* [[`72a942ebac`](https://github.com/fibjs/fibjs/commit/72a942ebac)] - **vender**: update vender version.(响马)
* [[`6da1126a78`](https://github.com/fibjs/fibjs/commit/6da1126a78)] - **v8, bugfix**: enable bigobj, fix compile error on windows debug mode.(响马)
* [[`4825ac82a6`](https://github.com/fibjs/fibjs/commit/4825ac82a6)] - **WebView, bugfix**: html document is not ready when the NavigateComplete2 event fired.(响马)
* [[`41642f44d0`](https://github.com/fibjs/fibjs/commit/41642f44d0)] - **WebView, feat**: log web script error to console.error.(响马)
* [[`4093dbb6cc`](https://github.com/fibjs/fibjs/commit/4093dbb6cc)] - **console, feat**: in Windows gui mode, call console.log popup console.(响马)
* [[`b953f0351b`](https://github.com/fibjs/fibjs/commit/b953f0351b)] - **Buffer, refactor**: format code.(响马)
* [[`0753f74164`](https://github.com/fibjs/fibjs/commit/0753f74164)] - **Buffer, bugfix**: unused variable.(响马)
* [[`9bf02c751f`](https://github.com/fibjs/fibjs/commit/9bf02c751f)] - **PKey, bugfix**: compile warning on windows.(响马)
* [[`a31745cfa3`](https://github.com/fibjs/fibjs/commit/a31745cfa3)] - **PKey, feat**: importKey docs.(响马)
* [[`30d38fee20`](https://github.com/fibjs/fibjs/commit/30d38fee20)] - **Buffer, bugfix**: fix idl error.(响马)
* [[`be727204db`](https://github.com/fibjs/fibjs/commit/be727204db)] - **PKey, feat**: support import/export jwe format json.(响马)
* [[`1b8f0b10e4`](https://github.com/fibjs/fibjs/commit/1b8f0b10e4)] - **Buffer, feat**: support require('buffer').Buffer.(响马)
* [[`8858e823ee`](https://github.com/fibjs/fibjs/commit/8858e823ee)] - **mongo, bugfix**: lock the database when the cursor is destroyed asynchronously.(响马)
* [[`bd12bbd2f7`](https://github.com/fibjs/fibjs/commit/bd12bbd2f7)] - **Routing, feat**: support express compatible path template.(响马)
* [[`50b55bc37c`](https://github.com/fibjs/fibjs/commit/50b55bc37c)] - add keys and values method in Buffer Class. (#312)(tapir)
* [[`33db04d4b4`](https://github.com/fibjs/fibjs/commit/33db04d4b4)] - **process, feat**: use SIGINT to stop child process.(响马)
* [[`8222ba0eff`](https://github.com/fibjs/fibjs/commit/8222ba0eff)] - **core, break**: not catch ctrl-c.(响马)
* [[`28fb345af6`](https://github.com/fibjs/fibjs/commit/28fb345af6)] - **WebView, break**: emit onclose event instead of onmessage('close').(响马)
* [[`3ad67e0574`](https://github.com/fibjs/fibjs/commit/3ad67e0574)] - **db, feat**: allow modify the propertis of DBRow.(响马)
* [[`92d90cbc8c`](https://github.com/fibjs/fibjs/commit/92d90cbc8c)] - **list, feat**: return null when index out of range.(响马)
* [[`91497973d0`](https://github.com/fibjs/fibjs/commit/91497973d0)] - **core, feat**: continue js default process when _named_setter return NULL.(响马)
* [[`a517cb1345`](https://github.com/fibjs/fibjs/commit/a517cb1345)] - **sanbox, bugfix**: fix error when require('.') and require('..').(响马)
* [[`6369fcc019`](https://github.com/fibjs/fibjs/commit/6369fcc019)] - **http, refactor**: change http.request to real async mode.(响马)
* [[`485f218bac`](https://github.com/fibjs/fibjs/commit/485f218bac)] - **test, feat**: add async execute test.(响马)
* [[`5d3e99e6e0`](https://github.com/fibjs/fibjs/commit/5d3e99e6e0)] - **db, refactor**: change execute(fmt, ...args) to real async mode.(响马)
* [[`bacf979fb8`](https://github.com/fibjs/fibjs/commit/bacf979fb8)] - **core, refactor**: declare OptArgs to process ...args.(响马)
* [[`cf5e8828b2`](https://github.com/fibjs/fibjs/commit/cf5e8828b2)] - **redis, refactor**: switch to ...args syntax.(响马)
* [[`c412a6f05e`](https://github.com/fibjs/fibjs/commit/c412a6f05e)] - **fiber, bugfix**: compile error in debug mode.(响马)
* [[`eae39d2f22`](https://github.com/fibjs/fibjs/commit/eae39d2f22)] - **core, refactor**: use std::vector to transfer ...args.(响马)
* [[`9fdf8181ce`](https://github.com/fibjs/fibjs/commit/9fdf8181ce)] - **core, feat**: enable modify the built-in object in strict mode (#311)(Hengfei Zhuang)
* [[`445be030e1`](https://github.com/fibjs/fibjs/commit/445be030e1)] - **redis, bugfix**: fix test case error.(响马)
* [[`e920013038`](https://github.com/fibjs/fibjs/commit/e920013038)] - **event, refactor**: switch to ...args syntax.(响马)
* [[`525f6e5984`](https://github.com/fibjs/fibjs/commit/525f6e5984)] - **db, refactor**: switch to ...args syntax.(响马)
* [[`868daf45b8`](https://github.com/fibjs/fibjs/commit/868daf45b8)] - **path, refactor**: switch to ...args syntax.(响马)
* [[`87519007fe`](https://github.com/fibjs/fibjs/commit/87519007fe)] - **List, refactor**: switch to ...args syntax.(响马)
* [[`9148441329`](https://github.com/fibjs/fibjs/commit/9148441329)] - **console, refactor**: switch to ...args syntax.(响马)
* [[`4175472893`](https://github.com/fibjs/fibjs/commit/4175472893)] - **core, bugfix**: do not skip undefined args in ...args mode.(响马)
* [[`886fd8b99c`](https://github.com/fibjs/fibjs/commit/886fd8b99c)] - **coroutine, refactor**: switch to ...args syntax.(响马)
* [[`5347b405a3`](https://github.com/fibjs/fibjs/commit/5347b405a3)] - **util, refactor**: switch to ...args syntax.(响马)
* [[`bb5d8b5141`](https://github.com/fibjs/fibjs/commit/bb5d8b5141)] - Merge branch 'master' into dev(响马)
* [[`7ab0252bff`](https://github.com/fibjs/fibjs/commit/7ab0252bff)] - fix memory leak in mssql.execute (#310)(onceyoung)
* [[`67451f6574`](https://github.com/fibjs/fibjs/commit/67451f6574)] - **util, refactor**: util.extend switch to ...args syntax.(响马)
* [[`fd5a746377`](https://github.com/fibjs/fibjs/commit/fd5a746377)] - **idlc, feat**: support ...args syntax, Use Array to pass variable-length arguments.(响马)
* [[`88d2ede145`](https://github.com/fibjs/fibjs/commit/88d2ede145)] - **WebView, bugfix**: not trigger 'load' event when iframe loaded.(响马)
* [[`86baa1177b`](https://github.com/fibjs/fibjs/commit/86baa1177b)] - **WebView, feat**: support window.close.(响马)
* [[`c6c5b549ba`](https://github.com/fibjs/fibjs/commit/c6c5b549ba)] - **doc, feat**: warning about postMessage may lose messages.(响马)
* [[`0e2b30ad96`](https://github.com/fibjs/fibjs/commit/0e2b30ad96)] - **Int64, feat**: add multi & div (#309)(Hengfei Zhuang)
* [[`e76778caa0`](https://github.com/fibjs/fibjs/commit/e76778caa0)] - **Routing, break**: msg.value is not modified when no child is matched.(响马)
* [[`88b21e314a`](https://github.com/fibjs/fibjs/commit/88b21e314a)] - update vscode setting.(响马)

## 2017-07-31, Version v0.10.0

* **refactor** :
    * fiber:
      - switch worker back to thread mode in windows. (xicilion)
      - use fiber worker in windows, enable full feature of multi thread fiber. (xicilion)
      - abstract fiber api. (xicilion)
    * async - move the result checker into the class to reconstruct the source code. (xicilion)
    * core - use ac->isAsync to test async mode. (xicilion)
    * console - use cc_ function instead of ac. (xicilion)
    * sanbox - not clone the module return by callback. (xicilion)
    * fs - cache files until the zip package is changed. (xicilion)
    * test - use path.join to build file name. (xicilion)

* **bugfix** :
    * sandbox:
      - remove unused variable. (xicilion)
      - resolve search root folder twice on windows. (xicilion)
      - circular dependency error when module.exports is changed.. (xicilion)
    * leveldb - db.between not include the begin key. (xicilion)
    * mongodb - ensureIndex error on mongodb v3.0+ . (xicilion)
    * net - compile warning in debug mode. (xicilion)

* **breakchange** :
    * core - only use the args before entry script as the system options. (xicilion)

* **deps** :
    * v8 - v6.2.52. (xicilion)

### Commits

* [[`f3a5386cc3`](https://github.com/fibjs/fibjs/commit/f3a5386cc3)] - **async, refactor**: move the result checker into the class to reconstruct the source code. (xicilion)
* [[`76a2bd5567`](https://github.com/fibjs/fibjs/commit/76a2bd5567)] - **fiber, refactor**: switch worker back to thread mode in windows. (xicilion)
* [[`d5304df12f`](https://github.com/fibjs/fibjs/commit/d5304df12f)] - **fiber, refactor**: use fiber worker in windows, enable full feature of multi thread fiber. (xicilion)
* [[`7c4a5bd368`](https://github.com/fibjs/fibjs/commit/7c4a5bd368)] - **fiber, refactor**: abstract fiber api. (xicilion)
* [[`a29757fd58`](https://github.com/fibjs/fibjs/commit/a29757fd58)] - **sandbox,  bugfix**: remove unused variable. (xicilion)
* [[`fe384087a0`](https://github.com/fibjs/fibjs/commit/fe384087a0)] - **core, refactor**: use ac->isAsync to test async mode. (xicilion)
* [[`be97c97879`](https://github.com/fibjs/fibjs/commit/be97c97879)] - **console, refactor**: use cc_ function instead of ac. (xicilion)
* [[`8ea02b7ba7`](https://github.com/fibjs/fibjs/commit/8ea02b7ba7)] - **v8, upgrade**: v6.2.52. (xicilion)
* [[`13072368f5`](https://github.com/fibjs/fibjs/commit/13072368f5)] - **sanbox, refactor**: not clone the module return by callback. (xicilion)
* [[`694659486c`](https://github.com/fibjs/fibjs/commit/694659486c)] - **core: enable** strip option in release mode. (xicilion)
* [[`0cf26f2cce`](https://github.com/fibjs/fibjs/commit/0cf26f2cce)] - **fs, refactor**: cache files until the zip package is changed. (xicilion)
* [[`9dd77c284c`](https://github.com/fibjs/fibjs/commit/9dd77c284c)] - **sandbox, bugfix**: resolve search root folder twice on windows. (xicilion)
* [[`c961d71173`](https://github.com/fibjs/fibjs/commit/c961d71173)] - **leveldb, bugfix**: db.between not include the begin key. (xicilion)
* [[`3420200ecb`](https://github.com/fibjs/fibjs/commit/3420200ecb)] - **mongodb, bugfix**: ensureIndex error on mongodb v3.0+ . (xicilion)
* [[`9e99a7a92a`](https://github.com/fibjs/fibjs/commit/9e99a7a92a)] - **sandbox, bugfix**: circular dependency error when module.exports is changed.. (xicilion)
* [[`61acfb512c`](https://github.com/fibjs/fibjs/commit/61acfb512c)] - **test, refactor**: use path.join to build file name. (xicilion)
* [[`5f56491d76`](https://github.com/fibjs/fibjs/commit/5f56491d76)] - **core, break**: only use the args before entry script as the system options. (xicilion)
* [[`1a04db5488`](https://github.com/fibjs/fibjs/commit/1a04db5488)] - **net, bugfix**: compile warning in debug mode. (xicilion)
* [[`b7ee5c809f`](https://github.com/fibjs/fibjs/commit/b7ee5c809f)] - **v8: upgrade** to v6.1.539 (xicilion)
* [[`348c1b0476`](https://github.com/fibjs/fibjs/commit/348c1b0476)] - **test: add** v8 hash seed case. (xicilion)

## 2017-07-19, Version v0.9.0

* **feature** :
  * assert: add assert.ifError (#303) (Hengfei Zhuang)
  * db: support trans method to auto control transaction. (xicilion)
  * encoding: base64 support url mode. (xicilion)
  * fs:
    - compatible with windows XP. (xicilion)
    - complete fs test case, complete fs english doc (#307) (Hengfei Zhuang)
    - complete symlink realpath readlink methods on windows (#305) (Hengfei Zhuang 
    - add fs.fchmod fs.fchown fs.fdatasync fs.fsync (#292) (Hengfei Zhuang)
  * gui: WebView support close method and event. (xicilion)
  * http:
    - sort file in index page. (xicilion)
    - fileHandler support autoIndex. (xicilion)
    - unlimited download size, support http.maxDownloadSize to change it. (xicilion)
  * net: add isIP, isIPv4, isIPv6 (#301) (Hengfei Zhuang)
  * module: add require.cache (#300) (Hengfei Zhuang)
  * process: support hrtime. (xicilion)
  * sandbox: resolve the realpath of symlink file. (xicilion)
  * process: process.exit defaults to 0 (#298) (Hengfei Zhuang)
  * tty: add tty module (#296) (Hengfei Zhuang)
  * util: feat: add isAsyncFunction (#295) (Hengfei Zhuang)
  * v8:
    - upgrade to v6.1.500 (xicilion)
    - share ia64 snapshot between posix and windows. (xicilion)
    - upgrade to 6.1.435, enable snapshot at bootstrap Snapshot. (xicilion)

* **breakchange** :
  * core: remove the default harmony & use_strict flag (#299) (Hengfei Zhuang)
  * gui: rename WebView close event to closed. (xicilion)

* **bugfix** :
  * db: fix compile error on freebsd. (xicilion)
  * http:
    - fix compile warning on windows. (xicilion)
    - test: add index.html test. (xicilion)
    - maxDownloadSize cannot modify. rename maxDownloadSize to maxBodySize. (xicilion)
  * net: fix comiple error on windows. (xicilion)
  * v8: fix crash on windows x86. (xicilion)
  * xml: fix document error. (xicilion)

* **deps** :
  * v8: upgrade v8 to v6.1.500


### Commits

* [[`72589ad38b`](https://github.com/fibjs/fibjs/commit/72589ad38b)] - **gui, break**: rename WebView close event to closed. (xicilion)
* [[`14283edc8c`](https://github.com/fibjs/fibjs/commit/14283edc8c)] - **fs, feat**: compatible with windows XP. (xicilion)
* [[`50c130983d`](https://github.com/fibjs/fibjs/commit/50c130983d)] - **fs, refactor**: move init_fs function. (xicilion)
* [[`f2aadf38cd`](https://github.com/fibjs/fibjs/commit/f2aadf38cd)] - **fs, refactor**: format source code. (xicilion)
* [[`20698d6545`](https://github.com/fibjs/fibjs/commit/20698d6545)] - **ci, chore**: delete useless config (#308) (Hengfei Zhuang)
* [[`4a3ec408d7`](https://github.com/fibjs/fibjs/commit/4a3ec408d7)] - **gui, feat**: WebView support close method and event. (xicilion)
* [[`55ab9a3eaa`](https://github.com/fibjs/fibjs/commit/55ab9a3eaa)] - **core, break**: remove the default harmony & use_strict flag (#299) (Hengfei Zhuang)
* [[`7fb6beba84`](https://github.com/fibjs/fibjs/commit/7fb6beba84)] - **v8, bugfix**: fix crash on windows x86. (xicilion)
* [[`8e79b9e09f`](https://github.com/fibjs/fibjs/commit/8e79b9e09f)] - **fs, feat**: complete fs test case, complete fs english doc (#307) (Hengfei Zhuang)
* [[`967b602953`](https://github.com/fibjs/fibjs/commit/967b602953)] - **v8, feat**: upgrade to v6.1.500 (xicilion)
* [[`f6db24bb49`](https://github.com/fibjs/fibjs/commit/f6db24bb49)] - **net, bugfix**: fix comiple error on windows. (xicilion)
* [[`83513660e4`](https://github.com/fibjs/fibjs/commit/83513660e4)] - **fs, feat**: complete symlink realpath readlink methods on windows (#305) (Hengfei Zhuang 
* [[`c57323d131`](https://github.com/fibjs/fibjs/commit/c57323d131)] - **net, feat**: add isIP, isIPv4, isIPv6 (#301) (Hengfei Zhuang)
* [[`d34a3beeae`](https://github.com/fibjs/fibjs/commit/d34a3beeae)] - **module, feat**: add require.cache (#300) (Hengfei Zhuang)
* [[`29326433e6`](https://github.com/fibjs/fibjs/commit/29326433e6)] - **fs, feat**: add fs.fchmod fs.fchown fs.fdatasync fs.fsync (#292) (Hengfei Zhuang)
* [[`28c2cb07e8`](https://github.com/fibjs/fibjs/commit/28c2cb07e8)] - **assert, feat**: add assert.ifError (#303) (Hengfei Zhuang)
* [[`5df852888d`](https://github.com/fibjs/fibjs/commit/5df852888d)] - **assert, break**: allow undefined (#302) (Hengfei Zhuang)
* [[`da2378cad0`](https://github.com/fibjs/fibjs/commit/da2378cad0)] - **sandbox, feat**: resolve the realpath of symlink file. (xicilion)
* [[`a747a46eaf`](https://github.com/fibjs/fibjs/commit/a747a46eaf)] - **process, feat**: process.exit defaults to 0 (#298) (Hengfei Zhuang)
* [[`befdb45ff1`](https://github.com/fibjs/fibjs/commit/befdb45ff1)] - **tty, feat**: add tty module (#296) (Hengfei Zhuang)
* [[`d332acb156`](https://github.com/fibjs/fibjs/commit/d332acb156)] - **db, bugfix**: fix compile error on freebsd. (xicilion)
* [[`7ec6c04dde`](https://github.com/fibjs/fibjs/commit/7ec6c04dde)] - **v8, feat**: share ia64 snapshot between posix and windows. (xicilion)
* [[`56060e9201`](https://github.com/fibjs/fibjs/commit/56060e9201)] - **v8, feat**: upgrade to 6.1.435, enable snapshot at bootstrap Snapshot. (xicilion)
* [[`d224743bc4`](https://github.com/fibjs/fibjs/commit/d224743bc4)] - **process, feat**: support hrtime. (xicilion)
* [[`f64415dfe4`](https://github.com/fibjs/fibjs/commit/f64415dfe4)] - **util, feat**, feat: add isAsyncFunction (#295) (Hengfei Zhuang)
* [[`9736723003`](https://github.com/fibjs/fibjs/commit/9736723003)] - **encoding, feat**: base64 support url mode. (xicilion)
* [[`856aaf660e`](https://github.com/fibjs/fibjs/commit/856aaf660e)] - **db, feat**: support trans method to auto control transaction. (xicilion)
* [[`7e5933621f`](https://github.com/fibjs/fibjs/commit/7e5933621f)] - **xml, bugfix**: fix document error. (xicilion)
* [[`8a381a2786`](https://github.com/fibjs/fibjs/commit/8a381a2786)] - **http, bugfix**: fix compile warning on windows. (xicilion)
* [[`364827698c`](https://github.com/fibjs/fibjs/commit/364827698c)] - **crypto, refactor**: use the alias to implement the pbkdf2Sync (#294) (Hengfei Zhuang)
* [[`9f37e2fb46`](https://github.com/fibjs/fibjs/commit/9f37e2fb46)] - **http, feat**: sort file in index page. (xicilion)
* [[`5f6d41b09f`](https://github.com/fibjs/fibjs/commit/5f6d41b09f)] - **http, feat**: fileHandler support autoIndex. (xicilion)
* [[`60c36574fb`](https://github.com/fibjs/fibjs/commit/60c36574fb)] - **http, bugfix**: test: add index.html test. (xicilion)
* [[`f05b605fd2`](https://github.com/fibjs/fibjs/commit/f05b605fd2)] - **http, bugfix**: bug: maxDownloadSize cannot modify. rename maxDownloadSize to maxBodySize. (xicilion)
* [[`ff0c6f4147`](https://github.com/fibjs/fibjs/commit/ff0c6f4147)] - **http, feat**: unlimited download size, support http.maxDownloadSize to change it. (xicilion)

## 2017-07-08, Version v0.8.0

* **feature** :
  * crypto: support RSA PUBLIC KEY format pem file.
  * ssl: update root certs.
  * installer: use sudo&runas to install as administrator.
  * fs: implement fs.copy on freebsd.

* **breakchange** :
  * ws: deprecated WebSocketHandler soon.

* **bugfix** :
  * http: check http cookies validity (#285)

* **deps** :
  * v8: upgrade v8 to v6.1.394

### Commits

* [[`1e9a40bcdb`](https://github.com/fibjs/fibjs/commit/1e9a40bcdb)] - **v8, feat**: upgrade to v6.1.394.. (xicilion)
* [[`f61b52f3ab`](https://github.com/fibjs/fibjs/commit/f61b52f3ab)] - **crypto, feat**: : support RSA PUBLIC KEY format pem file. (xicilion)
* [[`1927fbec79`](https://github.com/fibjs/fibjs/commit/1927fbec79)] - **ssl, feat**: update root certs. (xicilion)
* [[`12e655871d`](https://github.com/fibjs/fibjs/commit/12e655871d)] - **ws, break**: deprecated WebSocketHandler soon. (xicilion)
* [[`aaf66d5e07`](https://github.com/fibjs/fibjs/commit/aaf66d5e07)] - **installer, feat**: use sudo&runas to install as administrator. (xicilion)
* [[`08c6d0b205`](https://github.com/fibjs/fibjs/commit/08c6d0b205)] - **fs, feat**: implement fs.copy on freebsd. (xicilion)
* [[`8c1e42b8bf`](https://github.com/fibjs/fibjs/commit/8c1e42b8bf)] - **http, bugfix**: check http cookies validity (asionius)

## 2017-07-03, Version v0.7.0 @ngot

* **feature** :
  * xml: 
    - support el.getElementsByClassName.
    - support el.getElementById.
  * sandbox: 
    - support switch to .js mode when filename does not have ext.
    - support add version mark in .jsc file.
  * http: output fibjs version in http header.
  * core: all Sync alias to  all async apis.
* **breakchange** :
  * sandbox: remove --root and --approot option.
* **bugfix** :
  * v8: disable icu, fix toLocaleDateString error.
* **deps** :
  * v8: upgrade to v6.1.344

* [[`67fee618cc`](https://github.com/fibjs/fibjs/commit/67fee618cc)] - **core, fixbug**: fix freebsd compile error. (xicilion)
* [[`0e01161d4f`](https://github.com/fibjs/fibjs/commit/0e01161d4f)] - **core, chore**: fix compile warning. (xicilion)
* [[`a8467666aa`](https://github.com/fibjs/fibjs/commit/a8467666aa)] - **path, refactor**: remove unused variable. (xicilion)
* [[`1a013416fc`](https://github.com/fibjs/fibjs/commit/1a013416fc)] - **deps, refactor**: remove spidermonkey from vc project. (xicilion)
* [[`5ec1a58a80`](https://github.com/fibjs/fibjs/commit/5ec1a58a80)] - **deps, refactor**: remove unused library. (xicilion)
* [[`bb0584f4f3`](https://github.com/fibjs/fibjs/commit/bb0584f4f3)] - **sandbox, break**: deprecate --root and --approot option. (xicilion)
* [[`5fabc2a337`](https://github.com/fibjs/fibjs/commit/5fabc2a337)] - **core, fixbug**: fix build error on windows. (xicilion)
* [[`b8c388d61a`](https://github.com/fibjs/fibjs/commit/b8c388d61a)] - **cmake, feat**: enable ccache. (xicilion)
* [[`9e8ff1d755`](https://github.com/fibjs/fibjs/commit/9e8ff1d755)] - **deps, upgrade**: upgrade to v6.1.344 (xicilion)
* [[`a022bdf5e6`](https://github.com/fibjs/fibjs/commit/a022bdf5e6)] - **test, fixbug**: fix typo. (xicilion)
* [[`2d360557d0`](https://github.com/fibjs/fibjs/commit/2d360557d0)] - **core, refactor**: use internal flag variable to change v8 options. (xicilion)
* [[`93a9df5835`](https://github.com/fibjs/fibjs/commit/93a9df5835)] - **v8, fixbug**: disable icu, fix toLocaleDateString error. (xicilion)
* [[`d843532424`](https://github.com/fibjs/fibjs/commit/d843532424)] - **http, refactor**: change output compression limit to 64M. (xicilion)
* [[`272bd7752e`](https://github.com/fibjs/fibjs/commit/272bd7752e)] - **http, feat**: output fibjs version in http header. (xicilion)
* [[`739d633be7`](https://github.com/fibjs/fibjs/commit/739d633be7)] - **test, feat**: throw the case when network is disconnected. (xicilion)
* [[`91c342449c`](https://github.com/fibjs/fibjs/commit/91c342449c)] - **sandbox, feat**: switch to .js mode when filename does not have ext. (xicilion)
* [[`aa16d9b35f`](https://github.com/fibjs/fibjs/commit/aa16d9b35f)] - **doc, feat**: docs about function source code is not available in .jsc mode. (xicilion)
* [[`86b57a93f8`](https://github.com/fibjs/fibjs/commit/86b57a93f8)] - **fs, refactor**: split fs code (#284) (ngot)
* [[`f8b63086cb`](https://github.com/fibjs/fibjs/commit/f8b63086cb)] - **test, fixbug**: fix hangup in .jsc mode. (xicilion)
* [[`3a989b79b2`](https://github.com/fibjs/fibjs/commit/3a989b79b2)] - **SandBox, feat**: add version mark in .jsc file. (xicilion)
* [[`e74bcaa994`](https://github.com/fibjs/fibjs/commit/e74bcaa994)] - **coroutine, refactor**: use DiscardThreadSpecificMetadata to free thread data in v8. (xicilion)
* [[`cd20493916`](https://github.com/fibjs/fibjs/commit/cd20493916)] - **core, feat**: all Sync alias to  all async apis. (#282) (ngot)
* [[`1b31d1b706`](https://github.com/fibjs/fibjs/commit/1b31d1b706)] - **xml, feat**: support el.getElementsByClassName. (xicilion)
* [[`ca3862497b`](https://github.com/fibjs/fibjs/commit/ca3862497b)] - **xml, refactor**: use get_id to query el.id in el.getElementById. (xicilion)
* [[`01d3156db4`](https://github.com/fibjs/fibjs/commit/01d3156db4)] - **xml, feat**: el.id & el.className test case. (xicilion)
* [[`8089d2e4b0`](https://github.com/fibjs/fibjs/commit/8089d2e4b0)] - **xml, feat**: support el.getElementById. (xicilion)
* [[`3076b629bb`](https://github.com/fibjs/fibjs/commit/3076b629bb)] - **docs, refactor**: format dot output. (xicilion)
* [[`d7643ab285`](https://github.com/fibjs/fibjs/commit/d7643ab285)] - **docs, refactor**: change dot syntax. (xicilion)
* [[`cbc5e4ea9d`](https://github.com/fibjs/fibjs/commit/cbc5e4ea9d)] - **sandbox, feat**: report line number in jsc. (xicilion)
* [[`6a393e1dad`](https://github.com/fibjs/fibjs/commit/6a393e1dad)] - **util, refactor**: split source code. (xicilion)
* [[`24843e4f2e`](https://github.com/fibjs/fibjs/commit/24843e4f2e)] - **idl, refactor**: move idl to idl dir & move en idl to this repo (#281) (ngot)

## 2017-06-26, Version v0.6.1 @ngot

* **bugfix** :
  * fs: fix the fs.read zero.

### Commits

* [[`a34b53c4bb`](https://github.com/fibjs/fibjs/commit/a34b53c4bb)] - **fs, fixbug**: fix the fs.read zero (#277) (ngot) 

## 2017-06-26, Version v0.6.0 @ngot

* **feature** :
  * test: 
    - support async or callback case.
    - support it.only, it.skip, describe.skip, describe.only.
  * util: 
    - util.sync support async/await.
    - util.sync support promise microtask.
    - util.sync support specify async mode function manually.
  * fs: 
    - add fs.open & fs.openSync & fs.close & fs.closeSync to open and close the file descriptor.
    - readFile support encoding.
    - Stat add uid and gid property, add fs test case.
    - add fs.read & fs.readSync to read content from the file descriptor.
  * path: more compatible with nodejs path module.

* **breakchange** :
  * util: util.compile must specify srcname.
  * test: deprecate setup TDD mode.
  * vm: sandbox remove main script mode.
  * module: fix resolve typo from resovle to resolve.

* **bugfix** :
  * module: fix require error when entry of package.json is a folder.
  * fs: fix compile error on windows.
  * io: fix build warning on windows x86.
  * util: util.sync throw error.

* **deps** :
    - upgrade v8 to v6.1.266
    - upgrade sqlite to v3.19.3

### Commits
* [[`68142c7250`](https://github.com/fibjs/fibjs/commit/68142c7250)] - **fs, fixbug**: fix compile error on windows. (xicilion) 
* [[`496a5648c8`](https://github.com/fibjs/fibjs/commit/496a5648c8)] - **fs, feat**: add fs.read to read content from the file descriptor (#274) (ngot) 
* [[`6a00f0f413`](https://github.com/fibjs/fibjs/commit/6a00f0f413)] - **core, refactor**: use header file to define version information. (xicilion) 
* [[`3ce5162f5c`](https://github.com/fibjs/fibjs/commit/3ce5162f5c)] - **test, feat**: support async or callback case. (xicilion) 
* [[`f51019f869`](https://github.com/fibjs/fibjs/commit/f51019f869)] - **internal, downgrade**: downgrade expat. (xicilion) 
* [[`22710a7eff`](https://github.com/fibjs/fibjs/commit/22710a7eff)] - **module, fixbug**: fix require error when entry of package.json is a folder. (xicilion) 
* [[`2c59dd5be7`](https://github.com/fibjs/fibjs/commit/2c59dd5be7)] - **docs, feat**: update util.sync manual. (xicilion) 
* [[`32c151a4b2`](https://github.com/fibjs/fibjs/commit/32c151a4b2)] - **test, refactor**: rewrite wasm test case. (xicilion) 
* [[`419f63ea27`](https://github.com/fibjs/fibjs/commit/419f63ea27)] - **util, test**: util.sync test case. (xicilion) 
* [[`3b47b19bac`](https://github.com/fibjs/fibjs/commit/3b47b19bac)] - **util, feat**: util.sync: support specify async mode function manually. (xicilion) 
* [[`fc429016a8`](https://github.com/fibjs/fibjs/commit/fc429016a8)] - **test, refactor**: rewrite wasm case using util.sync. (xicilion) 
* [[`d31e484a1a`](https://github.com/fibjs/fibjs/commit/d31e484a1a)] - **test, feat**: support it.only, it.skip, describe.skip, describe.only. (xicilion) 
* [[`1856e6eade`](https://github.com/fibjs/fibjs/commit/1856e6eade)] - **internal, upgrade**: upgrade v8&sqlite&expart. (xicilion) 
* [[`7bd0fd3c15`](https://github.com/fibjs/fibjs/commit/7bd0fd3c15)] - **io, fixbug**: fix build warning on windows x86. (xicilion) 
* [[`4e3cf824fa`](https://github.com/fibjs/fibjs/commit/4e3cf824fa)] - **core, bug**: v8 x86 bug test. (xicilion) 
* [[`c7d91f6608`](https://github.com/fibjs/fibjs/commit/c7d91f6608)] - **path, feat**: more compatible with nodejs path module. (xicilion) 
* [[`301164a3ed`](https://github.com/fibjs/fibjs/commit/301164a3ed)] - **util, break**: util.compile must specify srcname. (xicilion) 
* [[`65c727f07b`](https://github.com/fibjs/fibjs/commit/65c727f07b)] - **docs, refactor**: use relative path. (xicilion) 
* [[`363527274b`](https://github.com/fibjs/fibjs/commit/363527274b)] - **test, break**: deprecate setup TDD mode. (xicilion) 
* [[`19c4e4134a`](https://github.com/fibjs/fibjs/commit/19c4e4134a)] - **docs, fixbug**: typo. (xicilion) 
* [[`4cf9687691`](https://github.com/fibjs/fibjs/commit/4cf9687691)] - **fs, fixbug**: fix build error in windows & linux (#272) (ngot) 
* [[`e634d4fe94`](https://github.com/fibjs/fibjs/commit/e634d4fe94)] - **docs, feat**: utils.sync demo. (xicilion) 
* [[`95a87427ba`](https://github.com/fibjs/fibjs/commit/95a87427ba)] - **util, feat**: util.sync: support async/await. (xicilion) 
* [[`c7bf148657`](https://github.com/fibjs/fibjs/commit/c7bf148657)] - **util, test**: test error in callback (xicilion) 
* [[`fa47702585`](https://github.com/fibjs/fibjs/commit/fa47702585)] - **util, fixbug**: util.sync: throw error test. (xicilion) 
* [[`68cdaf838f`](https://github.com/fibjs/fibjs/commit/68cdaf838f)] - **util, feat**: sync support promise microtask. (xicilion) 
* [[`5eb83d5559`](https://github.com/fibjs/fibjs/commit/5eb83d5559)] - **test, feat**: use repl to test main script. (xicilion) 
* [[`dbc3257993`](https://github.com/fibjs/fibjs/commit/dbc3257993)] - **vm, break**: sandbox remove main script mode. (xicilion) 
* [[`c6be2fc750`](https://github.com/fibjs/fibjs/commit/c6be2fc750)] - **docs, feat**: missing docs. (xicilion) 
* [[`0497ee39b4`](https://github.com/fibjs/fibjs/commit/0497ee39b4)] - **docs, feat**: missing docs. (xicilion) 
* [[`1db39afa01`](https://github.com/fibjs/fibjs/commit/1db39afa01)] - **fs, feat**: add fs.open & fs.openSync & fs.close & fs.closeSync to open and close the file descriptor (#271) (ngot) 
* [[`03bc345e07`](https://github.com/fibjs/fibjs/commit/03bc345e07)] - **docs, feat**: auto format javascript code. (xicilion) 
* [[`8acd9a649c`](https://github.com/fibjs/fibjs/commit/8acd9a649c)] - **docs, feat**: fix language. (xicilion) 
* [[`9899967fad`](https://github.com/fibjs/fibjs/commit/9899967fad)] - **fs, feat**: readFile support encoding. (xicilion) 
* [[`ebe3d5e802`](https://github.com/fibjs/fibjs/commit/ebe3d5e802)] - **module, break**: fix resolve typo. (xicilion) 
* [[`7fc08a4a52`](https://github.com/fibjs/fibjs/commit/7fc08a4a52)] - **docs, feat**: use css. (xicilion)
* [[`84f7576d83`](https://github.com/fibjs/fibjs/commit/84f7576d83)] - **docs, feat**: gegerate inherit graph. (xicilion) 
* [[`33388b8be2`](https://github.com/fibjs/fibjs/commit/33388b8be2)] - **idlc, feat**: not generate extend of object. (xicilion) 
* [[`54608ce37b`](https://github.com/fibjs/fibjs/commit/54608ce37b)] - **deps, chore**: set to master (#269) (ngot) 
* [[`b539f109a5`](https://github.com/fibjs/fibjs/commit/b539f109a5)] - **fs, chore**: regenerate Stat.h (#270) (ngot) 
* [[`928bccfe5f`](https://github.com/fibjs/fibjs/commit/928bccfe5f)] - **fs, feat**: Stat add uid and gid property, add fs test case (#268) (asionius) 
* [[`b5ac68161c`](https://github.com/fibjs/fibjs/commit/b5ac68161c)] - **docs, feat**: change style of method descript. (xicilion) 

## 2017-06-21, Version v0.5.0 @ngot

* **feature** :
  * ssl: add ssl.loadRootCerts.
  * fs: File add fd property.
  * util: add isNativeError, isPrimitive, isSymbol, isDataView, isExternal, isMap, isMapIterator, isPromise, isSet, isSetIterator, isTypedArray, isUint8Array.
  * events: add setMaxListeners & getMaxListeners & defaultMaxListers support.
  * core: add -v, --version, -h command line option.
  * SandBox: use snapshot to speed up the creation of context.
  * SandBox: support standalone global.

* **breakchange** :
   * fs: remove the fs.openSync method.
   * fs: rename fs.open to fs.openFile.
   * expect: remove expect module.If you still want to use these module, you can find some similar module on npm.
   * Sandbox: disable --noroot option, enable --root option.The root directly feature has been disabled by default now. You can use --root to enable it.
   * events: more compatible with nodejs events module.

* **bugfix** :
  * zip: fix crash when read a empty file.
  * fs: fix lchmod warning on linux.
  * ssl: fix crash when get stream.
  * zip: fix compile warning.

### Commits

* [[`1171c9376`](https://github.com/fibjs/fibjs/commit/1171c9376)] - **console, feat**:console.add support s%. (xicilion)
* [[`56f836e61b`](https://github.com/fibjs/fibjs/commit/56f836e61b)] - **ci, fixbug**: move fullsrc to darwin (#266) (ngot)
* [[`a5d1ce9d40`](https://github.com/fibjs/fibjs/commit/a5d1ce9d40)] - **ci, fixbug**: fix Permission error (#265) (ngot)
* [[`176969d599`](https://github.com/fibjs/fibjs/commit/176969d599)] - **test, feat**: enable single run directly (#264) (ngot)
* [[`ef7beeb13c`](https://github.com/fibjs/fibjs/commit/ef7beeb13c)] - **test, feat**: enable single run directly (ngot)
* [[`eb92d481ff`](https://github.com/fibjs/fibjs/commit/eb92d481ff)] - **doc, feats**: add type. (xicilion)
* [[`da643dc4a9`](https://github.com/fibjs/fibjs/commit/da643dc4a9)] - **ssl, feat**: add ssl.loadRootCerts. (xicilion)
* [[`af1f95e389`](https://github.com/fibjs/fibjs/commit/af1f95e389)] - **ci, chore**: bring forward the x64 ci (#263) (ngot)
* [[`f48354b22c`](https://github.com/fibjs/fibjs/commit/f48354b22c)] - **ci, chore**: only run x64 test on linux (#262) (ngot)
* [[`31953e5682`](https://github.com/fibjs/fibjs/commit/31953e5682)] - **zip, fixbug**: fix crash when read a empty file. (xicilion)
* [[`1fa88f43bb`](https://github.com/fibjs/fibjs/commit/1fa88f43bb)] - **core, feat**: utils.h check optional parameter. (xicilion)
* [[`f49f184c7d`](https://github.com/fibjs/fibjs/commit/f49f184c7d)] - **core, feat**: util.h ignore extra undefined in parameters. (xicilion)
* [[`46e4e48086`](https://github.com/fibjs/fibjs/commit/46e4e48086)] - **core, fixbug**: fix gen windows installer.exe (#261) (ngot)
* [[`276fc52ef8`](https://github.com/fibjs/fibjs/commit/276fc52ef8)] - **fs, break**: remove the fs.openSync method (#260) (ngot)
* [[`2759406fc6`](https://github.com/fibjs/fibjs/commit/2759406fc6)] - **test, doc**: add test docs (#259) (ngot)
* [[`9ed5946b51`](https://github.com/fibjs/fibjs/commit/9ed5946b51)] - **fs, feat**: File add fd property (#256) (ngot)
* [[`18cad502f0`](https://github.com/fibjs/fibjs/commit/18cad502f0)] - **fs, break**: rename fs.open to fs.openFile (#258) (ngot)
* [[`021560eedd`](https://github.com/fibjs/fibjs/commit/021560eedd)] - **docs, feat**: group static function and property. (xicilion)
* [[`eb04aaed5d`](https://github.com/fibjs/fibjs/commit/eb04aaed5d)] - **doc, chore**: update links (#255) (ngot)
* [[`9d2faf03a8`](https://github.com/fibjs/fibjs/commit/9d2faf03a8)] - **fs, fixbug**: fix lchmod warning on linux. (xicilion)
* [[`f44cfe2f54`](https://github.com/fibjs/fibjs/commit/f44cfe2f54)] - **util, fixbug**: fix warning on windows. (xicilion)
* [[`b5f8cca194`](https://github.com/fibjs/fibjs/commit/b5f8cca194)] - **internal, upgrade**: upgrade v8 to 6.1.201. (xicilion)
* [[`5dd2ca75de`](https://github.com/fibjs/fibjs/commit/5dd2ca75de)] - **ci, chore**: remove then windows ssh release (#253) (ngot)
* [[`40bcea3ee5`](https://github.com/fibjs/fibjs/commit/40bcea3ee5)] - **test, feat**: add i386 test runner (#252) (ngot)
* [[`26e2f98b51`](https://github.com/fibjs/fibjs/commit/26e2f98b51)] - **ci, feat**: add fullsrc.zip release (#250) (ngot)
* [[`f65738cd54`](https://github.com/fibjs/fibjs/commit/f65738cd54)] - **ci, feat**: add osx x86  build & release (#249) (ngot)
* [[`2505a5f753`](https://github.com/fibjs/fibjs/commit/2505a5f753)] - **ci, chore**: remove ssh deploy (#248) (ngot)
* [[`912fe25a2f`](https://github.com/fibjs/fibjs/commit/912fe25a2f)] - **encodin, fixbugg**: fix: hex_decode length error. (xicilion)
* [[`0c25a611a2`](https://github.com/fibjs/fibjs/commit/0c25a611a2)] - **ssl, fixbug**: fix crash when get stream. (xicilion)
* [[`5955c033a4`](https://github.com/fibjs/fibjs/commit/5955c033a4)] - **zip, fixbug**: fix compile warning. (xicilion)
* [[`06a3efbce2`](https://github.com/fibjs/fibjs/commit/06a3efbce2)] - **fs, , fixbug**: fix compile warning on mac. (xicilion)
* [[`5787c34ab0`](https://github.com/fibjs/fibjs/commit/5787c34ab0)] - **docs, fixbug**: fix style. (xicilion)
* [[`ae185dfa9c`](https://github.com/fibjs/fibjs/commit/ae185dfa9c)] - **docs, feat**: insert hr between methods. (xicilion)
* [[`9c9b6ee7ae`](https://github.com/fibjs/fibjs/commit/9c9b6ee7ae)] - **docs, feat**: generate operator method. (xicilion)
* [[`70d39f9fdc`](https://github.com/fibjs/fibjs/commit/70d39f9fdc)] - **docs, fixbug**: fix readme link error. (xicilion)
* [[`cbdc55f95d`](https://github.com/fibjs/fibjs/commit/cbdc55f95d)] - **docs, fixbug**: fix markdown link error. (xicilion)
* [[`ddfe959ceb`](https://github.com/fibjs/fibjs/commit/ddfe959ceb)] - **core, chore**: util.h format code. (xicilion)
* [[`5a645ae1d4`](https://github.com/fibjs/fibjs/commit/5a645ae1d4)] - **docs, feat**: auto link to other module. (xicilion)
* [[`c30ef7a3e7`](https://github.com/fibjs/fibjs/commit/c30ef7a3e7)] - **docs, fixbug**: fix const default value; (xicilion)
* [[`4b5dede844`](https://github.com/fibjs/fibjs/commit/4b5dede844)] - **docs, feat**: not encode message. (xicilion)
* [[`5d8fd217e2`](https://github.com/fibjs/fibjs/commit/5d8fd217e2)] - **docs, feat**: readme (xicilion)
* [[`eca569b8ea`](https://github.com/fibjs/fibjs/commit/eca569b8ea)] - **docs, feat**: check  document  of return. (xicilion)
* [[`7528fed61f`](https://github.com/fibjs/fibjs/commit/7528fed61f)] - **docs, feat**: check the comment. (xicilion)
* [[`cd44aa977d`](https://github.com/fibjs/fibjs/commit/cd44aa977d)] - **docs, feat**: 文档输出修饰, 输出对象 (xicilion)
* [[`9aacc04bad`](https://github.com/fibjs/fibjs/commit/9aacc04bad)] - **docs, fixbug**: fix style. (xicilion)
* [[`1d2f525021`](https://github.com/fibjs/fibjs/commit/1d2f525021)] - **docs, feat**: rewrite with markdown. (xicilion)
* [[`1f7bf9a673`](https://github.com/fibjs/fibjs/commit/1f7bf9a673)] - **docs, feat**: group members of the same name (xicilion)
* [[`da7c05105e`](https://github.com/fibjs/fibjs/commit/da7c05105e)] - **idlc, fixbug**: fix bold style in github. (xicilion)
* [[`eb3ad4699c`](https://github.com/fibjs/fibjs/commit/eb3ad4699c)] - **encoding, revert**: revert the hex modify (#247) (ngot)
* [[`b1da4a35b0`](https://github.com/fibjs/fibjs/commit/b1da4a35b0)] - **core, fixbug**: remove unused files (#246) (ngot)
* [[`b97fa6d7c1`](https://github.com/fibjs/fibjs/commit/b97fa6d7c1)] - **core, refactor**: move vsmake.js to tools dir (#245) (ngot)
* [[`0a30826889`](https://github.com/fibjs/fibjs/commit/0a30826889)] - **idlc, feat**: generate markdown manual. (xicilion)
* [[`7b198566a2`](https://github.com/fibjs/fibjs/commit/7b198566a2)] - **util, feat**: add isNativeError, isPrimitive, isSymbol, isDataView, isExternal, isMap, isMapIterator, isPromise, isSet, isSetIterator, isTypedArray, isUint8Array (#243) (ngot)
* [[`bc58e63620`](https://github.com/fibjs/fibjs/commit/bc58e63620)] - **idlc, feat**: parser doxygen comment. (xicilion)
* [[`5aa4d7a64e`](https://github.com/fibjs/fibjs/commit/5aa4d7a64e)] - **events, feat**: add setMaxListeners & getMaxListeners & defaultMaxListers support (#240) (ngot)
* [[`a88f9f8c67`](https://github.com/fibjs/fibjs/commit/a88f9f8c67)] - **idlc, fixbug**: fix parser error. (xicilion)
* [[`f0941d7f11`](https://github.com/fibjs/fibjs/commit/f0941d7f11)] - **core, feat**: add -v, --version, -h command line option (#241) (ngot)
* [[`8e870a8154`](https://github.com/fibjs/fibjs/commit/8e870a8154)] - **expect, break**: remove expect module (#242) (ngot)
* [[`31f285fe5b`](https://github.com/fibjs/fibjs/commit/31f285fe5b)] - **idlc, reactor**: split code. (xicilion)
* [[`64f775b7bb`](https://github.com/fibjs/fibjs/commit/64f775b7bb)] - **Sandbox, break**: disable root option. (xicilion)
* [[`62bfd01362`](https://github.com/fibjs/fibjs/commit/62bfd01362)] - **event, refactor**: remove ev list directly. (xicilion)
* [[`47369bda81`](https://github.com/fibjs/fibjs/commit/47369bda81)] - **events, break**: more compatible with nodejs events module (#232) (ngot)
* [[`a3e044b2e`](https://github.com/fibjs/fibjs/commit/a3e044b2e)] - **util, fixbug**: typo. (xicilion)
* [[`6b606fa7b`](https://github.com/fibjs/fibjs/commit/6b606fa7b)] - **ci, fixbug**:: windows build error. (xicilion)
* [[`188ba79ae`](https://github.com/fibjs/fibjs/commit/188ba79ae)] - **test, process****: using PATH environment variable viemacs (emacguo)
* [[`9ae359a14`](https://github.com/fibjs/fibjs/commit/9ae359a14)] - **SandBox, feat**: use snapshot to speed up the creation of context. (xicilion)
* [[`1c839c416`](https://github.com/fibjs/fibjs/commit/1c839c416)] - **memorystream, refactor**: flush is always success asionius (asionius)
* [[`d5ed8a8e7`](https://github.com/fibjs/fibjs/commit/d5ed8a8e7)] - **seekableStream, refactor**: move 'truncate' 'eof' and 'flush' interface from File to seekableStream asionius (asionius)
* [[`d9119beec`](https://github.com/fibjs/fibjs/commit/d9119beec)] - **SandBox, refactor**: Enhance the performance of require in standalone global. (xicilion)
* [[`8485dca41`](https://github.com/fibjs/fibjs/commit/8485dca41)] - **SandBox, feat**: support standalone global. (xicilion)
* [[`9a98eb117`](https://github.com/fibjs/fibjs/commit/9a98eb117)] - **test, selfzip**: add jsc test case. (xicilion)
* [[`950e996b1`](https://github.com/fibjs/fibjs/commit/950e996b1)] - **Buffer, break**: Compatible with nodejs hex encode format. (xicilion)

## 2017-06-13, Version v0.4.1 @ngot

* **bugfix** :
  * global: fix require error in repl.
  * process: fix run error when using PATH environment variable on linux.

### Commits

* [[`877fb216e1`](https://github.com/fibjs/fibjs/commit/877fb216e1)] - **global, fixbug**: fix require error in repl. (ngot)
* [[`4f55d674d8`](https://github.com/fibjs/fibjs/commit/4f55d674d8)] - **core, chore**: set the vender branch to 0.4.x (ngot)
* [[`c37c460f94`](https://github.com/fibjs/fibjs/commit/c37c460f94)] - **process, fixbug**: fix run error when using PATH environment variable on linux. (ngot)

## 2017-06-10, Version v0.4.0 @ngot

* **feature** :
  * wasm: Add wasm extension demo.Now you can use WebAssembly to write extention of fibjs!
  * util: format support TypedArray.
  * module:
    - support auto exec selfzip file. Now you can zip your application to an executable file and distribute it easily!
    - add --noroot options to disable application root directory. Application root directory will be removed in 0.5.0, so please pay attention to this.
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
  * SandBox.require adding base argv, set the start query dir
  * package.json support directory.
  * sandbox add --approot to set the run path.
  * remove the sandbox name from the source path
* **mq**:
  * remove mq.jsHandler
* **rpc**:
  * remove rpc module. Please use coroutine.Worker instead.
* **events**:
  * remove Trigger Object, use events.EventEmitter instead.
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
* [[`059e771`](https://github.com/fibjs/fibjs/commit/059e771)] - **http, refactor**: reuse request, avoid too many objects created (xicilion)
* [[`298530f`](https://github.com/fibjs/fibjs/commit/298530f)] - **coroutine, feat**: implement native object passing (xicilion)
* [[`4436f62`](https://github.com/fibjs/fibjs/commit/4436f62)] - **coroutine, feat**: implement Worker message passing (xicilion)
* [[`ddc13b8`](https://github.com/fibjs/fibjs/commit/ddc13b8)] - **coroutine, feat**: implement Worker basic message passing, onopen event. (xicilion)
* [[`326a677`](https://github.com/fibjs/fibjs/commit/326a677)] - **os, feat**: add mac address & mask (ngot)
* [[`0796c51`](https://github.com/fibjs/fibjs/commit/0796c51)] - **mq, remove**: remove mq.await, please use sync instead. (xicilion)
* [[`a1219dc`](https://github.com/fibjs/fibjs/commit/a1219dc)] - **module, refactor**: sandbox.run support different mod, add compile argv, compile different target. (xicilion)
* [[`642ddcb`](https://github.com/fibjs/fibjs/commit/642ddcb)] - **zip, fix**: ZipFile  add entry check (xicilion)
* [[`3773f1a`](https://github.com/fibjs/fibjs/commit/3773f1a)] - **coroutine, feat**: add Worker (xicilion)
* [[`7e9db7d`](https://github.com/fibjs/fibjs/commit/7e9db7d)] - **net, fix**: fix remotePort ntohs remotePort (#200) (Hengfei)
* [[`61bd021`](https://github.com/fibjs/fibjs/commit/61bd021)] - **global, fix**: fix repl error (xicilion)
* [[`c5a4786`](https://github.com/fibjs/fibjs/commit/c5a4786)] - **internal, deps**: upgrade SQLlite (xicilion)
* [[`108a180`](https://github.com/fibjs/fibjs/commit/108a180)] - **internal, deps**: upgrade libgd (xicilion)
* [[`d18f338`](https://github.com/fibjs/fibjs/commit/d18f338)] - **core, remove**: remove ppc arch support. (xicilion)
* [[`a0f78f2`](https://github.com/fibjs/fibjs/commit/a0f78f2)] - **util, feat**: LruCache support modifies timeout when invoked. (xicilion)
* [[`0be9fc3`](https://github.com/fibjs/fibjs/commit/0be9fc3)] - **db, fix**: redis pubsub avoid reentry. (xicilion)
* [[`3911558`](https://github.com/fibjs/fibjs/commit/3911558)] - **ws, refactor**: dalay web socket at background read, avoid miss event. (xicilion)
* [[`9437cfc`](https://github.com/fibjs/fibjs/commit/9437cfc)] - **fs, fix**: fix file.stat on windows (ngot)
* [[`3480a0c`](https://github.com/fibjs/fibjs/commit/3480a0c)] - **ssl, feat**: add SslSocket.stream, to query the socket (xicilion)
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
* [[`26a8c1b`](https://github.com/fibjs/fibjs/commit/26a8c1b)] - **crypto, feat**: modify pbkdf2 , compat nodejs (xicilion)
* [[`b61b9d9`](https://github.com/fibjs/fibjs/commit/b61b9d9)] - **http, feat**: Add Message.end to terminate chain transaction. (xicilion)
* [[`232657c`](https://github.com/fibjs/fibjs/commit/232657c)] - **http, refactor**: reuse Message.response, refactor httpHandler creates new instance by every request. (xicilion)
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
* [[`267defa`](https://github.com/fibjs/fibjs/commit/267defa)] - **ws, feat**: add ws.upgrade, implement the Event-Driver ws server. (xicilion)
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
* [[`b76d1a9`](https://github.com/fibjs/fibjs/commit/b76d1a9)] - **events, remove**: remove Trigger Object, use EventEmitter instead. (xicilion)
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
* [[`8134e68`](https://github.com/fibjs/fibjs/commit/8134e68)] - **db, fix**: mongodb change mongodb m_conn struct, avoid out of bounds errors. (xicilion)
* [[`1584c81`](https://github.com/fibjs/fibjs/commit/1584c81)] - **core, refactor**: exit wait all the async invoke & timer. (xicilion)
* [[`b3023ca`](https://github.com/fibjs/fibjs/commit/b3023ca)] - **coroutine, refactor**: coroutine.sleep use async way. (xicilion)
* [[`7c9e1b7`](https://github.com/fibjs/fibjs/commit/7c9e1b7)] - **core, refactor**: exit before all the fibers exit. (xicilion)
* [[`88e43a5`](https://github.com/fibjs/fibjs/commit/88e43a5)] - **module, fix**: fix .js dir can't be loaded error. (xicilion)
* [[`0e86ed9`](https://github.com/fibjs/fibjs/commit/0e86ed9)] - **console, refactor**: change console flush policy. (xicilion)
* [[`8b76da5`](https://github.com/fibjs/fibjs/commit/8b76da5)] - **fs, refactor**: POSIX init umask to 0 (xicilion)
* [[`93bf249`](https://github.com/fibjs/fibjs/commit/93bf249)] - **fs, feat**: more Sync API. (xicilion)
* [[`0b094f0`](https://github.com/fibjs/fibjs/commit/0b094f0)] - **fs, refactor**: change windows readdir. (xicilion)
* [[`013ac91`](https://github.com/fibjs/fibjs/commit/013ac91)] - **fs, refactor**: change the fs.readdir result, add readdirSync. (xicilion)
* [[`edcc5ca`](https://github.com/fibjs/fibjs/commit/edcc5ca)] - **collection, feat**: List support sort (xicilion)
* [[`d332d58`](https://github.com/fibjs/fibjs/commit/d332d58)] - **collection, feat**: List support callback args (xicilion)
* [[`95663b8`](https://github.com/fibjs/fibjs/commit/95663b8)] - **url, refactor**: Url property can be modified. (xicilion)
* [[`722fff1`](https://github.com/fibjs/fibjs/commit/722fff1)] - **db, refactor**: SQLite use the Object async lock instead of it's custom lock. (xicilion)
* [[`5f951c1`](https://github.com/fibjs/fibjs/commit/5f951c1)] - **core, feat**: callback supports async lock. (xicilion)
* [[`a48fada`](https://github.com/fibjs/fibjs/commit/a48fada)] - **global, refactor**: sync throw error to outside. (xicilion)
* [[`66b0e0b`](https://github.com/fibjs/fibjs/commit/66b0e0b)] - **db, fix**: SQLite execute add lock avoid a crash. (xicilion)
* [[`72a058f`](https://github.com/fibjs/fibjs/commit/72a058f)] - **db, refactor**: DBResult fields, insertId, affected return the default value without throw error. (xicilion)
* [[`d55ae30`](https://github.com/fibjs/fibjs/commit/d55ae30)] - **module, feat**: SandBox.require adding base argv, set the start query dir. (xicilion)
* [[`7f72bea`](https://github.com/fibjs/fibjs/commit/7f72bea)] - **process, feat**: add process.nextTick (xicilion)
* [[`8909cdc`](https://github.com/fibjs/fibjs/commit/8909cdc)] - **global, fix**: fix sync when err is null the mistake throw. (xicilion)
* [[`6999331`](https://github.com/fibjs/fibjs/commit/6999331)] - **url, feat**: url.parse add parseQueryString,  (xicilion)
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
* [[`cb25286`](https://github.com/fibjs/fibjs/commit/cb25286)] - **core, fix**: release callback function, avoid memory leak. (xicilion)
* [[`750cdf0`](https://github.com/fibjs/fibjs/commit/750cdf0)] - **io, feat**: add io.bridge for stream forward both direction. (xicilion)
* [[`059bab9`](https://github.com/fibjs/fibjs/commit/059bab9)] - **io, feat**: add io.copyStream (xicilion)
* [[`b4e95fe`](https://github.com/fibjs/fibjs/commit/b4e95fe)] - **http, feat**: httpClient add default userAgent. (xicilion)
* [[`7d61d13`](https://github.com/fibjs/fibjs/commit/7d61d13)] - **url: Add**:url module. (xicilion)
* [[`50a5d35`](https://github.com/fibjs/fibjs/commit/50a5d35)] - **net, refactor**: rename net.Url to net.UrlObject (xicilion)
* [[`18591b7`](https://github.com/fibjs/fibjs/commit/18591b7)] - **net, feat**: Socket support UDP. (xicilion)
* [[`7122959`](https://github.com/fibjs/fibjs/commit/7122959)] - **global, feat**: add global, compat Node.js (xicilion)
* [[`7fe69e6`](https://github.com/fibjs/fibjs/commit/7fe69e6)] - **db, feat**: SQLite add SQLITE_BUSY support. (xicilion)
* [[`cbde0fb`](https://github.com/fibjs/fibjs/commit/cbde0fb)] - **db, fix**: sqlite prepare retry, avoid error. (xicilion)
* [[`6f3e634`](https://github.com/fibjs/fibjs/commit/6f3e634)] - **coroutine, feat**: parallel add a new mode. (xicilion)
* [[`30b8a87`](https://github.com/fibjs/fibjs/commit/30b8a87)] - **mq, refactor**: adjust Routing match sequeue, add put method. (xicilion)
* [[`5e22575`](https://github.com/fibjs/fibjs/commit/5e22575)] - **http, fix**: fix httphandler zip error. (xicilion)
* [[`9fd0ce4`](https://github.com/fibjs/fibjs/commit/9fd0ce4)] - **gui, feat**: webview support DPI. (xicilion)
* [[`1ef9a62`](https://github.com/fibjs/fibjs/commit/1ef9a62)] - **gui, feat**: webview on HD display auto scale. (xicilion)
* [[`05c9357`](https://github.com/fibjs/fibjs/commit/05c9357)] - **module: support**:node_modules (xicilion)
* [[`27d092c`](https://github.com/fibjs/fibjs/commit/27d092c)] - **module, fix**: fix when require folder/index error can't be throw error. (xicilion)
* [[`4b5b358`](https://github.com/fibjs/fibjs/commit/4b5b358)] - **core fix**: fix socket error canceled on Window XP. (xicilion)
* [[`43832a7`](https://github.com/fibjs/fibjs/commit/43832a7)] - **fs, feat**: fs.open cache the zip file, speed require. (xicilion)
* [[`d6c4b71`](https://github.com/fibjs/fibjs/commit/d6c4b71)] - **fs, fix**: fix fs.open zip logic error on windows. (xicilion)
* [[`e1a0fdf`](https://github.com/fibjs/fibjs/commit/e1a0fdf)] - **encodeing, fix**: fix utf16 encoding error. (xicilion)
* [[`c30506c`](https://github.com/fibjs/fibjs/commit/c30506c)] - **gui, feat**: webview handle PARSE_SECURITY_DOMAIN, support xmlhttp make a request. (xicilion)
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
* [[`e2b3f4b`](https://github.com/fibjs/fibjs/commit/e2b3f4b)] - **gui, feat**: add gui mode, add windows mode entry. (xicilion)
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
* [[`c0f17ec`](https://github.com/fibjs/fibjs/commit/c0f17ec)] - **console, feat**: add width, height for query or set the console size. (xicilion)
* [[`94cecb3`](https://github.com/fibjs/fibjs/commit/94cecb3)] - **http, feat**: add httpClient (asionius)
* [[`f259bcc`](https://github.com/fibjs/fibjs/commit/f259bcc)] - **core, feat**: support mips arch (xicilion)
* [[`f94b0ee`](https://github.com/fibjs/fibjs/commit/f94b0ee)] - **Socket, feat**: socket add timeout (xicilion)
* [[`ef9577d`](https://github.com/fibjs/fibjs/commit/ef9577d)] - **internal, deps**: upgrade gd (xicilion)
