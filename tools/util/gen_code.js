var fs = require("fs");
var util = require("util");
var path = require('path');

module.exports = function (defs, baseFolder) {
    function gen_code(cls, def) {
        var typeMap = {
            "Integer": "int32_t",
            "Long": "int64_t",
            "Number": "double",
            "Boolean": "bool",
            "String": "exlib::string",
            "Date": "date_t",
            "Object": "v8::Local<v8::Object>",
            "Iterator": "v8::Local<v8::Object>",
            "Array": "v8::Local<v8::Array>",
            "TypedArray": "v8::Local<v8::TypedArray>",
            "ArrayBuffer": "v8::Local<v8::ArrayBuffer>",
            "ArrayBufferView": "v8::Local<v8::ArrayBufferView>",
            "Function": "v8::Local<v8::Function>",
            "Value": "v8::Local<v8::Value>",
            "Variant": "Variant",
            "NObject": "obj_ptr<NObject>",
            "NArray": "obj_ptr<NArray>",
            "...": "OptArgs"
        };

        var txts = [];
        var refers = [];
        var hasNew = false;
        var staticCallAsFunc = false;
        var callAsFunc = false;
        var fnIndexed = null;
        var fnNamed = null;

        function get_type(t) {
            return typeMap[t] || (t + "_base*");
        }

        function get_rtype(t) {
            if (Array.isArray(t))
                return "obj_ptr<" + t.name + ">";
            return typeMap[t] || ("obj_ptr<" + t + "_base>");
        }

        function get_name(n) {
            var ckws = {
                "union": true,
                "and": true,
                "or": true,
                "xor": true,
                "new": true,
                "assert": true,
                "delete": true
            };

            return ckws.hasOwnProperty(n) ? "_" + n : n;
        }

        var member_gens = {
            "method": {
                "declare": fns => {
                    fns.overs.forEach(ov => {
                        var fns = "    ";
                        var fname = get_name(ov.name);
                        var fstatic = ov.static;
                        var is_new = false;

                        if (fname == def.declare.name && !fstatic && !ov.type) {
                            fstatic = true;
                            fname = "_new";
                            is_new = true;
                            hasNew = true;
                        } else if (fname == "Function") {
                            fname = "_function";
                            callAsFunc = true;
                            staticCallAsFunc = fstatic;
                        }

                        fns += fstatic ? "static " : "virtual ";
                        fns += "result_t ";
                        fns += fname;
                        fns += "(";

                        var ps = [];

                        if (ov.params)
                            ov.params.forEach(p => {
                                if (p.name == "...")
                                    ps.push("const v8::FunctionCallbackInfo<v8::Value>& args");
                                else ps.push(get_type(p.type) + " " + p.name);
                            });

                        if (ov.type)
                            ps.push(get_rtype(ov.type) + "& retVal");

                        if (is_new) {
                            ps.push(get_rtype(cls) + "& retVal");
                            ps.push("v8::Local<v8::Object> This = v8::Local<v8::Object>()");
                        }

                        if (ov.async)
                            ps.push("AsyncEvent* ac");

                        fns += ps.join(", ");

                        fns += fstatic ? ");" : ") = 0;";

                        txts.push(fns);
                    });
                },
                "stub": fn => {
                    var fname = fn.name;

                    if (fname == def.declare.name && !fn.static && !fn.type)
                        fname = "_new";
                    else if (fname == "Function")
                        fname = "_function";

                    txts.push("    static void s_" + fname + "(const v8::FunctionCallbackInfo<v8::Value>& args);");
                },
                "stub_func": fn => {
                    var fname = fn.name;
                    var fstatic = fn.static;
                    var ftype = fn.type;

                    if (fname == 'Function')
                        fname = '_function';
                    else if (fname == def.declare.name && !fstatic && !ftype) {
                        fname = "_new";
                        fstatic = true;
                        ftype = def.declare.name;
                    }

                    txts.push('inline void ' + cls + '_base::s_' + fname + '(const v8::FunctionCallbackInfo<v8::Value>& args)\n{');

                    if (fname == '_new') {
                        txts.push('    CONSTRUCT_INIT();\n    __new(args);\n}\n');
                        txts.push('template <typename T>\nvoid ' + cls + '_base::__new(const T& args)\n{');
                    }

                    if (ftype)
                        txts.push('    ' + get_rtype(ftype) + ' vr;\n');

                    if (!fstatic)
                        txts.push(`    METHOD_INSTANCE(${cls}_base);`);

                    if (fname == '_new')
                        txts.push(`    CONSTRUCT_ENTER("new ${cls}()");\n`);
                    else if (fname == '_function')
                        txts.push(`    METHOD_ENTER("${cls}");\n`);
                    else
                        txts.push(`    METHOD_ENTER("${cls}.${fname}");\n`);

                    fn.overs.forEach(ov => {
                        var argc = 0;
                        var opts = 0;
                        var params = [];
                        var args = [];
                        var args_call;

                        if (ov.params) {
                            argc = opts = ov.params.length;
                            ov.params.forEach(p => {
                                args.push('v' + params.length);
                                if (p.name == '...') {
                                    opts--;
                                    argc = -1;
                                    args[args.length - 1] = 'args';
                                } else if (p.type == '...') {
                                    opts--;
                                    argc = -1;
                                    params.push('    ARG_LIST(' + params.length + ');');
                                } else if (p.default) {
                                    var defValue;
                                    opts--;
                                    if (p.default.value)
                                        defValue = p.default.value;
                                    else if (util.isArray(p.default.const))
                                        defValue = p.default.const[0] + '_base::_' + p.default.const[1];
                                    else
                                        defValue = '_' + p.default.const;

                                    params.push('    OPT_ARG(' + get_rtype(p.type) + ', ' + params.length + ', ' + defValue + ');');
                                } else {
                                    if (fname == '_new' && params.length == 0 && ov.params.length == 1 && p.type == ftype)
                                        params.push('    STRICT_ARG(' + get_rtype(p.type) + ', ' + params.length + ');');
                                    else
                                        params.push('    ARG(' + get_rtype(p.type) + ', ' + params.length + ');');
                                }
                            });
                        }

                        txts.push('    ' + (ov.async ? 'ASYNC_' : '') + 'METHOD_OVER(' + argc + ', ' + opts + ');\n');
                        if (params.length)
                            txts.push(params.join('\n') + '\n');

                        args_call = args.slice(0);
                        if (ftype)
                            args_call.push('vr');

                        if (ov.deprecated)
                            txts.push('    DEPRECATED_SOON("' + cls + '.' + fname + '");\n');

                        if (ov.async) {
                            args.push('cb');
                            txts.push('    if (!cb.IsEmpty()) {\n        ' + (fstatic ? 'acb_' : 'pInst->acb_') + get_name(fname) + '(' + args.join(', ') + ');');
                            txts.push('        hr = CALL_RETURN_NULL;\n    } else');
                            txts.push('        hr = ' + (fstatic ? 'ac_' : 'pInst->ac_') + get_name(fname) + '(' + args_call.join(', ') + ');\n');
                        } else {
                            if (fname == '_new')
                                args_call.push('args.This()');
                            txts.push('    hr = ' + (fstatic ? '' : 'pInst->') + get_name(fname) + '(' + args_call.join(', ') + ');\n');
                        }
                    });

                    if (ftype) {
                        if (fname == '_new')
                            txts.push('    CONSTRUCT_RETURN();\n}\n');
                        else
                            txts.push('    METHOD_RETURN();\n}\n');
                    } else
                        txts.push('    METHOD_VOID();\n}\n');
                }
            },
            "prop": {
                "declare": fn => {
                    var fname = fn.name;
                    var fstatic = fn.static;

                    if (fname) {
                        var fns = "    ";

                        fns += fstatic ? "static " : "virtual ";
                        fns += "result_t get_";
                        fns += fname;
                        fns += "(";

                        fns += get_rtype(fn.type) + "& retVal";

                        fns += fstatic ? ");" : ") = 0;";

                        txts.push(fns);

                        if (!fn.readonly) {
                            fns = "    ";

                            fns += fstatic ? "static " : "virtual ";
                            fns += "result_t set_";
                            fns += fname;
                            fns += "(";

                            fns += get_type(fn.type) + " newVal";

                            fns += fstatic ? ");" : ") = 0;";

                            txts.push(fns);

                        }
                    }
                },
                "stub": fn => {
                    var fname = fn.name;
                    var fstatic = fn.static;

                    if (fname) {
                        txts.push("    static void s_get_" + fname + "(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);");
                        if (!fn.readonly)
                            txts.push("    static void s_set_" + fname + "(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);");
                    }
                },
                "stub_func": fn => {
                    var fname = fn.name;
                    var fstatic = fn.static;

                    txts.push('inline void ' + cls + '_base::s_get_' + fname + '(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)\n{\n    ' + get_rtype(fn.type) + ' vr;\n');
                    if (!fstatic)
                        txts.push(`    METHOD_INSTANCE(${cls}_base);`);
                    txts.push(`    PROPERTY_ENTER("${cls}.${fname}");\n`);

                    if (fn.deprecated)
                        txts.push('    DEPRECATED_SOON("' + cls + '.' + fname + '");\n');

                    if (fstatic)
                        txts.push('    hr = get_' + fname + '(vr);\n');
                    else
                        txts.push('    hr = pInst->get_' + fname + '(vr);\n');
                    txts.push('    METHOD_RETURN();\n}\n');

                    if (!fn.readonly) {
                        txts.push('inline void ' + cls + '_base::s_set_' + fname + '(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)\n{');
                        if (!fstatic)
                            txts.push(`    METHOD_INSTANCE(${cls}_base);`);
                        txts.push(`    PROPERTY_ENTER("${cls}.${fname}");\n    PROPERTY_VAL(${get_rtype(fn.type)});\n`);

                        if (fn.deprecated)
                            txts.push('    DEPRECATED_SOON("' + cls + '.' + fname + '");\n');

                        if (fstatic)
                            txts.push('    hr = set_' + fname + '(v0);\n');
                        else
                            txts.push('    hr = pInst->set_' + fname + '(v0);\n');
                        txts.push('    PROPERTY_SET_LEAVE();\n}\n');
                    }

                }
            },
            "object": {
                "declare": () => {},
                "stub": () => {},
                "stub_func": () => {}
            },
            "const": {
                "declare": () => {},
                "stub": fn => {
                    var fname = fn.name;
                    txts.push("    static void s_get_" + fname + "(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);");
                },
                "stub_func": fn => {
                    var fname = fn.name;
                    txts.push('inline void ' + cls + '_base::s_get_' + fname + '(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)\n{');
                    if (fn.deprecated)
                        txts.push('    DEPRECATED_SOON("' + cls + '.' + fname + '");\n');
                    txts.push(`    int32_t vr = _${fname};\n    PROPERTY_ENTER("${cls}.${fname}");\n    METHOD_RETURN();\n}\n`);
                }
            },
            "operator": {
                "declare": fn => {
                    if (fn.index) {
                        fnNamed = fn;
                        txts.push("    virtual result_t _named_getter(exlib::string property, " + get_rtype(fn.type) + "& retVal) = 0;");
                        txts.push("    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal) = 0;");
                        if (!fn.readonly) {
                            txts.push("    virtual result_t _named_setter(exlib::string property, " + get_type(fn.type) + " newVal) = 0;");
                            txts.push("    virtual result_t _named_deleter(exlib::string property, v8::Local<v8::Boolean>& retVal) = 0;");
                        }
                    } else {
                        fnIndexed = fn;
                        txts.push("    virtual result_t _indexed_getter(uint32_t index, " + get_rtype(fn.type) + "& retVal) = 0;");
                        if (!fn.readonly) {
                            txts.push("    virtual result_t _indexed_setter(uint32_t index, " + get_type(fn.type) + " newVal) = 0;");
                        }
                    }
                },
                "stub": fn => {
                    if (fn.index) {
                        txts.push("    static void i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);");
                        txts.push("    static void i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args);");
                        if (!fn.readonly) {
                            txts.push("    static void i_NamedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args);");
                            txts.push("    static void i_NamedDeleter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Boolean>& args);");
                        }
                    } else {
                        txts.push("    static void i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args);");
                        if (!fn.readonly)
                            txts.push("    static void i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args);");
                    }
                },
                "stub_func": fn => {
                    if (fn.index) {
                        txts.push('inline void ' + cls + '_base::i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)\n{\n    ' + get_rtype(fn.type) + ' vr;\n');
                        txts.push(`    METHOD_INSTANCE(${cls}_base);\n    PROPERTY_ENTER("${cls}.${fname}[]");\n`);
                        txts.push('    exlib::string k;\n    GetArgumentValue(isolate, property, k);\n    if (class_info().has(k.c_str()))\n        return;\n');
                        if (fn.deprecated)
                            txts.push('    DEPRECATED_SOON("' + cls + fn.name + '");\n');
                        txts.push('    hr = pInst->_named_getter(k, vr);\n    if (hr == CALL_RETURN_NULL)\n        return;\n');
                        txts.push('    METHOD_RETURN();\n}\n');
                        txts.push('inline void ' + cls + '_base::i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args)\n{\n    v8::Local<v8::Array> vr;\n');
                        txts.push(`    METHOD_INSTANCE(${cls}_base);\n    PROPERTY_ENTER("${cls}.${fname}[]");\n`);
                        txts.push('    hr = pInst->_named_enumerator(vr);\n');
                        txts.push('    METHOD_RETURN1();\n}\n');

                        if (!fn.readonly) {
                            txts.push('inline void ' + cls + '_base::i_NamedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args)\n{');
                            txts.push(`    METHOD_INSTANCE(${cls}_base);\n    PROPERTY_ENTER("${cls}.${fname}[]");\n`);
                            txts.push('    PROPERTY_VAL(' + get_rtype(fn.type) + ');\n    exlib::string k;\n    GetArgumentValue(isolate, property, k);\n    if (class_info().has(k.c_str()))\n        return;\n');
                            if (fn.deprecated)
                                txts.push('    DEPRECATED_SOON("' + cls + fn.name + '");\n');
                            txts.push('    hr = pInst->_named_setter(k, v0);\n    if (hr == CALL_RETURN_NULL)\n        return;\n');
                            txts.push('    METHOD_VOID();\n}\n');
                            txts.push('inline void ' + cls + '_base::i_NamedDeleter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Boolean>& args)\n{\n    v8::Local<v8::Boolean> vr;\n');
                            txts.push(`    METHOD_INSTANCE(${cls}_base);\n    PROPERTY_ENTER("${cls}.${fname}[]");\n`);
                            txts.push('    exlib::string k;\n    GetArgumentValue(isolate, property, k);\n    if (class_info().has(k.c_str())) {\n        args.GetReturnValue().Set(v8::False(isolate));\n        return;\n    }\n');
                            txts.push('    hr = pInst->_named_deleter(k, vr);\n    METHOD_RETURN1();\n}\n');
                        }
                    } else {
                        txts.push('inline void ' + cls + '_base::i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args)\n{');
                        txts.push('    ' + get_rtype(fn.type) + ' vr;\n');
                        txts.push(`    METHOD_INSTANCE(${cls}_base);\n    PROPERTY_ENTER("${cls}.${fname}[]");\n`);
                        if (fn.deprecated)
                            txts.push('    DEPRECATED_SOON("' + cls + fn.name + '");\n');
                        txts.push('    hr = pInst->_indexed_getter(index, vr);\n    if (hr == CALL_RETURN_NULL)\n        return;\n');
                        txts.push('    METHOD_RETURN();\n}\n');

                        if (!fn.readonly) {
                            txts.push('inline void ' + cls + '_base::i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args)\n{');
                            txts.push(`    METHOD_INSTANCE(${cls}_base);\n    PROPERTY_ENTER("${cls}.${fname}[]");\n`);
                            if (fn.deprecated)
                                txts.push('    DEPRECATED_SOON("' + cls + fn.name + '");\n');
                            txts.push('    PROPERTY_VAL(' + get_rtype(fn.type) + ');\n    hr = pInst->_indexed_setter(index, v0);\n');
                            txts.push('    METHOD_VOID();\n}\n');
                        }
                    }
                }
            }
        };

        function gen_begin() {
            txts.push(`/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/
`);

            txts.push("#ifndef _" + cls + "_base_H_");
            txts.push("#define _" + cls + "_base_H_\n");

            txts.push("/**\n @author Leo Hoo <lion@9465.net>\n */\n");

            txts.push('#include "../object.h"');
            if (def.declare.extend && def.declare.extend !== 'object')
                txts.push('#include "' + def.declare.extend + '.h"');

            txts.push('\nnamespace fibjs {\n');
        }

        function gen_end() {
            txts.push("}\n\n#endif\n");
        }

        function gen_refer_cls() {
            var has_refer = false;

            if (def.declare.extend && def.declare.extend !== 'object') {
                txts.push('class ' + def.declare.extend + '_base;');
                has_refer = true;
            }

            refers.forEach(c => {
                txts.push('class ' + c + '_base;');
                has_refer = true;
            });

            if (has_refer)
                txts.push("");
        }

        function gen_cls() {
            function gen_cls_declare() {
                if (def.declare.extend)
                    txts.push("class " + cls + "_base : public " + def.declare.extend + "_base {");
                else
                    txts.push("class " + cls + "_base {");
                txts.push("    DECLARE_CLASS(" + cls + "_base);");
            }

            function gen_cls_consts() {
                var consts = [];

                def.members.forEach(fn => {
                    if (fn.memType === "const")
                        consts.push("        _" + fn.name + " = " + fn.default.value);
                });

                if (consts.length) {
                    txts.push("\npublic:\n    enum {");
                    txts.push(consts.join(",\n"));
                    txts.push("    };");
                }
            }

            function gen_cls_retTypes() {
                var types = {};
                var bHasTypes = false;

                def.members.forEach(fn => {
                    if (Array.isArray(fn.type)) {
                        bHasTypes = true;

                        var ts = [];

                        var name = fn.name;
                        name = name.substr(0, 1).toUpperCase() + name.substr(1) + "Type";
                        fn.type.name = name;

                        ts.push('    class ' + name + ' : public NType {');

                        ts.push('    public:');
                        ts.push('        virtual void fillMembers(Isolate* isolate, v8::Local<v8::Object>& retVal)');
                        ts.push('        {');

                        fn.type.forEach(t => {
                            ts.push('            retVal->Set(isolate->NewString("' + t.name + '"), GetReturnValue(isolate->m_isolate, ' + t.name + '));');
                        });

                        ts.push('        }\n');

                        ts.push('        virtual void fillArguments(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)');
                        ts.push('        {');

                        fn.type.forEach(t => {
                            ts.push('            args.push_back(GetReturnValue(isolate->m_isolate, ' + t.name + '));');
                        });

                        ts.push('        }\n');

                        ts.push('    public:');
                        fn.type.forEach(t => {
                            ts.push('        ' + get_rtype(t.type) + ' ' + t.name + ';');
                        });

                        ts.push('    };');

                        types[name] = ts.join('\n');
                    }
                });

                if (bHasTypes) {
                    txts.push("\npublic:");
                    txts.push(Object.values(types).join("\n"));
                }
            }

            function gen_cls_members() {
                txts.push("\npublic:\n    // " + cls + "_base");
                var l = txts.length;

                def.members.forEach(fn => member_gens[fn.memType].declare(fn));

                if (l == txts.length)
                    txts.pop();
            }

            function gen_cls_new() {
                txts.push("");
                if (hasNew)
                    txts.push("public:\n    template <typename T>\n    static void __new(const T& args);\n");
                else if (staticCallAsFunc)
                    txts.push("public:\n    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)\n    {\n" +
                        "        CONSTRUCT_INIT();\n        s__function(args);\n    }\n");
                else
                    txts.push("public:\n    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)\n    {\n" +
                        "        CONSTRUCT_INIT();\n\n" +
                        "        Isolate* isolate = Isolate::current();\n\n" +
                        "        isolate->m_isolate->ThrowException(\n" +
                        "            isolate->NewString(\"not a constructor\"));\n    }\n");
            }

            function gen_cls_member_stubs() {
                txts.push("public:");
                var l = txts.length;

                def.members.forEach(fn => member_gens[fn.memType].stub(fn));

                if (l == txts.length)
                    txts.pop();
            }

            function gen_cls_async_stubs() {
                txts.push("\npublic:");
                var l = txts.length;

                def.members.forEach(fn => {
                    if (fn.memType == "method") {
                        fn.overs.forEach(ov => {
                            if (ov.memType == "method" && ov.async) {
                                var fns = "    ASYNC_";
                                var pn = 0;
                                var ps = [cls + "_base", get_name(ov.name)];

                                if (ov.params) {
                                    pn = ov.params.length;
                                    ov.params.forEach(p => ps.push(get_type(p.type)));
                                }

                                fns += (ov.static ? "STATIC" : "MEMBER");

                                if (ov.type) {
                                    pn++;
                                    ps.push(get_rtype(ov.type));
                                    fns += "VALUE";
                                }

                                fns += pn + "(" + ps.join(", ") + ");";
                                txts.push(fns);
                            }
                        });
                    }
                });

                if (l == txts.length)
                    txts.pop();
            }

            function gen_cls_declare_end() {
                txts.push("};");
            }

            gen_cls_declare();
            gen_cls_consts();
            gen_cls_retTypes();
            gen_cls_members();
            gen_cls_new();
            gen_cls_member_stubs();
            gen_cls_async_stubs();
            gen_cls_declare_end();
        }

        function gen_refer_include() {
            txts.push('\n}');

            if (refers.length) {
                txts.push('');
                refers.forEach(c => txts.push('#include "' + c + '.h"'));
            }
            txts.push('\nnamespace fibjs {');
        }

        function gen_cls_info() {
            var method_count = 0;
            var object_count = 0;
            var prop_count = 0;

            function gen_method_info() {
                var deflist = [];

                def.members.forEach(fn => {
                    var fname = fn.name;
                    var fstatic = fn.static;

                    if (fname == def.declare.name && !fstatic && !fn.type)
                        return;

                    if (fname == "Function")
                        return;

                    if (fn.memType == "method") {
                        deflist.push('        { "' + fname + '", s_' + fname + ', ' + (fstatic ? 'true' : 'false') + ' }');
                        if (fn.async)
                            deflist.push('        { "' + `${fname}Sync` + '", s_' + fname + ', ' + (fstatic ? 'true' : 'false') + ' }');
                    }
                });

                if (deflist.length) {
                    method_count = deflist.length;
                    txts.push('    static ClassData::ClassMethod s_method[] = {');
                    txts.push(deflist.join(",\n"));
                    txts.push('    };\n');
                }
            }

            function gen_object_info() {
                var deflist = [];

                def.members.forEach(fn => {
                    if (fn.memType == 'object')
                        deflist.push('        { "' + fn.name + '", ' + fn.type + '_base::class_info }');
                });

                if (deflist.length) {
                    object_count = deflist.length;
                    txts.push('    static ClassData::ClassObject s_object[] = {');
                    txts.push(deflist.join(",\n"));
                    txts.push('    };\n');
                }
            }

            function gen_prop_info() {
                var deflist = [];

                def.members.forEach(fn => {
                    if (fn.memType == 'prop') {
                        var fname = fn.name;
                        deflist.push('        { "' + fname + '", s_get_' + fname + ', ' +
                            (fn.readonly ? 'block_set' : 's_set_' + fname) + ', ' +
                            (fn.static ? 'true' : 'false') + ' }');
                    } else if (fn.memType == 'const') {
                        var fname = fn.name;
                        deflist.push('        { "' + fname + '", s_get_' + fname + ', block_set, true }');
                    }
                });

                if (deflist.length) {
                    prop_count = deflist.length;
                    txts.push('    static ClassData::ClassProperty s_property[] = {');
                    txts.push(deflist.join(",\n"));
                    txts.push('    };\n');
                }
            }

            function gen_operator_info() {
                if (fnIndexed)
                    txts.push('    static ClassData::ClassIndexed s_indexed = {\n        i_IndexedGetter, i_IndexedSetter\n    };\n');

                if (fnNamed)
                    txts.push('    static ClassData::ClassNamed s_named = {\n        i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator\n    };\n');
            }

            function gen_def_info() {
                var ds;
                txts.push('    static ClassData s_cd = {');
                txts.push('        "' + cls + '", ' + !!def.declare.module + ', ' + 's__new, ' +
                    (callAsFunc ? 's__function' : 'NULL') + ',');

                ds = '        ';
                ds += method_count ? ('ARRAYSIZE(s_method), s_method, ') : '0, NULL, ';
                ds += object_count ? ('ARRAYSIZE(s_object), s_object, ') : '0, NULL, ';
                ds += prop_count ? ('ARRAYSIZE(s_property), s_property, ') : '0, NULL, ';
                ds += fnIndexed ? '&s_indexed, ' : 'NULL, ';
                ds += fnNamed ? '&s_named,' : 'NULL,';
                txts.push(ds);
                txts.push((def.declare.extend ?
                    ('        &' + def.declare.extend + '_base::class_info()') :
                    '        NULL'));
                txts.push('    };\n');
            }

            txts.push("inline ClassInfo& " + cls + "_base::class_info()\n{");

            gen_method_info();
            gen_object_info();
            gen_prop_info();
            gen_operator_info();

            gen_def_info();

            txts.push("    static ClassInfo s_ci(s_cd);\n    return s_ci;\n}\n");
        }

        function gen_stub() {
            def.members.forEach(fn => member_gens[fn.memType].stub_func(fn));
        }

        function build_refer() {
            var types = {};

            function add_type(type) {
                if (type && (type !== cls) && (type !== 'object') &&
                    (type !== def.declare.extend) &&
                    (!typeMap[type]))
                    types[type] = true;
            }

            def.members.forEach(m => {
                if (Array.isArray(m.type))
                    m.type.forEach(rt => add_type(rt.type));
                else
                    add_type(m.type);

                if (!m.overs)
                    return;
                m.overs.forEach(ov => {
                    if (!ov.params)
                        return;
                    ov.params.forEach(p => {
                        add_type(p.type);
                        if (p.default && util.isArray(p.default.const) && p.default.const.length > 1)
                            add_type(p.default.const[0]);
                    });
                });
            });

            refers = Object.keys(types);
        }

        function union_method() {
            var defs = {};
            var deflist = [];

            def.members.forEach(fn => {
                var fname = fn.name;
                var fstatic = fn.static;
                var fn1;

                if (!defs.hasOwnProperty(fname)) {
                    fn1 = util.clone(fn);
                    fn1.overs = [fn];

                    defs[fname] = fn1;
                    deflist.push(fn1);
                    return;
                } else if (fn.memType == "method")
                    fn1 = defs[fname];
                else
                    throw "ERROR";

                if (fstatic != fn1.static ||
                    fn.memType != fn1.memType ||
                    fn.type != fn1.type)
                    throw "Override a function with different return type.";

                fn1.overs.push(fn);
            });

            def.members = deflist;
        }

        union_method();

        build_refer();

        //---------------------------

        gen_begin();

        gen_refer_cls();

        gen_cls();

        gen_refer_include();

        gen_cls_info();

        gen_stub();

        gen_end();

        var txt = txts.join("\n");
        txt = txt.replace(/};\n\n}/g, '};\n}');
        txt = txt.replace(/}\n\n}/g, '}\n}');

        var fname = path.join(baseFolder, cls + ".h");

        if (!fs.exists(fname) || txt !== fs.readTextFile(fname)) {
            console.log(cls + ".h");
            fs.writeTextFile(fname, txt);
        }
    }

    for (var cls in defs) {
        gen_code(cls, defs[cls]);
    }
}