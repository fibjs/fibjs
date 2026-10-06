# 如何写文档

* IDL 是文档的唯一数据源：`idl/*.idl`（模块与内置对象），**注释一律英文** —— `fibjs --man` 直接以它为手册；
* 改完运行 `bin/<dist>/fibjs tools/idlc.js` 重新生成 `fibjs/include/ifs/**`、`npm/types/dts/**`
  与 `fibjs/scripts/internal/fibjs-types.js`（与 IDL 同批提交）；
* 站点手册：将 fibjs_docs clone 到 work/docs/，运行 `bin/<dist>/fibjs tools/docs.js`；
  markdown 输出在 work/docs/docs/manual 下，html 输出在 work/docs/web/dist/docs/manual 下。

## IDL 文档注释规范
规范细节见 `plans/idl-doc-completion-plan-2026-10-05.md` §5，要点如下。

**通用要求**

* 注释全英文（含示例代码里的注释）；摘要行不加句号、首字母大写、不重复类型；
* parser 只识别 `@param` / `@return`，不要写 `@throws` / `@example` 等自定义 tag（会原样进入正文）；围栏统一写 `JavaScript`，行宽 ≤ 100；
* 技术概念（fiber、keep-alive、CORS、AEAD…）只在模块/类级用 `Concepts:` 一次讲清，成员注释最多一行 `see ...` 指针；
* 抽象基类（`Stream`、`HttpMessage`、`XmlNode`…，清单见计划 §5.6）的成员示例必须用具体派生类
  （如 `new io.MemoryStream()`、`new http.Request()`）验证后写回，示例中禁止 `new <基类>`。

**模块模板**（示例集 ≥ 2 个；成员数 ≥ 30 的模块 ≥ 3 个；每个示例独立自包含、一句话说明场景）

````JavaScript
/*! @brief <one-line positioning: what the module provides and for whom>
 <2-5 lines expanding the capability groups and the programming model>
 Concepts:
 <the technical concepts of this module, explained once here>

 Import: use `require('fs')`; globals such as `Buffer` need no import.

 Example 1 — <scenario in one sentence>:
 ```JavaScript
 <self-contained runnable example>
 ```
 Example 2 — <scenario in one sentence>:
 ```JavaScript
 <second runnable scenario>
 ```
 */
````

**类模板**（无公开构造函数时必须用 `Obtained from:` 列出工厂/入口；普通类示例 ≥ 2 个，成员数 ≥ 20 的类 ≥ 3 个）

````JavaScript
/*! @brief <one-line positioning: when to use this class>
 <role, and the split of work with its base class>
 Concepts:
 <terms/protocols of this class>
 Obtained from:
 - `new <Class>(...)` — <constructor form>;
 - `<module>.<factory>()` — <entry returning this object>.

 Example 1 — <scenario in one sentence>:
 ```JavaScript
 <self-contained runnable example>
 ```
 */
````

**成员模板**（重载差异必须写进第一个重载：`--man` 只显示第一个带文档的重载）

````JavaScript
/*! @brief <what it does; side effects and return shape>
 <differences and boundaries: options items, error behavior, platform
 differences, Node/MDN comparison>
 options supports the following options:
 ```JavaScript
 { "timeout": 0, "signal": null }   // item defaults / effect conditions
 ```

 @param <name> <semantics, without the type>
 @return <return shape, without the type>
 */
````

**示例标记**（写在代码块第一行；无标记 = 可运行）

| 标记 | 含义 | 检查器行为 |
| --- | --- | --- |
| 无标记 | 完整可运行示例 | 在独立临时目录执行，退出码 0 才算通过 |
| `// fragment: <reason>` | 说明性片段（options 清单、伪代码） | 只做语法检查 |
| `// requires: <service>` | 需要环境：`redis` / `mysql` / `sqlite` / `network` / `windows` / `long-running` | 默认跳过，指定 `--requires` 时执行 |

**命令**

```sh
# 零容忍检查指定文件（改动 IDL 后必跑）
node tools/util/check_idl_docs.js --files idl/fs.idl
# 只检查 git 改动涉及的定义（tools/idlc.js 生成前已自动调用）
node tools/util/check_idl_docs.js --changed
# 全量比对 baseline（棘轮，只允许下降）
node tools/util/check_idl_docs.js
# 仅在有意重设基线时使用
node tools/util/check_idl_docs.js --update
# 运行某个定义的全部示例（--json 写 temp/idl_examples_report.json）
node tools/check_idl_examples.js --filter fs --json
```

改完必须执行 `bin/<dist>/fibjs tools/idlc.js` 重新生成 `npm/types/dts/**` 与
`fibjs/scripts/internal/fibjs-types.js`，并与 IDL 同批提交（纯注释改动不影响 `fibjs/include/ifs/**`）。