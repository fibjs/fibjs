var fs = require("fs");
var util = require("util");
var path = require('path');

/**
 * @description generate cpp code from idl definitions
 * 
 * @param {Record<string, import('../../idl/ir').IIDLDefinition>} defs 
 * @param {string} baseFolder 
 */
module.exports = function (defs, baseFolder) {
    const totalClasses = Object.keys(defs).filter(cls => !defs[cls].__skip).length;

    console.log(`   📋 Generating C++ code for ${totalClasses} classes...`);

    // Multi-round processing to ensure parent classes are processed before children
    const processed = new Set();
    const allClasses = Object.keys(defs).filter(cls => !defs[cls].__skip);

    let round = 1;
    while (processed.size < allClasses.length) {
        const initialSize = processed.size;

        for (var cls of allClasses) {
            if (processed.has(cls)) continue;

            const def = defs[cls];
            const parentClass = def.declare.extend;

            // Process if no parent or parent already processed
            if (!parentClass || parentClass === 'object' || processed.has(parentClass)) {
                // First do union_method for this class
                union_method_for_class(def, defs);

                // Then generate the code
                gen_code(cls, def, baseFolder, defs);
                processed.add(cls);
            }
        }

        // If no progress in this round, break to avoid infinite loop
        if (processed.size === initialSize) {
            console.warn(`⚠️  Warning: Some classes couldn't be processed due to circular dependencies or missing parents`);
            // Process remaining classes anyway
            for (var cls of allClasses) {
                if (!processed.has(cls)) {
                    const def = defs[cls];
                    union_method_for_class(def, defs);
                    gen_code(cls, def, baseFolder, defs);
                    processed.add(cls);
                }
            }
            break;
        }

        round++;
    }

    console.log(`   ✅ Completed processing in ${round - 1} rounds`);
}

/**
 * Process union_method for a single class definition
 * @param {import('../../idl/ir').IIDLDefinition} def 
 * @param {Record<string, import('../../idl/ir').IIDLDefinition>} allDefs 
 */
function union_method_for_class(def, allDefs) {
    var method_defs = {};
    var deflist = [];

    function check_type(t1, t2) {
        if (t1 == t2)
            return true;

        if (!Array.isArray(t1) || !Array.isArray(t2))
            return false;

        if (t1.length != t2.length)
            return false;

        for (var i = 0; i < t1.length; i++) {
            if (t1[i].type != t2[i].type)
                return false;
            if (t1[i].name != t2[i].name)
                return false;
        }

        return true;
    }

    // Don't add parent methods to def.members - they should not appear in virtual function definitions
    // We'll handle parent overloads separately in stub function generation

    def.members.forEach(fn => {
        var fname = fn.name;
        var fn1;

        if (fname === def.declare.name && fn.memType == "method")
            fname = "new " + fname;

        if (fn.memType == "event")
            fname = "event " + fname;

        if (fn.static)
            fname = "static " + fname;

        if (!method_defs.hasOwnProperty(fname)) {
            fn1 = JSON.parse(JSON.stringify(fn));
            fn1.overs = [fn];

            method_defs[fname] = fn1;
            deflist.push(fn1);
            return;
        } else if (fn.memType == "method")
            fn1 = method_defs[fname];
        else
            throw new Error("[union_method] only method can be override.");

        if (fn.memType != fn1.memType ||
            !check_type(fn.type, fn1.type)
        ) {
            throw new Error(`Override function '${fname}' with different return-type.`);
        }

        fn1.overs.push(fn);
    });

    def.members = deflist;
}

function record_exist() {
    const records = {};
    return {
        record: (name) => records[`#${name}`] = true,
        isRecorded: (name) => records[`#${name}`] === true
    }
}

/**
 * 
 * @param {string} cls key of def, name of fibjs's module/interface
 * @param {import('../../idl/ir').IIDLDefinition} def 
 * @param {string} baseFolder 
 * @param {object} allDefs all definitions for cross-reference
 */
function gen_code(cls, def, baseFolder, allDefs) {
    // Infer const type from value
    function inferConstType(fn) {
        if (fn.type) return fn.type;
        var value = fn.default.value;
        if (value === 'true' || value === 'false') return 'Boolean';
        if (value.startsWith('"') && value.endsWith('"')) return 'String';
        // Detect integers outside int32 range
        var num = Number(value);
        if (!isNaN(num) && (num > 2147483647 || num < -2147483648)) return 'Long';
        return 'Integer';
    }

    var typeMap = {
        "Integer": "int32_t",
        "Long": "int64_t",
        "Number": "double",
        "Boolean": "bool",
        "String": "exlib::string",
        "Date": "date_t",
        "Object": "v8::Local<v8::Object>",
        "RegExp": "v8::Local<v8::RegExp>",
        "Promise": "v8::Local<v8::Promise>",
        "Array": "v8::Local<v8::Array>",
        "Uint8Array": "v8::Local<v8::Uint8Array>",
        "ArrayBuffer": "std::shared_ptr<v8::BackingStore>",
        "TypedArray": "v8::Local<v8::TypedArray>",
        "Function": "v8::Local<v8::Function>",
        "Value": "v8::Local<v8::Value>",
        "Variant": "Variant",
        "NObject": "obj_ptr<NObject>",
        "NMap": "obj_ptr<NMap>",
        "NArray": "obj_ptr<NArray>",
        "...": "OptArgs"
    };

    var member_gens = {
        "method": {
            "declare": fn => {
                fn.overs.forEach(ov => {
                    // Only generate virtual function declarations for methods defined in current class
                    if (ov.sourceClass && ov.sourceClass !== def.declare.name) {
                        return; // Skip methods from parent classes
                    }

                    var fns = "    ";
                    var fname = get_specname(ov.name);
                    var fstatic = ov.static;
                    var is_new = false;

                    if (is_func_new(ov, def)) {
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
                    fns += get_name(fname, ov, def);
                    fns += "(";

                    var ps = [];

                    if (ov.params)
                        ov.params.forEach(p => {
                            if (p.name == "...")
                                ps.push("const v8::FunctionCallbackInfo<v8::Value>& args");
                            else ps.push(get_type(p) + " " + p.name);
                        });

                    if (ov.type)
                        ps.push(get_rtype(ov.type, ov.isarray) + "& retVal");

                    if (is_new) {
                        if (!ov.type) ps.push(get_rtype(cls) + "& retVal");
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
                var {
                    fncallee_ovs,
                    new_ovs,
                    static_ovs,
                    inst_mem_ovs
                } = vary_overs(fn, def);

                const recorder = record_exist();
                line = null
                fncallee_ovs.slice(0, 1).forEach(ov => {
                    line = `    static void ${get_stub_func_prefix(ov, def)}${get_name('_function', ov, def)}(const v8::FunctionCallbackInfo<v8::Value>& args);`;
                    if (!recorder.isRecorded(line)) txts.push(line);
                    recorder.record(line);
                });
                new_ovs.slice(0, 1).forEach(ov => {
                    line = `    static void ${get_stub_func_prefix(ov, def)}${get_name('_new', ov, def)}(const v8::FunctionCallbackInfo<v8::Value>& args);`;
                    if (!recorder.isRecorded(line)) txts.push(line);
                    recorder.record(line);
                });
                static_ovs.forEach(ov => {
                    line = `    static void ${get_stub_func_prefix(ov, def)}${get_name(ov.name, ov, def)}(const v8::FunctionCallbackInfo<v8::Value>& args);`;
                    if (!recorder.isRecorded(line)) txts.push(line);
                    recorder.record(line);
                });

                inst_mem_ovs.forEach(ov => {
                    line = `    static void ${get_stub_func_prefix(ov, def)}${get_name(ov.name, ov, def)}(const v8::FunctionCallbackInfo<v8::Value>& args);`;
                    if (!recorder.isRecorded(line)) txts.push(line);
                    recorder.record(line);
                });
            },
            "stub_func": fn => {
                var {
                    fncallee_ovs,
                    new_ovs,
                    static_ovs,
                    inst_mem_ovs
                } = vary_overs(fn, def);

                function make_ov_params(tp_overs, is_load = false) {
                    tp_overs.forEach(ov => {
                        var argc = 0;
                        var opts = 0;
                        var params = [];
                        var args = [];
                        var args_call;
                        var ftype = ov.type;

                        if (is_func_new(ov, def)) ftype = def.declare.name;

                        if (ov.params) {
                            argc = opts = ov.params.length;
                            ov.params.forEach(p => {
                                const vt = get_vtype(p);
                                args.push(vt.startsWith('obj_ptr<') ? 'v' + params.length + '.get()' : 'v' + params.length);
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
                                    if (p.isarray)
                                        defValue = `${vt}()`;
                                    else if (p.default.value)
                                        defValue = p.default.value;
                                    else if (Array.isArray(p.default.const))
                                        defValue = p.default.const[0] + '_base::C_' + p.default.const[1];
                                    else
                                        defValue = 'C_' + p.default.const;

                                    params.push(`    OPT_ARG(${vt + ', ' + params.length}, ` + defValue + `);`);
                                } else {
                                    if (is_func_new(ov, def) && params.length == 0 && ov.params.length == 1 && p.type == ftype)
                                        params.push(`    STRICT_ARG(${vt + ', ' + params.length});`);
                                    else
                                        params.push(`    ARG(${vt + ', ' + params.length});`);
                                }
                            });
                        }

                        // For load function, only output constructor code with required parameters = 1
                        if (is_load) {
                            if (argc == 0 || opts > 1) {
                                return;
                            }
                        }

                        txts.push(`    METHOD_OVER(${argc}, ${opts});\n`);
                        if (params.length)
                            txts.push(params.join('\n') + '\n');

                        args_call = args.slice(0);
                        if (ftype)
                            args_call.push('vr');

                        if (ov.deprecated)
                            txts.push('    DEPRECATED_SOON("' + cls + '.' + ov.name + '");\n');

                        if (ov.async) {
                            args.push('cb');;
                            ([
                                `    if (!cb.IsEmpty())\n        hr = ${(ov.static ? 'acb_' : 'pInst->acb_')}${get_name(ov.name, ov, def)}(${args.join(', ')}, args);`,
                                `    else`,
                                `        hr = ${(ov.static ? 'ac_' : 'pInst->ac_')}${get_name(ov.name, ov, def)}(${args_call.join(', ')});\n`,
                            ]).forEach(line => txts.push(line))
                        } else {
                            if (is_func_new(ov, def))
                                args_call.push('args.This()');

                            if (ov.static || is_func_new(ov, def))
                                txts.push([
                                    `    hr = ${get_name(get_fname(ov, def), ov, def)}(${args_call.join(', ')});\n`,
                                ].join(''));
                            else
                                txts.push([
                                    `    hr = pInst->${get_name(get_fname(ov, def), ov, def)}(${args_call.join(', ')});\n`,
                                ].join(''));

                        }
                    })
                }

                fncallee_ovs.slice(0, 1).forEach(ov => {
                    if (ov.static) {
                        txts.push(`inline void ${cls}_base::${get_stub_func_prefix(ov, def)}${get_name('_function', ov, def)}(const v8::FunctionCallbackInfo<v8::Value>& args)\n{`);
                        if (ov.type) txts.push(`    ${get_rtype(ov.type, ov.isarray)} vr;\n`);
                    } else {
                        txts.push(`inline void ${cls}_base::${get_stub_func_prefix(ov, def)}${get_name(get_fname(ov, def), ov, def)}(const v8::FunctionCallbackInfo<v8::Value>& args)\n{`);

                        if (ov.type) txts.push(`    ${get_rtype(ov.type, ov.isarray)} vr;\n`);

                        if (ov.async)
                            txts.push(`    ASYNC_METHOD_INSTANCE(${cls}_base);`);
                        else
                            txts.push(`    METHOD_INSTANCE(${cls}_base);`);
                    }

                    if (ov.async)
                        txts.push(`    ASYNC_METHOD_ENTER("${cls}");\n`);
                    else
                        txts.push(`    METHOD_ENTER();\n`);
                    make_ov_params(fncallee_ovs);

                    if (ov.type) txts.push(ov.async ? '    ASYNC_METHOD_RETURN();\n}\n' : '    METHOD_RETURN();\n}\n');
                    else txts.push(ov.async ? '    ASYNC_METHOD_VOID();\n}\n' : '    METHOD_VOID();\n}\n');
                });

                new_ovs.slice(0, 1).forEach(ov => {
                    txts.push(`inline void ${cls}_base::${get_stub_func_prefix(ov, def)}${get_name('_new', ov, def)}(const v8::FunctionCallbackInfo<v8::Value>& args)\n{`);
                    txts.push('    CONSTRUCT_INIT();\n    __new(args);\n}\n');

                    txts.push(`inline void ${cls}_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)\n{`);
                    txts.push(`    ${get_rtype(def.declare.name)} vr;\n`);
                    txts.push(`    CONSTRUCT_ENTER();\n`);
                    make_ov_params(new_ovs);
                    txts.push('    CONSTRUCT_RETURN();\n}\n');

                    txts.push(`inline result_t ${cls}_base::load(v8::Local<v8::Value> v, obj_ptr<${cls}_base>& retVal)\n{`);
                    txts.push(`    ${get_rtype(def.declare.name)} vr;\n`);
                    txts.push(`    LOAD_ENTER();\n`);
                    make_ov_params(new_ovs, true);
                    txts.push('    LOAD_RETURN();\n}\n');
                });

                // Check if any overload has Function as last parameter
                function hasLastParamFunction(ovs) {
                    return ovs.some(ov => {
                        if (ov.params && ov.params.length > 0) {
                            var lastParam = ov.params[ov.params.length - 1];
                            return lastParam.type === 'Function';
                        }
                        return false;
                    });
                }

                const recorder_statics = record_exist();
                static_ovs.forEach(ov => {
                    if (recorder_statics.isRecorded(ov.name)) return;

                    txts.push(`inline void ${cls}_base::${get_stub_func_prefix(ov, def)}${get_name(get_fname(ov, def), ov, def)}(const v8::FunctionCallbackInfo<v8::Value>& args)\n{`);

                    if (ov.type) txts.push(`    ${get_rtype(ov.type, ov.isarray)} vr;\n`);

                    if (ov.async) {
                        if (hasLastParamFunction(static_ovs))
                            txts.push(`    ASYNC_METHOD_ENTER_FUNC("${cls}.${ov.symbol}${ov.name}");\n`);
                        else
                            txts.push(`    ASYNC_METHOD_ENTER("${cls}.${ov.symbol}${ov.name}");\n`);
                    } else
                        txts.push(`    METHOD_ENTER();\n`);
                    make_ov_params(static_ovs);

                    if (ov.type) txts.push(ov.async ? '    ASYNC_METHOD_RETURN();\n}\n' : '    METHOD_RETURN();\n}\n');
                    else txts.push(ov.async ? '    ASYNC_METHOD_VOID();\n}\n' : '    METHOD_VOID();\n}\n');

                    recorder_statics.record(ov.name);
                });

                const recorder_insts = record_exist();
                inst_mem_ovs.forEach(ov => {
                    if (recorder_insts.isRecorded(ov.name)) return;

                    txts.push(`inline void ${cls}_base::${get_stub_func_prefix(ov, def)}${get_name(get_fname(ov, def), ov, def)}(const v8::FunctionCallbackInfo<v8::Value>& args)\n{`);

                    if (ov.type) txts.push(`    ${get_rtype(ov.type, ov.isarray)} vr;\n`);

                    if (ov.async) {
                        txts.push(`    ASYNC_METHOD_INSTANCE(${cls}_base);`);
                        if (hasLastParamFunction(inst_mem_ovs))
                            txts.push(`    ASYNC_METHOD_ENTER_FUNC("${cls}.${ov.symbol}${ov.name}");\n`);
                        else
                            txts.push(`    ASYNC_METHOD_ENTER("${cls}.${ov.symbol}${ov.name}");\n`);
                    } else {
                        txts.push(`    METHOD_INSTANCE(${cls}_base);`);
                        txts.push(`    METHOD_ENTER();\n`);
                    }
                    make_ov_params(inst_mem_ovs);

                    if (ov.type) txts.push(ov.async ? '    ASYNC_METHOD_RETURN();\n}\n' : '    METHOD_RETURN();\n}\n');
                    else txts.push(ov.async ? '    ASYNC_METHOD_VOID();\n}\n' : '    METHOD_VOID();\n}\n');

                    recorder_insts.record(ov.name);
                });
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
                    fns += get_name(fname, fn, def);
                    fns += "(";

                    fns += get_rtype(fn.type) + "& retVal";

                    fns += fstatic ? ");" : ") = 0;";

                    txts.push(fns);

                    if (!fn.readonly) {
                        fns = "    ";

                        fns += fstatic ? "static " : "virtual ";
                        fns += "result_t set_";
                        fns += get_name(fname, fn, def);
                        fns += "(";

                        fns += get_type(fn) + " newVal";

                        fns += fstatic ? ");" : ") = 0;";

                        txts.push(fns);

                    }
                }
            },
            "stub": fn => {
                var fname = fn.name;

                if (fname) {
                    txts.push(`    static void ${get_stub_func_prefix(fn, def)}get_${get_name(fname, fn, def)}(const v8::FunctionCallbackInfo<v8::Value>& args);`);
                    if (!fn.readonly)
                        txts.push(`    static void ${get_stub_func_prefix(fn, def)}set_${get_name(fname, fn, def)}(const v8::FunctionCallbackInfo<v8::Value>& args);`);
                }
            },
            "stub_func": fn => {
                var fname = fn.name;
                var fstatic = fn.static;

                txts.push(`inline void ${cls}_base::${get_stub_func_prefix(fn, def)}get_${get_name(fname, fn, def)}(const v8::FunctionCallbackInfo<v8::Value>& args)\n{\n    ${get_rtype(fn.type)} vr;\n`);

                if (!fstatic)
                    txts.push(`    METHOD_INSTANCE(${cls}_base);`);
                txts.push(`    METHOD_ENTER();\n\n    METHOD_OVER(0, 0);\n`);

                if (fn.deprecated)
                    txts.push(`    DEPRECATED_SOON("${cls}.${fn.symbol}${fname}");\n`);

                if (fstatic)
                    txts.push(`    hr = get_${get_name(fname, fn, def)}(vr);\n`);
                else
                    txts.push(`    hr = pInst->get_${get_name(fname, fn, def)}(vr);\n`);
                txts.push(`    METHOD_RETURN();\n}\n`);

                if (!fn.readonly) {
                    txts.push(`inline void ${cls}_base::${get_stub_func_prefix(fn, def)}set_${get_name(fname, fn, def)}(const v8::FunctionCallbackInfo<v8::Value>& args)\n{`);
                    if (!fstatic)
                        txts.push(`    METHOD_INSTANCE(${cls}_base);`);
                    txts.push(`    METHOD_ENTER();\n\n    METHOD_OVER(1, 1);\n\n    ARG(${get_vtype(fn)}, 0);\n`);

                    if (fn.deprecated)
                        txts.push(`    DEPRECATED_SOON("${cls}.${fn.symbol}${fname}");\n`);

                    if (fstatic)
                        txts.push(`    hr = set_${get_name(fname, fn, def)}(v0);\n`);
                    else
                        txts.push(`    hr = pInst->set_${get_name(fname, fn, def)}(v0);\n`);
                    txts.push(`    METHOD_VOID();\n}\n`);
                }

            }
        },
        "event": {
            "declare": () => { },
            "stub": fn => {
                var fname = fn.name;

                if (fname) {
                    txts.push(`    static void ${get_stub_func_prefix(fn, def)}get_on${get_name(fname, fn, def)}(const v8::FunctionCallbackInfo<v8::Value>& args);`);
                    if (!fn.readonly)
                        txts.push(`    static void ${get_stub_func_prefix(fn, def)}set_on${get_name(fname, fn, def)}(const v8::FunctionCallbackInfo<v8::Value>& args);`);
                }
            },
            "stub_func": fn => {
                var fname = fn.name;

                txts.push(`inline void ${cls}_base::${get_stub_func_prefix(fn, def)}get_on${get_name(fname, fn, def)}(const v8::FunctionCallbackInfo<v8::Value>& args)\n{\n    ${get_rtype("Function")} vr;\n`);

                txts.push(`    METHOD_INSTANCE(${cls}_base);`);
                txts.push(`    METHOD_ENTER();\n\n    METHOD_OVER(0, 0);\n`);

                if (fn.deprecated)
                    txts.push(`    DEPRECATED_SOON("${cls}.get_on${fname}");\n`);

                txts.push(`    hr = pInst->getListener("${fname}", vr);\n`);
                txts.push(`    METHOD_RETURN();\n}\n`);

                if (!fn.readonly) {
                    txts.push(`inline void ${cls}_base::${get_stub_func_prefix(fn, def)}set_on${get_name(fname, fn, def)}(const v8::FunctionCallbackInfo<v8::Value>& args)\n{`);
                    txts.push(`    METHOD_INSTANCE(${cls}_base);`);
                    txts.push(`    METHOD_ENTER();\n\n    METHOD_OVER(1, 1);\n\n    ARG(${get_rtype("Function")}, 0);\n`);

                    if (fn.deprecated)
                        txts.push(`    DEPRECATED_SOON("${cls}.set_on${fname}");\n`);

                    txts.push(`    hr = pInst->setListener("${fname}", v0);\n`);
                    txts.push(`    METHOD_VOID();\n}\n`);
                }

            }
        },
        "object": {
            "declare": () => { },
            "stub": () => { },
            "stub_func": () => { }
        },
        "const": {
            "declare": () => { },
            "stub": fn => { },
            "stub_func": fn => { }
        },
        "operator": {
            "declare": fn => {
                if (fn.index) {
                    fnNamed = fn;
                    txts.push(`    virtual result_t _named_getter(exlib::string property, ${get_rtype(fn.type)}& retVal) = 0;`);
                    txts.push(`    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal) = 0;`);
                    if (!fn.readonly) {
                        txts.push(`    virtual result_t _named_setter(exlib::string property, ${get_type(fn)} newVal) = 0;`);
                        txts.push(`    virtual result_t _named_deleter(exlib::string property, v8::Local<v8::Boolean>& retVal) = 0;`);
                    }
                } else {
                    fnIndexed = fn;
                    txts.push(`    virtual result_t _indexed_getter(uint32_t index, ${get_rtype(fn.type)}& retVal) = 0;`);
                    if (!fn.readonly) {
                        txts.push(`    virtual result_t _indexed_setter(uint32_t index, ${get_type(fn)} newVal) = 0;`);
                    }
                }
            },
            "stub": fn => {
                if (fn.index) {
                    txts.push("    static v8::Intercepted i_NamedGetter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);");
                    txts.push("    static void i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args);");
                    if (!fn.readonly) {
                        txts.push("    static v8::Intercepted i_NamedSetter(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);");
                        txts.push("    static v8::Intercepted i_NamedDeleter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& args);");
                    }
                } else {
                    txts.push("    static v8::Intercepted i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args);");
                    if (!fn.readonly)
                        txts.push("    static v8::Intercepted i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args);");
                }
            },
            "stub_func": fn => {
                if (fn.index) {
                    txts.push(`inline v8::Intercepted ${cls}_base::i_NamedGetter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)\n{\n    ${get_rtype(fn.type)} vr;\n`);
                    txts.push(`    NAMED_METHOD_INSTANCE(${cls}_base);\n    PROPERTY_ENTER();\n`);
                    txts.push('    exlib::string k;\n    GetArgumentValue(isolate, property, k);\n    if (class_info().has(k.c_str()))\n        return v8::Intercepted::kNo;\n');
                    if (fn.deprecated)
                        txts.push(`    DEPRECATED_SOON("${cls}${fn.name}");\n`);
                    txts.push('    hr = pInst->_named_getter(k, vr);\n    if (hr == CALL_RETURN_NULL)\n        return v8::Intercepted::kNo;\n');
                    txts.push('    NAMED_METHOD_RETURN();\n}\n');
                    txts.push(`inline void ${cls}_base::i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args)\n{\n    v8::Local<v8::Array> vr;\n`);
                    txts.push(`    METHOD_INSTANCE(${cls}_base);\n    PROPERTY_ENTER();\n`);
                    txts.push('    hr = pInst->_named_enumerator(vr);\n');
                    txts.push('    METHOD_RETURN1();\n}\n');

                    if (!fn.readonly) {
                        txts.push(`inline v8::Intercepted ${cls}_base::i_NamedSetter(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)\n{`);
                        txts.push(`    NAMED_METHOD_INSTANCE(${cls}_base);\n    PROPERTY_ENTER();\n`);
                        txts.push(`    PROPERTY_VAL(${get_rtype(fn.type)});\n    exlib::string k;\n    GetArgumentValue(isolate, property, k);\n    if (class_info().has(k.c_str()))\n        return v8::Intercepted::kNo;\n`);
                        if (fn.deprecated)
                            txts.push(`    DEPRECATED_SOON("${cls}${fn.name}");\n`);
                        txts.push('    hr = pInst->_named_setter(k, v0);\n    if (hr == CALL_RETURN_NULL)\n        return v8::Intercepted::kNo;\n');
                        txts.push('    NAMED_METHOD_VOID();\n}\n');
                        txts.push(`inline v8::Intercepted ${cls}_base::i_NamedDeleter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& args)\n{\n    v8::Local<v8::Boolean> vr;\n`);
                        txts.push(`    NAMED_METHOD_INSTANCE(${cls}_base);\n    PROPERTY_ENTER();\n`);
                        txts.push('    exlib::string k;\n    GetArgumentValue(isolate, property, k);\n    if (class_info().has(k.c_str())) {\n        args.GetReturnValue().Set(v8::False(isolate->m_isolate));\n        return v8::Intercepted::kYes;\n    }\n');
                        txts.push('    hr = pInst->_named_deleter(k, vr);\n    METHOD_RETURN2();\n}\n');
                    }
                } else {
                    txts.push(`inline v8::Intercepted ${cls}_base::i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args)\n{`);
                    txts.push(`    ${get_rtype(fn.type)} vr;\n`);
                    txts.push(`    NAMED_METHOD_INSTANCE(${cls}_base);\n    PROPERTY_ENTER();\n`);
                    if (fn.deprecated)
                        txts.push(`    DEPRECATED_SOON("${cls}${fn.name}");\n`);
                    txts.push('    hr = pInst->_indexed_getter(index, vr);\n    if (hr == CALL_RETURN_NULL)\n        return v8::Intercepted::kNo;\n');
                    txts.push('    NAMED_METHOD_RETURN();\n}\n');

                    if (!fn.readonly) {
                        txts.push(`inline v8::Intercepted ${cls}_base::i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args)\n{`);
                        txts.push(`    NAMED_METHOD_INSTANCE(${cls}_base);\n    PROPERTY_ENTER();\n`);
                        if (fn.deprecated)
                            txts.push(`    DEPRECATED_SOON("${cls}${fn.name}");\n`);
                        txts.push(`    PROPERTY_VAL(${get_rtype(fn.type)});\n    hr = pInst->_indexed_setter(index, v0);\n`);
                        txts.push(`    NAMED_METHOD_VOID();\n}\n`);
                    }
                }
            }
        }
    };

    var txts = [];
    var refers = [];
    var hasNew = false;
    var staticCallAsFunc = false;
    var callAsFunc = false;
    var fnIndexed = null;
    var fnNamed = null;

    MAIN: {
        build_refer();

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

        if (!fs.existsSync(fname) || txt !== fs.readFileSync(fname, 'utf8')) {
            console.log(`      ✏️  ${cls}.h`);
            fs.writeFileSync(fname, txt);
        }
    }

    function get_type(p) {
        if (p.isarray)
            return `std::vector<${typeMap[p.type] || (`obj_ptr<${p.type}_base>`)}>&`;
        return typeMap[p.type] || (p.type + "_base*");
    }

    function get_vtype(p) {
        var t = typeMap[p.type] || (`obj_ptr<${p.type}_base>`);
        if (p.isarray)
            t = `std::vector<${t}>`;
        return t;
    }

    function get_rtype(t, isarray) {
        if (Array.isArray(t))
            return `obj_ptr<${t.name}>`;
        var baseType = typeMap[t] || (`obj_ptr<${t}_base>`);
        if (isarray)
            return `std::vector<${baseType}>`;
        return baseType;
    }

    function is_func_Function(fn, def) {
        return fn.name === 'Function'
    }

    function is_func_new(fn, def) {
        return get_specname(fn.name) === def.declare.name && !fn.static /*  && !fn.type */
    }

    function get_specname(n) {
        var ckws = {
            "union": true,
            "and": true,
            "or": true,
            "xor": true,
            "new": true,
            "host": true,
            "assert": true,
            "export": true,
            "delete": true
        };

        return ckws.hasOwnProperty(n) ? "_" + n : n;
    }

    function get_fname(fn, def) {
        if (is_func_new(fn, def)) return '_new'
        if (is_func_Function(fn, def)) return '_function'

        return get_specname(fn.name)
    }

    function collect_parent_overloads(methodName, def) {
        var parentOverloads = [];

        // Check if this method exists in current class (indicating it's overridden)
        var isOverridden = def.members.some(m =>
            m.memType === "method" &&
            m.name === methodName &&
            !m.static &&
            m.name !== def.declare.name
        );

        if (!isOverridden || !def.declare.extend) {
            return parentOverloads;
        }

        // For 'object' class, we need to handle it specially since it might not be in allDefs
        if (def.declare.extend === 'object') {
            // object class has toString() method with no parameters
            if (methodName === 'toString') {
                var objectToStringOver = {
                    name: 'toString',
                    memType: 'method',
                    params: [],
                    type: 'String',
                    inherit: true,
                    sourceClass: 'object'
                };
                parentOverloads.push(objectToStringOver);
            }
            return parentOverloads;
        }

        if (!allDefs[def.declare.extend]) {
            return parentOverloads;
        }

        var parentDef = allDefs[def.declare.extend];
        var parentMethod = parentDef.members.find(m =>
            m.memType === "method" &&
            m.name === methodName &&
            !m.static &&
            m.name !== parentDef.declare.name
        );

        if (parentMethod && parentMethod.overs) {
            parentMethod.overs.forEach(parentOver => {
                // Only add parent overloads that don't exist in current class
                var existsInCurrent = def.members.some(m =>
                    m.memType === "method" &&
                    m.name === methodName &&
                    m.overs && m.overs.some(ov =>
                        ov.params && parentOver.params &&
                        ov.params.length === parentOver.params.length &&
                        ov.params.every((p, i) =>
                            parentOver.params[i] &&
                            p.type === parentOver.params[i].type
                        )
                    )
                );

                if (!existsInCurrent) {
                    var inheritedOver = JSON.parse(JSON.stringify(parentOver));
                    inheritedOver.inherit = true;
                    inheritedOver.sourceClass = parentDef.declare.name;
                    parentOverloads.push(inheritedOver);
                }
            });
        }

        return parentOverloads;
    }

    function vary_overs(fn, def) {
        var fncallee_ovs = fn.overs.filter(ov => is_func_Function(ov, def));
        var new_ovs = fn.overs.filter(ov => is_func_new(ov, def));
        var static_ovs = fn.overs.filter(ov => !!ov.static && !is_func_new(ov, def) && !is_func_Function(ov, def));
        var inst_mem_ovs = fn.overs.filter(ov => !ov.static && !is_func_new(ov, def) && !is_func_Function(ov, def));

        // For instance methods, also include parent class overloads
        if (inst_mem_ovs.length > 0) {
            var parentOverloads = collect_parent_overloads(fn.name, def);
            inst_mem_ovs = inst_mem_ovs.concat(parentOverloads);
        }

        return {
            fncallee_ovs,
            new_ovs,
            static_ovs,
            inst_mem_ovs,
        }
    }

    function get_name(fname, fn, def) {
        var base = get_specname(fname)

        if (fn.static) return base
        if (is_func_Function(fn, def)) return base
        if (is_func_new(fn, def)) return base
        if (fn.symbol)
            return `symbol_${base}`;

        return base
    }

    function get_stub_func_prefix(fn, def) {
        var base = 's_'

        if (is_func_Function(fn, def)) return base
        if (is_func_new(fn, def)) return base
        if (fn.static) return `${base}static_`

        return base
    }

    function gen_begin() {
        txts.push([
            '/***************************************************************************',
            ' *                                                                         *',
            ' *   This file was automatically generated using idlc.js                   *',
            ' *   PLEASE DO NOT EDIT!!!!                                                *',
            ' *                                                                         *',
            ' ***************************************************************************/',
            ''
        ].join('\n'));

        txts.push(`#pragma once\n`);

        txts.push("/**\n @author Leo Hoo <lion@9465.net>\n */\n");

        txts.push('#include "../object.h"');
        if (def.declare.extend && def.declare.extend !== 'object')
            txts.push(`#include "ifs/${def.declare.extend}.h"`);

        txts.push('\nnamespace fibjs {\n');
    }

    function gen_end() {
        txts.push("}\n");
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
                txts.push(`class ${cls}_base : public ${def.declare.extend}_base {`);
            else
                txts.push(`class ${cls}_base {`);
            txts.push(`    DECLARE_CLASS(${cls}_base);`);

            if (def.declare.extend === 'EventEmitter') {
                txts.push(`    EVENT_SUPPORT();`);
            }
        }

        function gen_cls_consts() {
            var int_consts = [];
            var long_consts = [];

            def.members.forEach(fn => {
                if (fn.memType !== "const") return;
                var ctype = inferConstType(fn);
                // Integer consts go into enum (int32_t)
                if (ctype === 'Integer')
                    int_consts.push(`        C_${fn.name} = ${fn.default.value}`);
                // Long consts go into a static int64_t declaration
                else if (ctype === 'Long')
                    long_consts.push(`    static const int64_t C_${fn.name} = ${fn.default.value}LL;`);
            });

            if (int_consts.length) {
                txts.push("\npublic:\n    enum {");
                txts.push(int_consts.join(",\n"));
                txts.push("    };");
            }
            if (long_consts.length) {
                txts.push("\npublic:");
                long_consts.forEach(l => txts.push(l));
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
                    fn.overs.forEach(ofun => {
                        ofun.type.name = name;
                    })

                    ts.push(`    class ${name} : public NType {`);

                    ts.push('    public:');
                    ts.push('        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)');
                    ts.push('        {');
                    ts.push('            v8::Local<v8::Context> context = retVal->GetCreationContextChecked();');

                    fn.type.forEach(t => {
                        ts.push(`            retVal->Set(context, isolate->NewString("${t.name}"), GetReturnValue(isolate, ${t.name})).Check();`);
                    });

                    ts.push('        }\n');

                    ts.push('        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)');
                    ts.push('        {');

                    fn.type.forEach(t => {
                        ts.push(`            args.push_back(GetReturnValue(isolate, ${t.name}));`);
                    });

                    ts.push('        }\n');

                    ts.push('    public:');
                    fn.type.forEach(t => {
                        ts.push(`        ${get_rtype(t.type)} ${t.name};`);
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
            txts.push(`\npublic:\n    // ${cls}_base`);
            var l = txts.length;

            def.members.forEach(fn => member_gens[fn.memType].declare(fn));

            if (l == txts.length)
                txts.pop();
        }

        function gen_cls_new() {
            txts.push("");
            if (hasNew) {
                txts.push("public:\n    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);");
                txts.push(`    static result_t load(v8::Local<v8::Value> v, obj_ptr<${cls}_base>& retVal);\n`);
            }
            else if (staticCallAsFunc)
                txts.push([
                    "public:\n    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)\n    {\n",
                    "        s__function(args);\n    }\n"
                ].join(''));
            else {
                txts.push([
                    "public:\n    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)\n    {\n",
                    "        CONSTRUCT_INIT();\n\n",
                    "        ThrowTypeError(\"not a constructor\");\n    }\n"
                ].join(''));

                txts.push(`    static result_t load(v8::Local<v8::Value> v, obj_ptr<${cls}_base>& retVal)`);
                txts.push(`    { return CALL_E_TYPEMISMATCH; }\n`);
            }
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
                            var ps = [cls + "_base", get_name(ov.name, ov, def)];

                            if (ov.params) {
                                pn = ov.params.length;
                                ov.params.forEach(p => ps.push(get_type(p).replace(/&/g, "")));
                            }

                            fns += (ov.static ? "STATIC" : "MEMBER");

                            if (ov.type) {
                                pn++;
                                ps.push(get_rtype(ov.type, ov.isarray));
                                fns += "VALUE";
                            }

                            fns += `${pn}(${ps.join(", ")});`;
                            txts.push(fns);
                        }
                    });
                }
            });

            if (l == txts.length)
                txts.pop();
        }

        function gen_cls_using_declarations() {
            // Generate using declarations for overridden parent methods
            if (!def.declare.extend) return;

            var parentDef = allDefs[def.declare.extend];
            if (!parentDef) return;

            var overriddenMethods = new Set();
            var usingDeclarations = [];

            // Find methods that are overridden in current class
            def.members.forEach(fn => {
                if (fn.memType === "method" && fn.name !== cls && !fn.static) {
                    overriddenMethods.add(fn.name);
                }
            });

            // Check parent class for methods with same names
            parentDef.members.forEach(parentFn => {
                if (parentFn.memType === "method" &&
                    parentFn.name !== parentDef.declare.name &&
                    !parentFn.static &&
                    overriddenMethods.has(parentFn.name)) {

                    // Add using declaration for overridden method
                    usingDeclarations.push(`    using ${def.declare.extend}_base::${parentFn.name};`);
                }
            });

            if (usingDeclarations.length > 0) {
                txts.push("");
                txts.push("public:");
                usingDeclarations.forEach(decl => txts.push(decl));
            }
        }

        function gen_cls_declare_end() {
            txts.push("};");
        }

        gen_cls_declare();
        gen_cls_using_declarations();
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
            refers.forEach(c => txts.push('#include "ifs/' + c + '.h"'));
        }
        txts.push('\nnamespace fibjs {');
    }

    function gen_cls_info() {
        var method_count = 0;
        var object_count = 0;
        var prop_count = 0;
        var const_count = 0;

        // Check if any ancestor class has async methods (so prototype chain is inherited)
        function base_has_async(extendName) {
            if (!extendName || extendName === 'object') return false;
            var baseDef = allDefs[extendName];
            if (!baseDef) return false;
            var hasIt = baseDef.members.some(fn =>
                fn.memType === 'method' && fn.overs && fn.overs.some(ov => ov.async === 'async' || ov.async === 'promise')
            );
            return hasIt || base_has_async(baseDef.declare && baseDef.declare.extend);
        }

        var has_async = base_has_async(def.declare && def.declare.extend);

        function async_type(async_) {
            switch (async_) {
                case 'async':
                    return 'ClassData::ASYNC_ASYNC';
                case 'promise':
                    return 'ClassData::ASYNC_PROMISE';
            }
            return 'ClassData::ASYNC_SYNC';
        }

        function gen_method_info() {
            var deflist = [];

            def.members.forEach(fn => {
                var fname = fn.name;

                var {
                    inst_mem_ovs,
                    static_ovs
                } = vary_overs(fn, def);

                const recorder_insts = record_exist();
                inst_mem_ovs.forEach(ov => {
                    if (recorder_insts.isRecorded(ov.name)) return;

                    if (ov.memType == "method") {
                        deflist.push(`        { "${fn.symbol}${fname}", ${get_stub_func_prefix(ov, def)}${get_name(fname, ov, def)}, false, ${async_type(ov.async)} }`);
                        if (ov.async == 'async')
                            has_async = true;

                        recorder_insts.record(ov.name);
                    }
                });

                const recorder_statics = record_exist();
                static_ovs.forEach(ov => {
                    if (recorder_statics.isRecorded(ov.name)) return;

                    if (ov.memType == "method") {
                        deflist.push(`        { "${fn.symbol}${fname}", ${get_stub_func_prefix(ov, def)}${get_name(fname, ov, def)}, true, ${async_type(ov.async)} }`);
                        if (ov.async == 'async')
                            has_async = true;

                        recorder_statics.record(ov.name);
                    }
                });
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
                    deflist.push(`        { "${fn.name}", ${fn.type}_base::class_info }`);
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
                    deflist.push([
                        `        { "${fn.symbol}${fname}", ${get_stub_func_prefix(fn, def)}get_${get_name(fname, fn, def)}, `,
                        `${fn.readonly ? `block_set` : (`${get_stub_func_prefix(fn, def)}set_` + get_name(fname, fn, def))}, `,
                        `${fn.static ? `true` : `false`} }`
                    ].join(''));
                } else if (fn.memType == 'event') {
                    var fname = fn.name;
                    deflist.push([
                        `        { "on${fname}", ${get_stub_func_prefix(fn, def)}get_on${get_name(fname, fn, def)}, `,
                        `${fn.readonly ? `block_set` : (`${get_stub_func_prefix(fn, def)}set_on` + get_name(fname, fn, def))}, `,
                        `${fn.static ? `true` : `false`} }`
                    ].join(''));
                }
            });

            if (deflist.length) {
                prop_count = deflist.length;
                txts.push('    static ClassData::ClassProperty s_property[] = {');
                txts.push(deflist.join(",\n"));
                txts.push('    };\n');
            }
        }

        function gen_const_info() {
            var deflist = [];

            def.members.forEach(fn => {
                if (fn.memType == 'const') {
                    var fname = fn.name;
                    var constType = inferConstType(fn);
                    var valueExpr;

                    if (constType === 'Integer') {
                        valueExpr = `{ .intValue = C_${fname} }`;
                    } else if (constType === 'Long') {
                        valueExpr = `{ .longValue = C_${fname} }`;
                    } else if (constType === 'Boolean') {
                        valueExpr = `{ .boolValue = ${fn.default.value} }`;
                    } else if (constType === 'String') {
                        // Escape backslashes for C++ string literal
                        var strVal = fn.default.value;
                        if (strVal.startsWith('"') && strVal.endsWith('"')) {
                            var content = strVal.slice(1, -1);
                            content = content.replace(/\\/g, '\\\\');
                            strVal = '"' + content + '"';
                        }
                        valueExpr = `{ .stringValue = ${strVal} }`;
                    }

                    deflist.push(`        { "${fname}", ClassData::CONST_${constType}, ${valueExpr} }`);
                }
            });

            if (deflist.length) {
                const_count = deflist.length;
                txts.push('    static ClassData::ClassConst s_const[] = {');
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
            txts.push('    static ClassData s_cd = {');
            txts.push(`        "${cls}", ${!!def.declare.module}, s__new, ${(callAsFunc ? `s__function` : `NULL`)},`);

            txts.push([
                '        ',
                method_count ? ('ARRAYSIZE(s_method), s_method, ') : '0, NULL, ',
                object_count ? ('ARRAYSIZE(s_object), s_object, ') : '0, NULL, ',
                prop_count ? ('ARRAYSIZE(s_property), s_property, ') : '0, NULL, ',
                const_count ? ('ARRAYSIZE(s_const), s_const, ') : '0, NULL, ',
                fnIndexed ? '&s_indexed, ' : 'NULL, ',
                fnNamed ? '&s_named,' : 'NULL,',
            ].join(''));

            if (def.declare.extend)
                txts.push(`        &${def.declare.extend}_base::class_info(),`);
            else
                txts.push('        NULL,');
            txts.push(`        ${has_async}`);
            txts.push('    };\n');
        }

        txts.push(`inline ClassInfo& ${cls}_base::class_info()\n{`);

        gen_method_info();
        gen_object_info();
        gen_prop_info();
        gen_const_info();
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
                    if (p.default && Array.isArray(p.default.const) && p.default.const.length > 1)
                        add_type(p.default.const[0]);
                });
            });
        });

        refers = Object.keys(types);
    }
}