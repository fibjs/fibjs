# 如何写文档

* IDL 是文档的唯一数据源：`idl/*.idl`（模块与内置对象），**注释一律英文** —— `fibjs --man` 直接以它为手册；
* 改完运行 `bin/<dist>/fibjs tools/idlc.js` 重新生成 `fibjs/include/ifs/**`、`npm/types/dts/**`
  与 `fibjs/scripts/internal/fibjs-types.js`（与 IDL 同批提交）；
* 站点手册：将 fibjs_docs clone 到 work/docs/，运行 `bin/<dist>/fibjs tools/docs.js`；
  markdown 输出在 work/docs/docs/manual 下，html 输出在 work/docs/web/dist/docs/manual 下。